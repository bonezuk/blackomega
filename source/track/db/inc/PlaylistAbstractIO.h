//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_DB_PLAYLISTABSTRACTIO_H
#define __OMEGA_TRACK_DB_PLAYLISTABSTRACTIO_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include <QCoreApplication>
#include <QDir>
#include <QUrl>

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/xpath.h>
#include <libxml/xpathInternals.h>
#include <libxml/tree.h>
#include <libxml/uri.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>

#include <QString>
#include <QStringList>
#include <QUrl>

#include "common/inc/Factory.h"
#include "common/inc/BOParse.h"
#include "common/inc/DiskOps.h"
#include "common/inc/BIOBufferedStream.h"
#include "track/info/inc/Info.h"
#include "track/db/inc/DBInfo.h"
#include "track/db/inc/TrackDB.h"
#include "common/inc/AbstractProgressInterface.h"

#if defined(OMEGA_MAC_STORE)
#include "widget/inc/SandBoxMac.h"
#include "track/db/inc/SBBookmarkTrackDB.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace db
{
//-------------------------------------------------------------------------------------------

#define PLAYLIST_EXPORT

//-------------------------------------------------------------------------------------------

class TRACK_DB_EXPORT PlaylistAbstractIO
{
    public:
        PlaylistAbstractIO();
        virtual ~PlaylistAbstractIO();
        
        static bool isSupported(const QString& name);
        static QString factoryName(const QString& name);
        
        virtual bool load(const QString& fileName,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);
        virtual bool save(const QString& fileName,const QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);
        
        void setParent(QObject *w);
        void useMountedDrives();
        
    protected:

        QObject *m_parent;
        common::BOParse *m_pathParser;
        tint m_pathParserState[9];
        
        // A playlist file saved on another OS using the same share that has been mounted.
        bool m_useMountedDrives;
    
        virtual QByteArray readLine(common::BIOStream& pFile);
        virtual bool writeLine(common::BIOStream& pFile,const QString& line);
        
        virtual track::info::InfoSPtr getTrack(const QString& fileName);
        
        QString getFilePath(const QString& inName,const QDir& homeDir,bool commentFlag);
        
        void appendToList(const QString& lPath,QVector<track::info::InfoSPtr>& pList,common::AbstractProgressInterface *progress);

        QString getURLFilename(const QString& uPath);
        
        QString findFileFromMounts(const QString& fileName);
};

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(TRACK_DB_EXPORT, PlaylistIOFactory, PlaylistAbstractIO)

//-------------------------------------------------------------------------------------------
} // namespace db
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
