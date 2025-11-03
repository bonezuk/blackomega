#include "common/inc/UString.h"
#include "engine/inc/File.h"

#if defined(OMEGA_MAC_STORE)
#include "common/inc/SBService.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

File::File() : m_file(common::c_invalidFile),
    m_name(),
    m_length(0),
    m_offset(0)
{}

//-------------------------------------------------------------------------------------------

File::~File()
{
    try
    {
        File::close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void File::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "File::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void File::printError(const tchar *strR,const tchar *strE1,const tchar *strE2) const
{
    common::BString err;
    err << strE1 << m_name.toLatin1().constData() << strE2;
    printError(strR,static_cast<const tchar *>(err));
}

//-------------------------------------------------------------------------------------------

void File::printError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint errNo) const
{
    common::BString err;

#if defined(OMEGA_WIN32)
    common::BString eStrB(512);

    if(::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,NULL,static_cast<DWORD>(errNo),0,eStrB.GetString(),512,0)!=0)
    {
        err << strE2 << ". " << eStrB;
    }
    else
    {
        err << strE2;
    }
#elif defined(OMEGA_POSIX)
    err << strE2 << ". " << ::strerror(errNo);
#endif
    printError(strR,strE1,static_cast<const tchar *>(err));
}

//-------------------------------------------------------------------------------------------

bool File::open(const tchar *fileName)
{
    QString name = fileName;
    return open(name);
}

//-------------------------------------------------------------------------------------------

bool File::open(const QString& fileName)
{
    QString fName(fileName);

    close();

    if(fileName.isEmpty())
    {
        printError("open","No file name given");
        return false;
    }

#if defined(OMEGA_WIN32)
    {
        DWORD len;
        common::UString uname(fileName.utf16());

        common::UString::SetASCIIEncoding(STANDARD_ASCII_8BIT);
        m_file = ::CreateFileW(reinterpret_cast<LPCWSTR>(uname.getString()),GENERIC_READ,FILE_SHARE_READ,0,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,0);
        if(m_file==common::c_invalidFile)
        {
            printError("open","Failed to open file '","'",static_cast<tint>(::GetLastError()));
            return false;
        }
        len = ::GetFileSize(m_file,0);
        if(len==0xffffffff)
        {
            printError("open","Failed to get length of file '","'",static_cast<tint>(::GetLastError()));
            return false;
        }
        m_length = static_cast<tint>(len);
    }
#elif defined(OMEGA_POSIX)
    {
        tint len,flags = O_RDONLY;
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

#if defined(OMEGA_MAC_STORE)
        {
            common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
            if(!sbBookmark->has(fileName,true))
            {
                sbBookmark->add(fileName,true);
            }
            sbBookmark->checkOut(fileName,true);
        }
#endif

        QByteArray name = fName.toUtf8();

        m_file = ::open(name.data(),flags,mode);
        if(m_file==common::c_invalidFile)
        {
            printError("open","Failed to open file '","'",static_cast<tint>(errno));
            return false;
        }
        len = static_cast<tint>(lseek(m_file,0,SEEK_END));
        if(len>=0)
        {
            if(lseek(m_file,0,SEEK_SET)==-1)
            {
                printError("open","Failed to get length of file '","'",errno);
                return false;
            }
        }
        else
        {
            printError("open","Failed to get length of file '","'",errno);
            return false;
        }
        m_length = len;
    }
#endif

    m_name = fName;
    m_offset = 0;
    return true;
}

//-------------------------------------------------------------------------------------------

void File::close()
{
    if(m_file!=common::c_invalidFile)
    {
#if defined(OMEGA_WIN32)
        if(::CloseHandle(m_file)==0)
        {
            printError("close","Error closing file handle on file '","'",static_cast<tint>(::GetLastError()));
        }
#elif defined(OMEGA_MACOSX)
        if(::close(m_file)!=0)
        {
            printError("close","Error closing file handle on file '","'",static_cast<tint>(errno));
        }
#endif

#if defined(OMEGA_MAC_STORE)
        {
            common::SBBookmarkPtr sbBookmark = common::SBBookmark::get();
            sbBookmark->checkIn(m_name,true);
        }
#endif

        m_file = common::c_invalidFile;
    }
    m_length = 0;
    m_offset = 0;
    m_name = QString();
}

//-------------------------------------------------------------------------------------------

tint File::read(tubyte *mem,tint len)
{
    return read(reinterpret_cast<tchar *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint File::read(tchar *mem,tint len)
{
    if(m_file==common::c_invalidFile)
    {
        printError("read","No file open to read from");
        return -1;
    }
    if(mem==0 || len<0)
    {
        printError("read","No memory buffer given to read into");
        return -1;
    }
    if(len==0)
    {
        return 0;
    }

#if defined(OMEGA_WIN32)
    DWORD amount;
    BOOL res;

    res = ::ReadFile(m_file,reinterpret_cast<LPVOID>(mem),static_cast<DWORD>(len),&amount,0);
    if(res==0)
    {
        printError("read","Error reading from file '","'",static_cast<tint>(::GetLastError()));
        close();
        return -1;
    }
#elif defined(OMEGA_POSIX)
    tint amount;

    amount = static_cast<tint>(::read(m_file,mem, static_cast<size_t>(len)));
    if(amount==-1)
    {
        printError("read","Error reading from file '","'",errno);
        close();
        return -1;
    }
#endif
    m_offset += static_cast<tint>(amount);
    return static_cast<tint>(amount);
}

//-------------------------------------------------------------------------------------------

bool File::seek(tint offset,Position position)
{
    if(m_file==common::c_invalidFile)
    {
        printError("seek","No file has been opened to seek position on");
        return false;
    }

    switch(position)
    {
        case e_currentPosition:
            offset += m_offset;
            break;

        case e_endPosition:
            offset += m_length;
            break;

        case e_startPosition:
        default:
            break;
    }

    if(offset<0)
    {
        offset = 0;
    }
    else if(offset>m_length)
    {
        offset = m_length;
    }

#if defined(OMEGA_WIN32)
    offset = static_cast<tint>(::SetFilePointer(m_file,static_cast<LONG>(offset),0,FILE_BEGIN));
    if(offset==-1)
    {
        printError("seek","Failed to relocate to new position on file '","'",static_cast<tint>(::GetLastError()));
        close();
        return false;
    }
#elif defined(OMEGA_POSIX)
    if(::lseek(m_file,static_cast<off_t>(offset),SEEK_SET)==-1)
    {
        printError("seek","Failed to relocate to new position on file '","'",static_cast<tint>(errno));
        close();
        return false;
    }
#endif
    m_offset = offset;
    return true;
}

//-------------------------------------------------------------------------------------------

tint File::length() const
{
    return m_length;
}

//-------------------------------------------------------------------------------------------

tint File::offset() const
{
    return m_offset;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
