#include "track/model/inc/ImageRepositary.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

QSharedPointer<ImageRepositary> ImageRepositary::m_instance;

ABSTRACT_FACTORY_CLASS_IMPL(ImageRepositaryFactory,ImageRepositary)

//-------------------------------------------------------------------------------------------
// ImageRepositary
//-------------------------------------------------------------------------------------------

ImageRepositary::ImageRepositary()
{}

//-------------------------------------------------------------------------------------------

ImageRepositary::~ImageRepositary()
{}

//-------------------------------------------------------------------------------------------

QSharedPointer<ImageRepositary> ImageRepositary::instance(const tchar *factoryKey)
{
    if(m_instance.data()==0)
    {
        ImageRepositarySPtr iRepo = ImageRepositaryFactory::createShared(factoryKey);
        m_instance = iRepo;
    }
    return m_instance;
}

//-------------------------------------------------------------------------------------------

QSharedPointer<ImageRepositary> ImageRepositary::instance()
{
    return m_instance;
}

//-------------------------------------------------------------------------------------------

void ImageRepositary::release()
{
    ImageRepositarySPtr eRepo;
    m_instance = eRepo;
}

//-------------------------------------------------------------------------------------------
// ImageRepositaryImpl
//-------------------------------------------------------------------------------------------

CONCRETE_FACTORY_CLASS_IMPL(ImageRepositaryFactory,ImageRepositary, \
                            ImageRepositaryImplFactory,ImageRepositaryImpl, \
                            "image",false)

//-------------------------------------------------------------------------------------------

ImageRepositaryImpl::ImageRepositaryImpl() : ImageRepositary(),
    m_imageMap(),
    m_referenceMap(),
    m_sizeMap()
{}

//-------------------------------------------------------------------------------------------

ImageRepositaryImpl::~ImageRepositaryImpl()
{
    ImageRepositaryImpl::deleteAllImageMap(m_imageMap);
    ImageRepositaryImpl::deleteImageMap(m_referenceMap);
}

//-------------------------------------------------------------------------------------------

QString ImageRepositaryImpl::getImageQuery(int imageID) const
{
    QString cmdQ;
    cmdQ = "SELECT format, data FROM image WHERE imageID=" + QString::number(imageID);
    return cmdQ;
}

//-------------------------------------------------------------------------------------------

