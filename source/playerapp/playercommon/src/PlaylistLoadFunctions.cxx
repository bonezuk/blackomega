#include "network/inc/Resource.h"

#include <QCoreApplication>
#include <QFileInfo>
#include <QDir>

#include "playerapp/playercommon/inc/PlaylistLoadFunctions.h"
#include "playerapp/playercommon/inc/CLIProgress.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

QString mountPointFromArguments(const QStringList& args)
{
    int idx, state;
    QString name;

    for(idx = 0, state = 0; idx < args.size() && name.isEmpty(); idx++)
    {
        if(args.at(idx) == "--add-mount")
        {
            state = 1;
        }
        else if(state == 1)
        {
            name = args.at(idx);
        }
    }
    return name;
}

//-------------------------------------------------------------------------------------------

QString playlistFromArguments(const QStringList& args)
{
    int idx, state;
    QString name;

    for(idx = 0, state = 0; idx < args.size() && name.isEmpty(); idx++)
    {
        if(args.at(idx) == "-p")
        {
            state = 1;
        }
        else if(state == 1)
        {
            name = args.at(idx);
#if QT_VERSION >= 0x050000
            if(!QFileInfo::exists(name))
#else
            if(!QFileInfo(name).exists())
#endif
            {
                name = QString();
            }
        }
    }
    return name;
}

//-------------------------------------------------------------------------------------------

bool rebuildDatabaseFromArguments(const QStringList& args)
{
    int idx;
    bool rebuild = false;

    for(idx = 0; idx < args.size() && !rebuild; idx++)
    {
        if(args.at(idx) == "--rebuild")
        {
            rebuild = true;
        }
    }
    return rebuild;
}

//-------------------------------------------------------------------------------------------

void playlistToDBList(QVector<track::info::InfoSPtr>& playList, QVector<QPair<track::db::DBInfoSPtr,tint> >& playListDB)
{
    for(QVector<track::info::InfoSPtr>::iterator ppI = playList.begin(); ppI != playList.end(); ppI++)
    {
        track::info::InfoSPtr pFInfo = *ppI;
        track::db::DBInfoSPtr pDBInfo = pFInfo.dynamicCast<track::db::DBInfo>();
        if(pDBInfo.isNull())
        {
            pFInfo = track::db::DBInfo::readInfo(pFInfo->getFilename());
            if(!pFInfo.isNull())
            {
                pDBInfo = pFInfo.dynamicCast<track::db::DBInfo>();
            }
        }
        if(!pDBInfo.isNull())
        {
            if(pDBInfo->isChildren())
            {
                for(int i = 0; i < pDBInfo->noChildren(); i++)
                {
                    playListDB.append(QPair<track::db::DBInfoSPtr,tint>(pDBInfo, i));
                }
            }
            else
            {
                playListDB.append(QPair<track::db::DBInfoSPtr,tint>(pDBInfo, -1));
            }
        }
    }
}


//-------------------------------------------------------------------------------------------

void playlistSubtrackToDBList(QVector<QPair<track::info::InfoSPtr, tint> >& playList, QVector<QPair<track::db::DBInfoSPtr,tint> >& playListDB)
{
    for(QVector<QPair<track::info::InfoSPtr, tint> >::iterator ppI = playList.begin(); ppI != playList.end(); ppI++)
    {
        int subTrack = (*ppI).second;
        track::info::InfoSPtr pFInfo = (*ppI).first;
        track::db::DBInfoSPtr pDBInfo = pFInfo.dynamicCast<track::db::DBInfo>();
        if(pDBInfo.isNull())
        {
            pFInfo = track::db::DBInfo::readInfo(pFInfo->getFilename());
            if(!pFInfo.isNull())
            {
                pDBInfo = pFInfo.dynamicCast<track::db::DBInfo>();
            }
        }
        if(!pDBInfo.isNull())
        {
            playListDB.append(QPair<track::db::DBInfoSPtr,tint>(pDBInfo, subTrack));
        }
    }
}

//-------------------------------------------------------------------------------------------

bool loadPlaylistFromDBOrArguments(const QStringList& args, QVector<QPair<track::db::DBInfoSPtr,tint> >& playListDB)
{
    QString plFilename;
    QVector<track::info::InfoSPtr> playList;
    track::db::TrackDB *db = track::db::TrackDB::instance();
    bool res = false;

    plFilename = playlistFromArguments(args);
    if(!plFilename.isEmpty())
    {
        if(track::db::PlaylistAbstractIO::isSupported(plFilename))
        {
            QSharedPointer<track::db::PlaylistAbstractIO> pLoader = track::db::PlaylistIOFactory::createShared(track::db::PlaylistAbstractIO::factoryName(plFilename));
            if(pLoader.data()!=0)
            {
                CLIProgress progressBar;

#if defined(OMEGA_LINUX)
                pLoader->useMountedDrives();
#endif
                if(pLoader->load(plFilename, playList, &progressBar))
                {
                    playlistToDBList(playList, playListDB);
                    res = true;
                }
                else
                {
                    common::Log::g_Log << "Failed to load playlist '" << plFilename << "'." << common::c_endl;
                }
            }
            else
            {
                common::Log::g_Log << "Failed to create playlist loader for '" << plFilename << "'." << common::c_endl;
            }
        }
        else
        {
            common::Log::g_Log << "Unsupported playlist format for '" << plFilename << "'." << common::c_endl;
        }
    }

    if(!res && db != NULL)
    {
        QMap<tint,QString> dbPlaylists = db->playlists();
        if(!dbPlaylists.isEmpty())
        {
            QVector<QPair<track::info::InfoSPtr, tint> > playListSubtracks;
#if QT_VERSION >= 0x050000
            int playlistID = dbPlaylists.lastKey();
#else
            int playlistID = dbPlaylists.isEmpty() ? -1 : dbPlaylists.keys().last();
#endif
            common::Log::g_Log << "Loading previous playlist from database." << common::c_endl;
            playList.clear();

            if(db->loadPlaylist(playlistID, playListSubtracks))
            {
                playlistSubtrackToDBList(playListSubtracks, playListDB);
                res = true;
            }
            else
            {
                common::Log::g_Log << "Failed to load pervious playlist from database." << common::c_endl;
            }
        }
        else
        {
            common::Log::g_Log << "No playlist in database. Use -p <playlist> to define a playlist." << common::c_endl;
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
