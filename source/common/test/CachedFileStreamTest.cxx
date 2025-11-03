#include "common/test/CachedFileStreamTest.h"
#include "common/inc/Random.h"
#include "common/test/AsynchronousFileReaderMock.h"

#include <QApplication>

//-------------------------------------------------------------------------------------------

class CachedFileStreamIsOffsetIntersectionTest : public CachedFileStream
{
    public:
        bool testIsOffsetIntersection(tint offsetA,tint lengthA,tint offsetB,tint lengthB) const;
};

//-------------------------------------------------------------------------------------------

bool CachedFileStreamIsOffsetIntersectionTest::testIsOffsetIntersection(tint offsetA,tint lengthA,tint offsetB,tint lengthB) const
{
    return isOffsetIntersection(offsetA,lengthA,offsetB,lengthB);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isOffsetIntersectionGivenASectionBeforeBSection)
{
    CachedFileStreamIsOffsetIntersectionTest file;
    EXPECT_FALSE(file.testIsOffsetIntersection(100,50,200,100));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,3,13,2));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,3,13,1));
    EXPECT_FALSE(file.testIsOffsetIntersection(12,1,13,2));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isOffsetIntersectionGivenASectionEndInsideBSection)
{
    CachedFileStreamIsOffsetIntersectionTest file;
    EXPECT_TRUE(file.testIsOffsetIntersection(100,150,200,100));
    EXPECT_TRUE(file.testIsOffsetIntersection(10,4,13,2));
    EXPECT_TRUE(file.testIsOffsetIntersection(10,4,13,1));
    EXPECT_TRUE(file.testIsOffsetIntersection(10,4,13,1));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isOffsetIntersectionGivenASectionStartInsideBSection)
{
    CachedFileStreamIsOffsetIntersectionTest file;
    EXPECT_TRUE(file.testIsOffsetIntersection(280,50,200,100));
    EXPECT_TRUE(file.testIsOffsetIntersection(10,3,12,2));
    EXPECT_TRUE(file.testIsOffsetIntersection(10,3,12,1));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isOffsetIntersectionGivenASectionAfterBSection)
{
    CachedFileStreamIsOffsetIntersectionTest file;
    EXPECT_FALSE(file.testIsOffsetIntersection(320,50,200,100));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,3,13,2));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,3,13,1));
    EXPECT_FALSE(file.testIsOffsetIntersection(13,2,10,3));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isOffsetIntersectionGivenASectionEnclosesBSection)
{
    CachedFileStreamIsOffsetIntersectionTest file;
    EXPECT_TRUE(file.testIsOffsetIntersection(100,300,200,100));
    EXPECT_TRUE(file.testIsOffsetIntersection(10,4,11,3));
    EXPECT_TRUE(file.testIsOffsetIntersection(10,4,10,3));
    EXPECT_TRUE(file.testIsOffsetIntersection(13,1,13,1));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isOffsetIntersectionGivenBSectionEnclosesASection)
{
    CachedFileStreamIsOffsetIntersectionTest file;
    EXPECT_TRUE(file.testIsOffsetIntersection(200,100,100,300));
    EXPECT_TRUE(file.testIsOffsetIntersection(11,3,10,4));
    EXPECT_TRUE(file.testIsOffsetIntersection(10,3,10,4));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isOffsetIntersectionGivenAZeroLength)
{
    CachedFileStreamIsOffsetIntersectionTest file;
    EXPECT_FALSE(file.testIsOffsetIntersection(9,0,10,5));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,0,10,5));
    EXPECT_FALSE(file.testIsOffsetIntersection(13,0,10,5));
    EXPECT_FALSE(file.testIsOffsetIntersection(14,0,10,5));
    EXPECT_FALSE(file.testIsOffsetIntersection(16,0,10,5));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isOffsetIntersectionGivenBZeroLength)
{
    CachedFileStreamIsOffsetIntersectionTest file;
    EXPECT_FALSE(file.testIsOffsetIntersection(10,5,9,0));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,5,10,0));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,5,13,0));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,5,14,0));
    EXPECT_FALSE(file.testIsOffsetIntersection(10,5,16,0));
}

//-------------------------------------------------------------------------------------------

class CachedFileStreamIsCachedTest : public CachedFileStream
{
    public:
        MOCK_CONST_METHOD0(getFileConst,const AsynchronousFileReader *());
        MOCK_CONST_METHOD0(getCacheConst,const QMap<tint64,QSharedPointer<QByteArray> >&());
        MOCK_CONST_METHOD0(getCacheMaskConst,const QMap<tint64,QMap<tint,tint> >&());
};

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenFileHasNotBeenOpened)
{
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(reinterpret_cast<AsynchronousFileReader *>(0)));
    
    ASSERT_FALSE(file.isCached(0,10));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenZeroLength)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
        
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + c_blockPartSize;
    tint length = 0;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    
    ASSERT_TRUE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenNoBlockCached)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA-1,dataA);
    cacheMap.insert(blockIdxA+1,dataA);
    
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 6);
    tint length = c_blockPartSize;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    
    ASSERT_FALSE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenBlockCachedAtIndexButOffsetOutsideOfBlock)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize / 2,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 6);
    tint length = c_blockPartSize;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    
    ASSERT_FALSE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenBlockCachedAtIndexButEndPositionOutsideOfBlock)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize / 2,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize);
    tint length = c_blockPartSize * 6;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    
    ASSERT_FALSE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenBlockCachedNoMaskAndEndPositionOnBlockBoundary)
{
    tint64 blockIdxA = 5;
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
        
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize);
    tint length = CachedFileStream::c_blockSize;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    ASSERT_TRUE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenBlockCachedNoMaskAndEndPositionBeforeBlockBoundary)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
        
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 6);
    tint length = c_blockPartSize * 2;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    ASSERT_TRUE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenBlockCachedWithIntersectingMask)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    
    // [1,3), [4,5), [7,9)
    QMap<tint,tint> cacheMaskA;
    cacheMaskA.insert(c_blockPartSize * 1,c_blockPartSize * 2);
    cacheMaskA.insert(c_blockPartSize * 4,c_blockPartSize * 1);
    cacheMaskA.insert(c_blockPartSize * 7,c_blockPartSize * 2);
    
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
    cacheMaskMap.insert(blockIdxA,cacheMaskA);
        
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 6);
    tint length = c_blockPartSize * 2;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    ASSERT_FALSE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenBlockCachedWithMaskNotIntersecting)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;

    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    
    // [1,3), [4,5), [8,9)
    QMap<tint,tint> cacheMaskA;
    cacheMaskA.insert(c_blockPartSize * 1,c_blockPartSize * 2);
    cacheMaskA.insert(c_blockPartSize * 4,c_blockPartSize * 1);
    cacheMaskA.insert(c_blockPartSize * 8,c_blockPartSize * 1);
    
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
    cacheMaskMap.insert(blockIdxA,cacheMaskA);
        
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 6);
    tint length = c_blockPartSize;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    ASSERT_TRUE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenRangeOver3BlocksAndNoMasks)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    tint64 blockIdxB = blockIdxA + 1;
    tint64 blockIdxC = blockIdxA + 2;
    
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QSharedPointer<QByteArray> dataB(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QSharedPointer<QByteArray> dataC(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    cacheMap.insert(blockIdxB,dataB);
    cacheMap.insert(blockIdxC,dataC);
    
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
    
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 6);
    tint length = (CachedFileStream::c_blockSize - (offset % CachedFileStream::c_blockSize)) + CachedFileStream::c_blockSize + (c_blockPartSize * 5);
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    ASSERT_TRUE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenRangeOver3BlocksAndIntersectingMaskOnLastBlock)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    tint64 blockIdxB = blockIdxA + 1;
    tint64 blockIdxC = blockIdxA + 2;
    
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QSharedPointer<QByteArray> dataB(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QSharedPointer<QByteArray> dataC(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    cacheMap.insert(blockIdxB,dataB);
    cacheMap.insert(blockIdxC,dataC);
    
    // [0,1), [2,5)
    QMap<tint,tint> cacheMaskA;
    cacheMaskA.insert(0,c_blockPartSize);
    cacheMaskA.insert(c_blockPartSize * 2,c_blockPartSize * 3);
    // [4,7)
    QMap<tint,tint> cacheMaskB;
    cacheMaskB.insert(c_blockPartSize * 4,c_blockPartSize * 3);
    
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
    cacheMaskMap.insert(blockIdxA,cacheMaskA);
    cacheMaskMap.insert(blockIdxC,cacheMaskB);
        
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 6);
    tint length = (CachedFileStream::c_blockSize - (offset % CachedFileStream::c_blockSize)) + CachedFileStream::c_blockSize + (c_blockPartSize * 5);
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    ASSERT_FALSE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isCachedGivenRangeOver3BlocksAndMaskNotIntersectingOnLastBlock)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    tint64 blockIdxB = blockIdxA + 1;
    tint64 blockIdxC = blockIdxA + 2;
    
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QSharedPointer<QByteArray> dataB(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QSharedPointer<QByteArray> dataC(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    cacheMap.insert(blockIdxB,dataB);
    cacheMap.insert(blockIdxC,dataC);
    
    // [0,1), [2,5)
    QMap<tint,tint> cacheMaskA;
    cacheMaskA.insert(0,c_blockPartSize);
    cacheMaskA.insert(c_blockPartSize * 2,c_blockPartSize * 3);
    // [6,7), [8,9)
    QMap<tint,tint> cacheMaskB;
    cacheMaskB.insert(c_blockPartSize * 6,c_blockPartSize);
    cacheMaskB.insert(c_blockPartSize * 8,c_blockPartSize);
    
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
    cacheMaskMap.insert(blockIdxA,cacheMaskA);
    cacheMaskMap.insert(blockIdxC,cacheMaskB);
        
    tint64 offset = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 6);
    tint length = (CachedFileStream::c_blockSize - (offset % CachedFileStream::c_blockSize)) + CachedFileStream::c_blockSize + (c_blockPartSize * 4);
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(CachedFileStream::c_blockSize * 10));
    
    CachedFileStreamIsCachedTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    ASSERT_TRUE(file.isCached(offset,length));
}

