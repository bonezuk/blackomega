#include "playerapp/playercommon/inc/PlayListModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

PlayListModel::PlayListModel(QObject *parent) : QOmegaListModel(parent),
    m_playlistID(0),
    m_playlistName("Default"),
    m_items(),
    m_playList(),
    m_idToIndexMap(),
    m_pAudioInterface(),
    m_pPlaybackState()
{}

//-------------------------------------------------------------------------------------------

PlayListModel::PlayListModel(QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent) : QOmegaListModel(parent),
    m_playlistID(0),
    m_playlistName("Default"),
    m_items(),
    m_playList(),
    m_idToIndexMap(),
    m_pAudioInterface(pAudioInterface),
    m_pPlaybackState()
{}

//-------------------------------------------------------------------------------------------

PlayListModel::PlayListModel(QVector<QPair<track::db::DBInfoSPtr,tint> >& playList, QSharedPointer<OmegaAudioInterface>& pAudioInterface, QObject *parent) : QOmegaListModel(parent),
    m_playlistID(0),
    m_playlistName("Default"),
    m_items(),
    m_playList(),
    m_idToIndexMap(),
    m_pAudioInterface(pAudioInterface),
    m_pPlaybackState(0)
{
    PlayListModel::appendToPlaylist(playList);
}

//-------------------------------------------------------------------------------------------

PlayListModel::~PlayListModel()
{}

//-------------------------------------------------------------------------------------------

