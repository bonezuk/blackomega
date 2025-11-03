#include "playerapp/playercommon/inc/QAlbumTrackListModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

QAlbumTrackListModel::QAlbumTrackListModel(QObject *parent) : QOmegaListModel(parent),
    m_pTracks()
{}

//-------------------------------------------------------------------------------------------

QAlbumTrackListModel::~QAlbumTrackListModel()
{}

//-------------------------------------------------------------------------------------------

void QAlbumTrackListModel::load(const track::model::TrackModelKey& key)
{
    QSharedPointer<track::model::AlbumTrackModel> pTracks(new track::model::AlbumTrackModel(key));
    if(pTracks->build())
    {
        beginResetModel();
        m_pTracks = pTracks;
        endResetModel();
        Q_EMIT onSizeOfModel();
    }
}

//-------------------------------------------------------------------------------------------

int QAlbumTrackListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid() || m_pTracks.isNull())
    {
        return 0;
    }
    return m_pTracks->size();
}

//-------------------------------------------------------------------------------------------

QVariant QAlbumTrackListModel::data(const QModelIndex& index, int role) const
{
    QVariant v;

    if(!m_pTracks.isNull())
    {
        if(index.isValid() && index.row() >= 0 && index.row() < m_pTracks->size())
        {
            if(role == Qt::DisplayRole || role == TrackRole)
            {
                v = m_pTracks->data(index.row(), static_cast<int>(track::model::AlbumTrackModel::e_trackName));
            }
            else if(role == AlbumRole)
            {
                v = m_pTracks->data(index.row(), static_cast<int>(track::model::AlbumTrackModel::e_albumName));
            }
            else if(role == FileRole)
            {
                v = m_pTracks->data(index.row(), static_cast<int>(track::model::AlbumTrackModel::e_fileName));
            }
            else if(role == LengthRole)
            {
                v = m_pTracks->data(index.row(), static_cast<int>(track::model::AlbumTrackModel::e_length));
            }
            else if(role == ArtistRole)
            {
                v = m_pTracks->data(index.row(), static_cast<int>(track::model::AlbumTrackModel::e_artist));
            }
            else if(role == OriginalArtistRole)
            {
                v = m_pTracks->data(index.row(), static_cast<int>(track::model::AlbumTrackModel::e_originalArtist));
            }
            else if(role == ComposerRole)
            {
                v = m_pTracks->data(index.row(), static_cast<int>(track::model::AlbumTrackModel::e_composer));
            }
        }
    }
    return v;
}

//-------------------------------------------------------------------------------------------

qint32 QAlbumTrackListModel::getSizeOfModel() const
{
    return (!m_pTracks.isNull()) ? m_pTracks->size() : 0;
}

//-------------------------------------------------------------------------------------------

QHash<int,QByteArray> QAlbumTrackListModel::roleNames() const
{
    QHash<int,QByteArray> h;

    h[TrackRole] = "track";
    h[AlbumRole] = "album";
    h[FileRole] = "file";
    h[LengthRole] = "length";
    h[ArtistRole] = "artist";
    h[OriginalArtistRole] = "originalArtist";
    h[ComposerRole] = "composer";
    return h;
}

//-------------------------------------------------------------------------------------------

void QAlbumTrackListModel::endInsertRows()
{
    QAbstractListModel::endInsertRows();
    Q_EMIT onSizeOfModel();
}

//-------------------------------------------------------------------------------------------

void QAlbumTrackListModel::endRemoveRows()
{
    QAbstractListModel::endRemoveRows();
    Q_EMIT onSizeOfModel();
}

//-------------------------------------------------------------------------------------------

void QAlbumTrackListModel::appendTrack(const QString& fileName)
{
    if(!m_pTracks.isNull() && common::DiskOps::exist(fileName))
    {
        QSharedPointer<track::db::DBInfo> pInfo = track::db::DBInfo::readInfo(fileName).dynamicCast<track::db::DBInfo>();
        if(!pInfo.isNull())
        {
            QVector<QPair<tint, tint> > indexes = m_pTracks->indexForDBInfo(pInfo, true);
            for(int i = indexes.size()-1; i >= 0; i--)
            {
                QPair<tint, tint> idx = indexes.at(i);
                beginInsertRows(QModelIndex(), idx.first, idx.first);
                m_pTracks->addDBInfo(idx.first, idx.second, pInfo);
                endInsertRows();
            }
            if(!indexes.isEmpty())
            {
                submit();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QAlbumTrackListModel::deleteTrack(const QString& fileName)
{
    if(!m_pTracks.isNull())
    {
        QSharedPointer<track::db::DBInfo> pInfo = track::db::DBInfo::readInfo(fileName, false).dynamicCast<track::db::DBInfo>();
        if(!pInfo.isNull())
        {
            QVector<QPair<tint, tint> > indexes = m_pTracks->indexForDBInfo(pInfo, false);
            for(int i = indexes.size()-1; i >= 0; i--)
            {
                QPair<tint, tint> idx = indexes.at(i);
                beginRemoveRows(QModelIndex(), idx.first, idx.first);
                m_pTracks->removeRow(idx.first);
                endRemoveRows();
            }
            if(m_pTracks->size() == 0)
            {
                m_pTracks.clear();
            }
            if(!indexes.isEmpty())
            {
                submit();
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QAlbumTrackListModel::appendTrackToPlaylist(int index)
{
    if(!m_pTracks.isNull())
    {
        QString fileName = m_pTracks->data(index, track::model::AlbumTrackModel::e_fileName).toString();
        if(!fileName.isEmpty() && common::DiskOps::exist(fileName))
        {
            Q_EMIT appendToPlaylist(fileName);
        }
    }
}

//-------------------------------------------------------------------------------------------

void QAlbumTrackListModel::appendAlbumToPlaylist()
{
    if(!m_pTracks.isNull())
    {
        for(int idx = 0; idx < m_pTracks->size(); idx++)
        {
            QString fileName = m_pTracks->data(idx, track::model::AlbumTrackModel::e_fileName).toString();
            if(!fileName.isEmpty() && common::DiskOps::exist(fileName))
            {
                Q_EMIT appendToPlaylist(fileName);
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

void QAlbumTrackListModel::resetAndReload(bool isReload)
{
    beginResetModel();
    m_pTracks.clear();
    endResetModel();
    Q_EMIT onSizeOfModel();
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
