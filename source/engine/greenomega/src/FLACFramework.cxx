#include "engine/greenomega/inc/FLACFramework.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

FLACFramework::FLACFramework() : m_file(0),
    m_bitstream(0),
    m_sequenceNo(1),
    m_metadata(),
    m_streamInfo(0),
    m_seekTable(0),
    m_comments(0),
    m_streamOffset(0)
{}

//-------------------------------------------------------------------------------------------

FLACFramework::~FLACFramework()
{
    try
    {
        int i;
        FLACMetaBase *meta;
        
        for(i=0;i<m_metadata.size();i++)
        {
            meta = m_metadata[i];
            delete meta;
        }
        if(m_bitstream!=0)
        {
            delete m_bitstream;
            m_bitstream = 0;
        }
        if(m_file!=0)
        {
            delete m_file;
            m_file = 0;
        }
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void FLACFramework::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "FLACFramework::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool FLACFramework::init(const tchar *name)
{
    return init(QString::fromUtf8(name));
}

//-------------------------------------------------------------------------------------------

bool FLACFramework::init(const QString& name)
{
    bool res = false;

    m_file = new engine::File;
    if(m_file->open(name))
    {
        res = init(m_file);
    }
    else
    {
        printError("init","Failed to open file");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FLACFramework::init(engine::File *f)
{
    bool res = false;
    
    m_bitstream = new engine::Bitstream(Bitstream::e_streamMode2);
    if(m_bitstream->open(f))
    {
        if(isValidFile())
        {
            if(readMetaData())
            {
                res = true;
            }
            else
            {
                printError("init","Error reading metadata information");
            }
        }
        else
        {
            printError("init","The file is not a FLAC audio file");
        }
    }
    else
    {
        printError("init","Failed to open required bitstream");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FLACFramework::isValidFile()
{
    engine::Sequence *seq = m_bitstream->getSequence(0);
    bool res = false;
    
    if(seq!=0)
    {
        tuint32 id = seq->readBitsI(32);
        
        m_streamOffset += 4;
        if(id==FLAC_FILE_ID)
        {
            res = true;
        }
        else
        {
            id = (id & 0xffffff00) | 0x0000002E;
            if(id==FLAC_ID3_ID)
            {
                tint i,state;
                tubyte mem[6];
                int tagSize;
                bool syncFlag;

                for(i=0;i<6;i++)
                {
                    mem[i] = static_cast<tubyte>(seq->readBitsI(8));
                    m_streamOffset++;
                }
                
                syncFlag = (mem[1] & 0x80) ? true : false;

                tagSize = static_cast<tint>( ((static_cast<tuint>(mem[2]) << 21) & 0x0fe00000) |
                                             ((static_cast<tuint>(mem[3]) << 14) & 0x001fc000) |
                                             ((static_cast<tuint>(mem[4]) <<  7) & 0x00003f80) |
                                             ((static_cast<tuint>(mem[5])      ) & 0x0000007f) );
                                             
                
                if(syncFlag)
                {
                    for(i=0,state=0;i<tagSize;)
                    {
                        tubyte x = seq->readBitsI(8);
                        
                        m_streamOffset++;
                        if(!state)
                        {
                            i++;
                            if(x==0xff)
                            {
                                state = 1;
                            }
                        }
                        else
                        {
                            if(x!=0x00)
                            {
                                i++;
                            }
                            state = 0;
                        }
                    }
                }
                else
                {
                    if(!seq->seek(tagSize << 3))
                    {
                        printError("isValidFile","Failed to seek past ID3 tag");
                        return false;
                    }
                    m_streamOffset += tagSize;
                }
                
                id = seq->readBitsI(32);
                m_streamOffset += 4;
                if(id==FLAC_FILE_ID)
                {
                    res = true;
                }
                else
                {
                    printError("isValidFile","File is not FLAC file");
                }
            }
            else
            {
                printError("isValidFile","File is not FLAC file");
            }
        }
    }
    else
    {
        printError("isValidFile","Failed to get sequence");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool FLACFramework::readMetaData()
{
    engine::Sequence *metaSeq,*initSeq = m_bitstream->getSequence(0);
    bool res = true;
    
    if(initSeq!=0)
    {
        bool isLast = false;
        tint type,len,bkStart,bkEnd = -1;
        
        while(!isLast && res)
        {
            isLast = initSeq->readBitI() ? true : false;
            type = initSeq->readBitsI(7);
            len = initSeq->readBitsI(24) << 3;
            m_streamOffset += (len >> 3) + 4;
            bkStart = initSeq->bookmark();
            if(bkStart>=0)
            {
                if(initSeq->seek(len))
                {
                    bkEnd = initSeq->bookmark();
                    if(bkEnd>=0)
                    {
                        if(m_bitstream->mark(m_sequenceNo,bkStart,-24) && m_bitstream->mark(0,bkEnd))
                        {
                            metaSeq = m_bitstream->getSequence(m_sequenceNo);
                            if(metaSeq!=0)
                            {
                                FLACMetaBase *meta = FLACMetaBase::get(type,metaSeq);
                                if(meta!=0)
                                {
                                    m_metadata.append(meta);
                                    
                                    switch(meta->type())
                                    {
                                        case FLACMetaBase::e_StreamInfo:
                                            if(m_streamInfo==0)
                                            {
                                                m_streamInfo = dynamic_cast<FLACMetaStreamInfo *>(meta);
                                            }
                                            break;
                                            
                                        case FLACMetaBase::e_SeekTable:
                                            if(m_seekTable==0)
                                            {
                                                m_seekTable = dynamic_cast<FLACMetaSeekTable *>(meta);
                                            }
                                            break;
                                            
                                        case FLACMetaBase::e_Comment:
                                            if(m_comments==0)
                                            {
                                                m_comments = dynamic_cast<FLACMetaComment *>(meta);
                                            }
                                            break;

                                        default:
                                            break;
                                    }
                                }
                            }
                            else
                            {
                                printError("readMetaData","Error getting bitstream sequence for metadata block");
                                res = false;
                            }
                            m_sequenceNo++;
                        }
                        else
                        {
                            printError("readMetaData","Error marking out sequence for metadata block");
                            res = false;
                        }
                    }
                    else
                    {
                        printError("readMetaData","Error bookmarking the end of metadata block");
                        res = false;
                    }
                }
                else
                {
                    printError("readMetaData","Failed to seek to end of metadata block");
                    res = false;
                }
            }
            else
            {
                printError("readMetaData","Error bookmarking the start of metadata block");
                res = false;
            }
        }
        
        if(m_streamInfo==0)
        {
            printError("readMetaData","No stream information metadata was found");
            res = false;
        }
        if(bkEnd>=1)
        {
            m_bitstream->complete(bkEnd);
        }
    }
    else
    {
        printError("readMetaData","Failed to get initial sequence");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint FLACFramework::bitrate() const
{
    return static_cast<tint>((static_cast<tfloat64>(m_bitstream->file()->length() - streamOffset()) * 8.0) / static_cast<tfloat64>(m_streamInfo->length()));
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
