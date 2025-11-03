#include "engine/greenomega/inc/GreenCodec.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------
// GreenCodecInitialize
//-------------------------------------------------------------------------------------------

GreenCodecInitialize *GreenCodecInitialize::m_instance = 0;

//-------------------------------------------------------------------------------------------

GreenCodecInitialize::GreenCodecInitialize()
{}

//-------------------------------------------------------------------------------------------

GreenCodecInitialize::~GreenCodecInitialize()
{}

//-------------------------------------------------------------------------------------------

void GreenCodecInitialize::start()
{
    if(m_instance==0)
    {
        m_instance = new GreenCodecInitialize();
    }
}

//-------------------------------------------------------------------------------------------

void GreenCodecInitialize::end()
{
    if(m_instance!=0)
    {
        delete m_instance;
        m_instance = 0;
    }
}

//-------------------------------------------------------------------------------------------
// GreenCodec
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(CodecFactory,Codec, \
                            GreenCodecFactory,GreenCodec, \
                            "flac",false)

//-------------------------------------------------------------------------------------------

GreenCodec::GreenCodec(QObject *parent) : Codec(Codec::e_codecFlac,parent),
    m_framework(0),
    m_frame(0),
    m_state(-1),
    m_time(),
    m_outOffset(0),
    m_outLen(0),
    m_cachedFile(0),
    m_file(0)
{}

//-------------------------------------------------------------------------------------------

