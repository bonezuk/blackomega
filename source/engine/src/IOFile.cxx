#include "engine/inc/IOFile.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

IOFile::IOFile(common::BIOStream *f) : File(),
    m_file(f)
{}

//-------------------------------------------------------------------------------------------

IOFile::~IOFile()
{}

//-------------------------------------------------------------------------------------------

void IOFile::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "IOFile::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void IOFile::printError(const tchar *strR,const tchar *strE1,const tchar *strE2) const
{
    File::printError(strR,strE1,strE2);
}

//-------------------------------------------------------------------------------------------

void IOFile::printError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint errNo) const
{
    File::printError(strR,strE1,strE2,errNo);
}

//-------------------------------------------------------------------------------------------

bool IOFile::open(const tchar *fileName)
{
    return m_file->open(fileName);
}

//-------------------------------------------------------------------------------------------

bool IOFile::open(const QString& fileName)
{
    return open(fileName.toUtf8().constData());
}

//-------------------------------------------------------------------------------------------

void IOFile::close()
{
    m_file->close();
}

//-------------------------------------------------------------------------------------------

tint IOFile::read(tubyte *mem,tint len)
{
    return read(reinterpret_cast<tchar *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint IOFile::read(tchar *mem,tint len)
{
    return m_file->read(mem,len);
}

//-------------------------------------------------------------------------------------------

bool IOFile::seek(tint offset,Position position)
{
    bool res;

    switch(position)
    {
        case e_startPosition:
            res = m_file->seek(offset,common::e_Seek_Start);
            break;

        case e_endPosition:
            res = m_file->seek(offset,common::e_Seek_End);
            break;

        case e_currentPosition:
        default:
            res = m_file->seek(offset,common::e_Seek_Current);
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint IOFile::length() const
{
    return m_file->size();
}

//-------------------------------------------------------------------------------------------

tint IOFile::offset() const
{
    return m_file->offset();
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
