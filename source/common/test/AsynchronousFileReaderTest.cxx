#include "common/test/AsynchronousFileReaderTest.h"
#include "common/inc/Random.h"
#include "common/inc/CommonFunctions.h"

#include <QApplication>

//-------------------------------------------------------------------------------------------
// AsynchronousFileReaderTester
//-------------------------------------------------------------------------------------------

AsynchronousFileReaderTester::AsynchronousFileReaderTester(ReadTestFunction pReadFunction,CompleteTestFunction pCompleteFunction) : QObject(),
    m_pReadFunction(pReadFunction),
    m_pCompleteFunction(pCompleteFunction),
    m_testReadIdMap(),
    m_testReadIdMapA(),
    m_testReadIdMapB(),
    m_testReadIdMapC(),
    m_pFile(0),
    m_pFileA(0),
    m_pFileB(0),
    m_pFileC(0),
    m_success(true)
{}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReaderTester::run(AsynchronousFileReader *pFile,tfloat64 timeout)
{
    bool res;
    common::TimeStamp t;

    m_testReadIdMap = m_pReadFunction(pFile);
    res = (!m_testReadIdMap.isEmpty());

    m_pFile = pFile;
    connect(pFile,SIGNAL(completed()),this,SLOT(onCompleted()));

    while(!m_testReadIdMap.isEmpty() && res)
    {
        if(t > timeout)
        {
            res = false;
        }
        else
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            common::msleepThread(200);
        }
        t += 0.2;
    }

    disconnect(pFile,SIGNAL(completed()),this,SLOT(onCompleted()));
    return res && m_success;
}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReaderTester::runMultiple(AsynchronousFileReader *pFileA,AsynchronousFileReader *pFileB,AsynchronousFileReader *pFileC,tfloat64 timeout)
{
    bool res;
    common::TimeStamp t;

    m_testReadIdMapA = m_pReadFunction(pFileA);
    m_testReadIdMapB = m_pReadFunction(pFileB);
    m_testReadIdMapC = m_pReadFunction(pFileC);
    res = (!m_testReadIdMapA.isEmpty() && !m_testReadIdMapB.isEmpty() && !m_testReadIdMapC.isEmpty());

    m_pFileA = pFileA;
    m_pFileB = pFileB;
    m_pFileC = pFileC;

    connect(pFileA,SIGNAL(completed()),this,SLOT(onCompletedA()));
    connect(pFileB,SIGNAL(completed()),this,SLOT(onCompletedB()));
    connect(pFileC,SIGNAL(completed()),this,SLOT(onCompletedC()));

    while(!(m_testReadIdMapA.isEmpty() || m_testReadIdMapB.isEmpty() || m_testReadIdMapC.isEmpty()) && res)
    {
        if(t > timeout)
        {
            res = false;
        }
        else
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            common::msleepThread(200);
        }
        t += 0.2;
    }

    disconnect(pFileA,SIGNAL(completed()),this,SLOT(onCompletedA()));
    disconnect(pFileB,SIGNAL(completed()),this,SLOT(onCompletedB()));
    disconnect(pFileC,SIGNAL(completed()),this,SLOT(onCompletedC()));

    return res && m_success;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onCompleted()
{
    QList<int> ids = m_pFile->hasCompleted();
    for(QList<int>::iterator ppI=ids.begin();ppI!=ids.end();ppI++)
    {
        tint readId = *ppI;
        QSharedPointer<QByteArray> pData = m_pFile->result(readId);
        if(!pData.isNull())
        {
            onComplete(readId,pData);
        }
        else
        {
            onError(readId);
        }
    }
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onCompletedA()
{
    QList<int> ids = m_pFileA->hasCompleted();
    for(QList<int>::iterator ppI=ids.begin();ppI!=ids.end();ppI++)
    {
        tint readId = *ppI;
        QSharedPointer<QByteArray> pData = m_pFileA->result(readId);
        if(!pData.isNull())
        {
            onCompleteA(readId,pData);
        }
        else
        {
            onErrorA(readId);
        }
    }
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onCompletedB()
{
    QList<int> ids = m_pFileB->hasCompleted();
    for(QList<int>::iterator ppI=ids.begin();ppI!=ids.end();ppI++)
    {
        tint readId = *ppI;
        QSharedPointer<QByteArray> pData = m_pFileB->result(readId);
        if(!pData.isNull())
        {
            onCompleteB(readId,pData);
        }
        else
        {
            onErrorB(readId);
        }
    }
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onCompletedC()
{
    QList<int> ids = m_pFileC->hasCompleted();
    for(QList<int>::iterator ppI=ids.begin();ppI!=ids.end();ppI++)
    {
        tint readId = *ppI;
        QSharedPointer<QByteArray> pData = m_pFileC->result(readId);
        if(!pData.isNull())
        {
            onCompleteC(readId,pData);
        }
        else
        {
            onErrorC(readId);
        }
    }
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onComplete(tint readId,QSharedPointer<QByteArray>& data)
{
    bool resA,resB;
    QMap<tint,tint>::iterator ppI = m_testReadIdMap.find(readId);
    resA = (ppI!=m_testReadIdMap.end());
    resB = m_pCompleteFunction(ppI.value(),data);
    m_testReadIdMap.erase(ppI);
    m_success = resA && resB;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onCompleteA(tint readId,QSharedPointer<QByteArray>& data)
{
    bool resA,resB;
    QMap<tint,tint>::iterator ppI = m_testReadIdMapA.find(readId);
    resA = (ppI!=m_testReadIdMapA.end());
    resB = m_pCompleteFunction(ppI.value(),data);
    m_testReadIdMapA.erase(ppI);
    m_success = resA && resB;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onCompleteB(tint readId,QSharedPointer<QByteArray>& data)
{
    bool resA,resB;
    QMap<tint,tint>::iterator ppI = m_testReadIdMapB.find(readId);
    resA = (ppI!=m_testReadIdMapB.end());
    resB = m_pCompleteFunction(ppI.value(),data);
    m_testReadIdMapB.erase(ppI);
    m_success = resA && resB;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onCompleteC(tint readId,QSharedPointer<QByteArray>& data)
{
    bool resA,resB;
    QMap<tint,tint>::iterator ppI = m_testReadIdMapC.find(readId);
    resA = (ppI!=m_testReadIdMapC.end());
    resB = m_pCompleteFunction(ppI.value(),data);
    m_testReadIdMapC.erase(ppI);
    m_success = resA && resB;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onError(int readId)
{
    Log::g_Log.print("Error in processing file with readId=%d\n",readId);
    m_success = false;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onErrorA(int readId)
{
    Log::g_Log.print("Error in processing file with readId=%d\n",readId);
    m_success = false;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onErrorB(int readId)
{
    Log::g_Log.print("Error in processing file with readId=%d\n",readId);
    m_success = false;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderTester::onErrorC(int readId)
{
    Log::g_Log.print("Error in processing file with readId=%d\n",readId);
    m_success = false;
}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReaderTester::cancelAllPending(AsynchronousFileReader *pFile)
{
    bool res;
    tint noReads;
    common::TimeStamp t;

    m_testReadIdMap = m_pReadFunction(pFile);
    res = (!m_testReadIdMap.isEmpty());
    noReads = m_testReadIdMap.size();

    m_pFile = pFile;
    connect(pFile,SIGNAL(completed()),this,SLOT(onCompleted()));

    while(m_testReadIdMap.size() > ((3 * noReads) / 4))
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    QList<tint> cancelled = pFile->cancelAllPending();

    for(QList<tint>::iterator ppI=cancelled.begin();ppI!=cancelled.end() && res;++ppI)
    {
        QMap<tint,tint>::iterator ppJ = m_testReadIdMap.find(*ppI);
        if(ppJ != m_testReadIdMap.end())
        {
            m_testReadIdMap.erase(ppJ);
        }
        else
        {
            res = false;
        }
    }

    QCoreApplication::processEvents(QEventLoop::AllEvents);
    if(res)
    {
        res = m_testReadIdMap.isEmpty();
    }

    disconnect(pFile,SIGNAL(completed()),this,SLOT(onCompleted()));
    return res && m_success;
}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReaderTester::cancelAllPendingForMultiple(AsynchronousFileReader *pFileA,AsynchronousFileReader *pFileB,AsynchronousFileReader *pFileC)
{
    bool res,resA,resB;
    common::TimeStamp t;

    m_testReadIdMapA = m_pReadFunction(pFileA);
    m_testReadIdMapB = m_pReadFunction(pFileB);
    m_testReadIdMapC = m_pReadFunction(pFileC);
    res = (!m_testReadIdMapA.isEmpty() && !m_testReadIdMapB.isEmpty() && !m_testReadIdMapC.isEmpty());

    tint noReadsA = m_testReadIdMapA.size();
    tint noReadsB = m_testReadIdMapB.size();

    m_pFileA = pFileA;
    m_pFileB = pFileB;
    m_pFileC = pFileC;

    connect(pFileA,SIGNAL(completed()),this,SLOT(onCompletedA()));
    connect(pFileB,SIGNAL(completed()),this,SLOT(onCompletedB()));
    connect(pFileC,SIGNAL(completed()),this,SLOT(onCompletedC()));

    while(m_testReadIdMapA.size() > ((3 * noReadsA) / 4) || m_testReadIdMapB.size() > ((3 * noReadsB) / 4))
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
    }

    QList<tint> cancelledA = pFileA->cancelAllPending();
    QList<tint> cancelledB = pFileB->cancelAllPending();

    resA = true;
    for(QList<tint>::iterator ppI=cancelledA.begin();ppI!=cancelledA.end() && resA;++ppI)
    {
        QMap<tint,tint>::iterator ppJ = m_testReadIdMapA.find(*ppI);
        if(ppJ != m_testReadIdMapA.end())
        {
            m_testReadIdMapA.erase(ppJ);
        }
        else
        {
            resA = false;
        }
    }

    resB = true;
    for(QList<tint>::iterator ppI=cancelledB.begin();ppI!=cancelledB.end() && resA;++ppI)
    {
        QMap<tint,tint>::iterator ppJ = m_testReadIdMapB.find(*ppI);
        if(ppJ != m_testReadIdMapB.end())
        {
            m_testReadIdMapB.erase(ppJ);
        }
        else
        {
            resB = false;
        }
    }

    QCoreApplication::processEvents(QEventLoop::AllEvents);
    if(resA)
    {
        resA = m_testReadIdMapA.isEmpty();
    }
    if(resB)
    {
        resB = m_testReadIdMapA.isEmpty();
    }

    while(!m_testReadIdMapC.isEmpty())
    {
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        common::msleepThread(1);
    }

    disconnect(pFileA,SIGNAL(completed()),this,SLOT(onCompletedA()));
    disconnect(pFileB,SIGNAL(completed()),this,SLOT(onCompletedB()));
    disconnect(pFileC,SIGNAL(completed()),this,SLOT(onCompletedC()));

    return res && m_success;
}

//-------------------------------------------------------------------------------------------
// Test
//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::openNonExistantFile()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest_noexist.dat");

    QVERIFY(!DiskOps::exist(fileName));

    AsynchronousFileReader reader;
    QVERIFY(!reader.open(fileName));

    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------
// Test
//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readAllDataFromEmptyFile()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest1.dat");

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    testFileWriter.close();

    AsynchronousFileReader reader;

    QVERIFY(reader.open(fileName));
    QCOMPARE(0,reader.size());
    QCOMPARE(0,static_cast<int>(reader.size64()));

    QCOMPARE(0,reader.read(0,0));
    QCOMPARE(0,reader.read(0,10));

    QCOMPARE(0,reader.read(-1,0));
    QCOMPARE(0,reader.read(-1,10));

    QCOMPARE(0,reader.read(10,0));
    QCOMPARE(0,reader.read(10,20));

    reader.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------
// Test
//-------------------------------------------------------------------------------------------

QMap<tint,tint> readerForReadAllDataFromFileGivenReadRequestAsksForMoreDataThanIsAvailable(AsynchronousFileReader *pFile)
{
    QMap<tint,tint> readIds;

    tint readId = pFile->read(0,2000);
    if(readId > 0)
    {
        readIds.insert(readId,0);
    }
    return readIds;
}

//-------------------------------------------------------------------------------------------

bool completeSlotForReadAllDataFromFileGivenReadRequestAsksForMoreDataThanIsAvailable(tint testId,QSharedPointer<QByteArray>& data)
{
    bool resA,resB,resC;
    resA = (testId==0);
    resB = (data->size() == 1024);
    BIOStreamFunctionalTest writer;
    resC = writer.verifyIsExpectedContents(data,0,1024);
    return resA && resB && resC;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readAllDataFromFileGivenReadRequestAsksForMoreDataThanIsAvailable()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest2.dat");

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,512,512));
    testFileWriter.close();

    AsynchronousFileReader reader;
    QVERIFY(reader.open(fileName));

    AsynchronousFileReaderTester tester(readerForReadAllDataFromFileGivenReadRequestAsksForMoreDataThanIsAvailable,
        completeSlotForReadAllDataFromFileGivenReadRequestAsksForMoreDataThanIsAvailable);
    QVERIFY(tester.run(&reader,1000.0));

    reader.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------
// Test
//-------------------------------------------------------------------------------------------

QMap<tint,tint> readerForReadAllDataFromFileGivenReadRequestAsksForExactlyTheDataThatIsAvailable(AsynchronousFileReader *pFile)
{
    QMap<tint,tint> readIds;

    tint readIdA = pFile->read(0,2000);
    readIds.insert(readIdA,0);

    tint readIdB = pFile->read(1000,1000);
    readIds.insert(readIdB,1);

    tint readIdC = pFile->read(1500,500);
    readIds.insert(readIdC,2);

    if(!(readIdA>0 && readIdB>0 && readIdC>0))
    {
        readIds.clear();
    }
    return readIds;
}

//-------------------------------------------------------------------------------------------

bool completeSlotForReadAllDataFromFileGivenReadRequestAsksForExactlyTheDataThatIsAvailable(tint testId,QSharedPointer<QByteArray>& data)
{
    bool resA = false,resB = false;
    BIOStreamFunctionalTest writer;

    if(testId==0)
    {
        resA = (2000 == data->size());
        resB = writer.verifyIsExpectedContents(data,0,2000);
    }
    else if(testId==1)
    {
        resA = (1000 == data->size());
        resB = writer.verifyIsExpectedContents(data,1000,1000);
    }
    else if(testId==2)
    {
        resA = (500 == data->size());
        resB = writer.verifyIsExpectedContents(data,1500,500);
    }
    return (resA && resB);
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readAllDataFromFileGivenReadRequestAsksForExactlyTheDataThatIsAvailable()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest3.dat");

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,1000,1000));
    testFileWriter.close();

    AsynchronousFileReader reader;
    QVERIFY(reader.open(fileName));

    AsynchronousFileReaderTester tester(readerForReadAllDataFromFileGivenReadRequestAsksForExactlyTheDataThatIsAvailable,
        completeSlotForReadAllDataFromFileGivenReadRequestAsksForExactlyTheDataThatIsAvailable);
    QVERIFY(tester.run(&reader,1000.0));

    reader.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------
// Test
//-------------------------------------------------------------------------------------------

QMap<tint,tint> readerForReadAllDataFromFileGivenAsASeriesOfReadRequests(AsynchronousFileReader *pFile)
{
    bool res = true;
    tint pos = 0,testId = 0;
    QMap<tint,tint> readIds;

    for(tint i=4096;i>0 && res;i-=2,testId++)
    {
        tint readId = pFile->read(pos,i);
        res = (readId > 0);
        readIds.insert(readId,testId);
        pos += i;
    }
    for(tint i=2;i<=4096 && res;i+=2,testId++)
    {
        tint readId = pFile->read(pos,i);
        res = (readId > 0);
        readIds.insert(readId,testId);
        pos += i;
    }
    if(!res)
    {
        readIds.clear();
    }
    return readIds;
}

//-------------------------------------------------------------------------------------------

bool completeSlotForReadAllDataFromFileGivenAsASeriesOfReadRequests(tint testId,QSharedPointer<QByteArray>& data)
{
    bool resA,resB,resC;
    tint pos = 0,len = -1,currentId = 0;
    BIOStreamFunctionalTest writer;

    for(tint i=4096;i>0 && len<0;i-=2,currentId++)
    {
        if(testId==currentId)
        {
            len = i;
        }
        else
        {
            pos += i;
        }
    }
    if(len<0)
    {
        for(tint i=2;i<=4096 && len<0;i+=2,currentId++)
        {
            if(testId==currentId)
            {
                len = i;
            }
            else
            {
                pos += i;
            }
        }
    }

    resA = (len > 0);
    resB = (len == data->size());
    resC = writer.verifyIsExpectedContents(data,pos,len);

    return resA && resB && resC;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readAllDataFromFileGivenAsASeriesOfReadRequests()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest4.dat");

    tint noElements = 0;
    for(tint i=4096;i>0;i-=2)
    {
        noElements += i >> 1;
    }
    for(tint i=2;i<=4096;i+=2)
    {
        noElements += i >> 1;
    }

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,noElements,noElements));
    testFileWriter.close();

    AsynchronousFileReader reader;
    QVERIFY(reader.open(fileName));

    AsynchronousFileReaderTester tester(readerForReadAllDataFromFileGivenAsASeriesOfReadRequests,
        completeSlotForReadAllDataFromFileGivenAsASeriesOfReadRequests);
    QVERIFY(tester.run(&reader,1000.0));

    reader.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readAllDataFromMultipleFilesGivenAsASeriesOfReadRequests()
{
    QVector<QString> fileNames;
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest7A.dat"));
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest7B.dat"));
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest7C.dat"));

    tint noElements = 0;
    for(tint i=4096;i>0;i-=2)
    {
        noElements += i >> 1;
    }
    for(tint i=2;i<=4096;i+=2)
    {
        noElements += i >> 1;
    }

    for(tint idx=0;idx<fileNames.size();idx++)
    {
        const QString& fileName = fileNames.at(idx);
        BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
        QVERIFY(testFileWriter.open(fileName));
        BIOStreamFunctionalTest writer;
        QVERIFY(writer.writeUShortTestFile(&testFileWriter,noElements,noElements));
        testFileWriter.close();
    }

    AsynchronousFileReader readerA;
    QVERIFY(readerA.open(fileNames.at(0)));
    AsynchronousFileReader readerB;
    QVERIFY(readerB.open(fileNames.at(1)));
    AsynchronousFileReader readerC;
    QVERIFY(readerC.open(fileNames.at(2)));

    AsynchronousFileReaderTester tester(readerForReadAllDataFromFileGivenAsASeriesOfReadRequests,
        completeSlotForReadAllDataFromFileGivenAsASeriesOfReadRequests);
    QVERIFY(tester.runMultiple(&readerA,&readerB,&readerC,1000.0));

    readerA.close();
    readerB.close();
    readerC.close();

    DiskOps::remove(fileNames.at(0));
    QVERIFY(!DiskOps::exist(fileNames.at(0)));
    DiskOps::remove(fileNames.at(1));
    QVERIFY(!DiskOps::exist(fileNames.at(1)));
    DiskOps::remove(fileNames.at(2));
    QVERIFY(!DiskOps::exist(fileNames.at(2)));
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readAllDataFromMultipleReaderButSameFileGivenAsASeriesOfReadRequests()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest8.dat");

    tint noElements = 0;
    for(tint i=4096;i>0;i-=2)
    {
        noElements += i >> 1;
    }
    for(tint i=2;i<=4096;i+=2)
    {
        noElements += i >> 1;
    }

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,noElements,noElements));
    testFileWriter.close();

    AsynchronousFileReader readerA;
    QVERIFY(readerA.open(fileName));
    AsynchronousFileReader readerB;
    QVERIFY(readerB.open(fileName));
    AsynchronousFileReader readerC;
    QVERIFY(readerC.open(fileName));

    AsynchronousFileReaderTester tester(readerForReadAllDataFromFileGivenAsASeriesOfReadRequests,
        completeSlotForReadAllDataFromFileGivenAsASeriesOfReadRequests);
    QVERIFY(tester.runMultiple(&readerA,&readerB,&readerC,1000.0));

    readerA.close();
    readerB.close();
    readerC.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------
// Test
//-------------------------------------------------------------------------------------------

QMap<tint,tint> readerForReadDataFromFileWithEachReadRequestAtARandomLocation(AsynchronousFileReader *pFile)
{
    bool res = true;
    Random *random = Random::instance();
    QMap<tint,tint> readIds;

    for(tint testId=0;testId<100 && res;testId++)
    {
        random->seed(testId);
        tint fromElement = static_cast<tint>(random->randomReal1() * 4000000.0);
        tint noElements = static_cast<tint>(random->randomReal1() * 4000000.0);

        if((fromElement + noElements) > 4000000)
        {
            noElements = 4000000 - fromElement;
        }

        if(noElements > 0)
        {
            tint readId = pFile->read(fromElement << 1,noElements << 1);
            res = (readId > 0);
            readIds.insert(readId,testId);
        }
    }
    if(!res)
    {
        readIds.clear();
    }
    return readIds;
}

//-------------------------------------------------------------------------------------------

bool completeSlotForReadDataFromFileWithEachReadRequestAtARandomLocation(tint testId,QSharedPointer<QByteArray>& data)
{
    bool resA,resB;
    Random *random = Random::instance();
    random->seed(testId);
    tint pos = static_cast<tint>(random->randomReal1() * 4000000.0);
    tint len = static_cast<tint>(random->randomReal1() * 4000000.0);
    if((pos + len) > 4000000)
    {
        len = 4000000 - pos;
    }
    pos <<= 1;
    len <<= 1;

    resA = (len == data->size());
    BIOStreamFunctionalTest writer;
    resB = writer.verifyIsExpectedContents(data,pos,len);

    return resA && resB;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readDataFromFileWithEachReadRequestAtARandomLocation()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest5.dat");

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,4000000,4000000));
    testFileWriter.close();

    AsynchronousFileReader reader;
    QVERIFY(reader.open(fileName));

    AsynchronousFileReaderTester tester(readerForReadDataFromFileWithEachReadRequestAtARandomLocation,
        completeSlotForReadDataFromFileWithEachReadRequestAtARandomLocation);
    QVERIFY(tester.run(&reader,100.0));

    reader.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readDataFromMultipleFilesWithEachReadRequestAtARandomLocation()
{
    QVector<QString> fileNames;
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest6A.dat"));
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest6B.dat"));
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest6C.dat"));

    for(tint idx=0;idx<fileNames.size();idx++)
    {
        const QString& fileName = fileNames.at(idx);
        BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
        QVERIFY(testFileWriter.open(fileName));
        BIOStreamFunctionalTest writer;
        QVERIFY(writer.writeUShortTestFile(&testFileWriter,4000000,4000000));
        testFileWriter.close();
    }

    AsynchronousFileReader readerA;
    QVERIFY(readerA.open(fileNames.at(0)));
    AsynchronousFileReader readerB;
    QVERIFY(readerB.open(fileNames.at(1)));
    AsynchronousFileReader readerC;
    QVERIFY(readerC.open(fileNames.at(2)));


    AsynchronousFileReaderTester tester(readerForReadDataFromFileWithEachReadRequestAtARandomLocation,
        completeSlotForReadDataFromFileWithEachReadRequestAtARandomLocation);
    QVERIFY(tester.runMultiple(&readerA,&readerB,&readerC,100.0));

    readerA.close();
    readerB.close();
    readerC.close();

    DiskOps::remove(fileNames.at(0));
    QVERIFY(!DiskOps::exist(fileNames.at(0)));
    DiskOps::remove(fileNames.at(1));
    QVERIFY(!DiskOps::exist(fileNames.at(1)));
    DiskOps::remove(fileNames.at(2));
    QVERIFY(!DiskOps::exist(fileNames.at(2)));
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::readDataFromMultipleReadersButSameFileWithEachReadRequestAtARandomLocation()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest9.dat");

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,4000000,4000000));
    testFileWriter.close();

    AsynchronousFileReader readerA;
    QVERIFY(readerA.open(fileName));
    AsynchronousFileReader readerB;
    QVERIFY(readerB.open(fileName));
    AsynchronousFileReader readerC;
    QVERIFY(readerC.open(fileName));


    AsynchronousFileReaderTester tester(readerForReadDataFromFileWithEachReadRequestAtARandomLocation,
        completeSlotForReadDataFromFileWithEachReadRequestAtARandomLocation);
    QVERIFY(tester.runMultiple(&readerA,&readerB,&readerC,100.0));

    readerA.close();
    readerB.close();
    readerC.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

QMap<tint,tint> readerForCancelAllPendingForContinousReads(AsynchronousFileReader *pFile)
{
    QMap<tint,tint> readIds;

    for(tint i=0;i<10000;i++)
    {
        tint readId = pFile->read(i * 10000,100);
        if(readId > 0)
        {
            readIds.insert(readId,i);
        }
    }
    return readIds;
}

//-------------------------------------------------------------------------------------------

bool completeSlotForCancelAllPendingForContinousReads(tint testId,QSharedPointer<QByteArray>& data)
{
    BIOStreamFunctionalTest writer;
    bool res = writer.verifyIsExpectedContents(data,testId * 10000,100);
    return res;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::cancelAllPendingForContinousReads()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest10.dat");

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,1000000,1000000));
    testFileWriter.close();

    AsynchronousFileReader reader;
    QVERIFY(reader.open(fileName));

    AsynchronousFileReaderTester tester(readerForCancelAllPendingForContinousReads,
        completeSlotForCancelAllPendingForContinousReads);
    QVERIFY(tester.cancelAllPending(&reader));

    reader.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::cancelAllPendingForContinousReadsOnMultipleFiles()
{
    QVector<QString> fileNames;
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest11A.dat"));
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest11B.dat"));
    fileNames.append(common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest11C.dat"));

    for(tint idx=0;idx<fileNames.size();idx++)
    {
        const QString& fileName = fileNames.at(idx);
        BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
        QVERIFY(testFileWriter.open(fileName));
        BIOStreamFunctionalTest writer;
        QVERIFY(writer.writeUShortTestFile(&testFileWriter,1000000,1000000));
        testFileWriter.close();
    }

    AsynchronousFileReader readerA;
    QVERIFY(readerA.open(fileNames.at(0)));
    AsynchronousFileReader readerB;
    QVERIFY(readerB.open(fileNames.at(1)));
    AsynchronousFileReader readerC;
    QVERIFY(readerC.open(fileNames.at(2)));

    AsynchronousFileReaderTester tester(readerForCancelAllPendingForContinousReads,
        completeSlotForCancelAllPendingForContinousReads);
    QVERIFY(tester.cancelAllPendingForMultiple(&readerA,&readerB,&readerC));

    readerA.close();
    readerB.close();
    readerC.close();

    DiskOps::remove(fileNames.at(0));
    QVERIFY(!DiskOps::exist(fileNames.at(0)));
    DiskOps::remove(fileNames.at(1));
    QVERIFY(!DiskOps::exist(fileNames.at(1)));
    DiskOps::remove(fileNames.at(2));
    QVERIFY(!DiskOps::exist(fileNames.at(2)));
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitTest::cancelAllPendingForContinousReadsOnSameFileWithMultipleReaders()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "asyncfileiotest12.dat");

    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,1000000,1000000));
    testFileWriter.close();

    AsynchronousFileReader readerA;
    QVERIFY(readerA.open(fileName));
    AsynchronousFileReader readerB;
    QVERIFY(readerB.open(fileName));
    AsynchronousFileReader readerC;
    QVERIFY(readerC.open(fileName));

    AsynchronousFileReaderTester tester(readerForCancelAllPendingForContinousReads,
        completeSlotForCancelAllPendingForContinousReads);
    QVERIFY(tester.cancelAllPendingForMultiple(&readerA,&readerB,&readerC));

    readerA.close();
    readerB.close();
    readerC.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------
// Runner
//-------------------------------------------------------------------------------------------

AsynchronousFileReaderQtUnitApplication::AsynchronousFileReaderQtUnitApplication(int argc,char **argv) : QCoreApplication(argc,argv),
    m_succeeded(false)
{
    QTimer::singleShot(10,this,SLOT(runTests()));
}

//-------------------------------------------------------------------------------------------

AsynchronousFileReaderQtUnitApplication::~AsynchronousFileReaderQtUnitApplication()
{}

//-------------------------------------------------------------------------------------------

bool AsynchronousFileReaderQtUnitApplication::testSucceeded() const
{
    return m_succeeded;
}

//-------------------------------------------------------------------------------------------

void AsynchronousFileReaderQtUnitApplication::runTests()
{
    AsynchronousFileReaderQtUnitTest tests;
    m_succeeded = (QTest::qExec(&tests)==0) ? true : false;
    quit();
}

//-------------------------------------------------------------------------------------------

TEST(AsynchronousFileReader,RunQtUnitTests)
{
    QStringList libPaths = QApplication::libraryPaths();
    {
        char *argv = 0;
        AsynchronousFileReaderQtUnitApplication tester(0,&argv);
        tester.exec();
        ASSERT_TRUE(tester.testSucceeded());
    }
    QApplication::setLibraryPaths(libPaths);
}

//-------------------------------------------------------------------------------------------
