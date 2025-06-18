#include "engine/greenomega/inc/FLACHeader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-----------------------------------------------------------------------------------------

FLACHeader::FLACHeader(FLACMetaStreamInfo *sInfo) : m_streamInfo(sInfo),
    m_blockSize(0),
    m_sampleRate(44100),
    m_channels(2),
    m_channelAssignment(e_Independent),
    m_bitsPerSample(16),
    m_numberType(e_FrameNumber),
    m_frameNumber(0),
    m_sampleNumber(0),
    m_fixedBlockSize(0),
    m_nextFixedBlockSize(0),
    m_bkHeader(-1)
{}

//-----------------------------------------------------------------------------------------

FLACHeader::~FLACHeader()
{}

//-----------------------------------------------------------------------------------------

bool FLACHeader::read(Sequence *seq)
{
    tint x,bk,bkCount = 0;
    tubyte hMem[16];
    
    seq->byteAlignment();
    
    bk = seq->bookmark();
    if(bk<0)
    {
        return false;
    }
    hMem[1] = static_cast<tubyte>(seq->readBitsI(8));
    while(!seq->isEnd())
    {
        if(bkCount>4096)
        {
            seq->bitstream()->complete(bkCount);
            if(!seq->seek(-8))
            {
                return false;
            }
            bk = seq->bookmark();
            if(bk<0)
            {
                return false;
            }
            if(!seq->seek(8))
            {
                return false;
            }
            bkCount = 0;
        }
        bkCount++;
        
        hMem[0] = hMem[1];
        hMem[1] = static_cast<tubyte>(seq->readBitsI(8));
        if(hMem[0]==0xff && (hMem[1] & 0xfe)==0xf8)
        {
            bool hFlag = true;
            tint blockBitHint = 0;
            tint sampleRateHint = 0;
            
            if(!seq->seek(-16))
            {
                return false;
            }
            m_bkHeader = seq->bookmark();
            if(m_bkHeader<0)
            {
                return false;
            }
            if(!seq->seek(16))
            {
                return false;
            }
            
            hMem[2] = static_cast<tubyte>(seq->readBitsI(8));
            hMem[3] = static_cast<tubyte>(seq->readBitsI(8));
            if(hMem[2]!=0xff || hMem[3]!=0xff)
            {
                x = static_cast<tint>((hMem[2] >> 4) & 0x0f);
                switch(x)
                {
                    case 0:
                        hFlag = false;
                        break;
                        
                    case 1:
                        m_blockSize = 192;
                        break;
                        
                    case 2:
                    case 3:
                    case 4:
                    case 5:
                        m_blockSize = 576 << (x - 2);
                        break;
                        
                    case 6:
                    case 7:
                        blockBitHint = x;
                        break;
                        
                    case 8:
                    case 9:
                    case 10:
                    case 11:
                    case 12:
                    case 13:
                    case 14:
                    case 15:
                        m_blockSize = 256 << (x - 8);
                        break;
                        
                    default:
                        return false;
                }
                
                x = static_cast<tint>(hMem[2] & 0x0f);
                switch(x)
                {
                    case 0:
                        m_sampleRate = m_streamInfo->m_frequency;
                        break;
                        
                    case 1:
                        m_sampleRate = 88200;
                        break;
                        
                    case 2:
                        m_sampleRate = 176400;
                        break;
                        
                    case 3:
                        m_sampleRate = 192000;
                        break;
                        
                    case 4:
                        m_sampleRate = 8000;
                        break;
                        
                    case 5:
                        m_sampleRate = 16000;
                        break;
                        
                    case 6:
                        m_sampleRate = 22050;
                        break;
                        
                    case 7:
                        m_sampleRate = 24000;
                        break;
                        
                    case 8:
                        m_sampleRate = 32000;
                        break;
                        
                    case 9:
                        m_sampleRate = 44100;
                        break;
                        
                    case 10:
                        m_sampleRate = 48000;
                        break;
                        
                    case 11:
                        m_sampleRate = 96000;
                        break;
                        
                    case 12:
                    case 13:
                    case 14:
                        sampleRateHint = x;
                        break;
                        
                    default:
                        hFlag = false;
                        break;
                }
                
                x = static_cast<tint>((hMem[3] >> 4) & 0x0f);
                if(x & 8)
                {
                    m_channels = 2;
                    switch(x & 7)
                    {
                        case 0:
                            m_channelAssignment = e_LeftSide;
                            break;
                        
                        case 1:
                            m_channelAssignment = e_RightSide;
                            break;
                        
                        case 2:
                            m_channelAssignment = e_MidSide;
                            break;
                        
                        default:
                            hFlag = false;
                            break;
                    }
                }
                else
                {
                    m_channels = x + 1;
                    m_channelAssignment = e_Independent;
                }
                
                x = static_cast<tint>((hMem[3] >> 1) & 0x07);
                switch(x)
                {
                    case 0:
                        m_bitsPerSample = m_streamInfo->m_bitsPerSample;
                        break;
                        
                    case 1:
                        m_bitsPerSample = 8;
                        break;
                        
                    case 2:
                        m_bitsPerSample = 12;
                        break;
                        
                    case 4:
                        m_bitsPerSample = 16;
                        break;
                        
                    case 5:
                        m_bitsPerSample = 20;
                        break;
                        
                    case 6:
                        m_bitsPerSample = 24;
                        break;
                        
                    case 3:
                    case 7:
                    default:
                        hFlag = false;
                        break;
                }
                
                if(hMem[3] & 0x01)
                {
                    hFlag = false;
                }
            }
            else
            {
                hFlag = false;
            }
            
            if(hFlag)
            {
                tint len = 4;
                
                if(hMem[1] & 0x01)
                {
                    m_numberType = e_SampleNumber;
                    m_sampleNumber = readUTF8Encoded64(seq,hMem,len);
                    if(m_sampleNumber==0xFFFFFFFFFFFFFFFFULL)
                    {
                        hFlag = false;
                    }
                }
                else
                {
                    m_numberType = e_FrameNumber;
                    m_frameNumber = readUTF8Encoded32(seq,hMem,len);
                    if(m_frameNumber==0xFFFFFFFF)
                    {
                        hFlag = false;
                    }
                }
                
                if(hFlag)
                {
                    tubyte crc;
                    
                    if(blockBitHint)
                    {
                        hMem[len++] = static_cast<tubyte>(seq->readBitsI(8));
                        if(blockBitHint==7)
                        {
                            hMem[len++] = static_cast<tubyte>(seq->readBitsI(8));
                            m_blockSize = (static_cast<tint>(hMem[len-2]) << 8) | static_cast<tint>(hMem[len-1]) + 1;
                        }
                        else
                        {
                            m_blockSize = static_cast<tint>(hMem[len-1]) + 1;
                        }
                    }
                    
                    if(sampleRateHint)
                    {
                        hMem[len++] = static_cast<tubyte>(seq->readBitsI(8));
                        if(sampleRateHint!=12)
                        {
                            hMem[len++] = static_cast<tubyte>(seq->readBitsI(8));
                            x = (static_cast<tint>(hMem[len-2]) << 8) | static_cast<tint>(hMem[len-1]);
                        }
                        else
                        {
                            x = static_cast<tint>(hMem[len-1]);
                        }
                        switch(sampleRateHint)
                        {
                            case 12:
                                m_sampleRate = x * 1000;
                                break;
                                
                            case 13:
                                m_sampleRate = x;
                                break;
                                
                            default:
                                m_sampleRate = x * 10;
                                break;
                        }
                    }
                    
                    crc = static_cast<tubyte>(seq->readBitsI(8));
                    if(crc==crc8(hMem,len))
                    {
                        if(calcSampleNumber())
                        {
                            return true;
                        }
                        else
                        {
                            seq->seek(-len);
                        }
                    }
                    else
                    {
                        seq->seek(-len);
                    }
                }
                else
                {
                    seq->seek(-(len - 8));
                }
            }
            else
            {
                seq->seek(-24);
            }
        }
    }
    return false;
}

