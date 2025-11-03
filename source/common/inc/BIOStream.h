//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_BIOSTREAM_H
#define __OMEGA_COMMON_BIOSTREAM_H
//-------------------------------------------------------------------------------------------

#include "common/inc/CommonTypes.h"

#if defined(OMEGA_WIN32)

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#elif defined(OMEGA_POSIX)

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

#endif

#include "common/inc/BString.h"
#include "common/inc/BOTree.h"

#include <QString>
#include <QMap>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

const tint e_BIOStream_FileCreate = 0x1;
const tint e_BIOStream_FileRead   = 0x2;
const tint e_BIOStream_FileWrite  = 0x4;

//-------------------------------------------------------------------------------------------

typedef enum
{
    e_Seek_Start = 1,
    e_Seek_Current,
    e_Seek_End
} BIOStreamPosition;

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WIN32)

typedef HANDLE file_type;
const file_type c_invalidFile = INVALID_HANDLE_VALUE;

#elif defined(OMEGA_POSIX)

typedef int file_type;
const file_type c_invalidFile = -1;

#endif

//-------------------------------------------------------------------------------------------

#if defined (OMEGA_MSVC)
#pragma warning(disable:4231)
#endif

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_DEFINE_TEMPLATE_HEADER)
#if !defined(COMP_UNIT_BIOStream)
COMMON_EXPORT_TEMPLATE template class COMMON_EXPORT BOTree<tint,tint>;
#else
template class COMMON_EXPORT BOTree<tint,tint>;
#endif
#endif

class COMMON_EXPORT BIOStream
{
    public:
        BIOStream(tint flags);
        virtual ~BIOStream();

        virtual const QString& name() const;

        virtual bool open(const tchar *name);
        virtual bool open(const BString& name);
        virtual bool open(const QString& name);

        virtual bool close();

        virtual tint read(tbyte *mem,tint len);
        virtual tint read(tubyte *mem,tint len);

        virtual tint write(const tbyte *mem,tint len);
        virtual tint write(const tubyte *mem,tint len);

        virtual bool seek(tint pos,BIOStreamPosition flag);
        virtual bool seek64(tint64 pos,BIOStreamPosition flag);

        virtual bool sof();
        virtual bool eof();

        virtual tint size();
        virtual tint64 size64();

        virtual tint bookmark(tint offset=0);

        virtual void deleteBookmark(tint bk);

        virtual bool position(tint bk);

        virtual bool readable() const;
        virtual bool writeable() const;

        virtual tint offset() const;
        virtual tint64 offset64() const;

#if defined(OMEGA_MAC_STORE)
        virtual void setCheckOutFlag(bool flag);
#endif

    protected:

        file_type m_File;

        QString m_Name;
        tint m_Flags;
        tint64 m_Position;

        QMap<tint,tint64> m_Bookmarks;

        tint m_nextBookmarkID;

#if defined(OMEGA_MAC_STORE)
        bool m_checkOutFlag;
#endif
#if defined(OMEGA_POSIX)
        bool m_latinNameEncoding;
#endif

        virtual void PrintError(const tchar *strR,const tchar *strE) const;
        virtual void PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2) const;
        virtual void PrintError(const tchar *strR,const tchar *strE1,const tchar *strE2,tint code) const;

        virtual bool path(QString& name);

        virtual file_type getFileHandle() const;
#if defined(OMEGA_WIN32)
        virtual DWORD win32GetFileSize(HANDLE hFile,LPDWORD pHigh);
        virtual DWORD win32GetLastError();
        virtual DWORD win32SetFilePointer(HANDLE hFile,LONG lDistanceToMove,PLONG lpDistanceToMoveHigh,DWORD dwMoveMethod);
#elif defined(OMEGA_POSIX)
        virtual off_t posixLSeek(int filedes,off_t offset,int whence);
        virtual tint posixErrno() const;
#endif

        bool isValidSeek(tint64& pos,tint64 currentPosition,BIOStreamPosition flag);
};

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
