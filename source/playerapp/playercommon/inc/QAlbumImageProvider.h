//-------------------------------------------------------------------------------------------
#ifndef __PLAYERAPP_PLAYERCOMMON_QALBUMIMAGEPROVIDER_H
#define __PLAYERAPP_PLAYERCOMMON_QALBUMIMAGEPROVIDER_H
//-------------------------------------------------------------------------------------------

#include "playerapp/playercommon/inc/PlayerCommonDLL.h"
#include "track/model/inc/ImageRepositary.h"

#include <QQuickImageProvider>

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

class PLAYERCOMMON_EXPORT QAlbumImageProvider : public QQuickImageProvider
{
    public:
        QAlbumImageProvider();
        virtual ~QAlbumImageProvider();
        
        virtual QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
        
    private:
    
        virtual int imageIDFromString(const QString& id) const;
};

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

