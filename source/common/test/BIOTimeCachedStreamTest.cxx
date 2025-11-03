
#include "common/test/BIOTimeCachedStreamTest.h"
#include "common/test/BIOStreamFunctionalTest.h"
#include "test/QUnitTestTimer.h"
#include <QApplication>

using namespace testing;
using namespace omega;
using namespace common;

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStreamQtUnitTest::BIOTimeCachedStreamTesterCreateTestFile(const QString& name,tint count)
{
    bool res = true;
    tint *nBuffer = new tint [1024];
    tbyte *oBuffer = new tbyte [1024 * sizeof(tint)];
    BIOBufferedStream file(e_BIOStream_FileCreate | e_BIOStream_FileWrite);

    if(file.open(name))
    {
        tint i,j;
        tint amount;

        for(i=0;i<count && res;i+=amount)
        {
            amount = 1024;
            if((i + amount) > count)
            {
                amount = count - i;
            }

            for(j=i;(j - i)<amount;j++)
            {
                nBuffer[j - i] = j;
            }

            engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(nBuffer),oBuffer,amount);

            if(file.write(oBuffer,amount * sizeof(tint))!=(amount * sizeof(tint)))
            {
                res = false;
            }
        }

        file.close();
    }
    else
    {
        res = false;
    }
    delete [] nBuffer;
    delete [] oBuffer;
    return res;
}

//-------------------------------------------------------------------------------------------

