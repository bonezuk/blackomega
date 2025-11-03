#include "common/test/BIOStreamFunctionalTest.h"

#include <QtTest/QtTest>

//-------------------------------------------------------------------------------------------

BIOStreamFunctionalTest::BIOStreamFunctionalTest()
{}

//-------------------------------------------------------------------------------------------

BIOStreamFunctionalTest::~BIOStreamFunctionalTest()
{}

//-------------------------------------------------------------------------------------------

bool BIOStreamFunctionalTest::writeUShortTestFile(BIOStream *pFile,tint64 noElements,tint elementsPerWrite)
{
    tushort *buffer;
    tuint64 count;
    tint64 eIndex;
    bool res = true;

    buffer = new tushort [elementsPerWrite];

    eIndex = 0;
    count = 0;

    while(eIndex<noElements && res)
    {
        tint i,amount;

        for(i=0;i<elementsPerWrite && eIndex<noElements;i++,eIndex++)
        {
            buffer[i] = static_cast<tushort>(count & 0x000000000000ffffULL);
            count++;
        }

        amount = pFile->write(reinterpret_cast<const tbyte *>(buffer),i * sizeof(tushort));
        if(amount!=(i * sizeof(tushort)))
        {
            res = false;
        }
    }

    delete [] buffer;

    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOStreamFunctionalTest::verifyUShortTestFileSize(BIOStream *pFile,tint64 noElements)
{
    tint64 expectSize = noElements * sizeof(tushort);
    return (pFile->size64() == expectSize);
}

//-------------------------------------------------------------------------------------------

bool BIOStreamFunctionalTest::verifyUShortTestFileContents(BIOStream *pFile,tint elementsPerRead)
{
    bool res = true;

    if(pFile->seek(0,e_Seek_Start))
    {
        tint64 eCount = 0,eSize = 0;
        bool loop = true;
        tushort *buffer = new tushort [elementsPerRead];

        while(loop && res)
        {
            tint amount = pFile->read(reinterpret_cast<tbyte *>(buffer),elementsPerRead * sizeof(tushort));

            if(amount >= 0)
            {
                if(amount>0)
                {
                    eSize += amount;

                    if(!(amount % sizeof(tushort)))
                    {
                        amount /= sizeof(tushort);
                        for(tint i=0;i<amount && res;i++,eCount++)
                        {
                            tushort eValue = static_cast<tushort>(eCount & 0x000000000000ffffULL);
                            if(eValue!=buffer[i])
                            {
                                res = false;
                            }
                        }
                    }
                }
                else
                {
                    loop = false;
                }
            }
            else
            {
                res = false;
            }
        }

        if(eSize!=pFile->size64())
        {
            res = false;
        }

        delete [] buffer;
    }
    else
    {
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

tushort BIOStreamFunctionalTest::expectedValueAt(tint64 position,bool isGoogleTest) const
{
    if(isGoogleTest)
    {
        EXPECT_EQ(0,position & 0x1);
    }
    return static_cast<tushort>((position >> 1) & 0x000000000000ffffULL);
}

//-------------------------------------------------------------------------------------------

bool BIOStreamFunctionalTest::verifyFileContentsFromBothDirections(BIOStream *pFile,tint elementsPerRead)
{
    tushort *buffer = new tushort [elementsPerRead];
    tint64 sPos,ePos;
    bool res = true;

    sPos = 0;
    ePos = pFile->size64() - (elementsPerRead * sizeof(tushort));

    while(sPos<ePos && res)
    {
        tint amount = elementsPerRead * sizeof(tushort);

        if(pFile->seek64(sPos,e_Seek_Start))
        {
            if(pFile->read(reinterpret_cast<tbyte *>(buffer),amount)==amount)
            {
                tint i;
                tint64 pos;

                for(i=0,pos=sPos;pos<sPos+amount && res;pos+=2,i++)
                {
                    if(expectedValueAt(pos,true)!=buffer[i])
                    {
                        res = false;
                    }
                }

                if(res)
                {
                    if(pFile->seek64(ePos,e_Seek_Start))
                    {
                        if(pFile->read(reinterpret_cast<tbyte *>(buffer),amount)==amount)
                        {
                            for(i=0,pos=ePos;pos<ePos+amount && res;pos+=2,i++)
                            {
                                if(expectedValueAt(pos,true)!=buffer[i])
                                {
                                    res = false;
                                }
                            }
                        }
                        else
                        {
                            res = false;
                        }
                    }
                    else
                    {
                        res = false;
                    }
                }
            }
            else
            {
                res = false;
            }
        }
        else
        {
            res = false;
        }

        sPos += amount;
        ePos -= amount;
    }

    delete [] buffer;
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOStreamFunctionalTest::verifyIsExpectedContents(QSharedPointer<QByteArray> pData,tint fromPosition,tint length)
{
    bool res = false;

    if(!pData.isNull() && pData->size()==length)
    {
        const tchar *mem = reinterpret_cast<const tchar *>(pData->constData());
        res = verifyIsExpectedContents(mem,fromPosition,length);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool BIOStreamFunctionalTest::verifyIsExpectedContents(const tchar *mem,tint fromPosition,tint length)
{
    bool res = false;

    if((length & 0x1) == 0)
    {
        tint64 position = static_cast<tint64>(fromPosition);
        const tushort *data = reinterpret_cast<const tushort *>(mem);

        res = true;
        for(tint i=0;i<(length >> 1) && res;i++,position+=2)
        {
            res = (data[i] == expectedValueAt(position,false));
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void BIOStreamFunctionalTestCreateContinousReadAndWrite(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOStream file(e_BIOStream_FileCreate | e_BIOStream_FileRead | e_BIOStream_FileWrite);
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.writeUShortTestFile(&file,noElements,elementsPerIO));
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyUShortTestFileContents(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createContinousReadAndWriteWith2ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateContinousReadAndWrite(fileName,2,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createContinousReadAndWriteWith4ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateContinousReadAndWrite(fileName,4,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createContinousReadAndWriteWith128ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateContinousReadAndWrite(fileName,128,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------


TEST(BIOStreamFunctional,createContinousReadAndWriteWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateContinousReadAndWrite(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createContinousReadAndWriteWith1000000ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateContinousReadAndWrite(fileName,1000000,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

void BIOBufferedStreamFunctionalTestCreateContinousReadAndWrite(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOBufferedStream file(e_BIOStream_FileCreate | e_BIOStream_FileRead | e_BIOStream_FileWrite);
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.writeUShortTestFile(&file,noElements,elementsPerIO));
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyUShortTestFileContents(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createContinousReadAndWriteWith2ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateContinousReadAndWrite(fileName,2,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createContinousReadAndWriteWith4ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateContinousReadAndWrite(fileName,4,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createContinousReadAndWriteWith128ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateContinousReadAndWrite(fileName,128,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------


TEST(BIOBufferedStreamFunctional,createContinousReadAndWriteWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateContinousReadAndWrite(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createContinousReadAndWriteWith1000000ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateContinousReadAndWrite(fileName,1000000,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

void BIOStreamFunctionalTestCreateAndWrite(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOStream file(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.writeUShortTestFile(&file,noElements,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

void BIOStreamFunctionalTestContinousRead(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOStream file(e_BIOStream_FileRead);
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyUShortTestFileContents(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

void BIOStreamFunctionalTestAccessRead(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOStream file(e_BIOStream_FileRead);
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyFileContentsFromBothDirections(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createWriteCloseReopenAndAccessReadWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateAndWrite(fileName,1024,128);
    BIOStreamFunctionalTestAccessRead(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createWriteCloseReopenAndAccessReadWith1000000ElementsAndOpSize1000)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateAndWrite(fileName,1000000,1000);
    BIOStreamFunctionalTestAccessRead(fileName,1000000,1000);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createWriteCloseReopenAndContinousReadWith2ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateAndWrite(fileName,2,128);
    BIOStreamFunctionalTestContinousRead(fileName,2,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createWriteCloseReopenAndContinousReadWith4ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateAndWrite(fileName,4,128);
    BIOStreamFunctionalTestContinousRead(fileName,4,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createWriteCloseReopenAndContinousReadWith128ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateAndWrite(fileName,128,128);
    BIOStreamFunctionalTestContinousRead(fileName,128,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createWriteCloseReopenAndContinousReadWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateAndWrite(fileName,1024,128);
    BIOStreamFunctionalTestContinousRead(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOStreamFunctional,createWriteCloseReopenAndContinousReadWith1000000ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOStreamFunctionalTestCreateAndWrite(fileName,1000000,128);
    BIOStreamFunctionalTestContinousRead(fileName,1000000,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

void BIOBufferedStreamFunctionalTestCreateAndWrite(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOBufferedStream file(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.writeUShortTestFile(&file,noElements,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

void BIOBufferedStreamFunctionalTestContinousRead(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOBufferedStream file(e_BIOStream_FileRead);
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyUShortTestFileContents(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

void BIOBufferedStreamFunctionalTestAccessRead(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOBufferedStream file(e_BIOStream_FileRead);
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyFileContentsFromBothDirections(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createWriteCloseReopenAndAccessReadWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1024,128);
    BIOBufferedStreamFunctionalTestAccessRead(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createWriteCloseReopenAndAccessReadWith1000000ElementsAndOpSize1000)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1000000,1000);
    BIOBufferedStreamFunctionalTestAccessRead(fileName,1000000,1000);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createWriteCloseReopenAndContinousReadWith2ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,2,128);
    BIOBufferedStreamFunctionalTestContinousRead(fileName,2,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createWriteCloseReopenAndContinousReadWith4ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,4,128);
    BIOBufferedStreamFunctionalTestContinousRead(fileName,4,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createWriteCloseReopenAndContinousReadWith128ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,128,128);
    BIOBufferedStreamFunctionalTestContinousRead(fileName,128,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createWriteCloseReopenAndContinousReadWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1024,128);
    BIOBufferedStreamFunctionalTestContinousRead(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOBufferedStreamFunctional,createWriteCloseReopenAndContinousReadWith1000000ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1000000,128);
    BIOBufferedStreamFunctionalTestContinousRead(fileName,1000000,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

void BIOMemoryStreamFunctionalTestContinousRead(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOMemoryStream file;
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyUShortTestFileContents(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

void BIOMemoryStreamFunctionalTestAccessRead(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOMemoryStream file;
    ASSERT_TRUE(file.open(name));

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyFileContentsFromBothDirections(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryStreamFunctional,createWriteCloseReopenAndAccessReadWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1024,128);
    BIOMemoryStreamFunctionalTestAccessRead(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryStreamFunctional,createWriteCloseReopenAndAccessReadWith1000000ElementsAndOpSize1000)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1000000,1000);
    BIOMemoryStreamFunctionalTestAccessRead(fileName,1000000,1000);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryStreamFunctional,createWriteCloseReopenAndContinousReadWith2ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,2,128);
    BIOMemoryStreamFunctionalTestContinousRead(fileName,2,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryStreamFunctional,createWriteCloseReopenAndContinousReadWith4ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,4,128);
    BIOMemoryStreamFunctionalTestContinousRead(fileName,4,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryStreamFunctional,createWriteCloseReopenAndContinousReadWith128ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,128,128);
    BIOMemoryStreamFunctionalTestContinousRead(fileName,128,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryStreamFunctional,createWriteCloseReopenAndContinousReadWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1024,128);
    BIOMemoryStreamFunctionalTestContinousRead(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryStreamFunctional,createWriteCloseReopenAndContinousReadWith1000000ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1000000,128);
    BIOMemoryStreamFunctionalTestContinousRead(fileName,1000000,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

void BIOMemoryFunctionalTestContinousRead(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOBufferedStream iFile(e_BIOStream_FileRead);
    ASSERT_TRUE(iFile.open(name));
    QByteArray mem(iFile.size(),'\0');
    ASSERT_EQ(mem.size(),iFile.read(mem.data(),mem.size()));
    iFile.close();

    BIOMemory file(mem);

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyUShortTestFileContents(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

void BIOMemoryFunctionalTestAccessRead(const QString& name,tint64 noElements,tint elementsPerIO)
{
    BIOBufferedStream iFile(e_BIOStream_FileRead);
    ASSERT_TRUE(iFile.open(name));
    QByteArray mem(iFile.size(),'\0');
    ASSERT_EQ(mem.size(),iFile.read(mem.data(),mem.size()));
    iFile.close();

    BIOMemory file(mem);

    BIOStreamFunctionalTest tester;
    ASSERT_TRUE(tester.verifyUShortTestFileSize(&file,noElements));
    ASSERT_TRUE(tester.verifyFileContentsFromBothDirections(&file,elementsPerIO));

    file.close();
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryFunctional,createWriteCloseReopenAndAccessReadWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1024,128);
    BIOMemoryFunctionalTestAccessRead(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryFunctional,createWriteCloseReopenAndAccessReadWith1000000ElementsAndOpSize1000)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1000000,1000);
    BIOMemoryFunctionalTestAccessRead(fileName,1000000,1000);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryFunctional,createWriteCloseReopenAndContinousReadWith2ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,2,128);
    BIOMemoryFunctionalTestContinousRead(fileName,2,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryFunctional,createWriteCloseReopenAndContinousReadWith4ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,4,128);
    BIOMemoryFunctionalTestContinousRead(fileName,4,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryFunctional,createWriteCloseReopenAndContinousReadWith128ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,128,128);
    BIOMemoryFunctionalTestContinousRead(fileName,128,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryFunctional,createWriteCloseReopenAndContinousReadWith1024ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1024,128);
    BIOMemoryFunctionalTestContinousRead(fileName,1024,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemoryFunctional,createWriteCloseReopenAndContinousReadWith1000000ElementsAndOpSize128)
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "fileiotest.dat");
    BIOBufferedStreamFunctionalTestCreateAndWrite(fileName,1000000,128);
    BIOMemoryFunctionalTestContinousRead(fileName,1000000,128);
    DiskOps::remove(fileName);
}

//-------------------------------------------------------------------------------------------
