#include "gtest/gtest.h"
#include "common/inc/DiskOps.h"

using namespace omega;

//-------------------------------------------------------------------------------------------

TEST(DiskOps,mergeNameNoName)
{
    EXPECT_TRUE(common::DiskOps::mergeName("","")=="");
    EXPECT_TRUE(common::DiskOps::mergeName(0,"")=="");
    EXPECT_TRUE(common::DiskOps::mergeName("",0)=="");
    EXPECT_TRUE(common::DiskOps::mergeName(0,0)=="");
}

//-------------------------------------------------------------------------------------------

TEST(DiskOps,mergeNameOnlyDir)
{
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp","")==common::DiskOps::toNativeSeparators("/usr/temp"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp/",0)==common::DiskOps::toNativeSeparators("/usr/temp"));
    EXPECT_TRUE(common::DiskOps::mergeName("\\usr\\temp","")==common::DiskOps::toNativeSeparators("/usr/temp"));
}

//-------------------------------------------------------------------------------------------

TEST(DiskOps,mergeNameOnlyFile)
{
    EXPECT_TRUE(common::DiskOps::mergeName("","simple.html")==common::DiskOps::toNativeSeparators("simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("","/simple.html")==common::DiskOps::toNativeSeparators("simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName(0,"\\simple.html")==common::DiskOps::toNativeSeparators("simple.html"));
}

//-------------------------------------------------------------------------------------------

TEST(DiskOps,mergNameBothDirAndFile)
{
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp","simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp/","simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp\\","simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp","/simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp","\\simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp///","\\\\simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp/","/simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp\\","\\simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/simple.html"));
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp/../","simple.html")==common::DiskOps::toNativeSeparators("/usr/temp/../simple.html"));
}

//-------------------------------------------------------------------------------------------

TEST(DiskOps,mergeNameVaritants)
{
    QString dName("/usr/temp");
    QString fName("simple.html");
    QString fullName("/usr/temp/simple.html");
    fullName = common::DiskOps::toNativeSeparators(fullName);
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp","simple.html")==fullName);
    EXPECT_TRUE(common::DiskOps::mergeName("/usr/temp",fName)==fullName);
    EXPECT_TRUE(common::DiskOps::mergeName(dName,"simple.html")==fullName);
    EXPECT_TRUE(common::DiskOps::mergeName(dName,fName)==fullName);    
}

//-------------------------------------------------------------------------------------------