tint *BIOTimeCachedStreamQtUnitTest::BIOTimeCachedStreamTesterCreateVBRBlockSizes(tint noBlocks,tint lowSize,tint highSize,tint seed)
{
    tint *blocks = new tint [noBlocks];

    Random::instance()->seed(seed);
    for(tint i=0;i<noBlocks;i++)
    {
        blocks[i] = static_cast<tint>(static_cast<tfloat64>(highSize - lowSize) * Random::instance()->randomReal1()) + lowSize;
    }
    return blocks;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitTest::openAndReadFromBeginningToEndWithConstantBitRate()
{
    const tint c_noBlocks = 1000;
    const tint c_elementsPerBlock = 256;
    const tint c_fileElementCount = c_noBlocks * c_elementsPerBlock; // 0.2s * 1000 = 200s

    // Setup cache such that it is not aligned with CBR bps rate.
    common::TimeStamp bufferTime(3.1);
    common::TimeStamp cacheTime(5.075);
    BIOTimeCachedStreamSettings::instance()->setBufferTimeLength(bufferTime);
    BIOTimeCachedStreamSettings::instance()->setCacheTimeLength(cacheTime);

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "biotimecacheread1.dat");
    QVERIFY(BIOTimeCachedStreamTesterCreateTestFile(fileName,c_fileElementCount));

    tbyte *eBuffer = new tbyte [c_elementsPerBlock * sizeof(tint)];
    tint *oBuffer = new tint [c_elementsPerBlock];

    BIOTimeCachedStream file;
    QVERIFY(file.open(fileName));

    QCOMPARE(file.size(),static_cast<tint>(c_fileElementCount * sizeof(tint)));
    QCOMPARE(file.size64(),static_cast<tint64>(c_fileElementCount * sizeof(tint)));

    QVERIFY(file.sof());
    QCOMPARE(file.offset(),0);
    QCOMPARE(static_cast<int>(file.offset64()),0);

    QVERIFY(file.readable());
    QVERIFY(!file.writeable());

    QCOMPARE(file.name(),fileName);

    // File is read in blocks of 256 elements each being 0.2s in time.
    // c_elementsPerBlock * 5 * sizeof(tint) * 8 bps
    file.setBitrate(c_elementsPerBlock * 5 * sizeof(tint) * 8);

    tint expectedOffset = 0;
    tint expectedCount = 0;
    for(tint block=0;block<c_noBlocks;block++)
    {
        tint expectAmount = c_elementsPerBlock * sizeof(tint);
        QCOMPARE(file.read(eBuffer,expectAmount),expectAmount);
        engine::writeBigEndian32BitsAsNative(eBuffer,reinterpret_cast<tbyte *>(oBuffer),c_elementsPerBlock);
        for(tint i=0;i<c_elementsPerBlock;i++,expectedCount++)
        {
            if(oBuffer[i]!=expectedCount)
            {
                qDebug("Error on reading block %d",block);
            }
            QCOMPARE(oBuffer[i],expectedCount);
        }

        file.springCleanTheCache();

        expectedOffset = (block + 1) * (c_elementsPerBlock * sizeof(tint));
        QCOMPARE(file.offset(),expectedOffset);
        QCOMPARE(file.offset64(),static_cast<tint64>(expectedOffset));
    }

    QVERIFY(file.eof());
    tbyte closeMem;
    QCOMPARE(file.read(&closeMem,1),0);

    file.close();

    delete [] eBuffer;
    delete [] oBuffer;

    DiskOps::deleteFile(fileName);
    BIOTimeCachedStreamSettings::release();
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitTest::openAndReadBeginningToEndWithVariableBitRate()
{
    const tint c_noBlocks = 1000;
    const tint c_lowElementsPerBlock = 128;
    const tint c_highElementsPerBlock = 256;
    tint *blockSize = BIOTimeCachedStreamTesterCreateVBRBlockSizes(c_noBlocks,c_lowElementsPerBlock,c_highElementsPerBlock,0);

    common::TimeStamp bufferTime(3.1);
    common::TimeStamp cacheTime(5.075);
    BIOTimeCachedStreamSettings::instance()->setBufferTimeLength(bufferTime);
    BIOTimeCachedStreamSettings::instance()->setCacheTimeLength(cacheTime);

    tint fileElementCount = 0;
    for(tint block=0;block<c_noBlocks;block++)
    {
        fileElementCount += blockSize[block];
    }
    tint avgBlockSize = fileElementCount / c_noBlocks;
    tint avgBps = avgBlockSize * 5 * sizeof(tint) * 8;

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "biotimecacheread2.dat");
    QVERIFY(BIOTimeCachedStreamTesterCreateTestFile(fileName,fileElementCount));

    tbyte *eBuffer = new tbyte [c_highElementsPerBlock * sizeof(tint)];
    tint *oBuffer = new tint [c_highElementsPerBlock];

    BIOTimeCachedStream file;
    QVERIFY(file.open(fileName));

    QCOMPARE(file.size(),static_cast<tint>(fileElementCount * sizeof(tint)));
    QCOMPARE(file.size64(),static_cast<tint64>(fileElementCount * sizeof(tint)));

    QVERIFY(file.sof());
    QCOMPARE(file.offset(),0);
    QCOMPARE(static_cast<int>(file.offset64()),0);

    QVERIFY(file.readable());
    QVERIFY(!file.writeable());

    QCOMPARE(file.name(),fileName);

    file.setBitrate(avgBps);

    // File is read in blocks of 256 elements each being 0.2s in time.
    tint expectedOffset = 0;
    tint expectedCount = 0;
    for(tint block=0;block<c_noBlocks;block++)
    {
        tint expectAmount = blockSize[block] * sizeof(tint);
        QCOMPARE(file.read(eBuffer,expectAmount),static_cast<tint>(expectAmount));
        engine::writeBigEndian32BitsAsNative(eBuffer,reinterpret_cast<tbyte *>(oBuffer),blockSize[block]);
        for(tint i=0;i<blockSize[block];i++,expectedCount++)
        {
            QCOMPARE(oBuffer[i],static_cast<tint>(expectedCount));
        }

        file.springCleanTheCache();

        expectedOffset += expectAmount;
        QCOMPARE(file.offset(),static_cast<tint>(expectedOffset));
        QCOMPARE(file.offset64(),static_cast<tint64>(expectedOffset));
    }

    QVERIFY(file.eof());
    tbyte closeMem;
    QCOMPARE(file.read(&closeMem,1),0);

    file.close();

    delete [] eBuffer;
    delete [] oBuffer;
    delete [] blockSize;

    DiskOps::deleteFile(fileName);
    BIOTimeCachedStreamSettings::release();
}

