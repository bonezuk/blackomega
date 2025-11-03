#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/model/inc/ImageRepositary.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/db/test/SQLiteMock.h"

using namespace ::testing;
using namespace omega::track::model;
using namespace omega::track;
using namespace omega;

//-------------------------------------------------------------------------------------------

class ImageRepositaryTest : public ImageRepositaryImpl
{
    public:
        ImageRepositaryTest();
        ImageRepositaryTest(db::SQLiteQuerySPtr pQuery);

        QString testGetImageQuery(int imageID) const;
        QString testFormatToString(track::info::Info::ImageFormat iFormat) const;
        QImage *testLoadFromData(track::info::ImageInfoArray *pData,track::info::Info::ImageFormat iFormat) const;
        QImage *testScaleImage(QImage *orgImage,int iWidth,int iHeight) const;
        track::info::ImageInfoArray *testLoadDataFromDatabase(const QString& cmdQ,track::info::Info::ImageFormat& iFormat) const;
        QImage *testLoadImage(int imageID,int iWidth,int iHeight) const;
        void testDeleteImageMap(QMap<QPair<int,int>,QImage *>& dMap);
        void testDeleteAllImageMap(QMap<int,QMap<QPair<int,int>,QImage *> >& dMap);
        QImage *testLoadImageFromArray(track::info::ImageInfoArray *iArray,int iWidth,int iHeight,track::info::Info::ImageFormat iFormat) const;
        QImage *testLoadImageFromFile(const QString& iFilename,int iWidth,int iHeight) const;


        QMap<int,QMap<QPair<int,int>,QImage *> >& imageMap();
        QMap<QPair<int,int>,QImage *>& referenceMap();
        QImage *getTestImage();
        virtual db::SQLiteQuerySPtr getDBQuery() const;

    protected:
        db::SQLiteQuerySPtr m_pQuery;
};

//-------------------------------------------------------------------------------------------

ImageRepositaryTest::ImageRepositaryTest() : ImageRepositaryImpl(),
    m_pQuery()
{}

//-------------------------------------------------------------------------------------------

ImageRepositaryTest::ImageRepositaryTest(db::SQLiteQuerySPtr pQuery) : ImageRepositaryImpl(),
    m_pQuery(pQuery)
{}

//-------------------------------------------------------------------------------------------

