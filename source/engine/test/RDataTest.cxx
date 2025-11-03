#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "engine/inc/RData.h"

using namespace omega;
using namespace engine;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenNoData)
{
    RData data(4,2,2);

    common::TimeStamp clipTime(1.5);
    data.clipToTime(clipTime);

    ASSERT_EQ(0,data.noParts());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenOnePartAndTimeBeforeData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    common::TimeStamp start1(2.0),end1(4.0);

    RData data(4,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    data.end() = end1;

    common::TimeStamp clipTime(1.5);

    data.clipToTime(clipTime);

    ASSERT_EQ(0,data.noParts());

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(4,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenOnePartAndTimeBisectsPart)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    common::TimeStamp start1(2.0),end1(4.0);

    RData data(4,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    data.end() = end1;

    common::TimeStamp clipTime(3.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(2,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(3.0,static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),4 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(3.0,static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(2,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenOnePartAndTimeAfterPart)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    common::TimeStamp start1(2.0),end1(4.0);

    RData data(4,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    data.end() = end1;

    common::TimeStamp clipTime(4.5);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(0,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreeContinuousPartsAndTimeBeforeData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 4.0), end2( 6.0);
    common::TimeStamp start3( 6.0), end3( 8.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(1.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(0,data.noParts());

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(12,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreeContinuousPartsAndTimeEqualToPartOneStart)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 4.0), end2( 6.0);
    common::TimeStamp start3( 6.0), end3( 8.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(2.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(0,data.noParts());

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(12,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreeContinuousPartsAndTimeBisectsPartOne)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 4.0), end2( 6.0);
    common::TimeStamp start3( 6.0), end3( 8.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(3.9);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(8,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreeContinuousPartsAndTimeEqualToPartOneEndAndPartTwoStart)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 4.0), end2( 6.0);
    common::TimeStamp start3( 6.0), end3( 8.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(4.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(8,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreeContinuousPartsAndTimeBisectsPartTwo)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 4.0), end2( 6.0);
    common::TimeStamp start3( 6.0), end3( 8.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(4.1);

    data.clipToTime(clipTime);

    ASSERT_EQ(2,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(1,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(4.5,static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),1 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(4.5,static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(7,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreeContinuousPartsAndTimeEqualToPartTwoEndAndPartThreeStart)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 4.0), end2( 6.0);
    common::TimeStamp start3( 6.0), end3( 8.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(6.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(2,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(4,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreeContinuousPartsAndTimeBisectsPartThree)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 4.0), end2( 6.0);
    common::TimeStamp start3( 6.0), end3( 8.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(7.2);

    data.clipToTime(clipTime);

    ASSERT_EQ(3,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),8 * sizeof(tfloat32))==0);

    RData::Part& testPart3 = data.part(2);
    EXPECT_EQ(3,testPart3.length());
    EXPECT_NEAR(static_cast<tfloat64>(start3),static_cast<tfloat64>(testPart3.start()),0.0000001);
    EXPECT_NEAR(7.5,static_cast<tfloat64>(testPart3.end()),0.0000001);
    EXPECT_TRUE(memcmp(part3Data,data.partData(2),6 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(7.5,static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(1,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreeContinuousPartsAndTimeAfterData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 4.0), end2( 6.0);
    common::TimeStamp start3( 6.0), end3( 8.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(14.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(3,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),8 * sizeof(tfloat32))==0);

    RData::Part& testPart3 = data.part(2);
    EXPECT_EQ(4,testPart3.length());
    EXPECT_NEAR(static_cast<tfloat64>(start3),static_cast<tfloat64>(testPart3.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end3),static_cast<tfloat64>(testPart3.end()),0.0000001);
    EXPECT_TRUE(memcmp(part3Data,data.partData(2),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end3),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(0,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeBeforeData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(1.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(0,data.noParts());

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(8,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeEqualToStartOfPartOne)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(2.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(0,data.noParts());

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(8,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeBisectsPartOne)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(2.1);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(1,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(2.5,static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),2 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(2.5,static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(7,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeEqualToEndOfPartOne)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(4.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(4,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeBisectsPeriodBetweenParts)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(4.5);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(4,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeEqualToStartOfPartTwo)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(4.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(4,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeBisectsPartTwo)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(6.9);

    data.clipToTime(clipTime);

    ASSERT_EQ(2,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(7.0,static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),6 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(7.0,static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(0,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeEqualToEndOfPartTwo)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(7.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(2,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(0,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenTwoSequentiallySeparatedPartsAndTimeAfterData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };

    common::TimeStamp start1( 2.0), end1( 4.0);
    common::TimeStamp start2( 5.0), end2( 7.0);

    RData data(8,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    data.end() = end2;

    common::TimeStamp clipTime(9.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(2,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(0,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreePartsWithNewTrackInLastTwoAndTimeBeforePartOneDataAndAfterPartTwoData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1(10.0), end1(12.0);
    common::TimeStamp start2( 2.0), end2( 4.0);
    common::TimeStamp start3( 5.0), end3( 7.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(9.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(2,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start3),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end3),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part3Data,data.partData(1),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end3),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(4,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreePartsWithNewTrackInLastTwoAndTimeBisectsPartOneData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1(10.0), end1(12.0);
    common::TimeStamp start2( 2.0), end2( 4.0);
    common::TimeStamp start3( 5.0), end3( 7.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(10.5);

    data.clipToTime(clipTime);

    ASSERT_EQ(3,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(1,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(10.5,static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),2 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),8 * sizeof(tfloat32))==0);

    RData::Part& testPart3 = data.part(2);
    EXPECT_EQ(4,testPart3.length());
    EXPECT_NEAR(static_cast<tfloat64>(start3),static_cast<tfloat64>(testPart3.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end3),static_cast<tfloat64>(testPart3.end()),0.0000001);
    EXPECT_TRUE(memcmp(part3Data,data.partData(2),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end3),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(3,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreePartsWithNewTrackInLastTwoAndTimeAfterPartOneDataAndAfterPartTwoData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1(10.0), end1(12.0);
    common::TimeStamp start2( 2.0), end2( 4.0);
    common::TimeStamp start3( 5.0), end3( 7.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(14.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(3,data.noParts());

    RData::Part& testPart1 = data.part(0);
    EXPECT_EQ(4,testPart1.length());
    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(testPart1.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end1),static_cast<tfloat64>(testPart1.end()),0.0000001);
    EXPECT_TRUE(memcmp(part1Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart2 = data.part(1);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(1),8 * sizeof(tfloat32))==0);

    RData::Part& testPart3 = data.part(2);
    EXPECT_EQ(4,testPart3.length());
    EXPECT_NEAR(static_cast<tfloat64>(start3),static_cast<tfloat64>(testPart3.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end3),static_cast<tfloat64>(testPart3.end()),0.0000001);
    EXPECT_TRUE(memcmp(part3Data,data.partData(2),8 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start1),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end3),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(0,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreePartsWithNewTrackInLastTwoAndTimeBisectsPartTwoData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1(10.0), end1(12.0);
    common::TimeStamp start2( 2.0), end2( 4.0);
    common::TimeStamp start3( 5.0), end3( 7.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(3.6);

    data.clipToTime(clipTime);

    ASSERT_EQ(1,data.noParts());

    RData::Part& testPart2 = data.part(0);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(4.0,static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(0),4 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(4.0),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(8,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData,clipToTimeGivenThreePartsWithNewTrackInLastTwoAndTimeBisectsPartThreeData)
{
    tfloat32 part1Data[8] = {  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f };
    tfloat32 part2Data[8] = {  9.0f, 10.0f, 11.0f, 12.0f, 13.0f, 14.0f, 15.0f, 16.0f };
    tfloat32 part3Data[8] = { 17.0f, 18.0f, 19.0f, 20.0f, 21.0f, 22.0f, 23.0f, 24.0f };

    common::TimeStamp start1(10.0), end1(12.0);
    common::TimeStamp start2( 2.0), end2( 4.0);
    common::TimeStamp start3( 5.0), end3( 7.0);

    RData data(12,2,2);

    data.start() = start1;

    RData::Part& part1 = data.nextPart();
    memcpy(data.partData(0),part1Data,8 * sizeof(tfloat32));
    part1.start() = start1;
    part1.end() = end1;
    part1.length() = 4;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    memcpy(data.partData(1),part2Data,8 * sizeof(tfloat32));
    part2.start() = start2;
    part2.end() = end2;
    part2.length() = 4;
    part2.done() = true;

    RData::Part& part3 = data.nextPart();
    memcpy(data.partData(2),part3Data,8 * sizeof(tfloat32));
    part3.start() = start3;
    part3.end() = end3;
    part3.length() = 4;
    part3.done() = true;

    data.end() = end3;

    common::TimeStamp clipTime(6.0);

    data.clipToTime(clipTime);

    ASSERT_EQ(2,data.noParts());

    RData::Part& testPart2 = data.part(0);
    EXPECT_EQ(4,testPart2.length());
    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(testPart2.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(end2),static_cast<tfloat64>(testPart2.end()),0.0000001);
    EXPECT_TRUE(memcmp(part2Data,data.partData(0),8 * sizeof(tfloat32))==0);

    RData::Part& testPart3 = data.part(1);
    EXPECT_EQ(2,testPart3.length());
    EXPECT_NEAR(static_cast<tfloat64>(start3),static_cast<tfloat64>(testPart3.start()),0.0000001);
    EXPECT_NEAR(6.0,static_cast<tfloat64>(testPart3.end()),0.0000001);
    EXPECT_TRUE(memcmp(part3Data,data.partData(1),4 * sizeof(tfloat32))==0);

    EXPECT_NEAR(static_cast<tfloat64>(start2),static_cast<tfloat64>(data.start()),0.0000001);
    EXPECT_NEAR(static_cast<tfloat64>(6.0),static_cast<tfloat64>(data.end()),0.0000001);

    EXPECT_EQ(6,data.rLength());
}

//-------------------------------------------------------------------------------------------

TEST(RData, centreChannelFromStereo)
{
    const sample_t c_tolerance = 0.0000001;
    sample_t partA[10] = { 0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
    sample_t partB[10] = { 0.9, 0.8, 0.7, 0.6, 0.5, 0.4, 0.3, 0.2, 0.1, 0.0 };

    sample_t expectA[10];
    for(tint i = 0; i < 5; i++)
    {
        expectA[i + 0] = (partA[(i * 2) + 0] + partA[(i * 2) + 1]) / 2.0;
        expectA[i + 5] = (partB[(i * 2) + 0] + partB[(i * 2) + 1]) / 2.0;
    }

    RData data(10, 2, 2);

    RData::Part& part1 = data.nextPart();
    sample_t *pA = data.partData(0);
    ::memcpy(pA, partA, 10 * sizeof(sample_t));
    part1.length() = 5;
    part1.done() = true;

    RData::Part& part2 = data.nextPart();
    sample_t *pB = data.partData(1);
    ::memcpy(pB, partB, 10 * sizeof(sample_t));
    part2.length() = 5;
    part2.done() = true;

    ASSERT_EQ(data.length(), 10);

    sample_t *actualC = data.center();
    ASSERT_TRUE(actualC != NULL);

    for(tint i = 0; i < 10; i++)
    {
        EXPECT_NEAR(actualC[i], expectA[i], c_tolerance);
    }

    sample_t *cpA = data.partDataCenter(0);
    for(tint i = 0; i < 5; i++)
    {
        EXPECT_NEAR(cpA[i], expectA[i], c_tolerance);
    }
    sample_t *cpB = data.partDataCenter(1);
    for(tint i = 0; i < 5; i++)
    {
        EXPECT_NEAR(cpB[i], expectA[i+5], c_tolerance);
    }

}

//-------------------------------------------------------------------------------------------