//-----------------------------------------------------------------------------------------

bool FLACHeader::calcSampleNumber()
{
    m_nextFixedBlockSize = 0;
    if(m_numberType==e_FrameNumber)
    {
        tuint64 x;
        
        x = static_cast<tuint64>(m_frameNumber);
        m_numberType = e_SampleNumber;
        if(m_fixedBlockSize)
        {
            m_sampleNumber = static_cast<tuint64>(m_fixedBlockSize) * x;
        }
        else
        {
            if(m_streamInfo->m_minBlockSize==m_streamInfo->m_maxBlockSize)
            {
                m_sampleNumber = static_cast<tuint64>(m_streamInfo->m_maxBlockSize) * x;
                m_nextFixedBlockSize = m_streamInfo->m_maxBlockSize;
            }
            else
            {
                return false;
            }
        }
    }
    return true;
}

//-----------------------------------------------------------------------------------------

tuint32 FLACHeader::readUTF8Encoded32(Sequence *seq)
{
    tint len = 0;
    tubyte mem[6];
    return readUTF8Encoded32(seq,mem,len);
}

//-----------------------------------------------------------------------------------------

tuint32 FLACHeader::readUTF8Encoded32(Sequence *seq,tubyte *mem,tint& len)
{
    tuint32 i,x,v = 0;
    
    x = static_cast<tuint32>(seq->readBitsI(8));
    if(mem!=0)
    {
        mem[len++] = static_cast<tubyte>(x);
    }
    if(!(x & 0x80)) // 0xxxxxxx
    {
        v = x;
        i = 0;
    }
    else if((x & 0xC0) && !(x & 0x20)) // 110xxxxx
    {
        v = x & 0x1F;
        i = 1;
    }
    else if((x & 0xE0) && !(x & 0x10)) // 1110xxxx
    {
        v = x & 0x0F;
        i = 2;
    }
    else if((x & 0xF0) && !(x & 0x08)) // 11110xxx
    {
        v = x & 0x07;
        i = 3;
    }
    else if((x & 0xF8) && !(x & 0x04)) // 111110xx
    {
        v = x & 0x03;
        i = 4;
    }
    else if((x & 0xFC) && !(x & 0x02)) // 1111110x
    {
        v = x & 0x01;
        i = 5;
    }
    else
    {
        return 0xFFFFFFFF;
    }
    while(i)
    {
        x = static_cast<tuint32>(seq->readBitsI(8));
        if(mem!=0)
        {
            mem[len++] = static_cast<tubyte>(x);
        }
        if(!(x & 0x80) || (x & 0x40)) // 10xxxxxx
        {
            return 0xFFFFFFFF;
        }
        v <<= 6;
        v |= (x & 0x3f);
        i--;
    }
    return v;
}