QString ImageRepositaryTest::testGetImageQuery(int imageID) const
{
    return getImageQuery(imageID);
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryTest::testScaleImage(QImage *orgImage,int iWidth,int iHeight) const
{
    return scaleImage(orgImage,iWidth,iHeight);
}

//-------------------------------------------------------------------------------------------

QString ImageRepositaryTest::testFormatToString(track::info::Info::ImageFormat iFormat) const
{
    return formatToString(iFormat);
}

//-------------------------------------------------------------------------------------------

track::info::ImageInfoArray *ImageRepositaryTest::testLoadDataFromDatabase(const QString& cmdQ,track::info::Info::ImageFormat& iFormat) const
{
    return loadDataFromDatabase(cmdQ,iFormat);
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryTest::testLoadFromData(track::info::ImageInfoArray *pData,track::info::Info::ImageFormat iFormat) const
{
    return loadFromData(pData,iFormat);
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryTest::testLoadImageFromArray(track::info::ImageInfoArray *iArray,int iWidth,int iHeight,track::info::Info::ImageFormat iFormat) const
{
    return loadImageFromArray(iArray,iWidth,iHeight,iFormat);
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryTest::testLoadImage(int imageID,int iWidth,int iHeight) const
{
    return loadImage(imageID,iWidth,iHeight);
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryTest::testLoadImageFromFile(const QString& iFilename,int iWidth,int iHeight) const
{
    return loadImageFromFile(iFilename,iWidth,iHeight);
}

//-------------------------------------------------------------------------------------------

void ImageRepositaryTest::testDeleteImageMap(QMap<QPair<int,int>,QImage *>& dMap)
{
    deleteImageMap(dMap);
}

//-------------------------------------------------------------------------------------------

void ImageRepositaryTest::testDeleteAllImageMap(QMap<int,QMap<QPair<int,int>,QImage *> >& dMap)
{
    deleteAllImageMap(dMap);
}

//-------------------------------------------------------------------------------------------

QMap<int,QMap<QPair<int,int>,QImage *> >& ImageRepositaryTest::imageMap()
{
    return m_imageMap;
}

//-------------------------------------------------------------------------------------------

QMap<QPair<int,int>,QImage *>& ImageRepositaryTest::referenceMap()
{
    return m_referenceMap;
}

//-------------------------------------------------------------------------------------------

db::SQLiteQuerySPtr ImageRepositaryTest::getDBQuery() const
{
    return m_pQuery;
}

//-------------------------------------------------------------------------------------------

QImage *ImageRepositaryTest::getTestImage()
{
    QFile file(":/images/Resources/images/omega.jpg");
    EXPECT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());
    return loadFromData(&iData,track::info::Info::e_imageJPEG);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,getImageQuery)
{
    QString testSQL,expectSQL;
    expectSQL = "SELECT format, data FROM image WHERE imageID=2";
    expectSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(expectSQL);
    ImageRepositaryTest rep;
    testSQL = rep.testGetImageQuery(2);
    testSQL = TrackDBTestEnviroment::testUtilitySQLNormaliseWhitespace(testSQL);
    EXPECT_TRUE(testSQL==expectSQL);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,formatToString)
{
    ImageRepositaryTest rep;
    EXPECT_TRUE(rep.testFormatToString(track::info::Info::e_imageJPEG)=="JPEG");
    EXPECT_TRUE(rep.testFormatToString(track::info::Info::e_imageGIF)=="GIF");
    EXPECT_TRUE(rep.testFormatToString(track::info::Info::e_imageBMP)=="BMP");
    EXPECT_TRUE(rep.testFormatToString(track::info::Info::e_imagePNG)=="PNG");
    EXPECT_TRUE(rep.testFormatToString(track::info::Info::e_imageUnknown)=="");
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataNoImageWhenNoData)
{
    QImage *img;
    ImageRepositaryTest rep;

    img = rep.testLoadFromData(0,track::info::Info::e_imageJPEG);
    EXPECT_TRUE(img==0);

    track::info::ImageInfoArray iData;
    img = rep.testLoadFromData(&iData,track::info::Info::e_imageJPEG);
    EXPECT_TRUE(img==0);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataJPEGWithGivenFormat)
{
    QFile file(":/images/Resources/images/omega.jpg");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imageJPEG);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataGIFWithGivenFormat)
{
    QFile file(":/images/Resources/images/omega.gif");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imageGIF);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataPNGWithGivenFormat)
{
    QFile file(":/images/Resources/images/omega.png");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imagePNG);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataBMPWithGivenFormat)
{
    QFile file(":/images/Resources/images/omega.bmp");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imageBMP);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataJPEGWithUnknownFormat)
{
    QFile file(":/images/Resources/images/omega.jpg");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imageUnknown);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataGIFWithUnknownFormat)
{
    QFile file(":/images/Resources/images/omega.gif");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imageUnknown);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataPNGWithUnknownFormat)
{
    QFile file(":/images/Resources/images/omega.png");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imageUnknown);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataBMPWithUnknownFormat)
{
    QFile file(":/images/Resources/images/omega.bmp");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imageUnknown);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataPNGWhenGivenFormatIsJPEG)
{
    QFile file(":/images/Resources/images/omega.png");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imageJPEG);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadFromDataJPEGWhenGivenFormatIsPNG)
{
    QFile file(":/images/Resources/images/omega.jpg");
    ASSERT_TRUE(file.open(QIODevice::ReadOnly));
    QByteArray fData = file.readAll();
    track::info::ImageInfoArray iData;
    iData.AppendRaw(fData.constData(),fData.size());

    ImageRepositaryTest rep;
    QImage *img = rep.testLoadFromData(&iData,track::info::Info::e_imagePNG);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,scaleImageNoImage)
{
    ImageRepositaryTest rep;
    EXPECT_TRUE(rep.testScaleImage(0,32,32)==0);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,scaleImageZeroWidth)
{
    ImageRepositaryTest rep;
    QImage *oImg = rep.getTestImage();
    ASSERT_TRUE(oImg!=0);
    QImage *sImg = rep.testScaleImage(oImg,0,48);
    EXPECT_TRUE(sImg==0);
    delete oImg;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,sclaeImageZeroHeight)
{
    ImageRepositaryTest rep;
    QImage *oImg = rep.getTestImage();
    ASSERT_TRUE(oImg!=0);
    QImage *sImg = rep.testScaleImage(oImg,32,0);
    EXPECT_TRUE(sImg==0);
    delete oImg;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,scaleImageNoScaleOperation)
{
    ImageRepositaryTest rep;
    QImage *oImg = rep.getTestImage();
    ASSERT_TRUE(oImg!=0);
    QImage *sImg = rep.testScaleImage(oImg,oImg->width(),oImg->height());
    EXPECT_TRUE(sImg!=0);
    EXPECT_TRUE(oImg!=sImg);
    delete oImg;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,scaleImageValidRescale)
{
    ImageRepositaryTest rep;
    QImage *oImg = rep.getTestImage();
    ASSERT_TRUE(oImg!=0);
    QImage *sImg = rep.testScaleImage(oImg,32,48);
    EXPECT_TRUE(sImg!=0);
    EXPECT_TRUE(sImg->width()==32);
    EXPECT_TRUE(sImg->height()==48);
    delete sImg;
    delete oImg;
}

