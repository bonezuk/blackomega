#include "common/inc/BIOStream.h"
#include "common/inc/Log.h"

#if defined(OMEGA_MAC_STORE)
#include "common/inc/SBService.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

COMMON_TEMPLATE_MEMBER tint BOTree<tint,tint>::nullIndex  = -1;
COMMON_TEMPLATE_MEMBER tint BOTree<tint,tint>::nullObject = 0;

//-------------------------------------------------------------------------------------------

BIOStream::BIOStream(tint flags) : m_File(c_invalidFile),
    m_Name(),
    m_Flags(flags),
    m_Position(0),
    m_Bookmarks(),
    m_nextBookmarkID(1)
#if defined(OMEGA_MAC_STORE)
    ,m_checkOutFlag(true)
#endif
#if defined(OMEGA_POSIX)
    ,m_latinNameEncoding(false)
#endif
{}

//-------------------------------------------------------------------------------------------

BIOStream::~BIOStream()
{
    try
    {
        if(!BIOStream::close())
        {
            BIOStream::PrintError("~BIOStream","Error closing file stream");
        }
        m_File = c_invalidFile;
    }
    catch(...) {}
}

//-------------------------------------------------------------------------------------------

void BIOStream::PrintError(const tchar *strR,const tchar *strE) const
{
    Log::g_Log.print("BIOStream::%s - %s.\n",strR,strE);
}

//-------------------------------------------------------------------------------------------

void BIOStream::PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2) const
{
    BString err;

    err  = strE1;
    err += m_Name.toUtf8().constData();
    err += strE2;
    PrintError(strR,static_cast<const tchar *>(err));
}

//-------------------------------------------------------------------------------------------

void BIOStream::PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint code) const
{
    BString err;

#if defined(OMEGA_WIN32)
    
    BString eStrB(512);

    if(::FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM,NULL,static_cast<DWORD>(code),0,eStrB.GetString(),512,NULL)!=0)
    {
        err << strE2 << ". " << eStrB;
    }
    else
    {
        err << strE2;
    }

#elif defined(OMEGA_POSIX)

    err << strE2 << ". " << ::strerror(code);

#endif

    PrintError(strR,strE1,static_cast<const char *>(err));
}

//-------------------------------------------------------------------------------------------