//-------------------------------------------------------------------------------------------

class CachedFileStreamBuildReadRequestsTest : public CachedFileStream
{
    public:
        MOCK_CONST_METHOD0(getFileConst,const AsynchronousFileReader *());
        MOCK_CONST_METHOD0(getCacheConst,const QMap<tint64,QSharedPointer<QByteArray> >&());
        MOCK_CONST_METHOD0(getCacheMaskConst,const QMap<tint64,QMap<tint,tint> >&());
        
        bool testBuildReadRequests(tint64 fromPosition,tint noBytes,QVector<QPair<tint64,tint> >& requests) const;
};

//-------------------------------------------------------------------------------------------

bool CachedFileStreamBuildReadRequestsTest::testBuildReadRequests(tint64 fromPosition,tint noBytes,QVector<QPair<tint64,tint> >& requests) const
{
    return buildReadRequests(fromPosition,noBytes,requests);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenNoFileOpen)
{
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(reinterpret_cast<AsynchronousFileReader *>(0)));
        
    QVector<QPair<tint64,tint> > requests;    
    ASSERT_FALSE(file.testBuildReadRequests(0,20,requests));
    ASSERT_EQ(0,requests.size());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenPositionIsOutOfRangeOfFile)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 fileLength = (CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6);

    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return(fileLength));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
        
    QVector<QPair<tint64,tint> > requests;
    
    ASSERT_FALSE(file.testBuildReadRequests(-1,20,requests));
    ASSERT_EQ(0,requests.size());

    ASSERT_FALSE(file.testBuildReadRequests(0,static_cast<tint>(fileLength) + 1,requests));
    ASSERT_EQ(0,requests.size());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenZeroNumberOfBytesRequested)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;

    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return((CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6)));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
        
    tint64 fromPosition = blockIdxA * CachedFileStream::c_blockSize;
    tint length = 0;
    
    QVector<QPair<tint64,tint> > requests;
    ASSERT_TRUE(file.testBuildReadRequests(fromPosition,length,requests));
    
    ASSERT_EQ(0,requests.size());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenReadForExactlyOneBlockThatIsUncached)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;

    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return((CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6)));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    
    tint64 fromPosition = blockIdxA * CachedFileStream::c_blockSize;
    tint length = CachedFileStream::c_blockSize;
    
    QVector<QPair<tint64,tint> > requests;
    ASSERT_TRUE(file.testBuildReadRequests(fromPosition,length,requests));
    
    ASSERT_EQ(1,requests.size());
    EXPECT_EQ(blockIdxA * CachedFileStream::c_blockSize,requests.at(0).first);
    EXPECT_EQ(c_blockSize,requests.at(0).second);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenReadForPositionInsideUncachedBlock)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;

    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return((CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6)));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    
    tint64 fromPosition = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 4);
    tint length = c_blockPartSize * 5;
    
    QVector<QPair<tint64,tint> > requests;
    ASSERT_TRUE(file.testBuildReadRequests(fromPosition,length,requests));
    
    ASSERT_EQ(1,requests.size());
    EXPECT_EQ(blockIdxA * CachedFileStream::c_blockSize,requests.at(0).first);
    EXPECT_EQ(c_blockSize,requests.at(0).second);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenReadForLastBlockInFileThatIsUncached)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;

    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return((CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6)));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    
    tint64 fromPosition = (CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 2);
    tint length = c_blockPartSize;
    
    QVector<QPair<tint64,tint> > requests;
    ASSERT_TRUE(file.testBuildReadRequests(fromPosition,length,requests));
    
    ASSERT_EQ(1,requests.size());
    EXPECT_EQ(10 * CachedFileStream::c_blockSize,requests.at(0).first);
    EXPECT_EQ(c_blockPartSize * 6,requests.at(0).second);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenReadForPositionInsideBlockThatIsAlreadyCachedAndNoMaskEntry)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;

    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);
    
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;

    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return((CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6)));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    tint64 fromPosition = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 4);
    tint length = c_blockPartSize * 5;
    
    QVector<QPair<tint64,tint> > requests;
    ASSERT_TRUE(file.testBuildReadRequests(fromPosition,length,requests));
    
    ASSERT_EQ(0,requests.size());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenReadForPositionInsideBlockThatIsAlreadyCachedAndEmptyMaskMap)
{
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;

    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);

    QMap<tint,tint> cacheMaskB;
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
    cacheMaskMap.insert(blockIdxA,cacheMaskB);

    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return((CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6)));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    tint64 fromPosition = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 4);
    tint length = c_blockPartSize * 5;
    
    QVector<QPair<tint64,tint> > requests;
    ASSERT_TRUE(file.testBuildReadRequests(fromPosition,length,requests));
    
    ASSERT_EQ(0,requests.size());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenReadForPositionInsideBlockThatIsAlreadyCachedAndNonEmptyMaskMap)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;

    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxA,dataA);

    QMap<tint,tint> cacheMaskB;
    cacheMaskB.insert(10,30);
    QMap<tint64,QMap<tint,tint> > cacheMaskMap;
    cacheMaskMap.insert(blockIdxA,cacheMaskB);

    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return((CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6)));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    tint64 fromPosition = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 4);
    tint length = c_blockPartSize * 5;
    
    QVector<QPair<tint64,tint> > requests;
    ASSERT_TRUE(file.testBuildReadRequests(fromPosition,length,requests));
    
    ASSERT_EQ(1,requests.size());
    EXPECT_EQ(blockIdxA * CachedFileStream::c_blockSize,requests.at(0).first);
    EXPECT_EQ(c_blockSize,requests.at(0).second);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,buildReadRequestsGivenReadSpanning3BlocksWithMiddleOneAlreadyCached)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;
    const tint c_blockPartSize = CachedFileStream::c_blockSize / 10;
    
    tint64 blockIdxA = 5;
    tint64 blockIdxB = blockIdxA + 1;
    tint64 blockIdxC = blockIdxA + 2;

    QSharedPointer<QByteArray> dataB(new QByteArray(CachedFileStream::c_blockSize,'\0'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cacheMap;
    cacheMap.insert(blockIdxB,dataB);

    QMap<tint64,QMap<tint,tint> > cacheMaskMap;

    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,size64()).WillRepeatedly(Return((CachedFileStream::c_blockSize * 10) + (c_blockPartSize * 6)));
    
    CachedFileStreamBuildReadRequestsTest file;
    EXPECT_CALL(file,getFileConst()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCacheConst()).WillRepeatedly(ReturnRef(cacheMap));
    EXPECT_CALL(file,getCacheMaskConst()).WillRepeatedly(ReturnRef(cacheMaskMap));
    
    tint64 fromPosition = (blockIdxA * CachedFileStream::c_blockSize) + (c_blockPartSize * 9);
    tint length = (CachedFileStream::c_blockSize - (c_blockPartSize * 9)) + CachedFileStream::c_blockSize + 1;
    
    QVector<QPair<tint64,tint> > requests;
    ASSERT_TRUE(file.testBuildReadRequests(fromPosition,length,requests));
    
    ASSERT_EQ(2,requests.size());
    EXPECT_EQ(blockIdxA * CachedFileStream::c_blockSize,requests.at(0).first);
    EXPECT_EQ(c_blockSize,requests.at(0).second);
    EXPECT_EQ(blockIdxC * CachedFileStream::c_blockSize,requests.at(1).first);
    EXPECT_EQ(c_blockSize,requests.at(1).second);
}