//-------------------------------------------------------------------------------------------

class IMITestLoadImageQuery
{
    public:
        int *iFormat;
        QByteArray *iMem;

        void bindFormat(int& v) {iFormat = &v;}
        void bindImage(QByteArray& v) {iMem = &v;}
        bool next();
};

bool IMITestLoadImageQuery::next()
{
    tubyte t[4] = {0x01, 0x02, 0x03, 0x04};
    QByteArray tMem(reinterpret_cast<tbyte *>(t),4);
    *iFormat = track::info::Info::e_imageJPEG;
    *iMem = tMem;
    return true;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadDataFromDatabaseWithSuccess)
{
    IMITestLoadImageQuery iMock;
    db::SQLiteQuerySPtr queryMock(new db::SQLiteQueryMock(db::TrackDB::instance()->db()));
    db::SQLiteQueryMock& query = dynamic_cast<db::SQLiteQueryMock&>(*(queryMock.data()));

    QString cmdQ;
    ImageRepositaryTest rep(queryMock);

    cmdQ = "SELECT image mock query";

    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<int&>())).Times(1)
        .WillOnce(Invoke(&iMock,&IMITestLoadImageQuery::bindFormat));
    EXPECT_CALL(query,bind(A<QByteArray&>())).Times(1)
        .WillOnce(Invoke(&iMock,&IMITestLoadImageQuery::bindImage));
    EXPECT_CALL(query,next()).Times(1)
        .WillOnce(Invoke(&iMock,&IMITestLoadImageQuery::next));

    track::info::Info::ImageFormat iFormat;
    track::info::ImageInfoArray *infoArray;
    infoArray = rep.testLoadDataFromDatabase(cmdQ,iFormat);

    ASSERT_TRUE(infoArray!=0);

    const tubyte *tMem = reinterpret_cast<const tubyte *>(infoArray->GetData());
    ASSERT_TRUE(infoArray->GetSize()==4);
    EXPECT_TRUE(tMem[0]==0x01 && tMem[1]==0x02 && tMem[2]==0x03 && tMem[3]==0x04);
    EXPECT_TRUE(iFormat==track::info::Info::e_imageJPEG);

    delete infoArray;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadDataFromDatabaseWithFailure)
{
    IMITestLoadImageQuery iMock;
    db::SQLiteQuerySPtr queryMock(new db::SQLiteQueryMock(db::TrackDB::instance()->db()));
    db::SQLiteQueryMock& query = dynamic_cast<db::SQLiteQueryMock&>(*(queryMock.data()));

    QString cmdQ;
    ImageRepositaryTest rep(queryMock);

    cmdQ = "SELECT image mock query";

    EXPECT_CALL(query,prepare(cmdQ)).Times(1);
    EXPECT_CALL(query,bind(A<int&>())).Times(1)
        .WillOnce(Invoke(&iMock,&IMITestLoadImageQuery::bindFormat));
    EXPECT_CALL(query,bind(A<QByteArray&>())).Times(1)
        .WillOnce(Invoke(&iMock,&IMITestLoadImageQuery::bindImage));
    EXPECT_CALL(query,next()).Times(1).WillOnce(Return(false));

    track::info::Info::ImageFormat iFormat;
    track::info::ImageInfoArray *infoArray;
    infoArray = rep.testLoadDataFromDatabase(cmdQ,iFormat);

    ASSERT_TRUE(infoArray==0);
}