QString ImageRepositaryImpl::formatToString(track::info::Info::ImageFormat iFormat) const
{
    QString format;
    
    switch(iFormat)
    {
        case track::info::Info::e_imageJPEG:
            format = "JPEG";
            break;
        
        case track::info::Info::e_imageGIF:
            format = "GIF";
            break;
        
        case track::info::Info::e_imagePNG:
            format = "PNG";
            break;
        
        case track::info::Info::e_imageBMP:
            format = "BMP";
            break;
        
        case track::info::Info::e_imageUnknown:
        default:
            format = "";
            break;
    }
    return format;
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryImpl::loadFromData(track::info::ImageInfoArray *pData,track::info::Info::ImageFormat iFormat) const
{
    QImage *img = 0;

    if(pData!=0 && pData->GetSize()>0)
    {
        QString format = formatToString(iFormat);
        QByteArray qArr(reinterpret_cast<const tchar *>(pData->GetData()),pData->GetSize());
        QBuffer qBuffer(&qArr);
        bool res;
        
        img = new QImage;
        if(!format.isEmpty())
        {
            res = img->load(&qBuffer,format.toLatin1().constData());
            if(!res)
            {
                res = img->load(&qBuffer,0);
            }
        }
        else
        {
            res = img->load(&qBuffer,0);
        }
        
        if(!res)
        {
            delete img;
            img = 0;
        }
    }
    return img;
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryImpl::scaleImage(QImage *orgImage,int iWidth,int iHeight) const
{
    QImage *img = 0;

    if(orgImage!=0)
    {
        if(orgImage->width()==iWidth && orgImage->height()==iHeight)
        {
            img = new QImage(*orgImage);
        }
        else if(iWidth!=0 && iHeight!=0)
        {
            img = new QImage(orgImage->scaled(QSize(iWidth,iHeight),Qt::IgnoreAspectRatio,Qt::SmoothTransformation));
        }
    }
    return img;
}

//-------------------------------------------------------------------------------------------

db::SQLiteQuerySPtr ImageRepositaryImpl::getDBQuery() const
{
    db::SQLiteQuerySPtr pDB(new db::SQLiteQuery(db::TrackDB::instance()->db()));
    return pDB;
}

//-------------------------------------------------------------------------------------------

track::info::ImageInfoArray *ImageRepositaryImpl::loadDataFromDatabase(const QString& cmdQ,track::info::Info::ImageFormat& iFormat) const
{
    track::info::ImageInfoArray *imageArray = 0;
    
    try
    {
        int format;
        QByteArray iMem;
        db::SQLiteQuerySPtr query = getDBQuery();
        
        query->prepare(cmdQ);
        query->bind(format);
        query->bind(iMem);
        if(query->next())
        {
            iFormat = static_cast<track::info::Info::ImageFormat>(format);
            imageArray = new track::info::ImageInfoArray();
            if(iMem.size() > 0)
            {
                imageArray->AppendRaw(iMem.constData(),iMem.size());
            }
        }
    }
    catch(const db::SQLiteException&)
    {
        if(imageArray!=0)
        {
            delete imageArray;
            imageArray = 0;
        }
    }
    return imageArray;
}

//-------------------------------------------------------------------------------------------
// The ImageRepositary can be overloaded such that different post-processing
// imaging techniques can be applied. This is done via overloading this method.
//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryImpl::processImage(QImage *iImage) const
{
    QImage *oImage = new QImage(*iImage);
    return oImage;
}

//-------------------------------------------------------------------------------------------

int ImageRepositaryImpl::originalWidth(int imageID)
{
    int w = 128;
    QMap<int, QSize>::iterator ppI = m_sizeMap.find(imageID);
    if(ppI != m_sizeMap.end())
    {
        w = ppI.value().width();
    }
    return w;
}

//-------------------------------------------------------------------------------------------

int ImageRepositaryImpl::originalHeight(int imageID)
{
    int h = 128;
    QMap<int, QSize>::iterator ppI = m_sizeMap.find(imageID);
    if(ppI != m_sizeMap.end())
    {
        h = ppI.value().width();
    }
    return h;
}

//-------------------------------------------------------------------------------------------

void ImageRepositaryImpl::addOriginalSize(int imageID, int orgWidth, int orgHeight) const
{
    QMap<int, QSize>::iterator ppI = m_sizeMap.find(imageID);
    if(ppI != m_sizeMap.end())
    {
        ppI.value() = QSize(orgWidth, orgHeight);
    }
    else
    {
        m_sizeMap.insert(imageID, QSize(orgWidth, orgHeight));
    }
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryImpl::loadImage(int imageID,int iWidth,int iHeight) const
{
    int orgWidth, orgHeight;
    QImage *img;
    QString cmdQ;
    track::info::Info::ImageFormat iFormat;
    track::info::ImageInfoArray *iArray;
    
    cmdQ = getImageQuery(imageID);
    iArray = loadDataFromDatabase(cmdQ,iFormat);
    img = loadImageFromArray(iArray,iWidth,iHeight,iFormat, orgWidth, orgHeight);
    addOriginalSize(imageID, orgWidth, orgHeight);
    delete iArray;
    return img;
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryImpl::loadImageFromFile(const QString& iFilename,int iWidth,int iHeight) const
{
    QImage *img = 0;
    QFile iFile(iFilename);
    if(iFile.open(QIODevice::ReadOnly))
    {
        int orgWidth, orgHeight;
        QByteArray iMem = iFile.readAll();
        track::info::ImageInfoArray *iArray = new track::info::ImageInfoArray;
        iArray->AppendRaw(iMem.constData(),iMem.size());
        img = loadImageFromArray(iArray,iWidth,iHeight,track::info::Info::e_imageUnknown, orgWidth, orgHeight);
        addOriginalSize(-1, orgWidth, orgHeight);
        delete iArray;
    }
    return img;
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryImpl::loadImageFromArray(track::info::ImageInfoArray *iArray,int iWidth,int iHeight,track::info::Info::ImageFormat iFormat, int& orgWidth, int& orgHeight) const
{
    QImage *img = 0;

    if(iArray!=0)
    {
        QImage *orgImage = loadFromData(iArray,iFormat);
        if(orgImage!=0)
        {
            orgWidth = orgImage->width();
            orgHeight = orgImage->height();
        
            QImage *sImage = scaleImage(orgImage,iWidth,iHeight);
            if(sImage!=0)
            {
                img = processImage(sImage);
                delete sImage;
            }
            delete orgImage;
        }
    }
    return img;
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryImpl::getImage(int imageID,int iWidth,int iHeight)
{
    QImage *img = 0;
    QPair<int,int> sizeIndex(iWidth,iHeight);
    QMap<int,QMap<QPair<int,int>,QImage *> >::iterator ppI;
    QMap<QPair<int,int>,QImage *>::iterator ppJ;

    ppI = m_imageMap.find(imageID);
    if(ppI!=m_imageMap.end())
    {
        QMap<QPair<int,int>,QImage *>& iMap = ppI.value();
        ppJ = iMap.find(sizeIndex);
        if(ppJ!=iMap.end())
        {
            img = ppJ.value();
        }
    }
    
    if(img==0)
    {
        img = loadImage(imageID,iWidth,iHeight);
        if(img!=0)
        {
            if(ppI!=m_imageMap.end())
            {
                QMap<QPair<int,int>,QImage *>& iMap = ppI.value();
                iMap.insert(sizeIndex,img);
            }
            else
            {
                QMap<QPair<int,int>,QImage *> iMap;
                iMap.insert(sizeIndex,img);
                m_imageMap.insert(imageID,iMap);
            }
        }
        else
        {
            img = getReference(iWidth,iHeight);
        }
    }
    return img;
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryImpl::getReference(int iWidth,int iHeight)
{
    QImage *img;
    QString referenceName(":/Resources/images/note.png");
    QPair<int,int> sizeIndex(iWidth,iHeight);
    QMap<QPair<int,int>,QImage *>::iterator ppI;

    ppI = m_referenceMap.find(sizeIndex);
    if(ppI==m_referenceMap.end())
    {
        img = loadImageFromFile(referenceName,iWidth,iHeight);
        if(img!=0)
        {
            m_referenceMap.insert(sizeIndex,img);
        }
    }
    else
    {
        img = ppI.value();
    }
    return img;
}

//-------------------------------------------------------------------------------------------

void ImageRepositaryImpl::deleteAllImageMap(QMap<int,QMap<QPair<int,int>,QImage *> >& dMap)
{
    QMap<int,QMap<QPair<int,int>,QImage *> >::iterator ppI;
    
    while(ppI=dMap.begin(),ppI!=dMap.end())
    {
        deleteImageMap(ppI.value());
        dMap.erase(ppI);
    }
}

//-------------------------------------------------------------------------------------------

void ImageRepositaryImpl::deleteImageMap(QMap<QPair<int,int>,QImage *>& dMap)
{
    QMap<QPair<int,int>,QImage *>::iterator ppI;
    
    while(ppI=dMap.begin(),ppI!=dMap.end())
    {
        QImage *img = ppI.value();
        dMap.erase(ppI);
        deleteImage(img);
    }
}

//-------------------------------------------------------------------------------------------

void ImageRepositaryImpl::deleteImage(QImage *img)
{
    if(img!=0)
    {
        delete img;
    }
}

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------