//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStreamQtUnitTest::BIOTimeCachedStreamTesterCreateTestMultiplexedFile(const QString& name,tint noBlocks,tint lowSize,tint highSize,tint seed)
{
    tint bps = 0;
    bool res = true;
    tint *blockSize = BIOTimeCachedStreamTesterCreateVBRBlockSizes(noBlocks,lowSize,highSize,seed);
    BIOBufferedStream file(e_BIOStream_FileCreate | e_BIOStream_FileWrite);

    // |HHH|DDDDDDDDD|HHH|DDDDDDD|HHH|DDDDDDD
    // Header structure
    // - Sync Pattern, 2 bytes - 0xffed
    // - Number of elements in block, 2 bytes (as Big Endian)

    if(file.open(name))
    {
        tint i,j;
        tint count = 0;
        tint blockCount = 0;

        for(i=0;i<noBlocks && res;i++)
        {
            tint amount = blockSize[i];
            tint *nBuffer = new tint [amount];
            tbyte *oBuffer = new tbyte [4 + (amount * sizeof(tint))];

            for(j=0;j<amount;j++,count++)
            {
                nBuffer[j] = count;
            }

            tint hdr = 0xffed0000 | (amount & 0x0000ffff);
            engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(&hdr),oBuffer,1);
            engine::writeNative32BitsAsBigEndian(reinterpret_cast<const tbyte *>(nBuffer),&oBuffer[4],amount);

            amount = 4 + (amount * sizeof(tint));

            if(file.write(oBuffer,amount)!=amount)
            {
                res = false;
            }

            delete [] nBuffer;
            delete [] oBuffer;

            blockCount += amount;
        }

        bps = (blockCount * 5) / noBlocks;

        file.close();
    }
    else
    {
        res = false;
    }
    delete [] blockSize;
    return (res) ? bps : -1;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitTest::openAndReadMultiplexedVBRStreamBeginningToEnd()
{
    const tint c_noBlocks = 1000;
    const tint c_lowElementsPerBlock = 128;
    const tint c_highElementsPerBlock = 256;

    common::TimeStamp bufferTime(3.1);
    common::TimeStamp cacheTime(5.075);
    BIOTimeCachedStreamSettings::instance()->setBufferTimeLength(bufferTime);
    BIOTimeCachedStreamSettings::instance()->setCacheTimeLength(cacheTime);

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "biotimecacheread3.dat");
    tint bps = BIOTimeCachedStreamTesterCreateTestMultiplexedFile(fileName,c_noBlocks,c_lowElementsPerBlock,c_highElementsPerBlock,0);
    QVERIFY(bps > 0);

    BIOTimeCachedStream file;
    QVERIFY(file.open(fileName));

    QVERIFY(file.sof());
    QCOMPARE(file.offset(),0);
    QCOMPARE(static_cast<int>(file.offset64()),0);

    QVERIFY(file.readable());
    QVERIFY(!file.writeable());

    QCOMPARE(file.name(),fileName);

    file.setBitrate(bps);

    tint expectOffset = 0;
    tint expectedCount = 0;

    for(tint block=0;block<c_noBlocks;block++)
    {
        tbyte hdrIn[4];
        tint hdr,hdrBookmark;

        QCOMPARE(file.read(hdrIn,4),4);
        engine::writeBigEndian32BitsAsNative(hdrIn,reinterpret_cast<tbyte *>(&hdr),1);
        QCOMPARE(static_cast<tint>(hdr & 0xffff0000),static_cast<tint>(0xffed0000));

        tint noElementsInBlock = hdr & 0x0000ffff;
        QVERIFY(c_lowElementsPerBlock <= noElementsInBlock);
        QVERIFY(c_highElementsPerBlock >= noElementsInBlock);

        hdrBookmark = file.bookmark(-4);
        QVERIFY(0 < hdrBookmark);

        tbyte *rBuffer = new tbyte [noElementsInBlock * sizeof(tint)];
        tint *oBuffer = new tint [noElementsInBlock];

        QCOMPARE(file.read(rBuffer,noElementsInBlock * sizeof(tint)),static_cast<tint>(noElementsInBlock * sizeof(tint)));
        engine::writeBigEndian32BitsAsNative(rBuffer,reinterpret_cast<tbyte *>(oBuffer),noElementsInBlock);

        for(tint i=0;i<noElementsInBlock;i++,expectedCount++)
        {
            QCOMPARE(oBuffer[i],static_cast<tint>(expectedCount));
        }

        delete [] rBuffer;
        delete [] oBuffer;

        file.springCleanTheCache();

        tint increment = 4 + (noElementsInBlock * sizeof(tint));
        QCOMPARE(file.offset(),static_cast<tint>(expectOffset + increment));
        QCOMPARE(file.offset64(),static_cast<tint64>(expectOffset + increment));

        QVERIFY(file.position(hdrBookmark));
        QCOMPARE(file.offset(),static_cast<tint>(expectOffset));
        QCOMPARE(file.offset64(),static_cast<tint64>(expectOffset));
        file.deleteBookmark(hdrBookmark);

        QCOMPARE(file.read(hdrIn,4),4);
        engine::writeBigEndian32BitsAsNative(hdrIn,reinterpret_cast<tbyte *>(&hdr),1);
        QCOMPARE(static_cast<tint>(hdr & 0xffff0000),static_cast<tint>(0xffed0000));
        QCOMPARE(static_cast<tint>(hdr & 0x0000ffff),static_cast<tint>(noElementsInBlock));

        increment -= 4;
        QVERIFY(file.seek(increment,e_Seek_Current));

        expectOffset += increment + 4;
        QCOMPARE(file.offset(),static_cast<tint>(expectOffset));
        QCOMPARE(file.offset64(),static_cast<tint64>(expectOffset));
    }

    QVERIFY(file.eof());
    tbyte closeMem;
    QCOMPARE(file.read(&closeMem,1),0);

    file.close();

    DiskOps::deleteFile(fileName);
    BIOTimeCachedStreamSettings::release();
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitTest::BIOTimeCachedStreamTesterReadAndSeekFromPositions(tint seed,BIOStreamPosition fromPosition)
{
    const tint c_noSeekTests = 20;

    const tint c_noBlocks = 1000;
    const tint c_elementsPerBlock = 256;
    const tint c_fileElementCount = c_noBlocks * c_elementsPerBlock; // 0.2s * 1000 = 200s

    // Setup cache such that it is not aligned with CBR bps rate.
    common::TimeStamp bufferTime(2.0);
    common::TimeStamp cacheTime(3.0);
    BIOTimeCachedStreamSettings::instance()->setBufferTimeLength(bufferTime);
    BIOTimeCachedStreamSettings::instance()->setCacheTimeLength(cacheTime);

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "biotimecacheread4.dat");
    QVERIFY(BIOTimeCachedStreamTesterCreateTestFile(fileName,c_fileElementCount));

    BIOTimeCachedStream file;
    QVERIFY(file.open(fileName));

    QCOMPARE(file.size(),static_cast<tint>(c_fileElementCount * sizeof(tint)));
    QCOMPARE(file.size64(),static_cast<tint64>(c_fileElementCount * sizeof(tint)));

    QVERIFY(file.sof());
    QCOMPARE(file.offset(),0);
    QCOMPARE(static_cast<int>(file.offset64()),0);

    QVERIFY(file.readable());
    QVERIFY(!file.writeable());

    QCOMPARE(file.name(),fileName);

    file.setBitrate(c_elementsPerBlock * 5 * sizeof(tint) * 8);

    Random::instance()->seed(seed);

    for(tint runNumber = 0; runNumber < c_noSeekTests; runNumber++)
    {
        common::TimeStamp trackTimeLength = static_cast<tfloat64>(c_noBlocks) * 0.2;
        common::TimeStamp playFor = Random::instance()->randomReal1() * 6.0;
        common::TimeStamp startPlayingFrom = static_cast<tfloat64>(trackTimeLength - playFor) * Random::instance()->randomReal1();

        tfloat64 timePerElement = static_cast<tfloat64>(trackTimeLength) / static_cast<tfloat64>(c_noBlocks * c_elementsPerBlock);

        tint startPlayFromElement = static_cast<tint>(floor(static_cast<tfloat64>(startPlayingFrom) / timePerElement));
        if(startPlayFromElement > c_fileElementCount)
        {
            startPlayFromElement = c_fileElementCount;
        }

        tint noElementsToRead = static_cast<tint>(floor(static_cast<tfloat64>(playFor) / timePerElement));
        if((startPlayFromElement + noElementsToRead) > c_fileElementCount)
        {
            noElementsToRead = c_fileElementCount - startPlayFromElement;
        }

        tint offset,fOffset;
        offset = startPlayFromElement * sizeof(tint);
        if(fromPosition == e_Seek_Current)
        {
            fOffset = offset - file.offset();
        }
        else if(fromPosition == e_Seek_End)
        {
            fOffset = offset - file.size();
        }
        else
        {
            fOffset = offset;
        }

        QVERIFY(file.seek(fOffset,fromPosition));
        QCOMPARE(file.offset(),static_cast<tint>(offset));
        QCOMPARE(file.offset64(),static_cast<tint64>(offset));

        tint element = startPlayFromElement;
        for(tint count = 0; count < noElementsToRead; count++, element++, offset+=sizeof(tint))
        {
            tint value;
            tbyte elementIn[4];

            QCOMPARE(file.read(elementIn,sizeof(tint)),static_cast<tint>(sizeof(tint)));
            QCOMPARE(file.offset(),static_cast<tint>(offset+sizeof(tint)));
            QCOMPARE(file.offset64(),static_cast<tint64>(offset+sizeof(tint)));

            engine::writeBigEndian32BitsAsNative(elementIn,reinterpret_cast<tbyte *>(&value),1);
            QCOMPARE(value,static_cast<tint>(element));

            if(count>0 && (count & 0x000000ff)==0)
            {
                file.springCleanTheCache();
            }
        }
    }

    file.close();

    DiskOps::deleteFile(fileName);
    BIOTimeCachedStreamSettings::release();
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitTest::readAndSeekFromBeginningPositions()
{
    BIOTimeCachedStreamTesterReadAndSeekFromPositions(1,e_Seek_Start);
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitTest::readAndSeekFromCurrentPositions()
{
    BIOTimeCachedStreamTesterReadAndSeekFromPositions(2,e_Seek_Current);
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitTest::readAndSeekFromEndPositions()
{
    BIOTimeCachedStreamTesterReadAndSeekFromPositions(3,e_Seek_End);
}

//-------------------------------------------------------------------------------------------
// Purpose to ensure that local seeks that maybe done when reading in the neighbourhood of
// a codec's frame do not cause a penalty by continously clearing the buffer out on each
// call to seek as apposed to a seek to a different position within the file.
//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitTest::readBlocksWithSequentialSeeksInBlockFromMultiplePositions()
{
    const tint c_noBlocks = 30 * 60 * 10; // minutes * seconds * blocks per second
    const tint c_elementsPerBlock = 500; // 500 elements = 1000 bytes per block, each block being 0.1s of audio
    const tint c_fileElementCount = c_noBlocks * c_elementsPerBlock;
    const tint c_bitRate = 80000; // 1000 * 10 * 8

    const tint c_noOfAudioSeekPositions = 10;

    common::TimeStamp bufferTime(1.0);
    common::TimeStamp cacheTime(3.0);
    BIOTimeCachedStreamSettings::instance()->setBufferTimeLength(bufferTime);
    BIOTimeCachedStreamSettings::instance()->setCacheTimeLength(cacheTime);

    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "biotimecacheread5.dat");

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_fileElementCount,c_fileElementCount / 10));
    testFileWriter.close();

    Random *random = Random::instance();
    random->seed(1);

    BIOTimeCachedStream file;
    QVERIFY(file.open(fileName));

    QCOMPARE(file.size(),static_cast<tint>(c_fileElementCount * sizeof(tushort)));
    QCOMPARE(file.size64(),static_cast<tint64>(c_fileElementCount * sizeof(tushort)));

    QVERIFY(file.sof());
    QCOMPARE(file.offset(),0);
    QCOMPARE(static_cast<int>(file.offset64()),0);

    QVERIFY(file.readable());
    QVERIFY(!file.writeable());

    QCOMPARE(file.name(),fileName);

    file.setBitrate(c_bitRate);

    const tint c_noOfReadTestsInBlock = 10; // number of seek and read tests to perform in block

    tint *elementsInBlock = new tint [c_noOfReadTestsInBlock * 2]; // element index, no of elements
    for(tint readTestCount = 0; readTestCount < c_noOfReadTestsInBlock; readTestCount++)
    {
        tint index = readTestCount << 1;
        tint fromElement = static_cast<tint>(floor(static_cast<tfloat64>(c_elementsPerBlock) * random->randomReal1()));
        if(fromElement>0 && fromElement<c_elementsPerBlock)
        {
            tint noOfRemainingElements = c_elementsPerBlock - fromElement;
            tint noElements = static_cast<tint>(static_cast<tfloat64>(noOfRemainingElements) * random->randomReal1());
            if(noElements > 0)
            {
                elementsInBlock[index + 0] = fromElement;
                elementsInBlock[index + 1] = noElements;
            }
            else
            {
                readTestCount--;
            }
        }
        else
        {
            readTestCount--;
        }
    }

    tchar *readMem = new tchar [c_elementsPerBlock * 2];

    {
        QUnitTestTimer timer;

        for(tint audioSeekPositionCount = 0; audioSeekPositionCount < c_noOfAudioSeekPositions; audioSeekPositionCount++)
        {
            tint fromBlock = static_cast<tint>(floor(static_cast<tfloat64>(c_noBlocks) * random->randomReal1()));
            if(fromBlock < c_noBlocks)
            {
                tint noOfRemainingBlocks = c_noBlocks - fromBlock;
                if(noOfRemainingBlocks > 100)
                {
                    noOfRemainingBlocks = 100;
                }
                tint noBlocksToRead = static_cast<tint>(static_cast<tfloat64>(noOfRemainingBlocks) * random->randomReal1());
                if(noBlocksToRead > 0)
                {
                    for(tint blockIdx = fromBlock; blockIdx < (fromBlock + noBlocksToRead); blockIdx++)
                    {
                        tint fromBytePosition = blockIdx * c_elementsPerBlock * 2;

                        for(tint readTestCount = 0; readTestCount < c_noOfReadTestsInBlock; readTestCount++)
                        {
                            tint elementBytePosition = fromBytePosition + (elementsInBlock[(readTestCount << 1) + 0] << 1);
                            tint length = elementsInBlock[(readTestCount << 1) + 1] << 1;
                            QVERIFY(file.seek(elementBytePosition,e_Seek_Start));
                            QCOMPARE(file.read(readMem,length),length);
                            QVERIFY(writer.verifyIsExpectedContents(readMem,elementBytePosition,length));
                        }
                        file.springCleanTheCache();
                    }
                }
                else
                {
                    audioSeekPositionCount--;
                }
            }
            else
            {
                audioSeekPositionCount--;
            }
        }
    }

    delete [] readMem;

    file.close();

    delete [] elementsInBlock;

    DiskOps::deleteFile(fileName);
    BIOTimeCachedStreamSettings::release();
}

