//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_QALBUMIMAGEPROVIDER_H
#define __PLAYERAPP_PLAYERCOMMON_QALBUMIMAGEPROVIDER_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "track/model/inc/ImageRepositary.h"

#if QT_VERSION >= 0x050000
#include <QQuickImageProvider>
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

#if QT_VERSION >= 0x050000
class PLAYERCOMMON_EXPORT QAlbumImageProvider : public QQuickImageProvider
{
    public:
        QAlbumImageProvider();
        virtual ~QAlbumImageProvider();

        virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

    private:
        virtual int imageIDFromString(const QString& id) const;
};
#else
class QAlbumImageProvider {
    public:
        QAlbumImageProvider() {}
        ~QAlbumImageProvider() {}
};
#endif

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