GreenCodec::~GreenCodec()
{
    try
    {
        GreenCodec::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void GreenCodec::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "GreenCodec::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::open(const QString& name)
{
    bool res = false;

    close();
    if(getFileExtension(name).toLower()=="flac")
    {
        m_cachedFile = new common::BIOBufferedStream(common::e_BIOStream_FileRead);
        m_file = new engine::IOFile(m_cachedFile);

        m_framework = new FLACFramework;
        if(m_file->open(name))
        {
            if(m_framework->init(m_file))
            {
                res = true;
            }
            else
            {
                printError("open","Failed to open and read metadata from flac file");
            }
        }
        else
        {
            printError("open","Failed to open flac file");
        }
    }
    else
    {
        printError("open","Given file type not supported by codec");
    }

    if(!res)
    {
        close();
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void GreenCodec::close()
{
    if(m_frame!=0)
    {
        delete m_frame;
        m_frame = 0;
    }
    if(m_framework!=0)
    {
        delete m_framework;
        m_framework = 0;
    }
    if(m_file!=0)
    {
        m_file->close();
        delete m_file;
        m_file = 0;
    }
    if(m_cachedFile!=0)
    {
        m_cachedFile->close();
        delete m_cachedFile;
        m_cachedFile = 0;
    }
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::init()
{
    m_frame = new FLACFrame(m_framework->streamInfo());
    m_state = 0;
    m_initFlag = true;
    return true;
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::next(AData& data)
{
    tint i,len;
    sample_t *buffer;
    tint16 *bufferInt16;
    tint32 *bufferInt32;
    bool res = true;
    CodecDataType dType = m_frame->dataType();
    engine::RData& rData = dynamic_cast<engine::RData&>(data);

    if(!rData.noParts())
    {
        data.start() = m_time;
    }

    if(m_state>=0)
    {
        engine::RData::Part *part = &(rData.nextPart());

        buffer = rData.partData(rData.noParts() - 1);
        if(dType & e_SampleInt16)
        {
            bufferInt16 = reinterpret_cast<tint16 *>(buffer);
        }
        else if((dType & e_SampleInt24) || (dType & e_SampleInt32))
        {
            bufferInt32 = reinterpret_cast<tint32 *>(buffer);
        }
        part->start() = m_time;

        i = 0;
        len = rData.rLength();

        while(i<len && res)
        {
            switch(m_state)
            {
                case 0:
                    {
                        if(m_frame->read(m_framework->sequence()))
                        {
                            m_state = 1;
                            m_outOffset = 0;
                            m_outLen = m_frame->length();
                            m_time = m_frame->start();
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
                        tint amount;

                        amount = len - i;
                        if(amount > (m_outLen - m_outOffset))
                        {
                            amount = m_outLen - m_outOffset;
                        }
                        if(amount > 0)
                        {
                            tint j,k,noChs = m_frame->noChannels();

                            if(dType & e_SampleInt16)
                            {
                                const tint16 *out[8];

                                for(k=0;k<noChs;k++)
                                {
                                    out[k] = m_frame->outputInt16(k);
                                }
                                for(j=0;j<amount;j++,m_outOffset++)
                                {
                                    for(k=0;k<noChs;k++)
                                    {
                                        const tint16 *oC = out[k];
                                        *bufferInt16++ = oC[m_outOffset];
                                    }
                                }
                            }
                            else if((dType & e_SampleInt24) || (dType & e_SampleInt32))
                            {
                                const tint32 *out[8];

                                for(k=0;k<noChs;k++)
                                {
                                    out[k] = m_frame->outputInt32(k);
                                }
                                for(j=0;j<amount;j++,m_outOffset++)
                                {
                                    for(k=0;k<noChs;k++)
                                    {
                                        const tint32 *oC = out[k];
                                        *bufferInt32++ = oC[m_outOffset];
                                    }
                                }
                            }
                            else
                            {
                                const sample_t *out[8];

                                for(k=0;k<noChs;k++)
                                {
                                    out[k] = m_frame->output(k);
                                }
                                for(j=0;j<amount;j++,m_outOffset++)
                                {
                                    for(k=0;k<noChs;k++)
                                    {
                                        const sample_t *oC = out[k];
                                        *buffer++ = oC[m_outOffset];
                                    }
                                }
                            }
                            i += amount;
                            m_time += static_cast<tfloat64>(amount) / static_cast<tfloat64>(frequency());
                        }

                        if(m_outOffset >= m_outLen)
                        {
                            m_state = 0;
                        }
                    }
                    break;
            }
        }

        part->length() = i;
        part->end() = m_time;
        part->done() = true;
        data.end() = m_time;
        setPartDataType(*part);
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::isSeek() const
{
    return true;
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::seek(const common::TimeStamp& v)
{
    bool res = false;

    if(m_frame!=0 && m_state>=0)
    {
        common::TimeStamp seekT(v);

        if(m_frame->seek(m_framework,seekT))
        {
            m_state = 0;
            m_time = seekT;
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::isComplete() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::isRemote() const
{
    return false;
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::isBuffered(tfloat32&)
{
    return false;
}

//-------------------------------------------------------------------------------------------

tint GreenCodec::frequency() const
{
    if(m_framework!=0)
    {
        return m_framework->streamInfo()->m_frequency;
    }
    else
    {
        return 44100;
    }
}

//-------------------------------------------------------------------------------------------

tint GreenCodec::noChannels() const
{
    if(m_framework!=0)
    {
        return m_framework->streamInfo()->m_channels;
    }
    else
    {
        return 2;
    }
}

//-------------------------------------------------------------------------------------------

common::TimeStamp GreenCodec::length() const
{
    common::TimeStamp t;

    if(m_framework!=0)
    {
        t = m_framework->streamInfo()->length();
    }
    return t;
}

//-------------------------------------------------------------------------------------------

tint GreenCodec::bitrate() const
{
    return (m_framework!=0) ? m_framework->bitrate() : 0;
}

//-------------------------------------------------------------------------------------------

CodecDataType GreenCodec::dataTypesSupported() const
{
    CodecDataType types = e_SampleFloat;

    if(m_framework != 0 && m_framework->streamInfo() != 0)
    {
        FLACMetaStreamInfo *info = m_framework->streamInfo();

        if(info->m_bitsPerSample <= 16)
        {
            types |= e_SampleInt16;
        }
        else if(info->m_bitsPerSample <= 24)
        {
            types |= e_SampleInt24;
        }
        else
        {
            types |= e_SampleInt32;
        }
    }
    return types;
}

//-------------------------------------------------------------------------------------------

bool GreenCodec::setDataTypeFormat(CodecDataType type)
{
    bool res;

    if(m_frame != 0)
    {
        CodecDataType caps;

        caps = dataTypesSupported();
        if((type == e_SampleInt16 && (caps & e_SampleInt16)) || (type == e_SampleInt24 && (caps & e_SampleInt24)) || (type == e_SampleInt32 && (caps & e_SampleInt32)))
        {
            m_frame->setDataTypeFormat(type);
            res = true;
        }
        else
        {
            res = Codec::setDataTypeFormat(type);
        }
    }
    else
    {
        res = Codec::setDataTypeFormat(type);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void GreenCodec::setPartDataType(RData::Part& part)
{
    CodecDataType type = e_SampleFloat;

    if(m_frame != 0)
    {
        if((m_frame->dataType() & e_SampleInt16) && (dataTypesSupported() & e_SampleInt16))
        {
            type = e_SampleInt16;
        }
        else if((m_frame->dataType() & e_SampleInt24) && (dataTypesSupported() & e_SampleInt24))
        {
            type = e_SampleInt24;
        }
        else if((m_frame->dataType() & e_SampleInt32) && (dataTypesSupported() & e_SampleInt32))
        {
            type = e_SampleInt32;
        }
    }
    part.setDataType(type);
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