//-------------------------------------------------------------------------------------------

BIOTimeCachedStreamQtUnitApplication::BIOTimeCachedStreamQtUnitApplication(int argc,char **argv) : QCoreApplication(argc,argv),
    m_succeeded(false)
{
    QTimer::singleShot(10,this,SLOT(runTests()));
}

//-------------------------------------------------------------------------------------------

BIOTimeCachedStreamQtUnitApplication::~BIOTimeCachedStreamQtUnitApplication()
{}

//-------------------------------------------------------------------------------------------

bool BIOTimeCachedStreamQtUnitApplication::testSucceeded() const
{
    return m_succeeded;
}

//-------------------------------------------------------------------------------------------

void BIOTimeCachedStreamQtUnitApplication::runTests()
{
    BIOTimeCachedStreamQtUnitTest tests;
    m_succeeded = (QTest::qExec(&tests)==0) ? true : false;
    quit();
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,RunQtUnitTests)
{
    QStringList libPaths = QApplication::libraryPaths();
    {
        char *argv = 0;
        BIOTimeCachedStreamQtUnitApplication tester(0,&argv);
        tester.exec();
        ASSERT_TRUE(tester.testSucceeded());
    }
    QApplication::setLibraryPaths(libPaths);
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStreamSettings,setAndGetCacheTimeLength)
{
    common::TimeStamp lenA(7.0);
    common::TimeStamp lenB(10.0);

    {
        BIOTimeCachedStreamSettings::instance()->setCacheTimeLength(lenA);
        BIOTimeCachedStreamSettings::release();
    }

    {
        ASSERT_NEAR(static_cast<tfloat64>(BIOTimeCachedStreamSettings::instance()->getCacheTimeLength()),static_cast<tfloat64>(lenA),0.000001);
        BIOTimeCachedStreamSettings::instance()->setCacheTimeLength(lenB);
        BIOTimeCachedStreamSettings::release();
    }

    {
        ASSERT_NEAR(static_cast<tfloat64>(BIOTimeCachedStreamSettings::instance()->getCacheTimeLength()),static_cast<tfloat64>(lenB),0.000001);
        BIOTimeCachedStreamSettings::release();
    }
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStreamSettings,setAndGetBufferTimeLength)
{
    common::TimeStamp lenA(7.0);
    common::TimeStamp lenB(10.0);

    {
        BIOTimeCachedStreamSettings::instance()->setBufferTimeLength(lenA);
        BIOTimeCachedStreamSettings::release();
    }

    {
        ASSERT_NEAR(static_cast<tfloat64>(BIOTimeCachedStreamSettings::instance()->getBufferTimeLength()),static_cast<tfloat64>(lenA),0.000001);
        BIOTimeCachedStreamSettings::instance()->setBufferTimeLength(lenB);
        BIOTimeCachedStreamSettings::release();
    }

    {
        ASSERT_NEAR(static_cast<tfloat64>(BIOTimeCachedStreamSettings::instance()->getBufferTimeLength()),static_cast<tfloat64>(lenB),0.000001);
        BIOTimeCachedStreamSettings::release();
    }
}