//-------------------------------------------------------------------------------------------

class IMILoadImageTest : public ImageRepositaryTest
{
    public:
        MOCK_CONST_METHOD1(getImageQuery,QString(int imageID));
        MOCK_CONST_METHOD2(loadFromData,QImage *(track::info::ImageInfoArray *pData,track::info::Info::ImageFormat iFormat));
        MOCK_CONST_METHOD3(scaleImage,QImage *(QImage *orgImage,int iWidth,int iHeight));
        MOCK_CONST_METHOD2(loadDataFromDatabase,track::info::ImageInfoArray *(const QString& cmdQ,track::info::Info::ImageFormat& iFormat));
        MOCK_CONST_METHOD1(processImage,QImage *(QImage *iImage));
};

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageNoImageInDatabase)
{
    QString mockCmdQ = "SELECT image";

    IMILoadImageTest rep;
    EXPECT_CALL(rep,getImageQuery(1)).Times(1).WillOnce(Return(mockCmdQ));
    EXPECT_CALL(rep,loadDataFromDatabase(Eq(mockCmdQ),A<track::info::Info::ImageFormat&>())).Times(1).WillOnce(ReturnNull());

    EXPECT_TRUE(rep.testLoadImage(1,32,48)==0);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageFailToLoadFromData)
{
    track::info::ImageInfoArray *iArray = new track::info::ImageInfoArray;
    QString mockCmdQ = "SELECT image";

    IMILoadImageTest rep;
    EXPECT_CALL(rep,getImageQuery(1)).Times(1).WillOnce(Return(mockCmdQ));
    EXPECT_CALL(rep,loadDataFromDatabase(Eq(mockCmdQ),A<track::info::Info::ImageFormat&>())).Times(1).WillOnce(DoAll(SetArgReferee<1>(track::info::Info::e_imageJPEG),Return(iArray)));
    EXPECT_CALL(rep,loadFromData(iArray,track::info::Info::e_imageJPEG)).Times(1).WillOnce(ReturnNull());

    EXPECT_TRUE(rep.testLoadImage(1,32,48)==0);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageFailToScaleImage)
{
    QImage *orgImage = new QImage;
    track::info::ImageInfoArray *iArray = new track::info::ImageInfoArray;
    QString mockCmdQ = "SELECT image";

    IMILoadImageTest rep;
    EXPECT_CALL(rep,getImageQuery(1)).Times(1).WillOnce(Return(mockCmdQ));
    EXPECT_CALL(rep,loadDataFromDatabase(Eq(mockCmdQ),A<track::info::Info::ImageFormat&>())).Times(1).WillOnce(DoAll(SetArgReferee<1>(track::info::Info::e_imageJPEG),Return(iArray)));
    EXPECT_CALL(rep,loadFromData(iArray,track::info::Info::e_imageJPEG)).Times(1).WillOnce(Return(orgImage));
    EXPECT_CALL(rep,scaleImage(orgImage,32,48)).Times(1).WillOnce(ReturnNull());

    EXPECT_TRUE(rep.testLoadImage(1,32,48)==0);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageFailToProcessImage)
{
    QImage *orgImage = new QImage;
    QImage *scaleImage = new QImage;
    track::info::ImageInfoArray *iArray = new track::info::ImageInfoArray;
    QString mockCmdQ = "SELECT image";

    IMILoadImageTest rep;
    EXPECT_CALL(rep,getImageQuery(1)).Times(1).WillOnce(Return(mockCmdQ));
    EXPECT_CALL(rep,loadDataFromDatabase(Eq(mockCmdQ),A<track::info::Info::ImageFormat&>())).Times(1).WillOnce(DoAll(SetArgReferee<1>(track::info::Info::e_imageJPEG),Return(iArray)));
    EXPECT_CALL(rep,loadFromData(iArray,track::info::Info::e_imageJPEG)).Times(1).WillOnce(Return(orgImage));
    EXPECT_CALL(rep,scaleImage(orgImage,32,48)).Times(1).WillOnce(Return(scaleImage));
    EXPECT_CALL(rep,processImage(scaleImage)).Times(1).WillOnce(ReturnNull());

    EXPECT_TRUE(rep.testLoadImage(1,32,48)==0);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageSuccess)
{
    QImage *orgImage = new QImage;
    QImage *scaleImage = new QImage;
    QImage *outImage = new QImage;
    track::info::ImageInfoArray *iArray = new track::info::ImageInfoArray;
    QString mockCmdQ = "SELECT image";

    IMILoadImageTest rep;
    EXPECT_CALL(rep,getImageQuery(1)).Times(1).WillOnce(Return(mockCmdQ));
    EXPECT_CALL(rep,loadDataFromDatabase(Eq(mockCmdQ),A<track::info::Info::ImageFormat&>())).Times(1).WillOnce(DoAll(SetArgReferee<1>(track::info::Info::e_imageJPEG),Return(iArray)));
    EXPECT_CALL(rep,loadFromData(iArray,track::info::Info::e_imageJPEG)).Times(1).WillOnce(Return(orgImage));
    EXPECT_CALL(rep,scaleImage(orgImage,32,48)).Times(1).WillOnce(Return(scaleImage));
    EXPECT_CALL(rep,processImage(scaleImage)).Times(1).WillOnce(Return(outImage));

    EXPECT_TRUE(rep.testLoadImage(1,32,48)==outImage);
    delete outImage;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageIntegration)
{
    db::SQLiteQuerySPtr query(new db::SQLiteQuery(db::TrackDB::instance()->db()));
    ImageRepositaryTest rep(query);
    QImage *img = rep.testLoadImage(1,48,48);
    EXPECT_TRUE(img!=0);
    delete img;
}

