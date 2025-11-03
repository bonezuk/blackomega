//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYERAPP_PLAYERCOMMON_QALBUMTRACKLISTMODEL_H
#define __OMEGA_PLAYERAPP_PLAYERCOMMON_QALBUMTRACKLISTMODEL_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "common/inc/DiskOps.h"
#include "track/model/inc/AlbumTrackModel.h"
#include "playerapp/playercommon/inc/QOmegaListModel.h"

#if QT_VERSION < 0x055000
#define Q_ENUM(x)
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT QAlbumTrackListModel : public QOmegaListModel
{
    Q_OBJECT
    public:
        enum AlbumTrackRoles
        {
            TrackRole = Qt::UserRole + 1,
            AlbumRole,
            FileRole,
            LengthRole,
            ArtistRole,
            OriginalArtistRole,
            ComposerRole
        };
        Q_ENUM(AlbumTrackRoles)

        Q_PROPERTY(qint32 sizeOfModel READ getSizeOfModel NOTIFY onSizeOfModel)

    public:
        QAlbumTrackListModel(QObject *parent = 0);
        virtual ~QAlbumTrackListModel();

        virtual void load(const track::model::TrackModelKey& key);

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual QHash<int,QByteArray> roleNames() const;

        virtual qint32 getSizeOfModel() const;

        Q_INVOKABLE void appendTrackToPlaylist(int index);
        Q_INVOKABLE void appendAlbumToPlaylist();

    Q_SIGNALS:
        void appendToPlaylist(const QString& fileName);
        void onSizeOfModel();

    public Q_SLOTS:
        virtual void appendTrack(const QString& fileName);
        virtual void deleteTrack(const QString& fileName);

    protected:
        QSharedPointer<track::model::AlbumTrackModel> m_pTracks;

        virtual void endInsertRows();
        virtual void endRemoveRows();

        virtual void resetAndReload(bool isReload);
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