//-------------------------------------------------------------------------------------------

class BIOTimeCachedStreamInitialCacheSize : public BIOTimeCachedStream
{
    public:
        MOCK_CONST_METHOD0(getCachedFileConst,const CachedFileStream *());
        tint testInitialCacheSize() const;
};

//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStreamInitialCacheSize::testInitialCacheSize() const
{
    return initialCacheSize();
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,initialCacheSizeWhen5PercentOfFileSize)
{
    CachedFileStreamMock cachedFile;
    EXPECT_CALL(cachedFile,size()).WillRepeatedly(Return(4000000));

    BIOTimeCachedStreamInitialCacheSize file;
    EXPECT_CALL(file,getCachedFileConst()).WillRepeatedly(Return(&cachedFile));

    ASSERT_EQ(200000,file.testInitialCacheSize());
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,initialCacheSizeWhen256KiloBytes)
{
    CachedFileStreamMock cachedFile;
    EXPECT_CALL(cachedFile,size()).WillRepeatedly(Return(8000000));

    BIOTimeCachedStreamInitialCacheSize file;
    EXPECT_CALL(file,getCachedFileConst()).WillRepeatedly(Return(&cachedFile));

    ASSERT_EQ(1024 * 256,file.testInitialCacheSize());
}

//-------------------------------------------------------------------------------------------