//-------------------------------------------------------------------------------------------

class IMIGetImageTest : public ImageRepositaryTest
{
    public:
        MOCK_CONST_METHOD3(loadImage,QImage *(int imageID,int iWidth,int iHeight));
        MOCK_METHOD2(getReference,QImage *(int iWidth,int iHeight));
};

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,getImageWhenImageIsAlreadyCached)
{
    QImage *eImg = new QImage;
    QMap<QPair<int,int>,QImage *> iMap;
    iMap.insert(QPair<int,int>(32,48),eImg);

    IMIGetImageTest rep;
    rep.imageMap().insert(2,iMap);

    EXPECT_TRUE(rep.getImage(2,32,48)==eImg);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,getImageWhenImageIDMapAlreadyExists)
{
    QImage *aImg = new QImage;
    QImage *eImg = new QImage;
    QMap<QPair<int,int>,QImage *> iMap;
    iMap.insert(QPair<int,int>(32,48),aImg);

    IMIGetImageTest rep;
    rep.imageMap().insert(2,iMap);
    EXPECT_CALL(rep,loadImage(2,16,18)).Times(1).WillOnce(Return(eImg));

    EXPECT_TRUE(rep.getImage(2,16,18)==eImg);
    EXPECT_TRUE(rep.getImage(2,16,18)==eImg);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,getImageWhenImageDoesNotExist)
{
    QImage *refImage = new QImage;
    IMIGetImageTest rep;
    EXPECT_CALL(rep,loadImage(2,16,18)).Times(2).WillRepeatedly(ReturnNull());
    EXPECT_CALL(rep,getReference(16,18)).Times(2).WillRepeatedly(Return(refImage));

    EXPECT_TRUE(rep.getImage(2,16,18)==refImage);
    EXPECT_TRUE(rep.getImage(2,16,18)==refImage);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,getImageWhenImageIDNotExist)
{
    QImage *eImg = new QImage;
    IMIGetImageTest rep;
    EXPECT_CALL(rep,loadImage(2,16,18)).Times(1).WillOnce(Return(eImg));

    EXPECT_TRUE(rep.getImage(2,16,18)==eImg);
    EXPECT_TRUE(rep.getImage(2,16,18)==eImg);
}