void PlayListModel::printError(const char *strR, const char *strE) const
{
    common::Log::g_Log << "PlayListModel::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool PlayListModel::initialise()
{
    return true;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<PlaybackStateController>& PlayListModel::playbackState()
{
    return m_pPlaybackState;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<OmegaAudioInterface>& PlayListModel::audioInterface()
{
    return m_pAudioInterface;
}

//-------------------------------------------------------------------------------------------

tuint64 PlayListModel::generateNewId() const
{
    tuint64 r;
    QMap<tuint64, tint>::const_iterator ppI;

    do
    {
        r = track::db::TrackDB::instance()->newPlaylistItemID();
        ppI = m_idToIndexMap.find(r);
        if(ppI != m_idToIndexMap.constEnd())
        {
            r = 0;
        }
    } while(!r);
    return r;
}

//-------------------------------------------------------------------------------------------

void PlayListModel::appendToPlaylist(const QVector<QPair<track::db::DBInfoSPtr,tint> >& list)
{
    QVector<QPair<track::db::DBInfoSPtr,tint> >::const_iterator ppI;
    common::Random *rand = common::Random::instance();

    for(ppI = list.constBegin(); ppI != list.constEnd(); ppI++)
    {
        const QPair<track::db::DBInfoSPtr,tint>& item = *ppI;
        tuint64 id = generateNewId();
        m_items.insert(id, *ppI);
        m_idToIndexMap.insert(id, m_playList.size());
        m_playList.append(id);
    }
}

//-------------------------------------------------------------------------------------------

QString PlayListModel::titleOfItem(const QPair<track::db::DBInfoSPtr,tint>& item) const
{
    QString title;

    if(item.second >= 0 && item.second < item.first->noChildren())
    {
        title = item.first->child(item.second).name();
    }
    else
    {
        title = item.first->title();
    }
    return title;
}

//-------------------------------------------------------------------------------------------

QVariant PlayListModel::dataAtIndex(int row, const QString& roleName)
{
    QModelIndex idx = index(row, 0);

    if(roleName == "artist")
    {
        return data(idx, ArtistRole);
    }
    else if(roleName == "id")
    {
        return data(idx, IdRole);
    }
    else if(roleName == "title")
    {
        return data(idx, TitleRole);
    }
    else if(roleName == "album")
    {
        return data(idx, AlbumRole);
    }
    else if(roleName == "year")
    {
        return data(idx, YearRole);
    }
    else if(roleName == "comment")
    {
        return data(idx, CommentRole);
    }
    else if(roleName == "genre")
    {
        return data(idx, GenreRole);
    }
    else if(roleName == "track")
    {
        return data(idx, TrackRole);
    }
    else if(roleName == "disc")
    {
        return data(idx, DiscRole);
    }
    else if(roleName == "composer")
    {
        return data(idx, ComposerRole);
    }
    else if(roleName == "originalArtist")
    {
        return data(idx, OriginalArtistRole);
    }
    else if(roleName == "copyright")
    {
        return data(idx, CopyrightRole);
    }
    else if(roleName == "encoder")
    {
        return data(idx, EncoderRole);
    }
    else if(roleName == "length")
    {
        return data(idx, LengthRole);
    }
    else if(roleName == "image")
    {
        return data(idx, ImageRole);
    }
    return QVariant();
}

//-------------------------------------------------------------------------------------------

QVariant PlayListModel::data(const QModelIndex& index, int role) const
{
    if(index.isValid() && index.row() >= 0 && index.row() < m_playList.size())
    {
        tuint64 id = m_playList.at(index.row());

        if(role == IdRole)
        {
            return QVariant(id);
        }
        else
        {
            QMap<tuint64, QPair<track::db::DBInfoSPtr,tint> >::const_iterator ppI;

            ppI = m_items.find(id);
            if(ppI != m_items.constEnd())
            {
                track::db::DBInfoSPtr pInfo = ppI.value().first;

                if(role == Qt::DisplayRole)
                {
                    QString s = titleOfItem(ppI.value()) + " (" + pInfo->artist() + ")";
                    return QVariant(s);
                }
                else if(role == ArtistRole)
                {
                    return QVariant(pInfo->artist());
                }
                else if(role == TitleRole)
                {
                    QString s = titleOfItem(ppI.value());
                    return QVariant(s);
                }
                else if(role == AlbumRole)
                {
                    return QVariant(pInfo->album());
                }
                else if(role == YearRole)
                {
                    return QVariant(pInfo->year());
                }
                else if(role == CommentRole)
                {
                    return QVariant(pInfo->comment());
                }
                else if(role == GenreRole)
                {
                    return QVariant(pInfo->originalArtist());
                }
                else if(role == TrackRole)
                {
                    return QVariant(pInfo->track());
                }
                else if(role == DiscRole)
                {
                    return QVariant(pInfo->disc());
                }
                else if(role == ComposerRole)
                {
                    return QVariant(pInfo->composer());
                }
                else if(role == OriginalArtistRole)
                {
                    return QVariant(pInfo->originalArtist());
                }
                else if(role == CopyrightRole)
                {
                    return QVariant(pInfo->copyright());
                }
                else if(role == EncoderRole)
                {
                    return QVariant(pInfo->encoder());
                }
                else if(role == LengthRole)
                {
                    return QVariant(static_cast<tfloat64>(pInfo->length()));
                }
                else if(role == ImageRole)
                {
                    return QVariant(getImageIDAtIndex(index.row()));
                }
            }
            else
            {
                printError("data", "Index item not found in playlist map");
            }
        }
    }
    return QVariant(QString(""));
}

//-------------------------------------------------------------------------------------------

int PlayListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid())
    {
        return 0;
    }
    return sizeOfPlaylist();
}

//-------------------------------------------------------------------------------------------

int PlayListModel::sizeOfPlaylist() const
{
    return m_playList.size();
}

//-------------------------------------------------------------------------------------------

QHash<int,QByteArray> PlayListModel::roleNames() const
{
    QHash<int,QByteArray> h;
    h[ArtistRole] = "artist";
    h[IdRole] = "id";
    h[TitleRole] = "title";
    h[AlbumRole] = "album";
    h[YearRole] = "year";
    h[CommentRole] = "comment";
    h[GenreRole] = "genre";
    h[TrackRole] = "track";
    h[DiscRole] = "disc";
    h[ComposerRole] = "composer";
    h[OriginalArtistRole] = "originalArtist";
    h[CopyrightRole] = "copyright";
    h[EncoderRole] = "encoder";
    h[LengthRole] = "length";
    h[ImageRole] = "image";
    return h;
}

//-------------------------------------------------------------------------------------------

qint32 PlayListModel::indexFromId(tuint64 id) const
{
    qint32 idx = -1;
    QMap<tuint64, tint>::const_iterator ppI;

    ppI = m_idToIndexMap.find(id);
    if(ppI != m_idToIndexMap.constEnd())
    {
        idx = ppI.value();
    }
    return idx;
}

//-------------------------------------------------------------------------------------------

track::db::DBInfoSPtr PlayListModel::itemFromId(tuint64 id) const
{
    track::db::DBInfoSPtr pItem;
    QMap<tuint64, QPair<track::db::DBInfoSPtr,tint> >::const_iterator ppI;

    ppI = m_items.find(id);
    if(ppI != m_items.constEnd())
    {
        pItem = ppI.value().first;
    }
    return pItem;
}

//-------------------------------------------------------------------------------------------

tint PlayListModel::childIndexFromId(tuint64 id) const
{
    tint childIndex = -1;
    QMap<tuint64, QPair<track::db::DBInfoSPtr,tint> >::const_iterator ppI;

    ppI = m_items.find(id);
    if(ppI != m_items.constEnd())
    {
        childIndex = ppI.value().second;
    }
    return childIndex;
}

//-------------------------------------------------------------------------------------------

void PlayListModel::playItemAtIndexWithNext(int index, bool isNext)
{
    if(index >= 0 && index < m_playList.size())
    {
        tuint64 id = m_playList.at(index);
        track::db::DBInfoSPtr pItem = itemFromId(id);
        if(!pItem.isNull())
        {
            QString fileName = pItem->getFilename();
            tint childIdx = childIndexFromId(id);

            common::Log::g_Log.print("PlayListModel::playItemAtIndex - %d '%s'\n", index, fileName.toUtf8().constData());
            if(childIdx >= 0 && childIdx < pItem->noChildren())
            {
                m_pAudioInterface->playFileWithTime(fileName, pItem->child(childIdx).startTime(), pItem->child(childIdx).length(), isNext);
            }
            else
            {
                m_pAudioInterface->playFile(fileName, isNext);
            }
            m_pPlaybackState->setNextItem(id);
        }
    }
    else
    {
        QString err = QString("Given index, %1, out of range").arg(index);
        printError("playItemAtIndex", err.toUtf8().constData());
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::playItemAtIndex(int index)
{
    playItemAtIndexWithNext(index, false);
}

//-------------------------------------------------------------------------------------------

void PlayListModel::onPlayPausePressed()
{
    int playIndex;

    playIndex = m_pPlaybackState->getIndex();
    if(playIndex >= 0 && playIndex < m_playList.size())
    {
        m_pPlaybackState->resumeOrPausePlayback();
    }
    else
    {
        playItemAtIndex(0);
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::playNextItem(bool isNext)
{
    int nextIndex;

    nextIndex = m_pPlaybackState->getIndex();
    if(nextIndex < 0)
    {
        nextIndex = 0;
    }
    else
    {
        nextIndex++;
    }
    if(nextIndex >= 0 && nextIndex < m_playList.size())
    {
        playItemAtIndexWithNext(nextIndex, isNext);
    }
    else
    {
        if(!isNext)
        {
            m_pPlaybackState->onAudioStop();
        }
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::onPlayPrevious()
{
    int prevIndex;

    prevIndex = m_pPlaybackState->getIndex() - 1;
    if(prevIndex < 0)
    {
        m_pAudioInterface->stop();
    }
    else
    {
        playItemAtIndexWithNext(prevIndex, false);
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::onPlayNext()
{
    int nextIndex;

    nextIndex = m_pPlaybackState->getIndex() + 1;
    if(nextIndex >= 0 && nextIndex < m_playList.size())
    {
        playItemAtIndexWithNext(nextIndex, false);
    }
    else
    {
        m_pAudioInterface->stop();
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::onPlay()
{
    if(m_pPlaybackState->getState() == PlaybackStateController::Pause)
    {
        m_pAudioInterface->play();
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::onPause()
{
    if(m_pPlaybackState->getState() == PlaybackStateController::Play)
    {
        m_pAudioInterface->pause();
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::onStop()
{
    m_pAudioInterface->stop();
}

//-------------------------------------------------------------------------------------------

void PlayListModel::remove(int index)
{
    removeAtIndex(index);
}

//-------------------------------------------------------------------------------------------

void PlayListModel::removeAtIndex(int index)
{
    if(index >= 0 && index < m_playList.size())
    {
        tuint64 idxId = m_playList.at(index);

        if(idxId == m_pPlaybackState->getCurrentId())
        {
            int nextIndex = index + 1;
            if(nextIndex < m_playList.size())
            {
                playItemAtIndexWithNext(nextIndex, false);
            }
            else
            {
                m_pAudioInterface->stop();
            }
        }

        beginRemoveRows(QModelIndex(), index, index);

        QMap<tuint64, QPair<track::db::DBInfoSPtr,tint> >::iterator ppJ = m_items.find(idxId);
        if(ppJ != m_items.end())
        {
            m_items.erase(ppJ);
        }
        QMap<tuint64, tint>::iterator ppK = m_idToIndexMap.find(idxId);
        if(ppK != m_idToIndexMap.end())
        {
            m_idToIndexMap.erase(ppK);
        }
        m_playList.removeAt(index);

        while(index < m_playList.size())
        {
            ppK = m_idToIndexMap.find(m_playList.at(index));
            if(ppK != m_idToIndexMap.end())
            {
                ppK.value() -= 1;
            }
            index++;
        }

        endRemoveRows();
        submit();
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::clearAllPlaylist()
{
    while(m_playList.size() > 0)
    {
        removeAtIndex(0);
    }
}

//-------------------------------------------------------------------------------------------

qint32 PlayListModel::getSizeOfModel() const
{
    return static_cast<qint32>(m_playList.size());
}

//-------------------------------------------------------------------------------------------

void PlayListModel::endInsertRows()
{
    QAbstractListModel::endInsertRows();
    emit onSizeOfModel();
}

//-------------------------------------------------------------------------------------------

void PlayListModel::endRemoveRows()
{
    QAbstractListModel::endRemoveRows();
    emit onSizeOfModel();
}

//-------------------------------------------------------------------------------------------

void PlayListModel::appendPlaylistTuple(const track::db::PlaylistTuple& t)
{
    track::db::TrackDB *pDB = track::db::TrackDB::instance();

    if(pDB != 0)
    {
        QSharedPointer<track::info::Info> pInfoItem = track::db::DBInfo::readInfo(t.albumID, t.trackID);
        track::db::DBInfoSPtr pDBItem = pInfoItem.dynamicCast<track::db::DBInfo>();
        if(!pDBItem.isNull())
        {
            m_items.insert(t.itemID, QPair<track::db::DBInfoSPtr,tint>(pDBItem, t.subtrackID));
            m_idToIndexMap.insert(t.itemID, m_playList.size());
            m_playList.append(t.itemID);
        }
        else
        {
            QString err = QString("Failed to load albumID=%1, trackID=%2").arg(t.albumID, t.trackID);
            printError("loadPlaylistFromDB", err.toUtf8().constData());
        }
    }
    else
    {
        printError("appendPlaylistTuple", "Track database has not been instantiated");
    }
}


//-------------------------------------------------------------------------------------------

bool PlayListModel::loadPlaylistFromDB()
{
    bool res = false;
    track::db::TrackDB *pDB = track::db::TrackDB::instance();

    m_items.clear();
    m_playList.clear();
    m_idToIndexMap.clear();

    if(pDB != 0)
    {
        if(pDB->isPlaylist(m_playlistID))
        {
            QVector<track::db::PlaylistTuple> idPList;

            if(pDB->loadPlaylist(m_playlistID, idPList))
            {
                QVector<QPair<track::db::DBInfoSPtr,tint> > pList;

                for(QVector<track::db::PlaylistTuple>::iterator ppI = idPList.begin(); ppI != idPList.end(); ppI++)
                {
                    track::db::PlaylistTuple t = *ppI;
                    appendPlaylistTuple(t);
                }
                m_playlistName = pDB->playlist(m_playlistID);
                res = true;
            }
            else
            {
                printError("loadPlaylistFromDB", "Failed to load playlist from database");
            }
        }
        else
        {
            res = true;
        }
    }
    else
    {
        printError("loadPlaylistFromDB", "Track database has not been instantiated");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void PlayListModel::savePlaylistToDB()
{
    track::db::TrackDB *pDB = track::db::TrackDB::instance();

    if(pDB != 0)
    {
        QVector<track::db::PlaylistTuple> list;

        for(QVector<tuint64>::iterator ppI = m_playList.begin(); ppI != m_playList.end(); ppI++)
        {
            QMap<tuint64, QPair<track::db::DBInfoSPtr,tint> >::iterator ppJ;
            track::db::PlaylistTuple t;

            t.itemID = *ppI;
            ppJ = m_items.find(t.itemID);
            if(ppJ != m_items.end())
            {
                track::db::DBInfoSPtr pDBInfo = ppJ.value().first;
                if(!pDBInfo.isNull())
                {
                    t.albumID = pDBInfo->albumID();
                    t.trackID = pDBInfo->trackID();
                    t.subtrackID = ppJ.value().second;
                    list.append(t);
                }
                else
                {
                    printError("savePlaylistToDB", "Expected DB item is null pointer");
                }
            }
            else
            {
                QString err = QString("Expected item ID %1 not found in items map").arg(t.itemID);
                printError("savePlaylistToDB", err.toUtf8().constData());
            }
        }
        if(pDB->replacePlaylist(m_playlistID, m_playlistName, list) != m_playlistID)
        {
            printError("savePlaylistToDB", "Error saving playlist to database");
        }
    }
    else
    {
        printError("savePlaylistToDB", "Track database has not been instantiated");
    }
}

//-------------------------------------------------------------------------------------------

void PlayListModel::resetAndReload(bool isReload)
{
    beginResetModel();

    m_pAudioInterface->stop();
    m_items.clear();
    m_idToIndexMap.clear();
    m_playList.clear();
    if(isReload)
    {
        loadPlaylistFromDB();
    }

    endResetModel();
    emit onSizeOfModel();
}

//-------------------------------------------------------------------------------------------

int PlayListModel::getImageIDAtIndex(int idx) const
{
    int imageID = -1;
    if(idx >= 0 && idx < m_playList.size())
    {
        tuint64 id = m_playList.at(idx);
        QMap<tuint64, QPair<track::db::DBInfoSPtr,tint> >::const_iterator ppI = m_items.find(id);
        if(ppI != m_items.constEnd())
        {
            track::db::DBInfoSPtr pInfo = ppI.value().first;
            imageID = pInfo->getImageID();
        }
    }
    return imageID;
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
