#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/DiskLayerIF.h"
#include "common/test/UPnPProviderTestEnviroment.h"

using namespace omega::common;
using namespace testing;

//-------------------------------------------------------------------------------------------

class DiskLayerIFTest : public DiskLayerIF
{
	public:
		DiskLayerIFTest();
		QString testDirectoryName(const QString& name) const;
};

//-------------------------------------------------------------------------------------------

DiskLayerIFTest::DiskLayerIFTest()
{}

//-------------------------------------------------------------------------------------------

QString DiskLayerIFTest::testDirectoryName(const QString& name) const
{
	return directoryName(name);
}

//-------------------------------------------------------------------------------------------

TEST(DiskLayerIF,directoryNameNoName)
{
	DiskLayerIFTest disk;
	QString name;
	EXPECT_TRUE(disk.testDirectoryName(name)=="");
}

//-------------------------------------------------------------------------------------------

TEST(DiskLayerIF,directoryNamePath)
{
	DiskLayerIFTest disk;
	EXPECT_TRUE(disk.testDirectoryName("/usr")==DiskOps::toNativeSeparators("/usr"));
	EXPECT_TRUE(disk.testDirectoryName("/usr/")==DiskOps::toNativeSeparators("/usr"));
	EXPECT_TRUE(disk.testDirectoryName("/usr\\")==DiskOps::toNativeSeparators("/usr"));
}

//-------------------------------------------------------------------------------------------

TEST(DiskLayerIF,isDirectoryImplementation)
{
	test::UPnPProviderTestEnviroment *env = test::UPnPProviderTestEnviroment::instance();
	DiskIFSPtr diskIF = DiskIF::instance("disk");
	QString fileNameA = env->root(1) + "\\images";
	EXPECT_TRUE(diskIF->isDirectory(fileNameA));
	EXPECT_FALSE(diskIF->isFile(fileNameA));
	QString fileNameB = env->root(1) + "\\notadir";
	EXPECT_FALSE(diskIF->isDirectory(fileNameB));
	QString fileNameC = env->root(1) + "\\images\\";
	EXPECT_TRUE(diskIF->isDirectory(fileNameC));
	EXPECT_FALSE(diskIF->isFile(fileNameC));
	QString fileNameD = env->root(1) + "\\images/";
	EXPECT_TRUE(diskIF->isDirectory(fileNameD));
	EXPECT_FALSE(diskIF->isFile(fileNameD));
	DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(DiskLayerIF,isFileImplementation)
{
	test::UPnPProviderTestEnviroment *env = test::UPnPProviderTestEnviroment::instance();
	DiskIFSPtr diskIF = DiskIF::instance("disk");
	QString fileNameA = env->root(1) + "\\lifetimeSCPD.xml";
	EXPECT_TRUE(diskIF->isFile(fileNameA));
	EXPECT_FALSE(diskIF->isDirectory(fileNameA));
	QString fileNameB = env->root(1) + "\\notafile.xml";
	EXPECT_FALSE(diskIF->isFile(fileNameB));
	DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(DiskLayerIF,openDirectoryWhenIsFileImplementation)
{
	test::UPnPProviderTestEnviroment *env = test::UPnPProviderTestEnviroment::instance();
	QString name = env->root(1) + "/simple.html";

	DiskIFSPtr diskIF = DiskIF::instance("disk");

	DiskIF::DirHandle h = diskIF->openDirectory(name);
	ASSERT_TRUE(h==DiskIF::invalidDirectory());

	DiskIF::release();	
}

//-------------------------------------------------------------------------------------------

TEST(DiskLayerIF,scanDirectoryImplementationGivenInvalid)
{
	test::UPnPProviderTestEnviroment *env = test::UPnPProviderTestEnviroment::instance();
	QString name = env->root(1) + "/not_a_file.html";

	DiskIFSPtr diskIF = DiskIF::instance("disk");

	DiskIF::DirHandle h = diskIF->openDirectory(name);
	ASSERT_TRUE(h==DiskIF::invalidDirectory());

	DiskIF::release();		
}

//-------------------------------------------------------------------------------------------

TEST(DiskLayerIF,scanDirectoryImplementationGivenEmptyDirectory)
{
	test::UPnPProviderTestEnviroment *env = test::UPnPProviderTestEnviroment::instance();
	QString rootDir = env->root(0);

	DiskIFSPtr diskIF = DiskIF::instance("disk");
	
	ASSERT_TRUE(diskIF->isDirectory(rootDir));

	DiskIF::DirHandle h = diskIF->openDirectory(rootDir);
	ASSERT_TRUE(h!=DiskIF::invalidDirectory());

	QString fName = diskIF->nextDirectoryEntry(h);
	EXPECT_TRUE(fName.isEmpty());
	
	diskIF->closeDirectory(h);

    DiskIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(DiskLayerIF,scanDirectoryImplementationGivenDirectory)
{
	test::UPnPProviderTestEnviroment *env = test::UPnPProviderTestEnviroment::instance();
	QString rootDir = env->root(1);

	DiskIFSPtr diskIF = DiskIF::instance("disk");
	
	ASSERT_TRUE(diskIF->isDirectory(rootDir));

	QString name;
	QSet<QString> dNameSet,fNameSet;
	QSet<QString>::iterator ppI;

	fNameSet.insert("lifetimeSCPD.xml");
	fNameSet.insert("simple.html");
	fNameSet.insert("simple_p1.html");
	fNameSet.insert("simple_p2.html");
	fNameSet.insert("supertoasterDDD.xml");
	fNameSet.insert("toastercontrolSCPD.xml");
	fNameSet.insert("toasterstatusSCPD.xml");
	dNameSet.insert("images");

	DiskIF::DirHandle h = diskIF->openDirectory(rootDir);
	ASSERT_TRUE(h!=DiskIF::invalidDirectory());

	while(name=diskIF->nextDirectoryEntry(h),!name.isEmpty())
	{
		QString fullName = rootDir + "/" + name;
		
		EXPECT_TRUE(diskIF->isFile(fullName) || diskIF->isDirectory(fullName));
		
		if(diskIF->isFile(fullName))
		{
			ppI = fNameSet.find(name);
			EXPECT_TRUE(ppI!=fNameSet.end());
			fNameSet.erase(ppI);
		}
		else if(diskIF->isDirectory(fullName))
		{
			ppI = dNameSet.find(name);
			EXPECT_TRUE(ppI!=dNameSet.end());
			dNameSet.erase(ppI);
		}
	}
	diskIF->closeDirectory(h);
	
	EXPECT_TRUE(fNameSet.isEmpty());
	EXPECT_TRUE(dNameSet.isEmpty());
	
	DiskIF::release();
}

//-------------------------------------------------------------------------------------------