//-------------------------------------------------------------------------------------------

class IRIDeleteImageMapTest : public ImageRepositaryTest
{
    public:
        MOCK_METHOD1(deleteImage,void(QImage *img));
};

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,deleteImageMapWithNoEntries)
{
    QMap<QPair<int,int>,QImage *> iMap;
    IRIDeleteImageMapTest rep;
    rep.testDeleteImageMap(iMap);
    EXPECT_TRUE(iMap.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,deleteImageMapWithOneEntry)
{
    QPair<int,int> indexA(10,10);
    QImage *imgA = new QImage;
    QMap<QPair<int,int>,QImage *> iMap;
    iMap.insert(indexA,imgA);

    IRIDeleteImageMapTest rep;
    EXPECT_CALL(rep,deleteImage(imgA)).Times(1);
    rep.testDeleteImageMap(iMap);
    EXPECT_TRUE(iMap.size()==0);
    delete imgA;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,deleteImageMapWithMultipleEntries)
{
    QPair<int,int> indexA(10,10);
    QImage *imgA = new QImage;
    QPair<int,int> indexB(11,10);
    QImage *imgB = new QImage;
    QPair<int,int> indexC(10,11);
    QImage *imgC = new QImage;

    QMap<QPair<int,int>,QImage *> iMap;
    iMap.insert(indexA,imgA);
    iMap.insert(indexB,imgB);
    iMap.insert(indexC,imgC);

    IRIDeleteImageMapTest rep;
    EXPECT_CALL(rep,deleteImage(imgA)).Times(1);
    EXPECT_CALL(rep,deleteImage(imgB)).Times(1);
    EXPECT_CALL(rep,deleteImage(imgC)).Times(1);

    rep.testDeleteImageMap(iMap);

    EXPECT_TRUE(iMap.size()==0);

    delete imgA;
    delete imgB;
    delete imgC;
}

//-------------------------------------------------------------------------------------------

class IRIDeleteAllImageMapTest : public ImageRepositaryTest
{
    public:
        MOCK_METHOD1(deleteImageMap,void(QMap<QPair<int,int>,QImage *>& dMap));
};

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,deleteAllImageMapNoEntries)
{
    QMap<int,QMap<QPair<int,int>,QImage *> > iMap;
    IRIDeleteAllImageMapTest rep;
    EXPECT_CALL(rep,deleteImageMap(_)).Times(0);
    rep.testDeleteAllImageMap(iMap);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,deleteAllImageMapOneEntry)
{
    QMap<QPair<int,int>,QImage *> aMap;
    QPair<int,int> indexA(10,10);
    QImage *imgA = new QImage;
    aMap.insert(indexA,imgA);

    QMap<int,QMap<QPair<int,int>,QImage *> > iMap;
    iMap.insert(1,aMap);

    IRIDeleteImageMapTest rep;
    EXPECT_CALL(rep,deleteImage(imgA)).Times(1);

    rep.testDeleteAllImageMap(iMap);

    delete imgA;
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,deleteAllImageMapMultipleEntries)
{
    QMap<QPair<int,int>,QImage *> aMap;
    QPair<int,int> indexA(10,10);
    QImage *imgA = new QImage;
    aMap.insert(indexA,imgA);

    QMap<QPair<int,int>,QImage *> bMap;
    QPair<int,int> indexB(20,20);
    QImage *imgB = new QImage;
    bMap.insert(indexB,imgB);

    QMap<QPair<int,int>,QImage *> cMap;
    QPair<int,int> indexC(30,30);
    QImage *imgC = new QImage;
    cMap.insert(indexC,imgC);

    QMap<int,QMap<QPair<int,int>,QImage *> > iMap;
    iMap.insert(1,aMap);
    iMap.insert(2,bMap);
    iMap.insert(3,cMap);

    IRIDeleteImageMapTest rep;
    EXPECT_CALL(rep,deleteImage(imgA)).Times(1);
    EXPECT_CALL(rep,deleteImage(imgB)).Times(1);
    EXPECT_CALL(rep,deleteImage(imgC)).Times(1);

    rep.testDeleteAllImageMap(iMap);

    delete imgA;
    delete imgB;
    delete imgC;
}

