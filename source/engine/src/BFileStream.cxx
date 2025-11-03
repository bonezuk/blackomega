#if !defined(COMP_UNIT_BFileStream)
#define COMP_UNIT_BFileStream
#endif

#include "engine/inc/BFileStream.h"
#include "common/inc/UString.h"
#include "common/inc/DiskOps.h"
#include "common/inc/Log.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

COMMON_TEMPLATE_MEMBER tint BOTree<tint,DLong>::nullIndex = -1;
COMMON_TEMPLATE_MEMBER DLong BOTree<tint,DLong>::nullObject = DLong(0);

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

BFileStream::BFileStream() : m_Type(e_StreamNormal),
    m_readOnly(true),
    m_Name(),
    m_File(c_invalidFile),
    m_Position(0),
    m_Bookmarks()
{}

//-------------------------------------------------------------------------------------------

BFileStream::BFileStream(bool readOnly) : m_Type(e_StreamNormal),
    m_readOnly(readOnly),
    m_Name(),
    m_File(c_invalidFile),
    m_Position(0),
    m_Bookmarks()
{}

//-------------------------------------------------------------------------------------------

BFileStream::BFileStream(BStreamType type,bool readOnly) : m_Type(type),
    m_readOnly(readOnly),
    m_Name(),
    m_File(c_invalidFile),
    m_Position(0),
    m_Bookmarks()
{}

//-------------------------------------------------------------------------------------------

