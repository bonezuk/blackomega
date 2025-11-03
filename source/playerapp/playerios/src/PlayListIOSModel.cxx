#include "playerapp/playerios/inc/PlayListIOSModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlayListIOSModel::PlayListIOSModel(QObject *parent) : PlayListModel(parent)
{}

//-------------------------------------------------------------------------------------------

PlayListIOSModel::PlayListIOSModel(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent) : PlayListModel(pAudioInterface, parent)
{}

//-------------------------------------------------------------------------------------------

PlayListIOSModel::~PlayListIOSModel()
{}

//-------------------------------------------------------------------------------------------

void PlayListIOSModel::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "PlayListIOSModel::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool PlayListIOSModel::initialise()
{
    bool res = false;

    if(PlayListModel::initialise())
    {
        if(loadPlaylistFromDB())
        {
            QSharedPointer<PlaybackStateController> pPlaybackState(new PlaybackStateController(m_pAudioInterface, this));
            m_pPlaybackState = pPlaybackState;
            res = true;
        }
        else
        {
            printError("initialise", "Failed to load playlist from database");
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PlayListIOSModel::removeAtIndex(int index)
{
    PlayListModel::removeAtIndex(index);
    savePlaylistToDB();
}

//-------------------------------------------------------------------------------------------

void PlayListIOSModel::appendTrack(const QString& fileName)
{
    if(common::DiskOps::exist(fileName))
    {
        QSharedPointer<track::db::DBInfo> pInfo = track::db::DBInfo::readInfo(fileName).dynamicCast<track::db::DBInfo>();
        if(!pInfo.isNull())
        {
            track::db::PlaylistTuple t;
            int nextPosition = sizeOfPlaylist();
            int albumID = pInfo->albumID();
            int trackID = pInfo->trackID();

            t.albumID = albumID;
            t.trackID = trackID;
            if(!pInfo->noChildren())
            {
                beginInsertRows(QModelIndex(), nextPosition, nextPosition);
                t.subtrackID = -1;
                t.itemID = generateNewId();
                appendPlaylistTuple(t);
            }
            else
            {
                beginInsertRows(QModelIndex(), nextPosition, nextPosition + (pInfo->noChildren() - 1));
                for(int i = 0; i < pInfo->noChildren(); i++)
                {
                    t.subtrackID = i;
                    t.itemID = generateNewId();
                    appendPlaylistTuple(t);
                }
            }
            endInsertRows();

            savePlaylistToDB();
        }
    }
}

//-------------------------------------------------------------------------------------------

void PlayListIOSModel::deleteTrack(const QString& fileName)
{
    tint albumID, trackID;

    if(track::db::TrackDB::instance()->getKeysFromFilename(fileName, albumID, trackID))
    {
        for(tint i = 0; i < m_playList.size(); i++)
        {
            tuint64 itemID = m_playList.at(i);
            QMap<tuint64, QPair<track::db::DBInfoSPtr,tint> >::iterator ppJ;

            ppJ = m_items.find(itemID);
            if(ppJ != m_items.end())
            {
                track::db::DBInfoSPtr pDBItem = ppJ.value().first;
                if(albumID == pDBItem->albumID() && trackID == pDBItem->trackID())
                {
                    beginRemoveRows(QModelIndex(), i, i);

                    QMap<tuint64, tint>::iterator ppK = m_idToIndexMap.find(itemID);
                    if(ppK != m_idToIndexMap.end())
                    {
                        m_idToIndexMap.erase(ppK);
                    }
                    m_items.erase(ppJ);
                    m_playList.removeAt(i);

                    endRemoveRows();
                    i--;
                }
            }
        }
        savePlaylistToDB();
    }
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