//-----------------------------------------------------------------------------------------

tuint64 FLACHeader::readUTF8Encoded64(Sequence *seq)
{
    tint len = 0;
    tubyte mem[6];
    return readUTF8Encoded64(seq,mem,len);
}

//-----------------------------------------------------------------------------------------

tuint64 FLACHeader::readUTF8Encoded64(Sequence *seq,tubyte *mem,tint& len)
{
    tuint32 i,x;
    tuint64 v = 0;
    
    x = static_cast<tuint32>(seq->readBitsI(8));
    if(mem!=0)
    {
        mem[len++] = static_cast<tubyte>(x);
    }
    if(!(x & 0x80)) // 0xxxxxxx
    {
        v = static_cast<tuint64>(x);
        i = 0;
    }
    else if((x & 0xC0) && !(x & 0x20)) // 110xxxxx
    {
        v = static_cast<tuint64>(x & 0x1F);
        i = 1;
    }
    else if((x & 0xE0) && !(x & 0x10)) // 1110xxxx
    {
        v = static_cast<tuint64>(x & 0x0F);
        i = 2;
    }
    else if((x & 0xF0) && !(x & 0x08)) // 11110xxx
    {
        v = static_cast<tuint64>(x & 0x07);
        i = 3;
    }
    else if((x & 0xF8) && !(x & 0x04)) // 111110xx
    {
        v = static_cast<tuint64>(x & 0x03);
        i = 4;
    }
    else if((x & 0xFC) && !(x & 0x02)) // 1111110x
    {
        v = static_cast<tuint64>(x & 0x01);
        i = 5;
    }
    else
    {
        return 0xFFFFFFFFFFFFFFFFULL;
    }
    while(i)
    {
        x = seq->readBitsI(8);
        if(mem!=0)
        {
            mem[len++] = static_cast<tubyte>(x);
        }
        if(!(x & 0x80) || (x & 0x40)) // 10xxxxxx
        {
            return 0xFFFFFFFFFFFFFFFFULL;
        }
        v <<= 6;
        v |= static_cast<tuint64>(x & 0x3f);
        i--;
    }
    return v;
}

//-----------------------------------------------------------------------------------------

tubyte FLACHeader::crc8(tubyte *mem,tint len)
{
    static const tubyte crcTable[256] = {
        0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
        0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
        0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
        0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
        0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
        0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
        0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
        0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
        0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
        0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
        0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
        0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
        0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
        0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
        0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
        0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
        0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
        0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
        0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
        0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
        0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
        0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
        0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
        0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
        0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
        0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
        0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
        0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
        0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
        0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
        0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
        0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
    };
    
    tubyte crc = 0;
    
    while(len--)
    {
        crc = crcTable[crc ^ *mem++];
    }
    return crc;
}

//-----------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-----------------------------------------------------------------------------------------