//-------------------------------------------------------------------------------------------

class IRILoadImageFromArrayTest : public ImageRepositaryTest
{
    public:
        MOCK_CONST_METHOD2(loadFromData,QImage *(track::info::ImageInfoArray *pData,track::info::Info::ImageFormat iFormat));
        MOCK_CONST_METHOD3(scaleImage,QImage *(QImage *orgImage,int iWidth,int iHeight));
        MOCK_CONST_METHOD1(processImage,QImage *(QImage *iImage));
};

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageFromArray)
{
    QImage *orgImage = new QImage;
    QImage *scaleImage = new QImage;
    QImage *finalImage = new QImage;
    track::info::ImageInfoArray *iArray = new track::info::ImageInfoArray;

    IRILoadImageFromArrayTest rep;
    EXPECT_CALL(rep,loadFromData(iArray,track::info::Info::e_imagePNG)).Times(1)
        .WillOnce(Return(orgImage));
    EXPECT_CALL(rep,scaleImage(orgImage,32,48)).Times(1)
        .WillOnce(Return(scaleImage));
    EXPECT_CALL(rep,processImage(scaleImage)).Times(1)
        .WillOnce(Return(finalImage));

    EXPECT_TRUE(rep.testLoadImageFromArray(iArray,32,48,track::info::Info::e_imagePNG)==finalImage);

    delete finalImage;
}

//-------------------------------------------------------------------------------------------

class IRILoadImageFromFileTest : public ImageRepositaryTest
{
    public:
        MOCK_CONST_METHOD4(loadImageFromArray,QImage *(track::info::ImageInfoArray *iArray,int iWidth,int iHeight,track::info::Info::ImageFormat iFormat));
};

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageFromFileNoFile)
{
    QString referenceName(":/player/Resources/note_no_file_exist.png");
    IRILoadImageFromFileTest rep;
    EXPECT_TRUE(rep.testLoadImageFromFile(referenceName,32,48)==0);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,loadImageFromFileSuccess)
{
    QImage *img = new QImage;
    QString referenceName(":/player/Resources/note.png");
    IRILoadImageFromFileTest rep;
    EXPECT_CALL(rep,loadImageFromArray(A<track::info::ImageInfoArray *>(),32,48,track::info::Info::e_imageUnknown)).Times(1).WillOnce(Return(img));
    EXPECT_TRUE(rep.testLoadImageFromFile(referenceName,32,48)==img);
    delete img;
}

//-------------------------------------------------------------------------------------------

class IRIGetReferenceTest : public ImageRepositaryTest
{
    public:
        MOCK_CONST_METHOD3(loadImageFromFile,QImage *(const QString& iFilename,int iWidth,int iHeight));
};

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,getReferenceUnit)
{
    QString referenceName(":/player/Resources/note.png");
    QImage *refImage = new QImage;
    IRIGetReferenceTest rep;
    EXPECT_CALL(rep,loadImageFromFile(referenceName,32,48)).Times(1).WillOnce(Return(refImage));

    EXPECT_TRUE(rep.getReference(32,48)==refImage);
    EXPECT_TRUE(rep.getReference(32,48)==refImage);
}

//-------------------------------------------------------------------------------------------

TEST(ImageRepositaryImpl,getReferenceIntegration)
{
    ImageRepositaryImpl rep;
    QImage *img = rep.getReference(32,48);
    EXPECT_TRUE(img!=0);
    EXPECT_TRUE(img->width()==32);
    EXPECT_TRUE(img->height()==48);
}

//-------------------------------------------------------------------------------------------
