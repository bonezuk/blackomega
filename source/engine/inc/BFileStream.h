#ifndef __OMEGA_BLACKOMEGA_BFILESTREAM_H
#define __OMEGA_BLACKOMEGA_BFILESTREAM_H

#include "engine/inc/EngineDLL.h"

#if defined(OMEGA_WINDOWS)

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

#include "common/inc/BOTree.h"
#include "common/inc/BString.h"
#include "common/inc/DLong.h"
#include "common/inc/BIOStream.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

#if defined(OMEGA_WINDOWS)

typedef HANDLE file_type;
const file_type c_invalidFile = INVALID_HANDLE_VALUE;

#elif defined(OMEGA_POSIX)

typedef tint file_type;
const file_type c_invalidFile = -1;

#endif

//-------------------------------------------------------------------------------------------

typedef enum
{
    e_StreamNormal = 0,
    e_StreamICY
} BStreamType;

//-------------------------------------------------------------------------------------------

typedef enum MAZ_PositionID
{
    FROM_START        =    1,
    FROM_CURRENT    =    2,
    FROM_END        =    3
} MAZ_PositionID;

//-------------------------------------------------------------------------------------------

#if defined(OMEGA_DEFINE_TEMPLATE_HEADER)
#if !defined(COMP_UNIT_BFileStream)
COMMON_EXPORT_TEMPLATE template class ENGINE_EXPORT common::BOTree<tint,common::DLong>;
#else
template class ENGINE_EXPORT common::BOTree<tint,common::DLong>;
#endif
#endif

//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT BFileStream
{
    public:
        BFileStream(); //
        BFileStream(bool readOnly); //
        BFileStream(BStreamType type,bool readOnly); //
        virtual ~BFileStream(); //

        virtual BStreamType Type() const; //

        virtual bool Open(const tchar *name); //
        virtual bool Open(const common::BString& name); //

        virtual void Close(); //

        virtual tint Write(const tchar *mem,tint len); //

        virtual tint Read(tchar *mem,tint len); //

        virtual bool ReadOnly() const; //

        virtual tint Bookmark(tint offset=0); //

        virtual bool Position(tint bookmark); //

        virtual void DeleteBookmark(tint bookmark); //

        virtual bool Seek(tint offset,MAZ_PositionID from); //

        virtual bool Seek(const common::DLong& offset,MAZ_PositionID from); //

        virtual common::DLong Size(); //

        virtual const common::BString& Name() const; //

    protected:
        BStreamType m_Type;

        bool m_readOnly;

        common::BString m_Name;

        file_type m_File;

        common::DLong m_Position;

        common::BOTree<tint,common::DLong> m_Bookmarks;

        virtual void PrintError(const tchar *strR,const tchar *strE) const; //
        virtual void PrintError(const tchar *strR,const tchar *strE1,const common::BString& name,const tchar *strE2=0) const; //

        virtual bool Path(const common::BString& name); //
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------

#endif
