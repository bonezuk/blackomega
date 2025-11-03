#include "engine/whiteomega/inc/WhiteCodec.h"
#include "engine/inc/RData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------
// WhiteCodecInitialize
//-------------------------------------------------------------------------------------------

WhiteCodecInitialize *WhiteCodecInitialize::m_instance = 0;

//-------------------------------------------------------------------------------------------

WhiteCodecInitialize::WhiteCodecInitialize()
{
    WSequence::start();
    AACDecode::start();
    Window::start();
}

//-------------------------------------------------------------------------------------------

WhiteCodecInitialize::~WhiteCodecInitialize()
{
    AACDecode::stop();
    WSequence::stop();
    Window::stop();
}

//-------------------------------------------------------------------------------------------

void WhiteCodecInitialize::start()
{
    if(m_instance==0)
    {
        m_instance = new WhiteCodecInitialize();
    }
}

//-------------------------------------------------------------------------------------------

void WhiteCodecInitialize::end()
{
    if(m_instance!=0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

//-------------------------------------------------------------------------------------------
// WhiteCodec
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            WhiteCodecFactory,WhiteCodec, \
                            "m4a",false)

//-------------------------------------------------------------------------------------------

WhiteCodec::WhiteCodec(QObject *parent) : Codec(Codec::e_codecMp4,parent),
    m_gaConfig(),
    m_fileStream(0),
    m_file(0),
    m_atom(0),
    m_decoder(0),
    m_dSampleNo(0),
    m_dSampleLen(0),
    m_state(-1),
    m_time(),
    m_outOffset(0),
    m_outLen(0),
    m_out(0),
    m_seqArray(new common::Array<tubyte,tubyte>()),
    m_sequence(0),
    m_alacContainer(0),
    m_alacDecoder(0),
    m_alacSequence(0),
    m_outputFormatType(e_SampleFloat)
{}

//-------------------------------------------------------------------------------------------

WhiteCodec::~WhiteCodec()
{
    try
    {
        WhiteCodec::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void WhiteCodec::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "WhiteCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::open(const QString& name)
{
    bool res = false;
    
    close();
    
    m_fileStream = new common::BIOBufferedStream(common::e_BIOStream_FileRead);

    if(m_fileStream->open(name))
    {
        m_file = new IOFile(m_fileStream);
        m_atom = new Atom;
        if(m_atom->readMP4File(m_file) && !m_atom->m_tracks.isEmpty())
        {
            Atom::Track *track = m_atom->m_tracks.first();
            
            if(track!=0)
            {
                if(track->m_type==Atom::Track::e_track_audio && track->m_decoderConfig!=0)
                {
                    QSharedPointer<common::Array<tubyte,tubyte> > mem(new common::Array<tubyte,tubyte>());
                    SequenceMemory::generateArray(track->m_decoderConfig,track->m_decoderConfigLen,mem);
                    WSequence gaSeq(mem,track->m_decoderConfigLen << 3);
                
                    m_atom->buildSampleTable(0);
                    if(m_gaConfig.readAudioSpecificConfig(&gaSeq))
                    {
                        m_decoder = new AACRoot;
                        m_decoder->setGAConfig(&m_gaConfig);
                        
                        m_out = new sample_t [1024 * 2];
                        m_outLen = 0;
                        m_outOffset = 0;
                        
                        m_dSampleNo = 0;
                        m_dSampleLen = m_atom->noSamples(0);
                        
                        m_state = 0;
                        m_time = 0;
                
                        res = true;
                    }
                    else
                    {
                        printError("open","Invalid general audio configuration");
                    }
                }
                else if(track->m_type==Atom::Track::e_track_alac)
                {
                    m_alacContainer = new ALACAtomContainer;
                    if(m_alacContainer->initWithAtom(m_atom))
                    {
                        m_alacDecoder = new redomega::ALACDecoder(m_alacContainer);
                        if(m_alacDecoder->init())
                        {
                            m_atom->buildSampleTable(0);
                            
                            m_out = new sample_t [m_alacContainer->description().framesPerPacket() * m_alacContainer->config().numChannels()];
                            m_outLen = 0;
                            m_outOffset = 0;
                                                    
                            m_dSampleNo = 0;
                            m_dSampleLen = m_atom->noSamples(0);
                        
                            m_state = 0;
                            m_time = 0;
                
                            res = true;                            
                        }
                        else
                        {
                            printError("open","Error initializing ALAC decoder");
                        }
                    }
                    else
                    {
                        printError("open","Could not initialize ALAC information");
                    }
                }
                else
                {
                    printError("open","Unsupported track type");
                }
            }
            else
            {
                printError("open","No tracks within M4A audio file");
            }
        }
        else
        {
            printError("open","Failed to read atomic M4A file structure");
        }
    }
    else
    {
        printError("open","Failed to open given stream file");
    }
    
    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void WhiteCodec::close()
{
    if(m_alacSequence!=0)
    {
        delete m_alacSequence;
        m_alacSequence = 0;
    }
    if(m_sequence!=0)
    {
        delete m_sequence;
        m_sequence = 0;
    }
    if(m_out!=0)
    {
        delete [] m_out;
        m_out = 0;
    }
    if(m_alacDecoder!=0)
    {
        delete m_alacDecoder;
        m_alacDecoder = 0;
    }
    if(m_alacContainer!=0)
    {
        delete m_alacContainer;
        m_alacContainer = 0;
    }
    if(m_decoder!=0)
    {
        delete m_decoder;
        m_decoder = 0;
    }
    if(m_atom!=0)
    {
        delete m_atom;
        m_atom = 0;
    }
    if(m_file!=0)
    {
        delete m_file;
        m_file = 0;
    }
    if(m_fileStream!=0)
    {
        m_fileStream->close();
        delete m_fileStream;
        m_fileStream = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::init()
{
    if(m_decoder!=0 || m_alacDecoder!=0)
    {
        m_initFlag = true;
        return true;
    }
    else
    {
        return false;
    }
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::next(AData& data)
{
    tint i,len;
    sample_t *buffer;
    bool res = true;
    engine::RData& rData = dynamic_cast<engine::RData&>(data);
    
    if(!rData.noParts())
    {
        data.start() = m_time;
    }
    
    if(m_state>=0)
    {
        engine::RData::Part *part = &(rData.nextPart());
        
        buffer = rData.partData(rData.noParts() - 1);
        part->start() = m_time;

        i = 0;
        len = rData.rLength();

        while(i<len && res)
        {
            switch(m_state)
            {
                case 0:
                    {
                        common::Array<tubyte,tubyte> mem;
                        
                        if(m_alacSequence!=0)
                        {
                            delete m_alacSequence;
                            m_alacSequence = 0;
                        }
                        if(m_sequence!=0)
                        {
                            delete m_sequence;
                            m_sequence = 0;
                        }
                        
                        if(m_dSampleNo < m_dSampleLen)
                        {
                            if(m_atom->readSample(m_file,0,m_dSampleNo,mem))
                            {
                                engine::SequenceMemory::generateArray(mem.GetData(),mem.GetSize(),m_seqArray);
                                m_dSampleNo++;
                                if(m_decoder!=0)
                                {
                                    m_sequence = new WSequence(m_seqArray);
                                    m_state = 1;
                                }
                                else if(m_alacDecoder!=0)
                                {
                                    m_alacSequence = new redomega::ALACSequence(m_seqArray);
                                    m_state = 1;
                                }
                                else
                                {
                                    m_state = -1;
                                    res = false;
                                }
                            }
                            else
                            {
                                res = false;
                            }
                        }
                        else
                        {
                            part->end() = m_time;
                            part->done() = true;
                            m_state = -1;
                            res = false;
                        }
                    }
                    break;
                    
                case 1:
                    {
                        tint decoderRes;
                        
                        m_outOffset = 0;
                        m_outLen = 0;
                        
                        if(m_decoder!=0)
                        {
                            decoderRes = m_decoder->read(m_sequence,m_out,m_outLen);
                            if(decoderRes>=0)
                            {
                                if(decoderRes==0)
                                {
                                    m_state = 2;
                                }
                                else
                                {
                                    m_state = 0;
                                }
                            }
                            else
                            {
                                res = false;
                            }
                        }
                        else if(m_alacDecoder!=0)
                        {
                            m_outLen = m_alacDecoder->decode(m_alacSequence, m_out, m_alacContainer->description().framesPerPacket(), m_outputFormatType);
                            if(m_outLen>=0)
                            {
                                m_outLen /= m_alacContainer->config().numChannels();
                                m_state = 2;
                            }
                            else
                            {
                                res = false;
                            }
                        }
                        else
                        {
                            res = false;
                        }
                    }
                    break;
                    
                case 2:
                    {
                        tint amount,noCh = noChannels();

                        amount = len - i;
                        if(amount > (m_outLen - m_outOffset))
                        {
                            amount = m_outLen - m_outOffset;
                        }
                        if(amount > 0)
                        {
                            if(m_decoder!=0)
                            {
                                memcpy(&buffer[i * noCh],&m_out[m_outOffset * noCh],amount * noCh * sizeof(sample_t));
                                sortChannels<sample_t>(&buffer[i * noCh],amount,noCh);
                            }
                            else if(m_alacDecoder!=0)
                            {
                                if(m_outputFormatType & e_SampleInt16)
                                {
                                    memcpy(sampleInt16AtOffset(buffer, i * noCh),sampleInt16AtOffset(m_out, m_outOffset * noCh),amount * noCh * sizeof(tint16));        
                                    sortChannels<tint16>(sampleInt16AtOffset(buffer, i * noCh),amount,noCh);
                                }
                                else if((m_outputFormatType & e_SampleInt24) || (m_outputFormatType & e_SampleInt32))
                                {
                                    memcpy(sampleInt24AtOffset(buffer, i * noCh),sampleInt24AtOffset(m_out, m_outOffset * noCh),amount * noCh * sizeof(tint32));        
                                    sortChannels<tint32>(sampleInt24AtOffset(buffer, i * noCh),amount,noCh);
                                }
                                else
                                {
                                    memcpy(&buffer[i * noCh],&m_out[m_outOffset * noCh],amount * noCh * sizeof(sample_t));
                                    sortChannels<sample_t>(&buffer[i * noCh],amount,noCh);
                                }
                            }
                            m_outOffset += amount;
                            i += amount;
                            m_time += static_cast<tfloat64>(amount) / static_cast<tfloat64>(frequency());
                        }

                        if(m_outOffset >= m_outLen)
                        {
                            if(m_alacDecoder!=0)
                            {
                                m_state = 0;
                            }
                            else
                            {
                                m_state = 1;
                            }
                        }
                    }
                    break;
            }
        }

        part->length() = i;
        part->end() = m_time;
        part->done() = true;
        setPartDataType(*part);
        
        data.end() = m_time;
    }
    else
    {
        res = false;
    }

    return res;
}

//-------------------------------------------------------------------------------------------
// Type A = 0.Center
// Type B = 0.F-Left, 1.F-Right
// Type C = 0.F-Left, 1.F-Right, 2.Center
// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
//-------------------------------------------------------------------------------------------

template<class S> void WhiteCodec::sortChannels(S *buffer, tint amount, tint noChs)
{
    tint i;
    S *x = buffer;
    
    switch(noChs)
    {
        case 3:
            {
                for(i=0;i<amount;i++)
                {
                    S fC,fL,fR;
                    fC = x[0];
                    fL = x[1];
                    fR = x[2];
                    x[0] = fL;
                    x[1] = fR;
                    x[2] = fC;
                    x += noChs;
                }
            }
            break;
            
        case 4:
            {
                for(i=0;i<amount;i++)
                {
                    S fC,fL,fR,bC;
                    fC = x[0];
                    fL = x[1];
                    fR = x[2];
                    bC = x[3];
                    x[0] = fL;
                    x[1] = fR;
                    x[2] = fC;
                    x[3] = bC;
                    x += noChs;
                }
            }
            break;
            
        case 5:
            {
                for(i=0;i<amount;i++)
                {
                    S fC,fL,fR,bL,bR;
                    fC = x[0];
                    fL = x[1];
                    fR = x[2];
                    bL = x[3];
                    bR = x[4];
                    x[0] = fL;
                    x[1] = fR;
                    x[2] = fC;
                    x[3] = bL;
                    x[4] = bR;
                    x += noChs;
                }
            }
            break;
            
        case 6:
            {
                for(i=0;i<amount;i++)
                {
                    S fC,fL,fR,bL,bR,lF;
                    fC = x[0];
                    fL = x[1];
                    fR = x[2];
                    bL = x[3];
                    bR = x[4];
                    lF = x[5];
                    x[0] = fL;
                    x[1] = fR;
                    x[2] = fC;
                    x[3] = lF;
                    x[4] = bL;
                    x[5] = bR;
                    x += noChs;
                }
            }
            break;
            
        case 8:
            {
                for(i=0;i<amount;i++)
                {
                    S fC,fL,fR,sL,sR,bL,bR,lF;
                    fC = x[0];
                    fL = x[1];
                    fR = x[2];
                    sL = x[3];
                    sR = x[4];
                    bL = x[5];
                    bR = x[6];
                    lF = x[7];
                    x[0] = fL;
                    x[1] = fR;
                    x[2] = fC;
                    x[3] = lF;
                    x[4] = sL;
                    x[5] = sR;
                    x[6] = bL;
                    x[7] = bR;
                    x += noChs;
                }
            }
            break;
            
        default:
            break;
    }
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::isSeek() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::seek(const common::TimeStamp& v)
{
    tint sampleIdx;
    common::TimeStamp seekT(v);
    bool res = false;
    
    if(m_atom!=0)
    {
        sampleIdx = m_atom->seekSamplePosition(0,seekT);
        if(sampleIdx>=0)
        {
            m_state = 0;
            m_dSampleNo = sampleIdx;
            m_time = seekT;
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::isComplete() const
{
    return (m_dSampleNo < m_dSampleLen) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::isRemote() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::isBuffered(tfloat32& percent)
{
    return false;
}

//-------------------------------------------------------------------------------------------

tint WhiteCodec::frequency() const
{
    if(m_alacContainer!=0)
    {
        return m_alacContainer->config().sampleRate();
    }
    else
    {
        return m_gaConfig.m_samplingFrequency;
    }
}

//-------------------------------------------------------------------------------------------

tint WhiteCodec::noChannels() const
{
    if(m_alacContainer!=0)
    {
        return m_alacContainer->config().numChannels();
    }
    else
    {
        return 2;
    }
}

//-------------------------------------------------------------------------------------------

common::TimeStamp WhiteCodec::length() const
{
    common::TimeStamp t;
    
    if(m_atom!=0)
    {
        Atom::Track *track = m_atom->m_tracks.first();
        
        if(track!=0)
        {
            t = static_cast<tfloat64>(track->m_duration) / static_cast<tfloat64>(track->m_timeScale);
        }
    }
    return t;
}

//-------------------------------------------------------------------------------------------

tint WhiteCodec::bitrate() const
{
    tint rate = 0;
    
    if(m_atom!=0)
    {
        Atom::Track *track = m_atom->m_tracks.first();
        if(track!=0)
        {
            if(m_alacContainer!=0)
            {
                rate = track->m_alacAvgBitRate;
            }
            else
            {
                rate = track->m_avgBitrate;
            }
        }
    }
    return rate;
}

//-------------------------------------------------------------------------------------------

CodecDataType WhiteCodec::dataTypesSupported() const
{
    CodecDataType types = e_SampleFloat;
    
    if(m_alacContainer != 0)
    {
        switch(m_alacContainer->config().bitDepth())
        {
            case 16:
                types |= e_SampleInt16;
                break;
            case 20:
            case 24:
                types |= e_SampleInt24;
                break;
            case 32:
                types |= e_SampleInt32;
                break;
            default:
                break;
        }
    }
    return types;
}

//-------------------------------------------------------------------------------------------

bool WhiteCodec::setDataTypeFormat(CodecDataType type)
{
    bool res;
    CodecDataType caps;
    
    caps = dataTypesSupported();
    if((type == e_SampleInt16 && (caps & e_SampleInt16)) || (type == e_SampleInt24 && (caps & e_SampleInt24)) || (type == e_SampleInt32 && (caps & e_SampleInt32)))
    {
        m_outputFormatType = type;
        res = true;
    }
    else
    {
        res = Codec::setDataTypeFormat(type);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void WhiteCodec::setPartDataType(RData::Part& part)
{
    CodecDataType type;
    
    if((m_outputFormatType & e_SampleInt16) && (dataTypesSupported() & e_SampleInt16))
    {
        type = e_SampleInt16;
    }
    else if((m_outputFormatType & e_SampleInt24) && (dataTypesSupported() & e_SampleInt24))
    {
        type = e_SampleInt24;
    }
    else if((m_outputFormatType & e_SampleInt32) && (dataTypesSupported() & e_SampleInt32))
    {
        type = e_SampleInt32;
    }
    else
    {
        type = e_SampleFloat;
    }
    part.setDataType(type);
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