//-------------------------------------------------------------------------------------------

class CachedFileStreamRemovePendingReadsTest : public CachedFileStream
{
    public:
        MOCK_CONST_METHOD0(readOpsConst,const QMap<tint64,QPair<tint,tint> >&());
        void testRemovePendingReads(QVector<QPair<tint64,tint> >& requests) const;
};

//-------------------------------------------------------------------------------------------

void CachedFileStreamRemovePendingReadsTest::testRemovePendingReads(QVector<QPair<tint64,tint> >& requests) const
{
    removePendingReads(requests);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,removePendingReadsGivenNonePending)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;

    QMap<tint64,QPair<tint,tint> > readOpsMap;
    
    QVector<QPair<tint64,tint> > requests;
    requests.append(QPair<tint64,tint>(1 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(3 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(4 * c_blockSize,c_blockSize));
    
    CachedFileStreamRemovePendingReadsTest file;
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOpsMap));
    
    file.testRemovePendingReads(requests);
    
    ASSERT_EQ(3,requests.size());
    EXPECT_EQ(1 * c_blockSize,requests.at(0).first); // 0 - 1
    EXPECT_EQ(3 * c_blockSize,requests.at(1).first); // 1 - 3
    EXPECT_EQ(4 * c_blockSize,requests.at(2).first); // 2 - 4
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,removePendingReadsGivenNoneListedArePending)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;

    QMap<tint64,QPair<tint,tint> > readOpsMap;
    readOpsMap.insert(2,QPair<tint,tint>(1,0));
    readOpsMap.insert(5,QPair<tint,tint>(2,0));
    readOpsMap.insert(6,QPair<tint,tint>(3,0));
    
    QVector<QPair<tint64,tint> > requests;
    requests.append(QPair<tint64,tint>(1 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(3 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(4 * c_blockSize,c_blockSize));
    
    CachedFileStreamRemovePendingReadsTest file;
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOpsMap));
    
    file.testRemovePendingReads(requests);
    
    ASSERT_EQ(3,requests.size());
    EXPECT_EQ(1 * c_blockSize,requests.at(0).first); // 0 - 1
    EXPECT_EQ(3 * c_blockSize,requests.at(1).first); // 1 - 3
    EXPECT_EQ(4 * c_blockSize,requests.at(2).first); // 2 - 4
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,removePendingReadsGivenSomeListedHaveErrors)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;

    QMap<tint64,QPair<tint,tint> > readOpsMap;
    readOpsMap.insert(6,QPair<tint,tint>(1,1));
    readOpsMap.insert(8,QPair<tint,tint>(2,1));
    
    QVector<QPair<tint64,tint> > requests;
    requests.append(QPair<tint64,tint>(4 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(6 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(8 * c_blockSize,c_blockSize));
    
    CachedFileStreamRemovePendingReadsTest file;
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOpsMap));
    
    file.testRemovePendingReads(requests);

    ASSERT_EQ(3,requests.size());
    EXPECT_EQ(4 * c_blockSize,requests.at(0).first);
    EXPECT_EQ(6 * c_blockSize,requests.at(1).first);
    EXPECT_EQ(8 * c_blockSize,requests.at(2).first);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,removePendingReadsGivenSomeListedArePendingOrHaveErrors)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;
    
    QMap<tint64,QPair<tint,tint> > readOpsMap;
    readOpsMap.insert(2,QPair<tint,tint>(1,0));
    readOpsMap.insert(3,QPair<tint,tint>(2,1));
    readOpsMap.insert(4,QPair<tint,tint>(3,1));
    readOpsMap.insert(5,QPair<tint,tint>(4,0));
    readOpsMap.insert(6,QPair<tint,tint>(5,0));
    readOpsMap.insert(8,QPair<tint,tint>(6,1));
    
    QVector<QPair<tint64,tint> > requests;
    requests.append(QPair<tint64,tint>(1 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(2 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(3 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(4 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(5 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(6 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(7 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(8 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(9 * c_blockSize,c_blockSize));
    
    CachedFileStreamRemovePendingReadsTest file;
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOpsMap));
    
    file.testRemovePendingReads(requests);
    
    ASSERT_EQ(6,requests.size());
    EXPECT_EQ(1 * c_blockSize,requests.at(0).first); // 0 - 1
    EXPECT_EQ(3 * c_blockSize,requests.at(1).first); // 1 - 3
    EXPECT_EQ(4 * c_blockSize,requests.at(2).first); // 2 - 4
    EXPECT_EQ(7 * c_blockSize,requests.at(3).first); // 3 - 7
    EXPECT_EQ(8 * c_blockSize,requests.at(4).first); // 4 - 8
    EXPECT_EQ(9 * c_blockSize,requests.at(5).first); // 5 - 9
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,removePendingReadsGivenAllListedArePending)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;

    QMap<tint64,QPair<tint,tint> > readOpsMap;
    readOpsMap.insert(4,QPair<tint,tint>(1,0));
    readOpsMap.insert(6,QPair<tint,tint>(2,0));
    readOpsMap.insert(8,QPair<tint,tint>(3,0));
    
    QVector<QPair<tint64,tint> > requests;
    requests.append(QPair<tint64,tint>(4 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(6 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(8 * c_blockSize,c_blockSize));
    
    CachedFileStreamRemovePendingReadsTest file;
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOpsMap));
    
    file.testRemovePendingReads(requests);
    
    ASSERT_TRUE(requests.isEmpty());
}

//-------------------------------------------------------------------------------------------

class CachedFileStreamIsErrorInReads : public CachedFileStream
{
    public:
        MOCK_CONST_METHOD0(readOpsConst,const QMap<tint64,QPair<tint,tint> >&());
        bool testIsErrorInReads(const QVector<QPair<tint64,tint> >& requests) const;
};

//-------------------------------------------------------------------------------------------

bool CachedFileStreamIsErrorInReads::testIsErrorInReads(const QVector<QPair<tint64,tint> >& requests) const
{
    return isErrorInReads(requests);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isErrorInReadsGivenNoEntriesForGivenRequests)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;

    QMap<tint64,QPair<tint,tint> > readOpsMap;
    readOpsMap.insert(1,QPair<tint,tint>(1,0));
    readOpsMap.insert(5,QPair<tint,tint>(2,1));
    
    QVector<QPair<tint64,tint> > requests;
    requests.append(QPair<tint64,tint>(2 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(6 * c_blockSize,c_blockSize));
    
    CachedFileStreamIsErrorInReads file;
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOpsMap));
    
    ASSERT_FALSE(file.testIsErrorInReads(requests));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isErrorInReadsGivenOnlyPendingForGivenRequests)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;

    QMap<tint64,QPair<tint,tint> > readOpsMap;
    readOpsMap.insert(2,QPair<tint,tint>(1,0));
    readOpsMap.insert(6,QPair<tint,tint>(2,0));
    
    QVector<QPair<tint64,tint> > requests;
    requests.append(QPair<tint64,tint>(2 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(6 * c_blockSize,c_blockSize));
    
    CachedFileStreamIsErrorInReads file;
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOpsMap));
    
    ASSERT_FALSE(file.testIsErrorInReads(requests));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,isErrorInReadsGivenErrorForOneOfTheGivenRequests)
{
    const tint c_blockSize = CachedFileStream::c_blockSize;

    QMap<tint64,QPair<tint,tint> > readOpsMap;
    readOpsMap.insert(2,QPair<tint,tint>(1,0));
    readOpsMap.insert(5,QPair<tint,tint>(2,1));
    readOpsMap.insert(6,QPair<tint,tint>(3,0));
    
    QVector<QPair<tint64,tint> > requests;
    requests.append(QPair<tint64,tint>(2 * c_blockSize,c_blockSize));
    requests.append(QPair<tint64,tint>(5 * c_blockSize,c_blockSize));
    
    CachedFileStreamIsErrorInReads file;
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOpsMap));
    
    ASSERT_TRUE(file.testIsErrorInReads(requests));
}

