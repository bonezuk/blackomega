#include "playerapp/playercommon/inc/QAlbumListModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

QAlbumListModel::QAlbumListModel(QObject *parent) : QOmegaListModel(parent),
    m_pAlbums()
{
    QSharedPointer<QAlbumTrackListModel> pTrackModel(new QAlbumTrackListModel());
    m_pTrackModel = pTrackModel;
}

//-------------------------------------------------------------------------------------------

QAlbumListModel::~QAlbumListModel()
{
    m_pTrackModel.clear();
}

//-------------------------------------------------------------------------------------------

int QAlbumListModel::rowCount(const QModelIndex& parent) const
{
    if(parent.isValid() || m_pAlbums.isNull())
    {
        return 0;
    }
    return m_pAlbums->size();
}

//-------------------------------------------------------------------------------------------

QVariant QAlbumListModel::data(const QModelIndex& index, int role) const
{
    QVariant v;
    
    if(!m_pAlbums.isNull())
    {
        if(index.isValid() && index.row() >= 0 && index.row() < m_pAlbums->size())
        {
            if(role == Qt::DisplayRole || role == AlbumRole)
            {
                v = m_pAlbums->data(index.row(), 1);
            }
            else if(role == ArtistRole)
            {
                v = m_pAlbums->data(index.row(), 2);
            }
            else if(role == YearRole)
            {
                v = m_pAlbums->data(index.row(), 3);
            }
            else if(role == ImageRole)
            {
                v = m_pAlbums->data(index.row(), 4);
            }
        }
    }
    return v;
}

//-------------------------------------------------------------------------------------------

QHash<int,QByteArray> QAlbumListModel::roleNames() const
{
    QHash<int,QByteArray> h;
    h[ArtistRole] = "artist";
    h[AlbumRole] = "album";
    h[YearRole] = "year";
    h[ImageRole] = "image";
    return h;
}

//-------------------------------------------------------------------------------------------

bool QAlbumListModel::initialise()
{
    QSharedPointer<track::model::AlbumModel> pAlbums(new track::model::AlbumModel());
    bool res = false;
    
    if(pAlbums->build())
    {
        m_pAlbums = pAlbums;
        res = true;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<QAlbumTrackListModel> QAlbumListModel::trackModel()
{
    return m_pTrackModel;
}

//-------------------------------------------------------------------------------------------

void QAlbumListModel::showAlbumTracks(int index)
{
    if(!m_pAlbums.isNull() && index >= 0 && index < m_pAlbums->size())
    {
        track::model::TrackModelKey key;
        QVariant aKey = m_pAlbums->data(index, 0);
        track::model::AlbumModelKey albumKey(aKey);
        key.album() = albumKey;
        m_pTrackModel->load(key);
    }
}

//-------------------------------------------------------------------------------------------

void QAlbumListModel::appendTrack(const QString& fileName)
{
    if(!m_pAlbums.isNull() && common::DiskOps::exist(fileName))
    {
        QSharedPointer<track::db::DBInfo> pInfo = track::db::DBInfo::readInfo(fileName, false).dynamicCast<track::db::DBInfo>();
        if(!pInfo.isNull())
        {
            QVector<tint> indexes = m_pAlbums->indexForDBInfo(pInfo, true);
            for(int i = indexes.size()-1; i >= 0; i--)
            {
                int idx = indexes.at(i);
                beginInsertRows(QModelIndex(), idx, idx);
                m_pAlbums->addDBInfo(indexes.at(i), pInfo);
                endInsertRows();
            }
            submit();
        }
    }
}

//-------------------------------------------------------------------------------------------

void QAlbumListModel::deleteTrack(const QString& fileName)
{
    if(!m_pAlbums.isNull())
    {
        QSharedPointer<track::db::DBInfo> pInfo = track::db::DBInfo::readInfo(fileName, false).dynamicCast<track::db::DBInfo>();
        if(!pInfo.isNull())
        {
            QVector<tint> indexes = m_pAlbums->indexForDBInfo(pInfo, false);
            for(int i = indexes.size()-1; i >= 0; i--)
            {
                int idx = indexes.at(i);
                beginRemoveRows(QModelIndex(), idx, idx);
                m_pAlbums->removeRow(idx);
                endRemoveRows();
            }
            submit();
        }
    }
}

//-------------------------------------------------------------------------------------------

void QAlbumListModel::resetAndReload(bool isReload)
{
    beginResetModel();
    {
        m_pAlbums.clear();    
        if(isReload)
        {
            QSharedPointer<track::model::AlbumModel> pAlbums(new track::model::AlbumModel());
            if(pAlbums->build())
            {
                m_pAlbums = pAlbums;
            }        
        }
    }
    endResetModel();
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