class BIOTimeCachedStreamLengthFromTimeTest : public BIOTimeCachedStream
{
    public:
        BIOTimeCachedStreamLengthFromTimeTest(tint bitrate);
        tint64 testLengthFromTime(const common::TimeStamp& tLen) const;
};

//-------------------------------------------------------------------------------------------

BIOTimeCachedStreamLengthFromTimeTest::BIOTimeCachedStreamLengthFromTimeTest(tint bitrate) : BIOTimeCachedStream()
{
    m_bitrate = bitrate;
}

//-------------------------------------------------------------------------------------------

tint64 BIOTimeCachedStreamLengthFromTimeTest::testLengthFromTime(const common::TimeStamp& tLen) const
{
    return lengthFromTime(tLen);
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,lengthFromTimeGivenBitRateNotSet)
{
    BIOTimeCachedStreamLengthFromTimeTest file(0);

    common::TimeStamp tA(0.0);
    EXPECT_EQ(-1,file.testLengthFromTime(tA));
    common::TimeStamp tB(1.0);
    EXPECT_EQ(-1,file.testLengthFromTime(tB));
    common::TimeStamp tC(2.0);
    EXPECT_EQ(-1,file.testLengthFromTime(tC));
    common::TimeStamp tD(3.123456);
    EXPECT_EQ(-1,file.testLengthFromTime(tD));
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,lengthFromTimeGiven128kpbs)
{
    BIOTimeCachedStreamLengthFromTimeTest file(128000);

    common::TimeStamp tA(0.0);
    EXPECT_EQ(0,file.testLengthFromTime(tA));
    common::TimeStamp tB(1.0);
    EXPECT_EQ(16000,file.testLengthFromTime(tB));
    common::TimeStamp tC(2.0);
    EXPECT_EQ(32000,file.testLengthFromTime(tC));
    common::TimeStamp tD(3.123456);
    EXPECT_EQ(49975,file.testLengthFromTime(tD));
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,lengthFromTimeGiven192kpbs)
{
    BIOTimeCachedStreamLengthFromTimeTest file(192000);

    common::TimeStamp tA(0.0);
    EXPECT_EQ(0,file.testLengthFromTime(tA));
    common::TimeStamp tB(1.0);
    EXPECT_EQ(24000,file.testLengthFromTime(tB));
    common::TimeStamp tC(2.0);
    EXPECT_EQ(48000,file.testLengthFromTime(tC));
    common::TimeStamp tD(3.123456);
    EXPECT_EQ(74963,file.testLengthFromTime(tD));
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,lengthFromTimeGivenCDTrack)
{
    // CD: 44100Hz * 16Bits * 2Chs = 1,411,200bps
    BIOTimeCachedStreamLengthFromTimeTest file(1411200);

    common::TimeStamp tA(0.0);
    EXPECT_EQ(0,file.testLengthFromTime(tA));
    common::TimeStamp tB(1.0);
    EXPECT_EQ(176400,file.testLengthFromTime(tB));
    common::TimeStamp tC(2.0);
    EXPECT_EQ(352800,file.testLengthFromTime(tC));
    common::TimeStamp tD(3.123456);
    EXPECT_EQ(550978,file.testLengthFromTime(tD));
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,lengthFromTimeGivenHiResTrack)
{
    // HiDef Multi-Channel: 192000Hz * 24Bits * 6Chs = 27,648,000bps
    BIOTimeCachedStreamLengthFromTimeTest file(27648000);

    common::TimeStamp tA(0.0);
    EXPECT_EQ(0,file.testLengthFromTime(tA));
    common::TimeStamp tB(1.0);
    EXPECT_EQ(3456000,file.testLengthFromTime(tB));
    common::TimeStamp tC(2.0);
    EXPECT_EQ(6912000,file.testLengthFromTime(tC));
}

