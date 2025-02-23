#include "gtest/gtest.h"

#include "engine/dsdomega/inc/DSDIFFFileReader.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/info/inc/ID3Info2.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(DSDIFFFileReader, openAndParseDSDIFFFilePerChannel)
{
	const tubyte c_expectL[8] = { 0x96, 0x69, 0xA5, 0xAA, 0x6A, 0xA9, 0xCC, 0xCD };
	const tubyte c_expectR[8] = { 0x96, 0x69, 0xA5, 0xAA, 0x6A, 0xA9, 0xCC, 0xCD };
	
	QString inFilename = common::DiskOps::mergeName("engine/dsdomega/test/samples", "Test1.dff");
	QString fileName = test::UnitTestEnviroment::instance()->testFileName(inFilename);
	
	common::BIOStream ioFile(common::e_BIOStream_FileRead);
	ASSERT_TRUE(ioFile.open(fileName));
	
	engine::dsd::DSDIFFFileReader dsd(&ioFile);
	ASSERT_TRUE(dsd.parse());
	
	EXPECT_EQ(dsd.numberOfChannels(), 2);
	EXPECT_EQ(dsd.frequency(), 2822400);
	EXPECT_EQ(dsd.bitsPerSample(), 1);
	
	EXPECT_FALSE(dsd.isLSB());
	EXPECT_TRUE(dsd.isMSB());
	EXPECT_EQ(dsd.channelBlockSize(), 4096);
	EXPECT_EQ(dsd.totalSamples(), (94080 * 8) / 2);
	
	// number of data blocks 
	EXPECT_EQ(dsf.numberOfBlocks(), 12);

	// dsf.data(int blockIdx, int channelIdx, QByteArray& arr)
	QByteArray arr;
	ASSERT_TRUE(dsd.data(0, 0, arr));
	EXPECT_EQ(arr.size(), 4096);
	EXPECT_EQ(memcmp(arr.data(), c_expectL, 8), 0);
	ASSERT_TRUE(dsd.data(0, 1, arr));
	EXPECT_EQ(arr.size(), 4096);
	EXPECT_EQ(memcmp(arr.data(), c_expectR, 8), 0);
	
	EXPECT_TRUE(dsd.data(22, 0, arr));
	EXPECT_EQ(arr.size(), 1984);
	
	EXPECT_FALSE(dsd.data(23, 0, arr));
}

//-------------------------------------------------------------------------------------------

TEST(DSDIFFFileReader, openAndParseDSDIFFFilePerBlock)
{
	const tubyte c_expectL[8] = { 0x96, 0x69, 0xA5, 0xAA, 0x6A, 0xA9, 0xCC, 0xCD };
	const tubyte c_expectR[8] = { 0x96, 0x69, 0xA5, 0xAA, 0x6A, 0xA9, 0xCC, 0xCD };
	
	QString inFilename = common::DiskOps::mergeName("engine/dsdomega/test/samples", "Test1.dff");
	QString fileName = test::UnitTestEnviroment::instance()->testFileName(inFilename);
	
	common::BIOStream ioFile(common::e_BIOStream_FileRead);
	ASSERT_TRUE(ioFile.open(fileName));
	
	engine::dsd::DSDIFFFileReader dsd(&ioFile);
	ASSERT_TRUE(dsd.parse());
	
	EXPECT_EQ(dsd.numberOfChannels(), 2);
	EXPECT_EQ(dsd.frequency(), 2822400);
	EXPECT_EQ(dsd.bitsPerSample(), 1);
	
	EXPECT_FALSE(dsd.isLSB());
	EXPECT_TRUE(dsd.isMSB());
	EXPECT_EQ(dsd.channelBlockSize(), 4096);
	EXPECT_EQ(dsd.totalSamples(), (94080 * 8) / 2);
	
	// number of data blocks 
	EXPECT_EQ(dsf.numberOfBlocks(), 12);

	// dsf.data(int blockIdx, int channelIdx, QByteArray& arr)
	QByteArray arr;
	ASSERT_TRUE(dsd.data(0, arr));
	EXPECT_EQ(arr.size(), 4096 * 2);
	const tuint8 *dsdData = reinterpret_cast<const tuint8 *>(arr.constData());
	EXPECT_EQ(memcmp(dsdData, c_expectL, 8), 0);
	EXPECT_EQ(memcmp(&dsdData[4096], c_expectR, 8), 0);
	
	EXPECT_TRUE(dsd.data(22, 0, arr));
	EXPECT_EQ(arr.size(), 1984);
	
	EXPECT_FALSE(dsd.data(23, 0, arr));
}

//-------------------------------------------------------------------------------------------
