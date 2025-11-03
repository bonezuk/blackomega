#include "engine/redomega/inc/CAFFile.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

CAFFile::CAFFile() : m_file(0),
    m_blockMap()
{}

//-------------------------------------------------------------------------------------------

CAFFile::~CAFFile()
{
    close();
}

//-------------------------------------------------------------------------------------------

void CAFFile::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "CAFFile::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool CAFFile::init(engine::File *f)
{
    bool res = false;
    
    if(f->seek(0,engine::File::e_startPosition))
    {
        tubyte hdr[12];
        
        if(f->read(hdr,8)==8)
        {
            tuint32 id = readUInt32(hdr);
            if(id==CAFF_CADD_ID)
            {
                tint bLen,bOffset;
                
                res = true;
                
                while(f->offset()<(f->length()-12) && res)
                {
                    if(f->read(hdr,12)==12)
                    {
                        id = readUInt32(hdr);
                        bLen = static_cast<tint>(readUInt32(&hdr[8]));
                        bOffset = f->offset();
                        
                        if((bOffset + bLen) <= f->length())
                        {
                            m_blockMap.insert(id,QPair<tint,tint>(bLen,bOffset));
                            if(!f->seek(bLen,engine::File::e_currentPosition))
                            {
                                printError("init","Error seeking to next header location");
                                res = false;
                            }
                        }
                        else
                        {
                            printError("init","Unexpected end of file");
                            res = false;
                        }
                    }
                    else
                    {
                        printError("init","Error reading expected block header");
                        res = false;
                    }
                }
                
                if(res)
                {
                    m_file = f;
                }
                else
                {
                    close();
                }
            }
            else
            {
                printError("init","Not a CAF file");
            }
        }
        else
        {
            printError("init","Error reading initial 8 bytes");
        }
    }
    else
    {
        printError("init","Error seeking to beginning of file");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void CAFFile::close()
{
    m_blockMap.clear();
}

//-------------------------------------------------------------------------------------------

tuint32 CAFFile::readUInt32(tubyte *y) const
{
    tuint32 x;
    
    x = ((static_cast<tuint32>(y[0]) << 24) & 0xff000000) |
        ((static_cast<tuint32>(y[1]) << 16) & 0x00ff0000) |
        ((static_cast<tuint32>(y[2]) <<  8) & 0x0000ff00) |
        ((static_cast<tuint32>(y[3])      ) & 0x000000ff);
    return x;    
}

//-------------------------------------------------------------------------------------------

ALACSequence *CAFFile::getSequence(tuint32 id)
{
    ALACSequence *seq = 0;
    QMap<tuint32,QPair<tint,tint> >::const_iterator ppI;
    
    ppI = m_blockMap.find(id);
    if(ppI!=m_blockMap.end())
    {
        const QPair<tint,tint>& bDim = ppI.value();
        common::Array<tubyte,tubyte> arrA;
        QSharedPointer<common::Array<tubyte,tubyte> > arrB(new common::Array<tubyte,tubyte>());
        
        arrA.SetSize(bDim.first);
        if(m_file->seek(bDim.second,engine::File::e_startPosition))
        {
            if(m_file->read(arrA.GetData(),bDim.first)==bDim.first)
            {
                SequenceMemory::generateArray(arrA.GetData(),arrA.GetSize(),arrB);
                seq = new ALACSequence(arrB);
            }
            else
            {
                printError("getSequence","Error loading body data");
            }
        }
        else
        {
            printError("getSequence","Error finding expected body data");
        }
    }
    else
    {
        printError("getSequence","Could not find specified ID block");
    }
    return seq;
}

//-------------------------------------------------------------------------------------------

tint CAFFile::blockOffset(tuint32 id) const
{
    tint res = -1;
    QMap<tuint32,QPair<tint,tint> >::const_iterator ppI = m_blockMap.find(id);
    
    if(ppI!=m_blockMap.end())
    {
        res = ppI.value().second;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint CAFFile::blockLength(tuint32 id) const
{
    tint res = -1;
    QMap<tuint32,QPair<tint,tint> >::const_iterator ppI = m_blockMap.find(id);
    
    if(ppI!=m_blockMap.end())
    {
        res = ppI.value().first;
    }
    return res;    
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
