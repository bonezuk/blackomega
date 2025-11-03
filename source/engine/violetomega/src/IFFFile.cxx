#include "engine/violetomega/inc/IFFFile.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace violetomega
{
//-------------------------------------------------------------------------------------------

IFFFile::IFFFile(common::BIOStream *file) : m_file(file),
    m_root()
{}

//-------------------------------------------------------------------------------------------

IFFFile::~IFFFile()
{}

//-------------------------------------------------------------------------------------------

QString IFFFile::headerAsBigEndian(const tbyte *mem)
{
    QByteArray arr(mem,4);
    return QString::fromLatin1(arr);
}

//-------------------------------------------------------------------------------------------

QString IFFFile::headerAsLittleEndian(const tbyte *mem)
{
    tbyte x[4];
    x[0] = mem[3];
    x[1] = mem[2];
    x[2] = mem[1];
    x[3] = mem[0];
    QByteArray arr(x,4);
    return QString::fromLatin1(arr);
}

//-------------------------------------------------------------------------------------------

IFFChunkSPtr IFFFile::createFromFactory(common::BIOStream *file,IFFChunk::EndianType type)
{
    tbyte mem[4];
    IFFChunkSPtr pChunk;
    
    if(file!=0 && file->read(mem,4)==4)
    {
        bool littleEndian = false;
        QString idStr;
        
        switch(type)
        {
            case IFFChunk::e_EndianLittle:
                try
                {
                    idStr = headerAsLittleEndian(mem);
                    pChunk = IFFChunkFactory::createShared(idStr);
                    littleEndian = true;
                }
                catch(const common::Exception&)
                {
                    pChunk.clear();
                }
                break;
                
            case IFFChunk::e_EndianBig:
                try
                {
                    idStr = headerAsBigEndian(mem);
                    pChunk = IFFChunkFactory::createShared(idStr);
                }
                catch(const common::Exception&)
                {
                    pChunk.clear();
                }
                break;
                
            case IFFChunk::e_EndianUnknown:
            default:
                try
                {
                    idStr = headerAsBigEndian(mem);
                    pChunk = IFFChunkFactory::createShared(idStr);
                }
                catch(const common::Exception&)
                {
                    pChunk.clear();
                }
                
                if(pChunk.isNull())
                {
                    try
                    {
                        idStr = headerAsLittleEndian(mem);
                        pChunk = IFFChunkFactory::createShared(idStr);
                        littleEndian = true;
                    }
                    catch(const common::Exception&)
                    {
                        pChunk.clear();
                    }
                }
                break;
        }
        
        if(file->seek(-4,common::e_Seek_Current))
        {
            if(!pChunk.isNull())
            {
                pChunk->setup(file,littleEndian);
                if(!pChunk->readHeader())
                {
                    pChunk.clear();
                }
            }
            else
            {
                littleEndian = (type==IFFChunk::e_EndianLittle) ? true : false;

                IFFChunkSPtr pNChunk(new IFFChunk(file,littleEndian));
                if(pNChunk->readHeader())
                {
                    pChunk = pNChunk;
                }
            }
        }
        else
        {
            pChunk.clear();
        }
    }
    return pChunk;
}

//-------------------------------------------------------------------------------------------

IFFChunkSPtr IFFFile::root()
{
    if(m_root.isNull())
    {
        if(m_file!=0 && m_file->seek(0,common::e_Seek_Start))
        {
            IFFChunkSPtr pRoot = createFromFactory(m_file,IFFChunk::e_EndianUnknown);
            if(pRoot->scan())
            {
                m_root = pRoot;
            }
        }
    }
    return m_root;
}

//-------------------------------------------------------------------------------------------
} // namespace violetomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
