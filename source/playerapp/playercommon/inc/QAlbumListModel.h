//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_PLAYERAPP_PLAYERCOMMON_QALBUMLISTMODEL_H
#define __OMEGA_PLAYERAPP_PLAYERCOMMON_QALBUMLISTMODEL_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "track/model/inc/AlbumModel.h"
#include "playerapp/playercommon/inc/QAlbumTrackListModel.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT QAlbumListModel : public QOmegaListModel
{
    Q_OBJECT
    public:
        enum AlbumListRoles
        {
            ArtistRole = Qt::UserRole + 1,
            AlbumRole,
            YearRole,
            ImageRole
        };
        Q_ENUM(AlbumListRoles)

    public:
        QAlbumListModel(QObject *parent = 0);
        virtual ~QAlbumListModel();

        virtual bool initialise();
        virtual QSharedPointer<QAlbumTrackListModel> trackModel();

        virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
        virtual QHash<int,QByteArray> roleNames() const;

        Q_INVOKABLE void showAlbumTracks(int index);

    public slots:
        virtual void appendTrack(const QString& fileName);
        virtual void deleteTrack(const QString& fileName);

    protected:
        QSharedPointer<track::model::AlbumModel> m_pAlbums;
        QSharedPointer<QAlbumTrackListModel> m_pTrackModel;

        virtual void resetAndReload(bool isReload);
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