//-------------------------------------------------------------------------------------------

class CachedFileStreamOnCompletedTest : public CachedFileStream
{
    public:
        MOCK_METHOD0(getFile,AsynchronousFileReader *());
        MOCK_METHOD0(getCache,QMap<tint64,QSharedPointer<QByteArray> >&());
        MOCK_METHOD0(getCacheMask,QMap<tint64,QMap<tint,tint> >&());
        MOCK_METHOD0(readOps,QMap<tint64,QPair<tint,tint> >&());
        MOCK_CONST_METHOD0(readOpsConst,const QMap<tint64,QPair<tint,tint> >&());

        void testOnComplete();
};

//-------------------------------------------------------------------------------------------

void CachedFileStreamOnCompletedTest::testOnComplete()
{
    onCompleted();
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,onCompletedGivenFileHasBeenClosed)
{
    CachedFileStreamOnCompletedTest file;
    EXPECT_CALL(file,getFile()).WillRepeatedly(Return(reinterpret_cast<AsynchronousFileReader *>(0)));
    
    file.testOnComplete();
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,onCompletedGivenEmptyIDListGivenByAsyncFile)
{
    QList<int> readIDList;

    QMap<tint64,QPair<tint,tint> > readOps;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,hasCompleted()).Times(1).WillOnce(Return(readIDList));
    
    CachedFileStreamOnCompletedTest file;
    EXPECT_CALL(file,getFile()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,readOps()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOps));
    
    file.testOnComplete();
    
    EXPECT_TRUE(readOps.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,onCompletedGivenIDIsNotListedAsExpectedReadOperation)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'a'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    
    QList<int> readIDList;
    readIDList.append(10);
    
    QMap<tint64,QPair<tint,tint> > readOps;
    QMap<tint64,QMap<tint,tint> > cacheMask;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,hasCompleted()).Times(1).WillOnce(Return(readIDList));
    EXPECT_CALL(fileASync,result(10)).Times(1).WillOnce(Return(dataA));
    
    CachedFileStreamOnCompletedTest file;
    EXPECT_CALL(file,getFile()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,readOps()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testOnComplete();
    
    EXPECT_TRUE(readOps.isEmpty());
    EXPECT_TRUE(cacheMask.isEmpty());
    EXPECT_TRUE(cache.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,onCompletedGivenNoDataGivenForIDByAsyncFile)
{
    QSharedPointer<QByteArray> dataA;
    QSharedPointer<QByteArray> cachedDataA(new QByteArray(CachedFileStream::c_blockSize,'d'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    cache.insert(5,cachedDataA);
    
    QList<int> readIDList;
    readIDList.append(10);
    
    QMap<tint64,QPair<tint,tint> > readOps;
    readOps.insert(5,QPair<tint,tint>(10,0));
    
    QMap<tint,tint> maskBlockA;
    maskBlockA.insert(10,20);
    maskBlockA.insert(50,60);
    QMap<tint64,QMap<tint,tint> > cacheMask;
    cacheMask.insert(5,maskBlockA);
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,hasCompleted()).Times(1).WillOnce(Return(readIDList));
    EXPECT_CALL(fileASync,result(10)).Times(1).WillOnce(Return(dataA));
    
    CachedFileStreamOnCompletedTest file;
    EXPECT_CALL(file,getFile()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,readOps()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testOnComplete();
    
    ASSERT_EQ(1,readOps.size());
    EXPECT_EQ(1,readOps.find(5).value().second);
    
    ASSERT_EQ(1,cacheMask.size());
    EXPECT_EQ(20,cacheMask.find(5).value().find(10).value());
    EXPECT_EQ(60,cacheMask.find(5).value().find(50).value());
    
    ASSERT_EQ(1,cache.size());
    EXPECT_EQ(0,memcmp(cache.find(5).value()->data(),cachedDataA->data(),CachedFileStream::c_blockSize));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,onCompletedGivenCacheAndMaskContainEntryForCorrispondingID)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'a'));
    QSharedPointer<QByteArray> cachedDataA(new QByteArray(CachedFileStream::c_blockSize,'d'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    cache.insert(5,cachedDataA);
    
    QList<int> readIDList;
    readIDList.append(10);
    
    QMap<tint64,QPair<tint,tint> > readOps;
    readOps.insert(5,QPair<tint,tint>(10,0));
    
    QMap<tint,tint> maskBlockA;
    maskBlockA.insert(10,20);
    maskBlockA.insert(50,60);
    QMap<tint64,QMap<tint,tint> > cacheMask;
    cacheMask.insert(5,maskBlockA);
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,hasCompleted()).Times(1).WillOnce(Return(readIDList));
    EXPECT_CALL(fileASync,result(10)).Times(1).WillOnce(Return(dataA));
    
    CachedFileStreamOnCompletedTest file;
    EXPECT_CALL(file,getFile()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,readOps()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testOnComplete();
    
    EXPECT_TRUE(readOps.isEmpty());
    EXPECT_TRUE(cacheMask.isEmpty());
    ASSERT_EQ(1,cache.size());
    EXPECT_EQ(0,memcmp(cache.find(5).value()->data(),dataA->data(),CachedFileStream::c_blockSize));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,onCompletedGivenNoCacheOrMaskContainEntryForCorrispondingID)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'a'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    
    QList<int> readIDList;
    readIDList.append(10);
    
    QMap<tint64,QPair<tint,tint> > readOps;
    readOps.insert(5,QPair<tint,tint>(10,0));
    
    QMap<tint64,QMap<tint,tint> > cacheMask;
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,hasCompleted()).Times(1).WillOnce(Return(readIDList));
    EXPECT_CALL(fileASync,result(10)).Times(1).WillOnce(Return(dataA));
    
    CachedFileStreamOnCompletedTest file;
    EXPECT_CALL(file,getFile()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,readOps()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testOnComplete();
    
    EXPECT_TRUE(readOps.isEmpty());
    EXPECT_TRUE(cacheMask.isEmpty());
    ASSERT_EQ(1,cache.size());
    EXPECT_EQ(0,memcmp(cache.find(5).value()->data(),dataA->data(),CachedFileStream::c_blockSize));
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,onCompletedGivenMultipleReadIDs)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(CachedFileStream::c_blockSize,'a'));
    QSharedPointer<QByteArray> dataB(new QByteArray(CachedFileStream::c_blockSize,'b'));
    QSharedPointer<QByteArray> dataC(new QByteArray(CachedFileStream::c_blockSize,'c'));
    
    QSharedPointer<QByteArray> cachedDataB(new QByteArray(CachedFileStream::c_blockSize,'d'));
    QSharedPointer<QByteArray> cachedDataC(new QByteArray(CachedFileStream::c_blockSize,'e'));
    
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    cache.insert(6,cachedDataB);
    cache.insert(7,cachedDataC);
    
    QList<int> readIDList;
    readIDList.append(10);
    readIDList.append(11);
    readIDList.append(12);
    
    QMap<tint64,QPair<tint,tint> > readOps;
    readOps.insert(5,QPair<tint,tint>(10,0));
    readOps.insert(6,QPair<tint,tint>(11,0));
    readOps.insert(7,QPair<tint,tint>(12,0));
    
    QMap<tint,tint> maskBlockB;
    maskBlockB.insert(10,20);
    maskBlockB.insert(50,60);
    QMap<tint64,QMap<tint,tint> > cacheMask;
    cacheMask.insert(6,maskBlockB);
    
    AsynchronousFileReaderMock fileASync;
    EXPECT_CALL(fileASync,hasCompleted()).Times(1).WillOnce(Return(readIDList));
    EXPECT_CALL(fileASync,result(10)).Times(1).WillOnce(Return(dataA));
    EXPECT_CALL(fileASync,result(11)).Times(1).WillOnce(Return(dataB));
    EXPECT_CALL(fileASync,result(12)).Times(1).WillOnce(Return(dataC));
    
    CachedFileStreamOnCompletedTest file;
    EXPECT_CALL(file,getFile()).WillRepeatedly(Return(&fileASync));
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,readOps()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,readOpsConst()).WillRepeatedly(ReturnRef(readOps));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testOnComplete();
    
    EXPECT_TRUE(readOps.isEmpty());
    EXPECT_TRUE(cacheMask.isEmpty());
    ASSERT_EQ(3,cache.size());
    EXPECT_EQ(0,memcmp(cache.find(5).value()->data(),dataA->data(),CachedFileStream::c_blockSize));
    EXPECT_EQ(0,memcmp(cache.find(6).value()->data(),dataB->data(),CachedFileStream::c_blockSize));
    EXPECT_EQ(0,memcmp(cache.find(7).value()->data(),dataC->data(),CachedFileStream::c_blockSize));
}

