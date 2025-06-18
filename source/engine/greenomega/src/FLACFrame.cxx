#include "engine/greenomega/inc/FLACFrame.h"
#if defined(OMEGA_FLAC_COMPARE)
#include "engine/inc/Compare.h"
#endif
#include "engine/inc/FormatTypeFromFloat.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

FLACFrame::FLACFrame(FLACMetaStreamInfo *info) : m_streamInfo(info),
    m_header(info),
    m_outSize(0),
    m_outSizeInt16(0),
    m_outSizeInt32(0),
    m_timeStart(),
    m_timeEnd(),
    m_count(0),
    m_outputFormatType(e_SampleFloat)
{
    ::memset(m_subframe,0,8 * sizeof(FLACSubframe *));
    ::memset(m_out,0,8 * sizeof(sample_t *));
    ::memset(m_outInt16, 0, 8 * sizeof(tint16 *));
    ::memset(m_outInt32, 0, 8 * sizeof(tint32 *));
}

//-------------------------------------------------------------------------------------------

FLACFrame::~FLACFrame()
{
    tint i;
    
    for(i=0;i<8;i++)
    {
        if(m_subframe[i]!=0)
        {
            delete m_subframe[i];
            m_subframe[i] = 0;
        }
        if(m_out[i]!=0)
        {
            delete [] m_out[i];
            m_out[i] = 0;
        }
        if(m_outInt16[i]!=0)
        {
            delete [] m_outInt16[i];
            m_outInt16[i] = 0;
        }
        if(m_outInt32[i]!=0)
        {
            delete [] m_outInt32[i];
            m_outInt32[i] = 0;
        }
    }
}

//-------------------------------------------------------------------------------------------

bool FLACFrame::read(Sequence *seq)
{
    tint ch;
    bool loop = true;
    
    while(loop)
    {
        loop = false;
        
        if(!m_header.read(seq))
        {
            return false;
        }
        if(!allocate())
        {
            return false;
        }

        m_count++;
    
        for(ch=0;ch<m_header.channels();ch++)
        {
            tint bps = m_header.bitsPerSample();
            
            switch(m_header.channelAssignment())
            {
                case FLACHeader::e_RightSide:
                    if(ch==0)
                    {
                        bps++;
                    }
                    break;
                
                case FLACHeader::e_LeftSide:
                case FLACHeader::e_MidSide:
                    if(ch==1)
                    {
                        bps++;
                    }
                    break;
                
                default:
                    break;
            }
            
            if(!m_subframe[ch]->read(seq,bps))
            {
                loop = true;
                break;
            }
        }
        seq->byteAlignment();
        
        if(!loop)
        {
            if(crc(seq))
            {
                tint i,*outL,*outR;
                
                switch(m_header.channelAssignment())
                {
                    case FLACHeader::e_LeftSide:
                        {
                            outL = m_subframe[0]->output();
                            outR = m_subframe[1]->output();
                            
                            for(i=0;i<m_header.blockSize();i++)
                            {
                                outR[i] = outL[i] - outR[i];
                            }
                        }
                        break;
                        
                    case FLACHeader::e_RightSide:
                        {
                            outL = m_subframe[0]->output();
                            outR = m_subframe[1]->output();
                            
                            for(i=0;i<m_header.blockSize();i++)
                            {
                                outL[i] += outR[i];
                            }
                        }
                        break;
                        
                    case FLACHeader::e_MidSide:
                        {
                            tint mid,side;
                            
                            outL = m_subframe[0]->output();
                            outR = m_subframe[1]->output();
                            
                            for(i=0;i<m_header.blockSize();i++)
                            {
                                mid = outL[i];
                                side = outR[i];
                                mid <<= 1;
                                mid |= (side & 1);
                                outL[i] = (mid + side) >> 1;
                                outR[i] = (mid - side) >> 1;
                            }
                        }
                        break;
                        
                    default:
                        break;
                }
            }
            else
            {
                loop = true;
            }
        }
    }
    processOutput();
    return true;
}