const QString& BIOStream::name() const
{
    return m_Name;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool BIOStream::path(QString& name)
{
    tint i;
    struct _stat fileStat;
    bool res = true;
    
    try
    {
        if(!name.isEmpty())
        {
            QChar *x = name.data();
            
            for(i=0;i<name.size();++i)
            {
                if(x[i]==QChar('/'))
                {
                    x[i] = QChar('\\');
                }
            }
        }
        else
        {
            PrintError("path","No file name give to access");
            return false;
        }
        
        // Start path checking and creation from root of drive
        if(name.size()>2 && name.left(2)=="\\\\")
        {
            // Network drive - cannot test for directory path
            return true;
        }
        else if(name.size()>=2 && name.at(1)!=QChar(':'))
        {
            i = 3;
        }
        else
        {
            for(i=0;i<name.size() && name.at(i)!=QChar('\\');)
            {
                i++;
            }
            i++;
        }
        
        while(i < name.size())
        {
            if(name.at(i)==QChar('\\'))
            {
                LPCWSTR wStr;
                QString pName;
                
                pName = name.mid(0,(i==2) ? 3 : i);
                wStr = reinterpret_cast<LPCWSTR>(pName.utf16());
                if(_wstat(wStr,&fileStat)==0)
                {
                    if(S_IFREG & (fileStat.st_mode))
                    {
                        PrintError("path","File in way of directory path");
                        return false;                        
                    }
                }
                else if(m_Flags & e_BIOStream_FileCreate)
                {
                    if(::CreateDirectoryW(wStr,0)==0)
                    {
                        PrintError("path","Could not create directory for path");
                        return false;
                    }
                }
                else
                {
                    PrintError("path","Directory of existing file does not exist");
                    return false;
                }
            }
            i++;
        }
    }
    catch(...)
    {
        PrintError("path","Exception thrown");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool BIOStream::path(QString& name)
{
    tint i, r;
    QString pName;
    struct stat fileStat;
    bool res = true;
    
    try
    {
        if(!name.isEmpty())
        {
            QChar *x = name.data();
            
            for(i=0;i<name.size();++i)
            {
                if(x[i]==QChar('\\'))
                {
                    x[i] = QChar('/');
                }
            }            
        }
        else
        {
            PrintError("path","No file name given to access");
            return false;
        }
        
        i = 1;
        while(i < name.size())
        {
            if(name.at(i)==QChar('/'))
            {
                pName = name.mid(0,i);
                
                r = 1;
                if(!m_latinNameEncoding)
                {
                    r = ::stat(pName.toUtf8().constData(),&fileStat);
                }
                if(r)
                {
                    r = ::stat(pName.toLatin1().constData(),&fileStat);
                    if(!r)
                    {
                        m_latinNameEncoding = true;
                    }
                }
                if(!r)
                {
                    if(S_ISREG(fileStat.st_mode))
                    {
                        PrintError("path","File in way of directory path");
                        return false;
                    }
                }
                else if(m_Flags & e_BIOStream_FileCreate)
                {
                    if(!m_latinNameEncoding)
                    {
                        r = ::mkdir(pName.toUtf8().constData(),S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
                    }
                    else
                    {
                        r = ::mkdir(pName.toLatin1().constData(),S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH);
                    }
                    if(r!=0)
                    {
                        PrintError("path","Could not create directory for path");
                        return false;
                    }                    
                }
                else
                {
                    PrintError("path","Directory of existing file does not exist");
                    return false;
                }
            }
            i++;
        }
    }
    catch(...)
    {
        PrintError("path","Exception thrown");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

bool BIOStream::open(const QString& name)
{
    bool res = true;

    try
    {
        DWORD shareMode,accessMode;
    
        if(!close())
        {
            PrintError("open","Error closing previous file stream");
            return false;
        }
    
        m_Name = name;

        accessMode =  (m_Flags & e_BIOStream_FileRead) ? GENERIC_READ : 0;
        if(m_Flags & e_BIOStream_FileWrite) 
        {
            accessMode |= GENERIC_WRITE;
            shareMode = 0;
        }
        else 
        {
            shareMode=FILE_SHARE_READ;
        }
    
        if(!path(m_Name)) 
        {
            PrintError("open","Cannot find path to file");
            return false;
        }

        LPCWSTR wStr = reinterpret_cast<LPCWSTR>(m_Name.utf16());
    
        if(m_Flags & e_BIOStream_FileCreate) 
        {
            m_File=::CreateFileW(wStr,accessMode,shareMode,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
        }
        else 
        {
            if(m_Flags & e_BIOStream_FileWrite)
            {
                m_File=::CreateFileW(wStr,accessMode,shareMode,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
            }
            else
            {
                m_File=::CreateFileW(wStr,accessMode,shareMode,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL);
            }
        }
        
        if(m_File==c_invalidFile) //lint !e1924 Windows C-type definition
        {
            PrintError("open","Failed to open file '","'",static_cast<int>(::GetLastError()));
            return false;
        }
        m_Position=0;
    }
    catch(...)
    {
        PrintError("open","Exception thrown");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

bool BIOStream::open(const QString& name)
{
    bool res = true;

    try
    {
        tint flags;
        mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP |  S_IROTH;

        close();

        m_Name = name;
        
#if defined(OMEGA_MAC_STORE)
        if(m_checkOutFlag)
        {
            bool readOnlyFlag = (m_Flags & e_BIOStream_FileWrite) ? false : true;
            
            SBBookmarkPtr sbBookmark = SBBookmark::get();
            if(!sbBookmark->has(m_Name,readOnlyFlag))
            {
                sbBookmark->add(m_Name,readOnlyFlag);
            }
            sbBookmark->checkOut(m_Name,readOnlyFlag);
        }
#endif

        if(m_Flags & e_BIOStream_FileWrite)
        {
            if(m_Flags & e_BIOStream_FileRead)
            {
                flags = O_RDWR;
            }
            else
            {
                flags = O_WRONLY;
            }
        }
        else
        {
            flags = O_RDONLY;
        }

#if !defined(OMEGA_MAC_STORE)
        if(!path(m_Name)) 
        {
            PrintError("open","Cannot find path to file");
            return false;
        }
#endif
        
        QByteArray nameArr(m_latinNameEncoding ? m_Name.toLatin1() : m_Name.toUtf8());
        if(m_Flags & e_BIOStream_FileCreate) 
        {
            m_File = ::open(nameArr.constData(),flags | O_CREAT | O_TRUNC,mode);
        }
        else 
        {
            if(m_Flags & e_BIOStream_FileWrite)
            {
                m_File = ::open(nameArr.constData(),flags | O_CREAT,mode);
                if(m_File==c_invalidFile && !m_latinNameEncoding)
                {
                    m_File = ::open(m_Name.toLatin1().constData(),flags | O_CREAT,mode);
                    if(m_File!=c_invalidFile)
                    {
                        m_latinNameEncoding = true;
                    }
                }
            }
            else
            {
                m_File = ::open(nameArr.constData(),flags,mode);
                if(m_File==c_invalidFile && !m_latinNameEncoding)
                {
                    m_File = ::open(m_Name.toLatin1().constData(),flags,mode);
                    if(m_File!=c_invalidFile)
                    {
                        m_latinNameEncoding = true;
                    }
                }
            }
        }

        if(m_File==c_invalidFile)
        {
            PrintError("open","Failed to open file '","'",errno);
            return false;
        }
        m_Position=0;
    }
    catch(...)
    {
        PrintError("open","Exception thrown");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

bool BIOStream::open(const tchar *name)
{
    return open(QString::fromUtf8(name));
}

//-------------------------------------------------------------------------------------------

bool BIOStream::open(const BString& name)
{
    return open(QString::fromUtf8(static_cast<const tchar *>(name)));
}

//-------------------------------------------------------------------------------------------

bool BIOStream::close()
{
    bool res = true;
    
    try
    {
        if(m_File!=c_invalidFile)
        {
#if defined(OMEGA_WIN32)
            if(::CloseHandle(m_File)==0)
#elif defined(OMEGA_POSIX)
            if(::close(m_File)!=0)
#endif
            {
                PrintError("Close","Error closing file");
                res = false;
            }

#if defined(OMEGA_MAC_STORE)
            if(m_checkOutFlag)
            {
                bool readOnlyFlag = (m_Flags & e_BIOStream_FileWrite) ? false : true;
                SBBookmarkPtr sbBookmark = SBBookmark::get();
                sbBookmark->checkIn(m_Name,readOnlyFlag);
            }
#endif

            m_File = c_invalidFile;
        }
        m_Position = 0;

        m_Bookmarks.clear();
    }
    catch(...)
    {
        PrintError("close","Exception thrown");
        m_File = c_invalidFile;
        m_Position = 0;
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tint BIOStream::read(tbyte *mem,tint len)
{
    tint amount = -1;

    try
    {
        if(m_File==c_invalidFile)
        {
            PrintError("read","No file is open to from");
            return -1;
        }
        if(mem==0)
        {
            PrintError("read","No memory buffer given to read data into");
            return -1;
        }
        if(!(m_Flags & e_BIOStream_FileRead))
        {
            PrintError("read","Cannot read from '","' as it has not been opened with read permissions");
            return -1;
        }

#if defined(OMEGA_WIN32)

        BOOL res;

        res = ::ReadFile(m_File,mem,static_cast<DWORD>(len),reinterpret_cast<LPDWORD>(&amount),0);
        if(res==0)
        {
            PrintError("read","Error reading from '","'",static_cast<int>(::GetLastError()));
            return -1;
        }
    
#elif defined(OMEGA_POSIX)
        
        amount = static_cast<tint>(::read(m_File,mem,static_cast<size_t>(len)));
        if(amount==-1)
        {
            PrintError("read","Error reading from '","'",errno);
            return -1;
        }
#endif
        m_Position += amount;

    }
    catch(...)
    {
        PrintError("read","Exception thrown");
        amount = -1;
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

tint BIOStream::read(tubyte *mem,tint len)
{
    return read(reinterpret_cast<tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

tint BIOStream::write(const tbyte *mem,tint len)
{
    tint amount = -1;

    try
    {
        if(m_File==c_invalidFile)
        {
            PrintError("write","No file is open to write too");
            return -1;
        }
        if(mem==0)
        {
            PrintError("write","No memory given to write from");
            return -1;
        }
        if(!(m_Flags & e_BIOStream_FileWrite))
        {
            PrintError("write","Cannot write to '","' as it has been opened with write permissions");
            return -1;
        }

        m_Position += len;

#if defined(OMEGA_WIN32)

        BOOL res;

        res = ::WriteFile(m_File,mem,static_cast<DWORD>(len),reinterpret_cast<LPDWORD>(&amount),0);
        if(res==0 || amount!=len)
        {
            PrintError("write","Error writing to '","'",static_cast<int>(::GetLastError()));
            return -1;
        }
    
#elif defined(OMEGA_POSIX)
        
        amount = static_cast<tint>(::write(m_File,mem,static_cast<size_t>(len)));
        if(amount==-1 || amount!=len)
        {
            PrintError("write","Error writing to '","'",errno);
            return -1;
        }
#endif

    }
    catch(...)
    {
        PrintError("write","Exception thrown");
        amount = -1;
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

tint BIOStream::write(const tubyte *mem,tint len)
{
    return write(reinterpret_cast<const tbyte *>(mem),len);
}

//-------------------------------------------------------------------------------------------

bool BIOStream::seek(tint pos,BIOStreamPosition flag)
{
    return seek64(static_cast<tint64>(pos),flag);
}

//-------------------------------------------------------------------------------------------

bool BIOStream::isValidSeek(tint64& pos,tint64 currentPosition,BIOStreamPosition flag)
{
    bool res = true;

    switch(flag)
    {
        case e_Seek_Start:
            if(pos<0)
            {
                PrintError("seek","Cannot seek to position beyond beginning of file");
                res = false;
            }
            else if(pos>size64())
            {
                PrintError("seek","Cannot seek to position beyond end of file");
                res = false;
            }
            break;

        case e_Seek_Current:
            pos = currentPosition + pos;
            if(pos<0)
            {
                PrintError("seek","Cannot seek to position beyond beginning of file");
                res = false;
            }
            else if(pos>size64())
            {
                PrintError("seek","Cannot seek to position beyond end of file");
                res = false;
            }
            break;

        case e_Seek_End:
            if(pos<=0)
            {
                pos = size64() + pos;
                if(pos<0)
                {
                    PrintError("seek","Cannot seek to position beyond beginning of file");
                    res = false;
                }
            }
            else
            {
                PrintError("seek","Cannot seek to position beyond end of file");
                res = false;
            }
            break;

        default:
            PrintError("seek","Unknown position flag given");
            res = false;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOStream::seek64(tint64 pos,BIOStreamPosition flag)
{
    bool res = isValidSeek(pos,m_Position,flag);

    if(res)
    {
#if defined(OMEGA_WIN32)
        LONG posL,posH;
        
        posL = static_cast<LONG>(pos & 0x00000000ffffffffULL);
        posH = static_cast<LONG>((pos >> 32) & 0x00000000ffffffffULL);

        posL = win32SetFilePointer(getFileHandle(),posL,&posH,FILE_BEGIN);

        if(posL!=INVALID_SET_FILE_POINTER)
        {
            m_Position = (static_cast<tint64>(posH) << 32) & 0xffffffff00000000ULL;
            m_Position |= static_cast<tint64>(posL) & 0x00000000ffffffffULL;
        }
        else
        {
            PrintError("seek","Failed to move file position in '","'",static_cast<int>(win32GetLastError()));
            res = false;
        }
#elif defined(OMEGA_POSIX)
        pos = posixLSeek(getFileHandle(),static_cast<off_t>(pos),SEEK_SET);

        if(pos>=0)
        {
            m_Position = pos;
        }
        else
        {
            PrintError("seek","Failed to move file position in '","'",posixErrno());
            res = false;
        }
#endif    
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOStream::sof()
{
    return (m_Position==0) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool BIOStream::eof()
{
    return (m_Position>=size()) ? true : false;
}

//-------------------------------------------------------------------------------------------

tint BIOStream::size()
{
    return static_cast<tint>(size64());
}

//-------------------------------------------------------------------------------------------

tint64 BIOStream::size64()
{
    tint64 s = -1;

    if(getFileHandle()!=c_invalidFile)
    {

#if defined(OMEGA_WIN32)
        DWORD high,low;
        
        high = 0;
        low = win32GetFileSize(getFileHandle(),&high);
        if(low==INVALID_FILE_SIZE && win32GetLastError()!=NO_ERROR)
        {
            PrintError("size","Failed to obtain file size from '","'",static_cast<int>(win32GetLastError()));
        }
        else
        {
            tuint64 v;
            v  = (static_cast<tuint64>(high) << 32) & 0xffffffff00000000ULL;
            v |= (static_cast<tuint64>(low )      ) & 0x00000000ffffffffULL;
            s = static_cast<tint64>(v);
        }

#elif defined(OMEGA_POSIX)
        off_t val;

        val = posixLSeek(getFileHandle(),0,SEEK_END);
        if(val!=-1)
        {
            off_t nPos;
            
            nPos = posixLSeek(getFileHandle(),static_cast<off_t>(m_Position),SEEK_SET);
            if(nPos >= 0)
            {
                s = static_cast<tint64>(val);
                m_Position = static_cast<tint64>(nPos);
            }
            else
            {
                PrintError("size","Failed to seek current position in '","'",posixErrno());
            }
        }
        else
        {
            PrintError("size","Failed to seek to end of file position in '","'",posixErrno());
        }
        
#endif

    }
    else
    {
        PrintError("size","No file open to get size from");
    }
    return s;
}

//-------------------------------------------------------------------------------------------

tint BIOStream::bookmark(tint offset)
{
    tint nextID = m_nextBookmarkID;
    tint64 pos = m_Position + offset;
    m_Bookmarks.insert(nextID,pos);
    m_nextBookmarkID++;
    return nextID;
}

//-------------------------------------------------------------------------------------------

void BIOStream::deleteBookmark(tint bk)
{
    QMap<tint,tint64>::iterator ppI = m_Bookmarks.find(bk);
    if(ppI!=m_Bookmarks.end())
    {
        m_Bookmarks.erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

bool BIOStream::position(tint bk)
{
    bool res;
    QMap<tint,tint64>::iterator ppI = m_Bookmarks.find(bk);
    if(ppI!=m_Bookmarks.end())
    {
        res = seek64(ppI.value(),e_Seek_Start);
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOStream::readable() const
{
    return (m_Flags & e_BIOStream_FileRead) ? true : false;
}

//-------------------------------------------------------------------------------------------

bool BIOStream::writeable() const
{
    return (m_Flags & e_BIOStream_FileWrite) ? true : false;
}

//-------------------------------------------------------------------------------------------

tint BIOStream::offset() const
{
    return static_cast<tint>(offset64());
}

//-------------------------------------------------------------------------------------------

tint64 BIOStream::offset64() const
{
    return m_Position;
}

//-------------------------------------------------------------------------------------------

file_type BIOStream::getFileHandle() const
{
    return m_File;
}

//-------------------------------------------------------------------------------------------
#if defined(OMEGA_WIN32)
//-------------------------------------------------------------------------------------------

DWORD BIOStream::win32GetFileSize(HANDLE hFile,LPDWORD pHigh)
{
    return ::GetFileSize(hFile,pHigh);
}

//-------------------------------------------------------------------------------------------

DWORD BIOStream::win32GetLastError()
{
    return ::GetLastError();
}

//-------------------------------------------------------------------------------------------

DWORD BIOStream::win32SetFilePointer(HANDLE hFile,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod)
{
    return ::SetFilePointer(hFile,lDistanceToMove,lpDistanceToMoveHigh,dwMoveMethod);
}

//-------------------------------------------------------------------------------------------
#elif defined(OMEGA_POSIX)
//-------------------------------------------------------------------------------------------

off_t BIOStream::posixLSeek(int filedes,off_t offset,int whence)
{
    return ::lseek(filedes,offset,whence);
}

//-------------------------------------------------------------------------------------------

tint BIOStream::posixErrno() const
{
    return errno;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
#if defined(OMEGA_MAC_STORE)
//-------------------------------------------------------------------------------------------

void BIOStream::setCheckOutFlag(bool flag)
{
    m_checkOutFlag = flag;
}

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------