//-------------------------------------------------------------------------------------------

class CachedFileStreamMergeAdjacentMasksTest : public CachedFileStream
{
    public:
        void testMergeAdjacentMasks(QMap<tint,tint>& maskMap);
};

//-------------------------------------------------------------------------------------------

void CachedFileStreamMergeAdjacentMasksTest::testMergeAdjacentMasks(QMap<tint,tint>& maskMap)
{
    mergeAdjacentMasks(maskMap);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenMapIsEmpty)
{
    QMap<tint,tint> mask;
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(0,mask.size());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenSingleEntry)
{
    QMap<tint,tint> mask;
    mask.insert(10,20);
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(1,mask.size());
    ASSERT_TRUE(mask.find(10) != mask.end());
    ASSERT_EQ(20,mask.find(10).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenTwoDisjointedEntries)
{
    QMap<tint,tint> mask;
    mask.insert(10,20);
    mask.insert(70,10);
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(2,mask.size());
    ASSERT_TRUE(mask.find(10) != mask.end());
    ASSERT_EQ(20,mask.find(10).value());
    ASSERT_TRUE(mask.find(70) != mask.end());
    ASSERT_EQ(10,mask.find(70).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenTwoAdjacentEntries)
{
    QMap<tint,tint> mask;
    mask.insert(10,20);
    mask.insert(30,30);
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(1,mask.size());
    ASSERT_TRUE(mask.find(10) != mask.end());
    ASSERT_EQ(50,mask.find(10).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenTwoAdjacentFollowedByDisjointedEntries)
{
    QMap<tint,tint> mask;
    mask.insert(10,20);
    mask.insert(30,30);
    mask.insert(70,10);
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(2,mask.size());
    ASSERT_TRUE(mask.find(10) != mask.end());
    ASSERT_EQ(50,mask.find(10).value());
    ASSERT_TRUE(mask.find(70) != mask.end());
    ASSERT_EQ(10,mask.find(70).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenDisjointedByTwoAdjacentEntries)
{
    QMap<tint,tint> mask;
    mask.insert(10,20);
    mask.insert(40,30);
    mask.insert(70,10);
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(2,mask.size());
    ASSERT_TRUE(mask.find(10) != mask.end());
    ASSERT_EQ(20,mask.find(10).value());
    ASSERT_TRUE(mask.find(40) != mask.end());
    ASSERT_EQ(40,mask.find(40).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenThreeAdjacentEntries)
{
    QMap<tint,tint> mask;
    mask.insert(10,30);
    mask.insert(40,20);
    mask.insert(60,10);
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(1,mask.size());
    ASSERT_TRUE(mask.find(10) != mask.end());
    ASSERT_EQ(60,mask.find(10).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenFourEntriesWithTwoAdjacent)
{
    QMap<tint,tint> mask;
    mask.insert(10,30);
    mask.insert(40,10);
    mask.insert(70,10);
    mask.insert(80,20);
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(2,mask.size());
    ASSERT_TRUE(mask.find(10) != mask.end());
    ASSERT_EQ(40,mask.find(10).value());
    ASSERT_TRUE(mask.find(70) != mask.end());
    ASSERT_EQ(30,mask.find(70).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,mergeAdjustMasksGivenFourEntriesWithMiddleAdjacent)
{
    QMap<tint,tint> mask;
    mask.insert(10,30);
    mask.insert(50,20);
    mask.insert(70,10);
    mask.insert(90,10);
    
    CachedFileStreamMergeAdjacentMasksTest file;
    file.testMergeAdjacentMasks(mask);
    
    ASSERT_EQ(3,mask.size());
    ASSERT_TRUE(mask.find(10) != mask.end());
    ASSERT_EQ(30,mask.find(10).value());
    ASSERT_TRUE(mask.find(50) != mask.end());
    ASSERT_EQ(30,mask.find(50).value());
    ASSERT_TRUE(mask.find(90) != mask.end());
    ASSERT_EQ(10,mask.find(90).value());
}

//-------------------------------------------------------------------------------------------

class CachedFileStreamInsertIntoMasksTest : public CachedFileStream
{
    public:
        void testInsertIntoMasks(tint offset,tint length,QMap<tint,tint>& maskMap);
};

//-------------------------------------------------------------------------------------------

void CachedFileStreamInsertIntoMasksTest::testInsertIntoMasks(tint offset,tint length,QMap<tint,tint>& maskMap)
{
    insertIntoMasks(offset,length,maskMap);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenEmptyMask)
{
    QMap<tint,tint> maskMap;
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(70,20,maskMap);
    
    ASSERT_EQ(1,maskMap.size());
    ASSERT_TRUE(maskMap.find(70)!=maskMap.end());
    EXPECT_EQ(20,maskMap.find(70).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenNoIntersectionWithOneEntry)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(40,30);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(70,20,maskMap);
    
    ASSERT_EQ(2,maskMap.size());
    ASSERT_TRUE(maskMap.find(40)!=maskMap.end());
    EXPECT_EQ(30,maskMap.find(40).value());
    ASSERT_TRUE(maskMap.find(70)!=maskMap.end());
    EXPECT_EQ(20,maskMap.find(70).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionStartingBeforeWithOneEntry)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(40,30);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(30,20,maskMap);
    
    ASSERT_EQ(1,maskMap.size());
    ASSERT_TRUE(maskMap.find(30)!=maskMap.end());
    EXPECT_EQ(40,maskMap.find(30).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionEndingAfterWithOneEntry)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(40,30);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(69,21,maskMap);
    
    ASSERT_EQ(1,maskMap.size());
    ASSERT_TRUE(maskMap.find(40)!=maskMap.end());
    EXPECT_EQ(50,maskMap.find(40).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionSurroundingWithOneEntry)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(40,30);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(30,50,maskMap);
    
    ASSERT_EQ(1,maskMap.size());
    ASSERT_TRUE(maskMap.find(30)!=maskMap.end());
    EXPECT_EQ(50,maskMap.find(30).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionEmbeddedWithOneEntry)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(40,30);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(50,10,maskMap);
    
    ASSERT_EQ(1,maskMap.size());
    ASSERT_TRUE(maskMap.find(40)!=maskMap.end());
    EXPECT_EQ(30,maskMap.find(40).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionStartingBeforeWithThreeEntries)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(10,20);
    maskMap.insert(40,30);
    maskMap.insert(80,10);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(30,20,maskMap);
    
    ASSERT_EQ(3,maskMap.size());
    ASSERT_TRUE(maskMap.find(10)!=maskMap.end());
    EXPECT_EQ(20,maskMap.find(10).value());
    ASSERT_TRUE(maskMap.find(30)!=maskMap.end());
    EXPECT_EQ(40,maskMap.find(30).value());
    ASSERT_TRUE(maskMap.find(80)!=maskMap.end());
    EXPECT_EQ(10,maskMap.find(80).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionEndingAfterWithThreeEntries)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(10,20);
    maskMap.insert(40,30);
    maskMap.insert(80,10);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(20,15,maskMap);
    
    ASSERT_EQ(3,maskMap.size());
    ASSERT_TRUE(maskMap.find(10)!=maskMap.end());
    EXPECT_EQ(25,maskMap.find(10).value());
    ASSERT_TRUE(maskMap.find(40)!=maskMap.end());
    EXPECT_EQ(30,maskMap.find(40).value());
    ASSERT_TRUE(maskMap.find(80)!=maskMap.end());
    EXPECT_EQ(10,maskMap.find(80).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionSurroundingWithThreeEntries)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(10,20);
    maskMap.insert(40,30);
    maskMap.insert(80,10);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(70,30,maskMap);
    
    ASSERT_EQ(3,maskMap.size());
    ASSERT_TRUE(maskMap.find(10)!=maskMap.end());
    EXPECT_EQ(20,maskMap.find(10).value());
    ASSERT_TRUE(maskMap.find(40)!=maskMap.end());
    EXPECT_EQ(30,maskMap.find(40).value());
    ASSERT_TRUE(maskMap.find(70)!=maskMap.end());
    EXPECT_EQ(30,maskMap.find(70).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionEmbeddedWithThreeEntries)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(10,20);
    maskMap.insert(40,30);
    maskMap.insert(80,10);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(50,10,maskMap);
    
    ASSERT_EQ(3,maskMap.size());
    ASSERT_TRUE(maskMap.find(10)!=maskMap.end());
    EXPECT_EQ(20,maskMap.find(10).value());
    ASSERT_TRUE(maskMap.find(40)!=maskMap.end());
    EXPECT_EQ(30,maskMap.find(40).value());
    ASSERT_TRUE(maskMap.find(80)!=maskMap.end());
    EXPECT_EQ(10,maskMap.find(80).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionFirstTwoWithThreeEntries)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(10,20);
    maskMap.insert(40,30);
    maskMap.insert(80,10);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(20,40,maskMap);
    
    ASSERT_EQ(2,maskMap.size());
    ASSERT_TRUE(maskMap.find(10)!=maskMap.end());
    EXPECT_EQ(60,maskMap.find(10).value());
    ASSERT_TRUE(maskMap.find(80)!=maskMap.end());
    EXPECT_EQ(10,maskMap.find(80).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionLastTwoWithThreeEntries)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(10,20);
    maskMap.insert(40,30);
    maskMap.insert(80,10);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(50,50,maskMap);
    
    ASSERT_EQ(2,maskMap.size());
    ASSERT_TRUE(maskMap.find(10)!=maskMap.end());
    EXPECT_EQ(20,maskMap.find(10).value());
    ASSERT_TRUE(maskMap.find(40)!=maskMap.end());
    EXPECT_EQ(60,maskMap.find(40).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,insertIntoMasksGivenIntersectionOverAllThreeEntries)
{
    QMap<tint,tint> maskMap;
    maskMap.insert(10,20);
    maskMap.insert(40,30);
    maskMap.insert(80,10);
    
    CachedFileStreamInsertIntoMasksTest file;
    file.testInsertIntoMasks(0,85,maskMap);
    
    ASSERT_EQ(1,maskMap.size());
    ASSERT_TRUE(maskMap.find(0)!=maskMap.end());
    EXPECT_EQ(90,maskMap.find(0).value());
}

//-------------------------------------------------------------------------------------------

class CachedFileStreamAddMaskToBlock : public CachedFileStream
{
    public:
        MOCK_METHOD0(getCache,QMap<tint64,QSharedPointer<QByteArray> >&());
        MOCK_METHOD0(getCacheMask,QMap<tint64,QMap<tint,tint> >&());
        void testAddMaskToBlock(tint64 blockIndex,tint offset,tint length);
};

//-------------------------------------------------------------------------------------------

void CachedFileStreamAddMaskToBlock::testAddMaskToBlock(tint64 blockIndex,tint offset,tint length)
{
    addMaskToBlock(blockIndex,offset,length);
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,addMaskToBlockGivenCacheForBlockNotFound)
{
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    
    QMap<tint,tint> maskMap;
    maskMap.insert(0,200);
    maskMap.insert(800,200);
    QMap<tint64,QMap<tint,tint> > cacheMask;
    cacheMask.insert(5,maskMap);
    
    CachedFileStreamAddMaskToBlock file;
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testAddMaskToBlock(5,-20,1040);

    ASSERT_TRUE(cache.isEmpty());
    ASSERT_TRUE(cacheMask.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,addMaskToBlockGivenRangeBoundsTheCacheDataBlock)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(1000,'a'));
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    cache.insert(5,dataA);
    
    QMap<tint,tint> maskMap;
    maskMap.insert(0,200);
    maskMap.insert(800,200);
    QMap<tint64,QMap<tint,tint> > cacheMask;
    cacheMask.insert(5,maskMap);
    
    CachedFileStreamAddMaskToBlock file;
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testAddMaskToBlock(5,-20,1040);

    ASSERT_TRUE(cache.isEmpty());
    ASSERT_TRUE(cacheMask.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,addMaskToBlockGivenNoExistingMask)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(1000,'a'));
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    cache.insert(5,dataA);
    
    QMap<tint64,QMap<tint,tint> > cacheMask;
    
    CachedFileStreamAddMaskToBlock file;
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testAddMaskToBlock(5,300,300);
    
    ASSERT_EQ(1,cache.size());
    ASSERT_TRUE(cache.find(5)!=cache.end());
    
    ASSERT_EQ(1,cacheMask.size());
    ASSERT_EQ(1,cacheMask.find(5).value().size());
    ASSERT_EQ(300,cacheMask.find(5).value().find(300).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,addMaskToBlockGivenEmptyMask)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(1000,'a'));
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    cache.insert(5,dataA);
    
    QMap<tint,tint> maskMap;
    QMap<tint64,QMap<tint,tint> > cacheMask;
    cacheMask.insert(5,maskMap);
    
    CachedFileStreamAddMaskToBlock file;
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testAddMaskToBlock(5,300,300);
    
    ASSERT_EQ(1,cache.size());
    ASSERT_TRUE(cache.find(5)!=cache.end());
    
    ASSERT_EQ(1,cacheMask.size());
    ASSERT_EQ(1,cacheMask.find(5).value().size());
    ASSERT_EQ(300,cacheMask.find(5).value().find(300).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,addMaskToBlockGivenTwoMasksWithDisjointAddedBetween)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(1000,'a'));
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    cache.insert(5,dataA);
    
    QMap<tint,tint> maskMap;
    maskMap.insert(0,200);
    maskMap.insert(800,200);
    QMap<tint64,QMap<tint,tint> > cacheMask;
    cacheMask.insert(5,maskMap);
    
    CachedFileStreamAddMaskToBlock file;
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testAddMaskToBlock(5,300,300);
    
    ASSERT_EQ(1,cache.size());
    ASSERT_TRUE(cache.find(5)!=cache.end());
    
    ASSERT_EQ(1,cacheMask.size());
    ASSERT_EQ(3,cacheMask.find(5).value().size());
    ASSERT_EQ(200,cacheMask.find(5).value().find(0).value());
    ASSERT_EQ(300,cacheMask.find(5).value().find(300).value());
    ASSERT_EQ(200,cacheMask.find(5).value().find(800).value());
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,addMaskToBlockGivenTwoMasksAndOverlappingMaskAdded)
{
    QSharedPointer<QByteArray> dataA(new QByteArray(1000,'a'));
    QMap<tint64,QSharedPointer<QByteArray> > cache;
    cache.insert(5,dataA);
    
    QMap<tint,tint> maskMap;
    maskMap.insert(0,200);
    maskMap.insert(800,200);
    QMap<tint64,QMap<tint,tint> > cacheMask;
    cacheMask.insert(5,maskMap);
    
    CachedFileStreamAddMaskToBlock file;
    EXPECT_CALL(file,getCache()).WillRepeatedly(ReturnRef(cache));
    EXPECT_CALL(file,getCacheMask()).WillRepeatedly(ReturnRef(cacheMask));
    
    file.testAddMaskToBlock(5,100,800);
    
    ASSERT_TRUE(cache.isEmpty());
    ASSERT_TRUE(cacheMask.isEmpty());
}

//-------------------------------------------------------------------------------------------
// CachedFileStreamQtUnitTest
//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::readFromEmptyFile()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest7.dat");
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    testFileWriter.close();

    char mem[256];

    CachedFileStream file;
    QVERIFY(file.open(fileName));
    
    QCOMPARE(static_cast<int>(file.size()),0);
    
    QCOMPARE(0,file.read(mem,0,0));
    QCOMPARE(0,file.cache(0,0));
    QVERIFY(file.isCached(0,0));
    
    QCOMPARE(0,file.read(mem,0,10));
    QCOMPARE(0,file.cache(0,10));
    QVERIFY(file.isCached(0,10));
    
    QCOMPARE(-1,file.read(mem,-1,0));
    QCOMPARE(-1,file.cache(-1,0));
    QVERIFY(!file.isCached(-1,0));
    
    QCOMPARE(-1,file.read(mem,-1,10));
    QCOMPARE(-1,file.cache(-1,10));
    QVERIFY(!file.isCached(-1,10));
    
    QCOMPARE(-1,file.read(mem,10,0));
    QCOMPARE(-1,file.cache(10,0));
    QVERIFY(!file.isCached(10,0));
    
    QCOMPARE(-1,file.read(mem,10,20));
    QCOMPARE(-1,file.cache(10,20));
    QVERIFY(!file.isCached(10,20));

    file.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::readOnFileBoundaries()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest8.dat");
    
    const tint c_noElementsReadSize = 100;
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_noElementsReadSize,c_noElementsReadSize));
    testFileWriter.close();

    tchar *mem = new tchar [c_noElementsReadSize << 1];

    CachedFileStream file;
    QVERIFY(file.open(fileName));
    
    QCOMPARE(static_cast<int>(file.size()),c_noElementsReadSize * 2);
    
    QCOMPARE(file.read(mem,0,c_noElementsReadSize << 1),c_noElementsReadSize << 1);
    QVERIFY(writer.verifyIsExpectedContents(mem,0,c_noElementsReadSize << 1));
    QVERIFY(file.isCached(0,c_noElementsReadSize << 1));
    QCOMPARE(file.read(mem,0,c_noElementsReadSize << 1),c_noElementsReadSize << 1);
    QVERIFY(writer.verifyIsExpectedContents(mem,0,c_noElementsReadSize << 1));
    file.clear();
    
    QCOMPARE(file.read(mem,-1,c_noElementsReadSize << 1),-1);
    QVERIFY(!file.isCached(0,c_noElementsReadSize << 1));
    file.clear();
    
    QCOMPARE(file.read(mem,(c_noElementsReadSize - 1) << 1,c_noElementsReadSize << 1),2);
    QVERIFY(writer.verifyIsExpectedContents(mem,(c_noElementsReadSize - 1) << 1,2));
    QVERIFY(file.isCached((c_noElementsReadSize - 1) << 1,c_noElementsReadSize << 1));
    QVERIFY(file.isCached((c_noElementsReadSize - 1) << 1,2));
    QCOMPARE(file.read(mem,(c_noElementsReadSize - 1) << 1,c_noElementsReadSize << 1),2);
    QVERIFY(writer.verifyIsExpectedContents(mem,(c_noElementsReadSize - 1) << 1,2));
    file.clear();

    QCOMPARE(file.read(mem,0,(c_noElementsReadSize * 2) << 1),c_noElementsReadSize << 1);
    QVERIFY(writer.verifyIsExpectedContents(mem,0,c_noElementsReadSize << 1));
    QVERIFY(file.isCached(0,(c_noElementsReadSize * 2) << 1));
    QVERIFY(file.isCached(0,c_noElementsReadSize << 1));
    QCOMPARE(file.read(mem,0,(c_noElementsReadSize * 2) << 1),c_noElementsReadSize << 1);
    QVERIFY(writer.verifyIsExpectedContents(mem,0,c_noElementsReadSize << 1));
    file.clear();
    
    delete [] mem;
        
    file.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::readFileUncachedFromBeginningToEnd()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest1.dat");
    
    const tint c_noElementsReadSize = 200;
    const tint c_noOfReads = 200;
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_noElementsReadSize * c_noOfReads,c_noElementsReadSize * c_noOfReads));
    testFileWriter.close();

    CachedFileStream file;
    QVERIFY(file.open(fileName));
    
    tchar *mem = new tchar [c_noElementsReadSize << 1];
    
    for(tint i=0;i<c_noOfReads;i++)
    {
        tint fromPosition = (i * c_noElementsReadSize) << 1;
        tint length = c_noElementsReadSize << 1;
        QCOMPARE(file.read(mem,fromPosition,length),length);
        QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));
    }
    
    delete [] mem;
        
    file.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::readFileAfterCachingFromBeginningToEnd()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest2.dat");
    
    const tint c_noElementsReadSize = 200;
    const tint c_noOfReads = 200;
    tint length = c_noElementsReadSize << 1;
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_noElementsReadSize * c_noOfReads,c_noElementsReadSize * c_noOfReads));
    testFileWriter.close();

    CachedFileStream file;
    QVERIFY(file.open(fileName));
    
    QSet<tint> cacheWaitSet;
    for(tint i=0;i<c_noOfReads;i++)
    {
        tint fromPosition = (i * c_noElementsReadSize) << 1;
        QCOMPARE(file.cache(fromPosition,length),length);
        cacheWaitSet.insert(fromPosition);
    }
    
    common::TimeStamp timeout(100.0);
    common::TimeStamp currentTime;
    while(!cacheWaitSet.isEmpty())
    {
        QSet<int>::iterator ppI = cacheWaitSet.begin();
        while(ppI!=cacheWaitSet.end())
        {
            if(file.isCached(*ppI,length))
            {
                ppI = cacheWaitSet.erase(ppI);
            }
            else
            {
                ppI++;
            }
        }
        QCoreApplication::processEvents(QEventLoop::AllEvents);
        common::usleepThread(1);
        currentTime += 0.0001;
    }
    
    QVERIFY(cacheWaitSet.isEmpty());
    QVERIFY(file.isCached(0,c_noOfReads * length));
    
    tchar *mem = new tchar [c_noElementsReadSize << 1];
    
    for(tint i=0;i<c_noOfReads;i++)
    {
        tint fromPosition = (i * c_noElementsReadSize) << 1;
        QVERIFY(file.isCached(fromPosition,length));
        QCOMPARE(file.read(mem,fromPosition,length),length);
        QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));
    }
    
    delete [] mem;
        
    file.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::readFileFromBeginningToEndOver4LoopsWithLookAheadCaching()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest3.dat");
    
    const tint c_noReadsAhead = 10;
    const tint c_noReadsBehind = 5;
    const tint c_noElementsReadSize = 200;
    const tint c_noOfReads = 200;
    tint length = c_noElementsReadSize << 1;
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_noElementsReadSize * c_noOfReads,c_noElementsReadSize * c_noOfReads));
    testFileWriter.close();

    CachedFileStream file;
    QVERIFY(file.open(fileName));
    
    tchar *mem = new tchar [c_noElementsReadSize << 1];

    for(tint j=0;j<4;j++)
    {
        tint i;
    
        // Pre-cache
        for(i=0;i<c_noReadsBehind + c_noReadsAhead;i++)
        {
            tint fromPosition = (i * c_noElementsReadSize) << 1;
            QCOMPARE(file.cache(fromPosition,length),length);
        }
        
        // Read
        for(i=0;i<c_noOfReads - c_noReadsAhead;i++)
        {
            tint fromPosition = (i * c_noElementsReadSize) << 1;
            tint cacheFromPosition = ((i + c_noReadsAhead) * c_noElementsReadSize) << 1;
            tint dropFromPosition = ((i - c_noReadsBehind) * c_noElementsReadSize) << 1;
            
            QCOMPARE(file.read(mem,fromPosition,length),length);
            QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));
            
            if(!file.isCached(fromPosition,length))
            {
                common::msleepThread(10);
            }
            QCOMPARE(file.cache(cacheFromPosition,length),length);
        
            if(i > c_noReadsBehind)
            {
                file.drop(dropFromPosition,length);
                QVERIFY(!file.isCached(dropFromPosition,length));
            }
        }
    
        while(i < c_noOfReads)
        {
            tint fromPosition = (i * c_noElementsReadSize) << 1;
            QCOMPARE(file.read(mem,fromPosition,length),length);
            QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));
            i++;
        }
    
        file.clear();
        for(i=0;i<c_noOfReads;i++)
        {
            tint fromPosition = (i * c_noElementsReadSize) << 1;
            QVERIFY(!file.isCached(fromPosition,length));
        }
    }
    
    delete [] mem;
        
    file.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::readFileFromRandomLocationsUncached()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest4.dat");
    
    const tint c_noElementsReadSize = 200;
    const tint c_noOfReads = 200;
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_noElementsReadSize * c_noOfReads,c_noElementsReadSize * c_noOfReads));
    testFileWriter.close();

    Random *random = Random::instance();
    random->seed(0);

    CachedFileStream file;
    QVERIFY(file.open(fileName));
    
    for(tint i=0;i<c_noOfReads;i++)
    {
        tint fromPosition = static_cast<tint>(static_cast<tfloat64>(c_noElementsReadSize * c_noOfReads) * random->randomReal1()) << 1;
        tint length = static_cast<tint>(static_cast<tfloat64>(static_cast<tint>(file.size()) - fromPosition) * random->randomReal1()) << 1;
        
        if(fromPosition + length > file.size())
        {
            length = file.size() - fromPosition;
        }
        if(length > 0)
        {
            tchar *mem = new tchar [length];
            QCOMPARE(file.read(mem,fromPosition,length),length);
            QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));
            delete [] mem;
        }
        file.clear();
    }
    
    file.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::readFileFromRandomLocationsWithCachingLarge()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest5.dat");
    
    const tint c_noElementsReadSize = 200;
    const tint c_noOfReads = 200;
    const tint c_noOfRuns = 100;
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_noElementsReadSize * c_noOfReads,c_noElementsReadSize * c_noOfReads));
    testFileWriter.close();

    Random *random = Random::instance();
    random->seed(0);

    CachedFileStream file;
    QVERIFY(file.open(fileName));
    
    for(tint i=0;i<c_noOfRuns;i++)
    {
        tint fromPosition = static_cast<tint>(static_cast<tfloat64>(c_noElementsReadSize * c_noOfReads) * random->randomReal1()) << 1;
        tint length = static_cast<tint>(static_cast<tfloat64>(static_cast<tint>(file.size()) - fromPosition) * random->randomReal1()) << 1;
        
        if(fromPosition + length > file.size())
        {
            length = file.size() - fromPosition;
        }
        if(length > 0)
        {
            tchar *mem = new tchar [length];

            QCOMPARE(file.read(mem,fromPosition,length),length);
            QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));
            
            QVERIFY(file.isCached(fromPosition,length));
            QCOMPARE(file.read(mem,fromPosition,length),length);
            QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));

            delete [] mem;
        }
        
        fromPosition = static_cast<tint>(static_cast<tfloat64>(c_noElementsReadSize * c_noOfReads) * random->randomReal1()) << 1;
        length = static_cast<tint>(static_cast<tfloat64>(static_cast<tint>(file.size()) - fromPosition) * random->randomReal1()) << 1;
        
        if(fromPosition + length > file.size())
        {
            length = file.size() - fromPosition;
        }
        if(length > 0)
        {
            file.drop(fromPosition,length);
            QVERIFY(!file.isCached(fromPosition,length));
        }        
    }
    
    file.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::readFileFromRandomLocationsWithCachingSmall()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest6.dat");
    
    const tint c_noElementsReadSize = 200;
    const tint c_noOfReads = 200;
    const tint c_noOfRuns = 500;
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_noElementsReadSize * c_noOfReads,c_noElementsReadSize * c_noOfReads));
    testFileWriter.close();

    Random *random = Random::instance();
    random->seed(0);

    CachedFileStream file;
    QVERIFY(file.open(fileName));
    
    for(tint i=0;i<c_noOfRuns;i++)
    {
        tint fromPosition = static_cast<tint>(static_cast<tfloat64>(c_noElementsReadSize * c_noOfReads) * random->randomReal1()) << 1;
        tint length = static_cast<tint>(static_cast<tfloat64>(file.size()) * random->randomReal1() * 0.01) << 1;
        
        if(fromPosition + length > file.size())
        {
            length = file.size() - fromPosition;
        }
        if(length > 0)
        {
            tchar *mem = new tchar [length];

            QCOMPARE(file.read(mem,fromPosition,length),length);
            QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));
            
            QVERIFY(file.isCached(fromPosition,length));
            QCOMPARE(file.read(mem,fromPosition,length),length);
            QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,length));

            delete [] mem;
        }

        fromPosition = static_cast<tint>(static_cast<tfloat64>(c_noElementsReadSize * c_noOfReads) * random->randomReal1()) << 1;
        length = static_cast<tint>(static_cast<tfloat64>(file.size()) * random->randomReal1() * 0.01) << 1;
        
        if(fromPosition + length > file.size())
        {
            length = file.size() - fromPosition;
        }
        if(length > 0)
        {
            file.drop(fromPosition,length);
            QVERIFY(!file.isCached(fromPosition,length));
        }        
    }
    
    file.close();

    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitTest::cachingAllFileAndSetRange()
{
    track::model::TrackDBTestEnviroment *testEnv = track::model::TrackDBTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(testEnv->getDBDirectory(), "cachedfilestreamtest7.dat");
    
    const tint c_noElementsReadSize = 100000;
    const tint c_fileLength = c_noElementsReadSize * 2;
    
    BIOStream testFileWriter(e_BIOStream_FileCreate | e_BIOStream_FileWrite);
    QVERIFY(testFileWriter.open(fileName));
    BIOStreamFunctionalTest writer;
    QVERIFY(writer.writeUShortTestFile(&testFileWriter,c_noElementsReadSize,c_noElementsReadSize));
    testFileWriter.close();
    
    const tint testRange[3][2] = {
        {0, c_fileLength / 4},                      // start (0, 50000)
        {c_fileLength / 4, (3 * c_fileLength) / 4}, // middle (50000, 150000)
        {(3 * c_fileLength) / 4, c_fileLength}      // end (150000, 200000)
    };
    
    for(tint rangeTestIndex = 0;rangeTestIndex < 3;rangeTestIndex++)
    {
        CachedFileStream file;
        QVERIFY(file.open(fileName));

        QCOMPARE(file.cache(0,c_fileLength),c_fileLength);

        while(!file.isCached(0,c_fileLength))
        {
            QCoreApplication::processEvents(QEventLoop::AllEvents);
            common::usleepThread(1);
        }
        
        tint64 fromPosition = static_cast<tint64>(testRange[rangeTestIndex][0]);
        tint rangeLength = testRange[rangeTestIndex][1] - testRange[rangeTestIndex][0];
        
        file.clearWithRetention(fromPosition,rangeLength);
    
        if(fromPosition > 0)
        {
            QVERIFY(!file.isCached(0,fromPosition));
            QVERIFY(!file.isCached(fromPosition-1,rangeLength+1));
        }
        if(fromPosition + rangeLength < c_fileLength)
        {
            tint limit = testRange[rangeTestIndex][1];
            QVERIFY(!file.isCached(limit,c_fileLength - limit));
            QVERIFY(!file.isCached(fromPosition,rangeLength+1));
        }
        
        QVERIFY(file.isCached(fromPosition,rangeLength));

        tchar *mem = new tchar [rangeLength];
        QCOMPARE(file.read(mem,fromPosition,rangeLength),rangeLength);
        QVERIFY(writer.verifyIsExpectedContents(mem,fromPosition,rangeLength));
        delete [] mem;
        
        file.close();
    }
    
    DiskOps::remove(fileName);
    QVERIFY(!DiskOps::exist(fileName));
}