//-------------------------------------------------------------------------------------------

bool FLACFrame::allocate()
{
    tint i;
    
    for(i=0;i<m_header.channels();i++)
    {
        if(m_subframe[i]==0)
        {
            m_subframe[i] = new FLACSubframe(m_streamInfo,&m_header,i);
        }
        if(!m_subframe[i]->allocate())
        {
            return false;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool FLACFrame::crc(Sequence *seq)
{
    static const tuint crcTable[256] = {
        0x0000,  0x8005,  0x800f,  0x000a,  0x801b,  0x001e,  0x0014,  0x8011,
        0x8033,  0x0036,  0x003c,  0x8039,  0x0028,  0x802d,  0x8027,  0x0022,
        0x8063,  0x0066,  0x006c,  0x8069,  0x0078,  0x807d,  0x8077,  0x0072,
        0x0050,  0x8055,  0x805f,  0x005a,  0x804b,  0x004e,  0x0044,  0x8041,
        0x80c3,  0x00c6,  0x00cc,  0x80c9,  0x00d8,  0x80dd,  0x80d7,  0x00d2,
        0x00f0,  0x80f5,  0x80ff,  0x00fa,  0x80eb,  0x00ee,  0x00e4,  0x80e1,
        0x00a0,  0x80a5,  0x80af,  0x00aa,  0x80bb,  0x00be,  0x00b4,  0x80b1,
        0x8093,  0x0096,  0x009c,  0x8099,  0x0088,  0x808d,  0x8087,  0x0082,
        0x8183,  0x0186,  0x018c,  0x8189,  0x0198,  0x819d,  0x8197,  0x0192,
        0x01b0,  0x81b5,  0x81bf,  0x01ba,  0x81ab,  0x01ae,  0x01a4,  0x81a1,
        0x01e0,  0x81e5,  0x81ef,  0x01ea,  0x81fb,  0x01fe,  0x01f4,  0x81f1,
        0x81d3,  0x01d6,  0x01dc,  0x81d9,  0x01c8,  0x81cd,  0x81c7,  0x01c2,
        0x0140,  0x8145,  0x814f,  0x014a,  0x815b,  0x015e,  0x0154,  0x8151,
        0x8173,  0x0176,  0x017c,  0x8179,  0x0168,  0x816d,  0x8167,  0x0162,
        0x8123,  0x0126,  0x012c,  0x8129,  0x0138,  0x813d,  0x8137,  0x0132,
        0x0110,  0x8115,  0x811f,  0x011a,  0x810b,  0x010e,  0x0104,  0x8101,
        0x8303,  0x0306,  0x030c,  0x8309,  0x0318,  0x831d,  0x8317,  0x0312,
        0x0330,  0x8335,  0x833f,  0x033a,  0x832b,  0x032e,  0x0324,  0x8321,
        0x0360,  0x8365,  0x836f,  0x036a,  0x837b,  0x037e,  0x0374,  0x8371,
        0x8353,  0x0356,  0x035c,  0x8359,  0x0348,  0x834d,  0x8347,  0x0342,
        0x03c0,  0x83c5,  0x83cf,  0x03ca,  0x83db,  0x03de,  0x03d4,  0x83d1,
        0x83f3,  0x03f6,  0x03fc,  0x83f9,  0x03e8,  0x83ed,  0x83e7,  0x03e2,
        0x83a3,  0x03a6,  0x03ac,  0x83a9,  0x03b8,  0x83bd,  0x83b7,  0x03b2,
        0x0390,  0x8395,  0x839f,  0x039a,  0x838b,  0x038e,  0x0384,  0x8381,
        0x0280,  0x8285,  0x828f,  0x028a,  0x829b,  0x029e,  0x0294,  0x8291,
        0x82b3,  0x02b6,  0x02bc,  0x82b9,  0x02a8,  0x82ad,  0x82a7,  0x02a2,
        0x82e3,  0x02e6,  0x02ec,  0x82e9,  0x02f8,  0x82fd,  0x82f7,  0x02f2,
        0x02d0,  0x82d5,  0x82df,  0x02da,  0x82cb,  0x02ce,  0x02c4,  0x82c1,
        0x8243,  0x0246,  0x024c,  0x8249,  0x0258,  0x825d,  0x8257,  0x0252,
        0x0270,  0x8275,  0x827f,  0x027a,  0x826b,  0x026e,  0x0264,  0x8261,
        0x0220,  0x8225,  0x822f,  0x022a,  0x823b,  0x023e,  0x0234,  0x8231,
        0x8213,  0x0216,  0x021c,  0x8219,  0x0208,  0x820d,  0x8207,  0x0202
    };
    
    tint bkEnd,len;
    tuint crcV = 0,crcE;
    common::Array<tubyte,tubyte> arr;
    
    bkEnd = seq->bookmark();
    if(bkEnd<0)
    {
        return false;
    }
    if(!seq->readBookmarkMemory(m_header.bookmarkStart(),bkEnd,arr))
    {
        return false;
    }
    
    const tubyte *mem = arr.GetData();
    len = arr.GetSize();
    
    while(len--)
    {
        crcV = ((crcV<<8) ^ crcTable[(crcV>>8) ^ *mem++]) & 0xffff;
    }
    crcE = static_cast<tuint>(seq->readBitsI(16));
    seq->bitstream()->complete(bkEnd);

    return (crcV==crcE) ? true : false;
}

//-------------------------------------------------------------------------------------------

void FLACFrame::processOutput()
{
    tint size = m_header.blockSize();
    
    if(m_outputFormatType & e_SampleFloat)
    {
        processOutputFloat(size);
    }
    else if(m_outputFormatType & e_SampleInt16)
    {
        processOutputInt16(size);
    }
    else if(m_outputFormatType & e_SampleInt24)
    {
        processOutputInt24(size);
    }
    else if(m_outputFormatType & e_SampleInt32)
    {
        processOutputInt32(size);
    }
    
    tfloat64 rate = static_cast<tfloat64>(m_header.sampleRate());
    tfloat64 sT = static_cast<tfloat64>(m_header.sampleNumber()) / rate;
    tfloat64 eT = sT + (static_cast<tfloat64>(size) / rate);
    m_timeStart = sT;
    m_timeEnd = eT;
}

//-------------------------------------------------------------------------------------------

void FLACFrame::processOutputFloat(tint size)
{
    tint i,j;
    SampleFromInteger s(m_header.bitsPerSample());

#if defined(OMEGA_FLAC_COMPARE)
    engine::Compare *comp = &g_Compare;

    if(comp->isThreadA())
    {
        tint frame = comp->frameA();
        common::Log::g_Log.print("greenomega buffer0 - %d\n",frame);
        comp->compareA(m_subframe[0]->output(),size);
        frame = comp->frameA();
    }
    else
    {
        tint frame = comp->frameB();
        common::Log::g_Log.print("greenomega buffer0 - %d\n",frame);
        comp->compareB(m_subframe[0]->output(),size);
        frame = comp->frameB();        
    }

    if(comp->isThreadA())
    {
        tint frame = comp->frameA();
        common::Log::g_Log.print("greenomega buffer1 - %d\n",frame);
        comp->compareA(m_subframe[1]->output(),size);
        frame = comp->frameA();
    }
    else
    {
        tint frame = comp->frameB();
        common::Log::g_Log.print("greenomega buffer1 - %d\n",frame);
        comp->compareB(m_subframe[1]->output(),size);
        frame = comp->frameB();        
    }
#endif

    if(size>m_outSize)
    {
        for(i=0;i<m_header.channels() || i<2;i++)
        {
            if(m_out[i]!=0)
            {
                delete [] m_out[i];
            }
            m_out[i] = new sample_t [size];
        }
        m_outSize = size;
    }
    
    if(m_header.channels()==1)
    {
        tint *in = m_subframe[0]->output();
        
        
        sample_t *outL = m_out[0], *outR = m_out[1];
        
        for(i=0;i<size;i++)
        {
            outL[i] = outR[i] = s.convert(in[i]);
        }
    }
    else if(m_header.channels()==2)
    {
        tint *inL = m_subframe[0]->output();
        tint *inR = m_subframe[1]->output();
        sample_t *outL = m_out[0], *outR = m_out[1];
        
        for(i=0;i<size;i++)
        {
            outL[i] = s.convert(inL[i]);
            outR[i] = s.convert(inR[i]);
        }
    }
    else
    {
        for(j=0;j<m_header.channels();j++)
        {
            tint *in = m_subframe[j]->output();
            sample_t *out = m_out[j];
            
            for(i=0;i<size;i++)
            {
                out[i] = s.convert(in[i]);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void FLACFrame::processOutputInt16(tint size)
{
    tint i,j;
    SampleFromInteger s(m_header.bitsPerSample());

    if(size > m_outSizeInt16)
    {
        for(i=0;i<m_header.channels() || i<2;i++)
        {
            if(m_outInt16[i]!=0)
            {
                delete [] m_outInt16[i];
            }
            m_outInt16[i] = new tint16 [size];
        }
        m_outSizeInt16 = size;
    }
    
    if(m_header.channels()==1)
    {
        tint *in = m_subframe[0]->output();
        tint16 *outL = m_outInt16[0], *outR = m_outInt16[1];
        
        for(i=0;i<size;i++)
        {
            outL[i] = outR[i] = s.convertInt16(in[i]);
        }
    }
    else if(m_header.channels()==2)
    {
        tint *inL = m_subframe[0]->output();
        tint *inR = m_subframe[1]->output();
        tint16 *outL = m_outInt16[0], *outR = m_outInt16[1];
        
        for(i=0;i<size;i++)
        {
            outL[i] = s.convertInt16(inL[i]);
            outR[i] = s.convertInt16(inR[i]);
        }
    }
    else
    {
        for(j=0;j<m_header.channels();j++)
        {
            tint *in = m_subframe[j]->output();
            tint16 *out = m_outInt16[j];
            
            for(i=0;i<size;i++)
            {
                out[i] = s.convertInt16(in[i]);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void FLACFrame::processOutputInt24(tint size)
{
    tint i,j;
    SampleFromInteger s(m_header.bitsPerSample());

    if(size > m_outSizeInt32)
    {
        for(i=0;i<m_header.channels() || i<2;i++)
        {
            if(m_outInt32[i]!=0)
            {
                delete [] m_outInt32[i];
            }
            m_outInt32[i] = new tint32 [size];
        }
        m_outSizeInt32 = size;
    }
    
    if(m_header.channels()==1)
    {
        tint *in = m_subframe[0]->output();
        tint32 *outL = m_outInt32[0], *outR = m_outInt32[1];
        
        for(i=0;i<size;i++)
        {
            outL[i] = outR[i] = s.convertInt24(in[i]);
        }
    }
    else if(m_header.channels()==2)
    {
        tint *inL = m_subframe[0]->output();
        tint *inR = m_subframe[1]->output();
        tint32 *outL = m_outInt32[0], *outR = m_outInt32[1];
        
        for(i=0;i<size;i++)
        {
            outL[i] = s.convertInt24(inL[i]);
            outR[i] = s.convertInt24(inR[i]);
        }
    }
    else
    {
        for(j=0;j<m_header.channels();j++)
        {
            tint *in = m_subframe[j]->output();
            tint32 *out = m_outInt32[j];
            
            for(i=0;i<size;i++)
            {
                out[i] = s.convertInt24(in[i]);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void FLACFrame::processOutputInt32(tint size)
{
    tint i,j;
    SampleFromInteger s(m_header.bitsPerSample());

    if(size > m_outSizeInt32)
    {
        for(i=0;i<m_header.channels() || i<2;i++)
        {
            if(m_outInt32[i]!=0)
            {
                delete [] m_outInt32[i];
            }
            m_outInt32[i] = new tint32 [size];
        }
        m_outSizeInt32 = size;
    }
    
    if(m_header.channels()==1)
    {
        tint *in = m_subframe[0]->output();
        tint32 *outL = m_outInt32[0], *outR = m_outInt32[1];
        
        for(i=0;i<size;i++)
        {
            outL[i] = outR[i] = s.convertInt32(in[i]);
        }
    }
    else if(m_header.channels()==2)
    {
        tint *inL = m_subframe[0]->output();
        tint *inR = m_subframe[1]->output();
        tint32 *outL = m_outInt32[0], *outR = m_outInt32[1];
        
        for(i=0;i<size;i++)
        {
            outL[i] = s.convertInt32(inL[i]);
            outR[i] = s.convertInt32(inR[i]);
        }
    }
    else
    {
        for(j=0;j<m_header.channels();j++)
        {
            tint *in = m_subframe[j]->output();
            tint32 *out = m_outInt32[j];
            
            for(i=0;i<size;i++)
            {
                out[i] = s.convertInt32(in[i]);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool FLACFrame::seek(FLACFramework *framework,common::TimeStamp& t)
{
    Bitstream *bs = framework->bitstream();
    FLACMetaStreamInfo *info = framework->streamInfo();
    tint lPos,hPos,mPos;
    tuint64 fSample;
    
    fSample = static_cast<tuint64>(static_cast<tfloat64>(t) * static_cast<tfloat64>(info->m_frequency));
    
    if(fSample>=info->m_totalSamples)
    {
        return false;
    }
    
    lPos = framework->streamOffset();
    if(framework->seekTable()!=0)
    {
        FLACMetaSeekTable::SeekPoint lPoint,hPoint;
        
        switch(framework->seekTable()->findPoints(fSample,lPoint,hPoint))
        {
            case 1:
                lPos += lPoint.offset();
                hPos = bs->file()->length();
                break;
            
            case 2:
                hPos = lPos + hPoint.offset();
                lPos += lPoint.offset();
                break;
            
            default:
                hPos = bs->file()->length();            
                break;
        }
    }
    else
    {
        hPos = bs->file()->length();
    }
    
    while(lPos < hPos)
    {
        mPos = lPos + ((hPos - lPos) / 2);
        
        if(bs->seek(mPos,File::e_startPosition))
        {
            Sequence *seq = bs->getSequence(0);
            
            if(seq!=0)
            {
                FLACHeader hdr(info);

                if(hdr.read(seq))
                {
                    if(fSample>=hdr.sampleNumber() && fSample<(hdr.sampleNumber() + static_cast<tuint64>(hdr.blockSize())))
                    {
                        if(seq->move(hdr.bookmarkStart()))
                        {
                            t = static_cast<tfloat64>(hdr.sampleNumber()) / static_cast<tfloat64>(info->m_frequency);
                            return true;
                        }
                        else
                        {
                            return false;
                        }
                    }
                    
                    if(hdr.sampleNumber() < fSample)
                    {
                        lPos = mPos + 1;
                    }
                    else
                    {
                        hPos = mPos;
                    }
                }
                else
                {
                    return false;
                }
            }
            else
            {
                return false;
            }
        }
        else
        {
            return false;
        }
    }
    
    if(bs->seek(lPos,File::e_startPosition))
    {
        Sequence *seq = bs->getSequence(0);
        
        if(seq!=0)
        {
            FLACHeader hdr(info);
            
            if(hdr.read(seq))
            {
                if(seq->move(hdr.bookmarkStart()))
                {
                    t = static_cast<tfloat64>(hdr.sampleNumber()) / static_cast<tfloat64>(info->m_frequency);
                    return true;
                }
            }
        }
    }
    return false;
}

//-------------------------------------------------------------------------------------------

void FLACFrame::setDataTypeFormat(CodecDataType type)
{
    m_outputFormatType = type;
}

//-------------------------------------------------------------------------------------------

CodecDataType FLACFrame::dataType() const
{
    return m_outputFormatType;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
