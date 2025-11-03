//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_COMMON_TEST_ASYNCHRONOUSFILEREADERTEST_H
#define __OMEGA_COMMON_TEST_ASYNCHRONOUSFILEREADERTEST_H
//-------------------------------------------------------------------------------------------

#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <QCoreApplication>
#include <QtTest/QtTest>

#include "common/inc/AsynchronousFileReader.h"
#include "common/test/BIOStreamFunctionalTest.h"

using namespace omega;
using namespace common;
using namespace testing;

//-------------------------------------------------------------------------------------------

class AsynchronousFileReaderQtUnitTest : public QObject
{
    public:
        Q_OBJECT

    private Q_SLOTS:
        void openNonExistantFile();

        void readAllDataFromEmptyFile();
        void readAllDataFromFileGivenReadRequestAsksForMoreDataThanIsAvailable();
        void readAllDataFromFileGivenReadRequestAsksForExactlyTheDataThatIsAvailable();
        void readAllDataFromFileGivenAsASeriesOfReadRequests();
        void readAllDataFromMultipleFilesGivenAsASeriesOfReadRequests();
        void readAllDataFromMultipleReaderButSameFileGivenAsASeriesOfReadRequests();
        void readDataFromFileWithEachReadRequestAtARandomLocation();
        void readDataFromMultipleFilesWithEachReadRequestAtARandomLocation();
        void readDataFromMultipleReadersButSameFileWithEachReadRequestAtARandomLocation();

        void cancelAllPendingForContinousReads();
        void cancelAllPendingForContinousReadsOnMultipleFiles();
        void cancelAllPendingForContinousReadsOnSameFileWithMultipleReaders();
};

//-------------------------------------------------------------------------------------------

class AsynchronousFileReaderQtUnitApplication : public QCoreApplication
{
    public:
        Q_OBJECT

    public:
        AsynchronousFileReaderQtUnitApplication(int argc,char **argv);
        virtual ~AsynchronousFileReaderQtUnitApplication();
        bool testSucceeded() const;

    private:
        bool m_succeeded;

    private Q_SLOTS:
        void runTests();
};

//-------------------------------------------------------------------------------------------

class AsynchronousFileReaderTester : public QObject
{
    Q_OBJECT

    public:
        typedef QMap<tint,tint> (*ReadTestFunction)(AsynchronousFileReader *);
        typedef bool (*CompleteTestFunction)(tint,QSharedPointer<QByteArray>&);

    public:
        AsynchronousFileReaderTester(ReadTestFunction pReadFunction,CompleteTestFunction pCompleteFunction);

        bool run(AsynchronousFileReader *pFile,tfloat64 timeout);
        bool runMultiple(AsynchronousFileReader *pFileA,AsynchronousFileReader *pFileB,AsynchronousFileReader *pFileC,tfloat64 timeout);
        bool cancelAllPending(AsynchronousFileReader *pFile);
        bool cancelAllPendingForMultiple(AsynchronousFileReader *pFileA,AsynchronousFileReader *pFileB,AsynchronousFileReader *pFileC);

    protected:
        ReadTestFunction m_pReadFunction;
        CompleteTestFunction m_pCompleteFunction;
        QMap<tint,tint> m_testReadIdMap;

        QMap<tint,tint> m_testReadIdMapA;
        QMap<tint,tint> m_testReadIdMapB;
        QMap<tint,tint> m_testReadIdMapC;

        AsynchronousFileReader *m_pFile;
        AsynchronousFileReader *m_pFileA;
        AsynchronousFileReader *m_pFileB;
        AsynchronousFileReader *m_pFileC;

        bool m_success;

        void onComplete(int readId,QSharedPointer<QByteArray>& data);
        void onCompleteA(int readId,QSharedPointer<QByteArray>& data);
        void onCompleteB(int readId,QSharedPointer<QByteArray>& data);
        void onCompleteC(int readId,QSharedPointer<QByteArray>& data);

        void onError(int readId);
        void onErrorA(int readId);
        void onErrorB(int readId);
        void onErrorC(int readId);

    protected Q_SLOTS:
        void onCompleted();
        void onCompletedA();
        void onCompletedB();
        void onCompletedC();
};

//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
