#include "engine/blueomega/inc/WaveInformation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blueomega
{
//-------------------------------------------------------------------------------------------

WaveInformation::WaveInformation() : m_format(0),
    m_noChannels(0),
    m_sampleRate(0),
    m_bytesPerSecond(0),
    m_bytesPerSample(0),
    m_bitsPerSample(0),
    m_offset(0),
    m_type(e_WaveUnknown),
    m_chMask(0)
{}

//-------------------------------------------------------------------------------------------

WaveInformation::WaveInformation(const WaveInformation& rhs) : m_format(0),
    m_noChannels(0),
    m_sampleRate(0),
    m_bytesPerSecond(0),
    m_bytesPerSample(0),
    m_bitsPerSample(0),
    m_offset(0),
    m_type(e_WaveUnknown),
    m_chMask(0)
{
    WaveInformation::copy(rhs);
}

//-------------------------------------------------------------------------------------------

WaveInformation::~WaveInformation()
{}

//-------------------------------------------------------------------------------------------

void WaveInformation::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "WaveInformation::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

const WaveInformation& WaveInformation::operator = (const WaveInformation& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------
        
void WaveInformation::copy(const WaveInformation& rhs)
{
    m_format = rhs.m_format;
    m_noChannels = rhs.m_noChannels;
    m_sampleRate = rhs.m_sampleRate;
    m_bytesPerSecond = rhs.m_bytesPerSecond;
    m_bytesPerSample = rhs.m_bytesPerSample;
    m_bitsPerSample = rhs.m_bitsPerSample;
    m_offset = rhs.m_offset;
    m_type = rhs.m_type;
    m_channelHandler = rhs.m_channelHandler;
    m_chMask = rhs.m_chMask;
}

//-------------------------------------------------------------------------------------------

tint WaveInformation::intFromMemory(tchar *mem) const
{
    tuint x;
    
    if(mem!=0)
    {
        x  = (static_cast<tuint>(static_cast<tint>(mem[0]))) & 0x000000ff;
        x |= (static_cast<tuint>(static_cast<tint>(mem[1])) <<  8) & 0x0000ff00;
        x |= (static_cast<tuint>(static_cast<tint>(mem[2])) << 16) & 0x00ff0000;
        x |= (static_cast<tuint>(static_cast<tint>(mem[3])) << 24) & 0xff000000;
    }
    else
    {
        x = 0;
    }
    return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

tint WaveInformation::shortFromMemory(tchar *mem) const
{
    tuint x;
    
    if(mem!=0)
    {
        x  = (static_cast<tuint>(static_cast<tint>(mem[0]))) & 0x000000ff;
        x |= (static_cast<tuint>(static_cast<tint>(mem[1])) << 8) & 0x0000ff00;
    }
    else
    {
        x = 0;
    }
    return static_cast<tint>(x);
}

//-------------------------------------------------------------------------------------------

bool WaveInformation::read(common::BIOStream *in)
{
    tchar tmp[12];
    bool res = false;
    
    if(in==0)
    {
        printError("read","No input file given");
        return false;
    }
    
    m_offset = 0;
    
    if(in->read(tmp,12)==12)
    {
        m_offset += 12;
        
        if(intFromMemory(tmp)==RIFF_ID && intFromMemory(&tmp[8])==WAVE_ID)
        {
            if(in->read(tmp,8)==8 && intFromMemory(tmp)==FMT_ID)
            {
                tint chMask = 0;
                tint len = intFromMemory(&tmp[4]);
                tchar *x = new tchar [static_cast<tuint>(len)];
                tchar *x2 = x;
                
                m_offset += 8;
                
                if(len>=16 && in->read(x,len)==len)
                {
                    m_format = shortFromMemory(x);
                    m_noChannels = shortFromMemory(&x[2]);
                    m_sampleRate = intFromMemory(&x[4]);
                    m_bytesPerSecond = intFromMemory(&x[8]);
                    m_bytesPerSample = shortFromMemory(&x[12]);
                    m_bitsPerSample = shortFromMemory(&x[14]);
                    
                    chMask = (m_noChannels==2) ? 0x00000003 : 0x00000004;
                    
                    if(m_format==e_formatPCM)
                    {
                        if(m_bitsPerSample<=8)
                        {
                            m_type = e_8Bit;
                        }
                        else if(m_bitsPerSample<=16)
                        {
                            m_type = e_16Bit_LittleEndian;
                        }
                        else if(m_bitsPerSample<=24)
                        {
                            m_type = e_24Bit_LittleEndian;
                        }
                        else
                        {
                            m_type = e_32Bit_LittleEndian;
                        }
                        res = true;
                    }
                    else if(m_format==e_formatFloat)
                    {
                        m_type = e_Float_LittleEndian;
                        res = true;
                    }
                    else if(m_format==e_formatExtensible && len>=0x16)
                    {
                        tint pType;
                        tubyte pcmTypeID[6] = { 0x00, 0xAA, 0x00, 0x38, 0x9B, 0x71 };
                        
                        x = &x[18];
                        chMask = intFromMemory(&x[2]);
                        if(memcmp(&x[16],pcmTypeID,6)==0)
                        {
                            pType = intFromMemory(&x[6]);
                            if(pType==1)
                            {
                                if(m_bitsPerSample<=8)
                                {
                                    m_type = e_8Bit;
                                }
                                else if(m_bitsPerSample<=16)
                                {
                                    m_type = e_16Bit_LittleEndian;
                                }
                                else if(m_bitsPerSample<=24)
                                {
                                    m_type = e_24Bit_LittleEndian;
                                }
                                else
                                {
                                    m_type = e_32Bit_LittleEndian;
                                }
                                res = true;
                            }
                            else if(pType==3)
                            {
                                m_type = e_Float_LittleEndian;
                                res = true;
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
                    else
                    {
                        res = false;
                    }
                    if(res)
                    {
                        m_chMask = chMask;
                    }
                    m_offset += len;
                }
                else
                {
                    printError("read","Error reading wave format header");
                }
                delete [] x2;
            }
            else
            {
                printError("read","Failed to read expected wave format section");
            }            
        }
        else
        {
            printError("read","Given file is not a WAV file");
        }
    }
    else
    {
        printError("read","Failed to read in wave header");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void WaveInformation::setupChannelMap(tint noOutChannels)
{
    m_channelHandler.setup(m_noChannels,noOutChannels,m_chMask);
}

//-------------------------------------------------------------------------------------------
} // namespace blueomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
