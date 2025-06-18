#include "playerapp/playercommon/inc/QAlbumImageProvider.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
//-------------------------------------------------------------------------------------------

QAlbumImageProvider::QAlbumImageProvider() : QQuickImageProvider(QQmlImageProviderBase::Image)
{}

//-------------------------------------------------------------------------------------------

QAlbumImageProvider::~QAlbumImageProvider()
{}

//-------------------------------------------------------------------------------------------

int QAlbumImageProvider::imageIDFromString(const QString& id) const
{
    int imageID;
    QString num;
    
    for(int idx = 0; idx < id.size(); idx++)
    {
        QChar v = id.at(idx);
        if(v.isDigit() || v == QChar('-'))
        {
            num += v;
        }
    }
    imageID = num.toInt();
    return imageID;
}

//-------------------------------------------------------------------------------------------

QImage QAlbumImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QSharedPointer<track::model::ImageRepositary> pImageRepo = track::model::ImageRepositary::instance();
    if(!pImageRepo.isNull())
    {
        QImage *rImg;
        int imageID = imageIDFromString(id);
        int w = (requestedSize.width() > 0) ? requestedSize.width() : 128;
        int h = (requestedSize.height() > 0) ? requestedSize.height() : 128;
        if(imageID >= 0)
        {
            rImg = pImageRepo->getImage(imageID, w, h);
        }
        else
        {
            rImg = pImageRepo->getReference(w, h);
            imageID = -1;
        }
        if(rImg != 0)
        {
            QImage dImg(*rImg);
            if(size != 0)
            {
                *size = QSize(pImageRepo->originalWidth(imageID), pImageRepo->originalHeight(imageID));
            }
            return dImg;
        }
    }

    if(size != 0)
    {
        *size = QSize(0, 0);
    }
    return QImage();
}

//-------------------------------------------------------------------------------------------
} // namespace omega
//-------------------------------------------------------------------------------------------