//-------------------------------------------------------------------------------------------

class BIOTimeCachedStreamSpringCleanTheCache : public BIOTimeCachedStream
{
    public:
        MOCK_METHOD0(getCachedFile,CachedFileStream *());
        MOCK_CONST_METHOD0(getCachedFileConst,const CachedFileStream *());
        MOCK_METHOD0(readPosition,tint64&());
};

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,springCleanTheCache)
{
    common::TimeStamp bufferTime(4.0);
    common::TimeStamp cacheTime(15.0);
    BIOTimeCachedStreamSettings::instance()->setBufferTimeLength(bufferTime);
    BIOTimeCachedStreamSettings::instance()->setCacheTimeLength(cacheTime);

    tint64 readPosition = 0;

    CachedFileStreamMock cachedFile;
    EXPECT_CALL(cachedFile,size()).WillRepeatedly(Return(11000));

    using ::testing::InSequence;
    {
        InSequence dropSequence;
        EXPECT_CALL(cachedFile,cache(0,1500)).Times(1).WillOnce(Return(1500));
        EXPECT_CALL(cachedFile,drop(0,500)).Times(1);
        EXPECT_CALL(cachedFile,drop(400,800)).Times(1);
        EXPECT_CALL(cachedFile,drop(1100,400)).Times(1);
        EXPECT_CALL(cachedFile,drop(1400,1700)).Times(1);
        EXPECT_CALL(cachedFile,drop(3000,6500)).Times(1);
        EXPECT_CALL(cachedFile,drop(9400,1100)).Times(1);
    }

    BIOTimeCachedStreamSpringCleanTheCache file;
    EXPECT_CALL(file,readPosition()).WillRepeatedly(ReturnRef(readPosition));
    EXPECT_CALL(file,getCachedFile()).WillRepeatedly(Return(&cachedFile));
    EXPECT_CALL(file,getCachedFileConst()).WillRepeatedly(Return(&cachedFile));

    file.setBitrate(800);

    readPosition = 1000;
    file.springCleanTheCache();
    readPosition = 1700;
    file.springCleanTheCache();
    readPosition = 2000;
    file.springCleanTheCache();
    readPosition = 3600;
    file.springCleanTheCache();
    readPosition = 10000;
    file.springCleanTheCache();
    readPosition = 11000;
    file.springCleanTheCache();

    BIOTimeCachedStreamSettings::release();
}