BFileStream::~BFileStream()
{
    try
    {
        Close();
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void BFileStream::PrintError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "BFileStream::" << strR << " - " << strE << "." << common::c_endl;
}

//-------------------------------------------------------------------------------------------

void BFileStream::PrintError(const tchar *strR,const tchar *strE1,const common::BString& name,const tchar *strE2) const
{
    common::BString eStr;

    eStr << strE1 << " '" << name << "' ";
    if(strE2!=0)
    {
        eStr << strE2;
    }
    PrintError(strR,static_cast<const tchar *>(eStr));
}

//-------------------------------------------------------------------------------------------

BStreamType BFileStream::Type() const
{
    return m_Type;
}

//-------------------------------------------------------------------------------------------

bool BFileStream::Path(const common::BString& name)
{
    return common::DiskOps::path(name);
}

//-------------------------------------------------------------------------------------------

bool BFileStream::Open(const tchar *name)
{
    common::BString n(name);
    return Open(n);
}

//-------------------------------------------------------------------------------------------

bool BFileStream::Open(const common::BString& name)
{
    common::UString uName(name);
    common::BString n;

    Close();
    common::UString::SetASCIIEncoding(STANDARD_ASCII_8BIT);
    n = uName.AStr();

    m_Name=name;

    if(!m_readOnly)
    {
        if(!Path(name))
        {
            PrintError("Open","Could not obtain path to file ",name);
            return false;
        }
    }

    if(m_readOnly)
    {
#if defined(OMEGA_WIN32)
        m_File = ::CreateFileA(static_cast<const tchar *>(n),GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
#elif defined(OMEGA_POSIX)
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        m_File = ::open(static_cast<const tchar *>(n),O_RDONLY,mode);
#endif

        if(m_File==c_invalidFile)
        {
            PrintError("Open","Could not open ",name);
            return false;
        }
    }
    else
    {
#if defined(OMEGA_WIN32)
        m_File=::CreateFileA(static_cast<const tchar *>(n),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
#elif defined(OMEGA_POSIX)
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;

        m_File = ::open(static_cast<const tchar *>(n),O_WRONLY | O_CREAT,mode);
#endif

        if(m_File==c_invalidFile)
        {
            PrintError("Open","Could not create file ",name," to write too");
            return false;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

void BFileStream::Close()
{
    if(m_File!=c_invalidFile)
    {
#if defined(OMEGA_WIN32)
        ::CloseHandle(m_File);
#elif defined(OMEGA_POSIX)
        ::close(m_File);
#endif
        m_File = c_invalidFile;
    }
    m_Name = "";
    m_Position = 0;
    while(m_Bookmarks.Start())
    {
        m_Bookmarks.Remove(m_Bookmarks.CurrentIndex());
    }
}

//-------------------------------------------------------------------------------------------

tint BFileStream::Write(const tchar *mem,tint len)
{
    tint amount;
    bool res = true;

    if(m_File==c_invalidFile)
    {
        PrintError("Write","no file is open to write too");
        return -1;
    }
    if(m_readOnly)
    {
        PrintError("Write","Cannot write to ",m_Name," as it is open with read-only permissions");
        return -1;
    }

#if defined(OMEGA_WIN32)
    DWORD done=0;
    res = (::WriteFile(m_File,mem,static_cast<DWORD>(len),&done,NULL)) ? true : false;
    amount = static_cast<tint>(done);
#elif defined(OMEGA_POSIX)
    amount = static_cast<tint>(::write(m_File,mem,static_cast<size_t>(len)));
#endif

    if(res || amount!=len)
    {
        PrintError("Write","Failed to write all the contents to ",m_Name);
        return -1;
    }
    m_Position += amount;
    return amount;
}

//-------------------------------------------------------------------------------------------

tint BFileStream::Read(char *mem,tint len)
{
    tint amount;
    bool res;

    if(m_File!=c_invalidFile)
    {
        PrintError("Read","No file is open read from");
        return -1;
    }
    if(!m_readOnly)
    {
        PrintError("Read","Cannot read from ",m_Name," as it is open write-only permissions");
        return -1;
    }

#if defined(OMEGA_WIN32)
    DWORD done;
    res = (::ReadFile(m_File,mem,len,&done,0)) ? true : false;
    amount = static_cast<tint>(done);
#elif defined(OMEGA_POSIX)
    amount = static_cast<tint>(::read(m_File,mem,static_cast<size_t>(len)));
    res = (amount==-1) ? false : true;
#endif

    if(!res)
    {
        PrintError("Read","Failed to read all the contents from ",m_Name);
        return -1;
    }
    m_Position += amount;
    return amount;
}

//-------------------------------------------------------------------------------------------

bool BFileStream::ReadOnly() const
{
    return m_readOnly;
}

//-------------------------------------------------------------------------------------------

tint BFileStream::Bookmark(tint offset)
{
    tint nextID;
    common::DLong a;

    nextID = m_Bookmarks.LastIndex();
    nextID++;
    a = m_Position + offset;
    m_Bookmarks[nextID] = a;
    return nextID;
}

//-------------------------------------------------------------------------------------------

void BFileStream::DeleteBookmark(tint bookmark)
{
    if(m_Bookmarks.Exist(bookmark))
    {
        m_Bookmarks.Remove(bookmark);
    }
}

//-------------------------------------------------------------------------------------------

bool BFileStream::Position(tint bookmark)
{
    common::DLong pos;

    if(!m_Bookmarks.Exist(bookmark))
    {
        PrintError("Position","Requested stream bookmark position has not been registed");
        return false;
    }
    pos = m_Bookmarks.Find(bookmark);
    return Seek(pos,FROM_START);
}

//-------------------------------------------------------------------------------------------

common::DLong BFileStream::Size()
{
    common::DLong size;

    if(!m_readOnly)
    {
        PrintError("Size","Can only give size on read-only files");
    }
    else
    {
        if(m_File!=c_invalidFile)
        {
#if defined(OMEGA_WIN32)
            DWORD high,low;

            low=::GetFileSize(m_File,&high);
            if(low==0xFFFFFFFF && ::GetLastError()!=NO_ERROR)
            {
                PrintError("Size","Failed to obtain size from ",m_Name);
            }
            else
            {
                size=common::DLong(low,high);
            }
#elif defined(OMEGA_POSIX)
            off_t s;

            s = ::lseek(m_File,0,SEEK_END);
            if(s!=-1)
            {
                if(::lseek(m_File,static_cast<off_t>(m_Position.Low()),SEEK_SET)!=-1)
                {
                    PrintError("Size","Failed to seek current postion");
                }
                size=common::DLong(static_cast<tuint>(s));
            }
            else
            {
                PrintError("Size","Failed to seek to end of file position");
            }
#endif
        }
        else {
            PrintError("Size","No file open to get size from");
        }
    }
    return size;
}

//-------------------------------------------------------------------------------------------

bool BFileStream::Seek(tint offset,MAZ_PositionID from)
{
    common::DLong off(offset);
    return Seek(off,from);
}

//-------------------------------------------------------------------------------------------

bool BFileStream::Seek(const common::DLong& offset,MAZ_PositionID from)
{
    common::DLong pos;

    switch(from)
    {
        case FROM_START:
            m_Position=offset;
            pos=offset;
            break;

        case FROM_CURRENT:
            m_Position+=offset;
            pos=m_Position;
            break;

        case FROM_END:
            m_Position=Size()-offset;
            pos=m_Position;
            break;

        default:
            PrintError("Seek","Unknown seek position from code");
            return false;
    }

#if defined(OMEGA_WIN32)
    {
        LONG high,low;

        high = static_cast<DWORD>(pos.High());
        low = ::SetFilePointer(m_File,pos.Low(),&high,FILE_BEGIN);
        if(low==0xFFFFFFFF && ::GetLastError()!=NO_ERROR)
        {
            PrintError("Seek","Error occured while seeking bookmarked position in ",m_Name);
            return false;
        }
        {
            common::DLong posB(low,high);

            if(pos!=posB)
            {
                PrintError("Seek","The returned file position does not match that given in to ",m_Name);
                return false;
            }
        }
    }
#elif defined(OMEGA_POSIX)
    {
        if(::lseek(m_File,static_cast<off_t>(pos.Low()),SEEK_SET)==-1)
        {
            PrintError("Seek","Error occured while seeking bookmarked position in ",m_Name);
            return false;
        }
    }
#endif
    return true;
}

//-------------------------------------------------------------------------------------------

const common::BString& BFileStream::Name() const
{
    return m_Name;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
