#include "track/info/inc/WavInfo.h"
#include "common/inc/BIOMemory.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

WavInfo::WavInfo() : EmbeddedID3Info()
{
    m_type = e_InfoWAV;
}

//-------------------------------------------------------------------------------------------

WavInfo::~WavInfo()
{}

//-------------------------------------------------------------------------------------------

void WavInfo::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "WavInfo::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool WavInfo::read(common::BIOStream *input)
{
    bool res = false;
    
    if(input!=0 && input->seek(0,common::e_Seek_Start))
    {
        tchar tmp[12];
        
        if(input->read(tmp,12)==12)
        {
            if(intFromMemory(tmp)==RIFF_ID && intFromMemory(&tmp[8])==WAVE_ID)
            {
                tchar tmp[8];
                bool loop = true;
                
                while(loop)
                {
                    if(input->read(tmp,8)==8)
                    {
                        tint len = intFromMemory(&tmp[4]);
                        
                        if(intFromMemory(tmp)==WAVINFO_LIST_ID)
                        {
                            tchar *infoMem = new tchar [len];
                            
                            if(input->read(infoMem,len)==len)
                            {
                                if(readInfoBlock(infoMem,len))
                                {
                                    res = true;
                                }
                            }
                            else
                            {
                                loop = false;
                            }
                            delete [] infoMem;
                        }
                        else if(intFromMemory(tmp)==WAVINFO_ID3A_ID)
                        {
                            tchar *id3Mem = new tchar [len];
                            
                            if(input->read(id3Mem,len)==len)
                            {
                                QByteArray id3Array(id3Mem,len);
                                common::BIOMemory id3File(id3Array);
                                readID3(&id3File);
                                if(!m_Title.isEmpty())
                                {
                                    res = true;
                                }
                            }
                            else
                            {
                                loop = false;
                            }
                            delete [] id3Mem;
                        }
                        else
                        {
                            loop = input->seek(len,common::e_Seek_Current);
                        }
                    }
                    else
                    {
                        loop = false;
                    }
                }
                
                if(m_chapters.isEmpty() && isSeparateCuesheet(input->name()))
                {
                    readCueSheet(separateCueFilename(input->name()));
                    if(!m_chapters.isEmpty())
                    {
                        res = true;
                    }
                }                
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint WavInfo::intFromMemory(const tchar *mem) const
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

bool WavInfo::readInfoBlock(const tchar *mem,tint len)
{
    bool res = false;
    
    if(mem!=0 && len>=4 && intFromMemory(mem)==WAVINFO_INFO_ID)
    {
        tint offset = 4;
        bool loop = true;
        
        while(offset<len && loop)
        {
            if((offset+8) <= len)
            {
                tint ID = intFromMemory(&mem[offset]);
                tint size = intFromMemory(&mem[offset + 4]);
                
                if((offset + size + 8) <= len)
                {
                    QString text = QString::fromLatin1(&mem[offset + 8],size).trimmed();
                    
                    if(ID==WAVINFO_IART_ID)
                    {
                        m_Artist = text;
                    }
                    else if(ID==WAVINFO_INAM_ID)
                    {
                        m_Title = text;
                    }
                    else if(ID==WAVINFO_IPRD_ID)
                    {
                        m_Album = text;
                    }
                    else if(ID==WAVINFO_ITRK_ID)
                    {
                        m_Track = text;
                    }
                    else if(ID==WAVINFO_ICRD_ID)
                    {
                        m_Year = text;
                    }
                    else if(ID==WAVINFO_IGNR_ID)
                    {
                        m_Genre = text;
                    }
                    else if(ID==WAVINFO_ICMT_ID)
                    {
                        m_Comment = text;
                    }
                    else if(ID==WAVINFO_ICOP_ID)
                    {
                        m_Copyright = text;
                    }
                    else if(ID==WAVINFO_ISFT_ID)
                    {
                        m_Encoder = text;
                    }
                    
                    offset += 8 + size;    
                }
                else
                {
                    loop = false;
                }
            }
            else
            {
                loop = false;
            }
        }
        
        if(loop && !m_Title.isEmpty())
        {
            res = true;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
