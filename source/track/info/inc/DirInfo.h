//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_DIRINFO_H
#define __OMEGA_TRACK_INFO_DIRINFO_H
//-------------------------------------------------------------------------------------------

#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QMap>
#include <QPair>

#include "common/inc/BIOStream.h"
#include "track/info/inc/Info.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT DirInfo
{
    public:
        typedef QMap<QString , QPair<QFileInfo,tint> > Map;

    public:
        DirInfo();
        DirInfo(const QString& name);
        DirInfo(const DirInfo& info);

        static DirInfo& directory(const tchar *name);
        static DirInfo& directory(const QString& dName);

        const DirInfo& operator = (const DirInfo& info);

        bool isNull() const;

        bool exist(const tchar *name) const;
        bool exist(const QString& name) const;

        const QString& name() const;

        const Map& list() const;

        const QFileInfo& info(const tchar *name) const;
        const QFileInfo& info(const QString& name) const;

        tint index(const tchar *name) const;
        tint index(const QString& name) const;

        void refresh();

    protected:

        static QMap<QString,DirInfo> m_cacheMap;
        static DirInfo m_nullDir;
        static QFileInfo m_nullFile;

        QString m_name;
        Map m_files;

        void copy(const DirInfo& info);
        bool read();
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