//-------------------------------------------------------------------------------------------

class BIOTimeCachedStreamCalculateL2CacheSizeTest : public BIOTimeCachedStream
{
    public:
        BIOTimeCachedStreamCalculateL2CacheSizeTest(tint bitrate);
        tint testCalculateL2CacheSize() const;
        MOCK_CONST_METHOD0(getCachedFileConst,const CachedFileStream *());
};

//-------------------------------------------------------------------------------------------

BIOTimeCachedStreamCalculateL2CacheSizeTest::BIOTimeCachedStreamCalculateL2CacheSizeTest(tint bitrate) : BIOTimeCachedStream()
{
    m_bitrate = bitrate;
}

//-------------------------------------------------------------------------------------------

tint BIOTimeCachedStreamCalculateL2CacheSizeTest::testCalculateL2CacheSize() const
{
    return calculateL2CacheSize();
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,calculateL2CacheSizeGivenNoBitrateAnd5PercentOfFileIsInitialCacheRequest)
{
    CachedFileStreamMock cachedFile;
    EXPECT_CALL(cachedFile,size()).WillRepeatedly(Return(500000));

    BIOTimeCachedStreamCalculateL2CacheSizeTest file(0);
    EXPECT_CALL(file,getCachedFileConst()).WillRepeatedly(Return(&cachedFile));

    ASSERT_EQ(2048,file.testCalculateL2CacheSize());
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,calculateL2CacheSizeGivenNoBitrateAnd256KbInitialCacheRequest)
{
    CachedFileStreamMock cachedFile;
    EXPECT_CALL(cachedFile,size()).WillRepeatedly(Return(6000000));

    BIOTimeCachedStreamCalculateL2CacheSizeTest file(0);
    EXPECT_CALL(file,getCachedFileConst()).WillRepeatedly(Return(&cachedFile));

    ASSERT_EQ(26624,file.testCalculateL2CacheSize());
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,calculateL2CacheSizeGivenBitrateAt800bps)
{
    BIOTimeCachedStreamCalculateL2CacheSizeTest file(800);
    ASSERT_EQ(1024,file.testCalculateL2CacheSize());
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,calculateL2CacheSizeGivenBitrateAt128kbps)
{
    BIOTimeCachedStreamCalculateL2CacheSizeTest file(128000);
    ASSERT_EQ(3072,file.testCalculateL2CacheSize());
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,calculateL2CacheSizeGivenBitrateAt1411kbpsCDQuality)
{
    // Cache time set to 5s
    // 16-bit * 2Chs * 44100Hz
    BIOTimeCachedStreamCalculateL2CacheSizeTest file(16 * 2 * 44100);
    ASSERT_EQ(34816,file.testCalculateL2CacheSize());
}

//-------------------------------------------------------------------------------------------

TEST(BIOTimeCachedStream,calculateL2CacheSizeGivenBitrateAt27648kbpsHiResMultiChannelQuality)
{
    // Cache time set to 5s
    // 24-bit * 6Chs * 192000Hz
    BIOTimeCachedStreamCalculateL2CacheSizeTest file(24 * 6 * 192000);
    ASSERT_EQ(691200,file.testCalculateL2CacheSize());
}

//-------------------------------------------------------------------------------------------
