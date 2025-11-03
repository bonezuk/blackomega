//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_MODEL_IMAGEREPOSITARY_H
#define __OMEGA_TRACK_MODEL_IMAGEREPOSITARY_H
//-------------------------------------------------------------------------------------------

#include "track/db/inc/TrackDB.h"
#include "track/info/inc/Info.h"
#include "track/model/inc/AbstractTrackModel.h"

#include <QtCore/QBuffer>
#include <QtCore/QFile>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace model
{
//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT ImageRepositary
{
    public:
        ImageRepositary();
        virtual ~ImageRepositary();
        
        static QSharedPointer<ImageRepositary> instance(const tchar *factoryKey);
        static QSharedPointer<ImageRepositary> instance();
        static void release();
        
        virtual int originalWidth(int imageID) = 0;
        virtual int originalHeight(int imageID) = 0;
        
        virtual QImage *getImage(int imageID,int iWidth,int iHeight) = 0;
        virtual QImage *getReference(int iWidth,int iHeight) = 0;
        
    protected:
        
        static QSharedPointer<ImageRepositary> m_instance;
};

typedef QSharedPointer<ImageRepositary> ImageRepositarySPtr;

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(TRACK_MODEL_EXPORT,ImageRepositaryFactory,ImageRepositary);

//-------------------------------------------------------------------------------------------

class TRACK_MODEL_EXPORT ImageRepositaryImpl : public ImageRepositary
{
    public:
        ImageRepositaryImpl();
        virtual ~ImageRepositaryImpl();

        virtual int originalWidth(int imageID);
        virtual int originalHeight(int imageID);

        virtual QImage *getImage(int imageID,int iWidth,int iHeight);
        virtual QImage *getReference(int iWidth,int iHeight);
    
    protected:
    
        QMap<int,QMap<QPair<int,int>,QImage *> > m_imageMap;
        QMap<QPair<int,int>,QImage *> m_referenceMap;
        mutable QMap<int, QSize> m_sizeMap;
    
        virtual QString formatToString(track::info::Info::ImageFormat iFormat) const;
        virtual QString getImageQuery(int imageID) const;
        virtual QImage *loadFromData(track::info::ImageInfoArray *pData,track::info::Info::ImageFormat iFormat) const;
        virtual QImage *scaleImage(QImage *orgImage,int iWidth,int iHeight) const;
        
        // Can be used in overloaded methods to apply post process image steps.
        virtual QImage *processImage(QImage *iImage) const;
        
        virtual void addOriginalSize(int imageID, int orgWidth, int orgHeight) const;
        
        virtual db::SQLiteQuerySPtr getDBQuery() const;
        virtual track::info::ImageInfoArray *loadDataFromDatabase(const QString& cmdQ,track::info::Info::ImageFormat& iFormat) const;

        virtual QImage *loadImageFromArray(track::info::ImageInfoArray *iArray,int iWidth,int iHeight,track::info::Info::ImageFormat iFormat, int& orgWidth, int& orgHeight) const;

        virtual QImage *loadImage(int imageID,int iWidth,int iHeight) const;
        virtual QImage *loadImageFromFile(const QString& iFilename,int iWidth,int iHeight) const;
        
        virtual void deleteAllImageMap(QMap<int,QMap<QPair<int,int>,QImage *> >& dMap);
        virtual void deleteImageMap(QMap<QPair<int,int>,QImage *>& dMap);
        virtual void deleteImage(QImage *img);
};

//-------------------------------------------------------------------------------------------
} // namespace model
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
