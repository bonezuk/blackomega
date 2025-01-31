#include "gtest/gtest.h"

#include "engine/dsdomega/inc/DSFFileReader.h"
#include "track/model/test/TrackDBTestEnviroment.h"
#include "track/info/inc/ID3Info2.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(DSFFileReader,openAndParseDSFFilePerChannel)
{
	const tuint8 c_expectL[4] = {0xcd, 0xd2, 0xd2, 0x52};
	const tuint8 c_expectR[4] = {0xab, 0xca, 0xaa, 0x2c};
	QString fileName = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"testexample1.dsf");
	
	common::BIOStream ioFile(common::e_BIOStream_FileRead);
	ASSERT_TRUE(ioFile.open(fileName));
	
	engine::dsd::DSFFileReader dsf(&ioFile);
	ASSERT_TRUE(dsf.parse());
	
	EXPECT_EQ(dsf.numberOfChannels(), 2);
	EXPECT_EQ(dsf.frequency(), 2822400);
	EXPECT_EQ(dsf.bitsPerSample(), 1);
	EXPECT_TRUE(dsf.isLSB());
	EXPECT_FALSE(dsf.isMSB());
	EXPECT_EQ(dsf.channelBlockSize(), 4096);
	EXPECT_EQ(dsf.totalSamples(), 4096 * 8);
	
	// number of data blocks 
	EXPECT_EQ(dsf.numberOfBlocks(), 1);

	// dsf.data(int blockIdx, int channelIdx, QByteArray& arr)
	QByteArray arr;
	ASSERT_TRUE(dsf.data(0, 0, arr));
	EXPECT_EQ(arr.size(), 4096);
	EXPECT_EQ(memcmp(arr.data(), c_expectL, 4), 0);
	ASSERT_TRUE(dsf.data(0, 1, arr));
	EXPECT_EQ(arr.size(), 4096);
	EXPECT_EQ(memcmp(arr.data(), c_expectR, 4), 0);
	
	EXPECT_FALSE(dsf.data(1, 0, arr));
	
	EXPECT_TRUE(dsf.isMetadata());
	EXPECT_EQ(dsf.metaOffset(), 0x205c);
	EXPECT_TRUE(ioFile.seek64(dsf.metaOffset(), common::e_Seek_Start));
	
	track::info::ID3Info2 tag;
	ASSERT_TRUE(tag.read(&ioFile));
	EXPECT_TRUE(tag.title() == "Vision of Her");
	EXPECT_TRUE(tag.artist() == "David Elias");
}

//-------------------------------------------------------------------------------------------

TEST(DSFFileReader,openAndParseDSFFilePerBlock)
{
	const tuint8 c_expectL[4] = {0xcd, 0xd2, 0xd2, 0x52};
	const tuint8 c_expectR[4] = {0xab, 0xca, 0xaa, 0x2c};
	QString fileName = common::DiskOps::mergeName(track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"testexample1.dsf");
	
	common::BIOStream ioFile(common::e_BIOStream_FileRead);
	ASSERT_TRUE(ioFile.open(fileName));
	
	engine::dsd::DSFFileReader dsf(&ioFile);
	ASSERT_TRUE(dsf.parse());
	
	EXPECT_EQ(dsf.numberOfChannels(), 2);
	EXPECT_EQ(dsf.frequency(), 2822400);
	EXPECT_EQ(dsf.bitsPerSample(), 1);
	EXPECT_TRUE(dsf.isLSB());
	EXPECT_FALSE(dsf.isMSB());
	EXPECT_EQ(dsf.channelBlockSize(), 4096);
	EXPECT_EQ(dsf.totalSamples(), 4096 * 8);
	
	// number of data blocks 
	EXPECT_EQ(dsf.numberOfBlocks(), 1);

	// dsf.data(int blockIdx, int channelIdx, QByteArray& arr)
	QByteArray arr;
	ASSERT_TRUE(dsf.data(0, arr));
	EXPECT_EQ(arr.size(), 4096 * 2);
	const tuint8 *dsdData = reinterpret_cast<const tuint8 *>(arr.constData());
	EXPECT_EQ(memcmp(dsdData, c_expectL, 4), 0);
	EXPECT_EQ(memcmp(&dsdData[4096], c_expectR, 4), 0);
	
	ASSERT_TRUE(dsf.data(0, 1, arr));
	EXPECT_EQ(arr.size(), 4096);
	EXPECT_EQ(memcmp(arr.data(), c_expectR, 4), 0);
	
	EXPECT_FALSE(dsf.data(1, arr));
}

//-------------------------------------------------------------------------------------------