//-------------------------------------------------------------------------------------------

CachedFileStreamQtUnitApplication::CachedFileStreamQtUnitApplication(int argc,char **argv) : QCoreApplication(argc,argv),
    m_succeeded(false)
{
    QTimer::singleShot(10,this,SLOT(runTests()));
}

//-------------------------------------------------------------------------------------------

CachedFileStreamQtUnitApplication::~CachedFileStreamQtUnitApplication()
{}

//-------------------------------------------------------------------------------------------

bool CachedFileStreamQtUnitApplication::testSucceeded() const
{
    return m_succeeded;
}

//-------------------------------------------------------------------------------------------

void CachedFileStreamQtUnitApplication::runTests()
{
    CachedFileStreamQtUnitTest tests;
    m_succeeded = (QTest::qExec(&tests)==0) ? true : false;
    quit();
}

//-------------------------------------------------------------------------------------------

TEST(CachedFileStream,RunQtUnitTests)
{
    QStringList libPaths = QApplication::libraryPaths();
    {
        char *argv = 0;
        CachedFileStreamQtUnitApplication tester(0,&argv);
        tester.exec();
        ASSERT_TRUE(tester.testSucceeded());
    }
    QApplication::setLibraryPaths(libPaths);
}

//-------------------------------------------------------------------------------------------
