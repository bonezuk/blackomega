#include "gtest/gtest.h"

#include "engine/inc/FIRFilter.h"

using namespace omega::engine;

//-------------------------------------------------------------------------------------------

const sample_t *testSampleFIRFilterA()
{
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;

    static const sample_t sampleA2Ch[c_noOfTestSamples * c_noChannels] = {
         0.1,  0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,
         0.9,  0.8,  0.7,  0.6,  0.5,  0.4,  0.3,  0.2,  0.1,  0.0,
        -0.1, -0.2, -0.3, -0.4, -0.5, -0.6, -0.7, -0.8, -0.9, -1.0,
        -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1,  0.0
    };
    return sampleA2Ch;
}

//-------------------------------------------------------------------------------------------

const sample_t *testSampleFIRFilterB()
{
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;

    static const sample_t sampleB2Ch[c_noOfTestSamples * c_noChannels] = {
         0.9,  0.8,  0.7,  0.6,  0.5,  0.4,  0.3,  0.2,  0.1,  0.0,
        -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1,  0.0,
         0.1,  0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,
        -0.1, -0.2, -0.3, -0.4, -0.5, -0.6, -0.7, -0.8, -0.9, -1.0
    };
    return sampleB2Ch;
}

//-------------------------------------------------------------------------------------------

const sample_t *testSampleFIRFilterC()
{
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;

    static const sample_t sampleC2Ch[c_noOfTestSamples * c_noChannels] = {
        -0.1, -0.2, -0.3, -0.4, -0.5, -0.6, -0.7, -0.8, -0.9, -1.0
        -0.9, -0.8, -0.7, -0.6, -0.5, -0.4, -0.3, -0.2, -0.1,  0.0,
         0.1,  0.2,  0.3,  0.4,  0.5,  0.6,  0.7,  0.8,  0.9,  1.0,
         0.9,  0.8,  0.7,  0.6,  0.5,  0.4,  0.3,  0.2,  0.1,  0.0,
    };
    return sampleC2Ch;
}
//-------------------------------------------------------------------------------------------

void testActualToExpectedFIRFilter(RData& data, const sample_t *expect, tint filterIndex, tint noSamples, tint noChannels)
{
    const tfloat64 c_tolerance = 0.0000001;
    tint i, idx;

    sample_t *actual = data.filterData(filterIndex);
    ASSERT_TRUE(actual != NULL);
    for(i = 0; i < noSamples; i++)
    {
        idx = i * 2;
        EXPECT_NEAR(actual[idx + 0], expect[idx + 0], c_tolerance);
        EXPECT_NEAR(actual[idx + 1], expect[idx + 1], c_tolerance);
    }

    actual = data.partFilterData(0, filterIndex);
    ASSERT_TRUE(actual != NULL);
    for(i = 0; i < noSamples; i++)
    {
        idx = i * 2;
        EXPECT_NEAR(actual[idx + 0], expect[idx + 0], c_tolerance);
        EXPECT_NEAR(actual[idx + 1], expect[idx + 1], c_tolerance);
    }
}

//-------------------------------------------------------------------------------------------

void expectDataNoNextPart(const sample_t *sample, tint noSamples, tint offset, const sample_t *filterCooefs, tint noCooefs, sample_t *expect)
{
    tint i, j, idx;

    for(i = 0; i < noSamples; i++)
    {
        sample_t sL, sR, fL, fR;

        fL = 0.0;
        fR = 0.0;
        for(j = 0; j < noCooefs; j++)
        {
            idx = i + j - offset;
            sL = (idx >= 0 && idx < noSamples) ? sample[(idx * 2) + 0] : 0.0;
            sR = (idx >= 0 && idx < noSamples) ? sample[(idx * 2) + 1] : 0.0;
            fL += sL * filterCooefs[noCooefs - (j + 1)];
            fR += sR * filterCooefs[noCooefs - (j + 1)];
        }
        expect[(i * 2) + 0] = fL;
        expect[(i * 2) + 1] = fR;
    }
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterNoOffsetCompleteNoNextPart)
{
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 0;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};

    const sample_t *sampleA2Ch = testSampleFIRFilterA();
    sample_t expectA2Ch[c_noOfTestSamples * c_noChannels];

    expectDataNoNextPart(sampleA2Ch, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expectA2Ch);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& part1 = dataA.nextPart();
    ::memcpy(dataA.partData(0), sampleA2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    part1.length() = c_noOfTestSamples;
    part1.done() = true;

    FIRFilter filterL(c_filterCooefs, c_noCooefs);
    FIRFilter filterR(c_filterCooefs, c_noCooefs);

    ASSERT_EQ(filterL.offset(), c_offset);
    ASSERT_EQ(filterR.offset(), c_offset);

    filterL.process(&dataA, 0, c_filterIndex, true, false);
    filterR.process(&dataA, 1, c_filterIndex, true, false);

    testActualToExpectedFIRFilter(dataA, expectA2Ch, c_filterIndex, c_noOfTestSamples, c_noChannels);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterOffsetCompleteNoNextPart)
{
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 5;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};

    const sample_t *sampleA2Ch= testSampleFIRFilterA();
    sample_t expectA2Ch[c_noOfTestSamples * c_noChannels];

    expectDataNoNextPart(sampleA2Ch, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expectA2Ch);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& part1 = dataA.nextPart();
    ::memcpy(dataA.partData(0), sampleA2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    part1.length() = c_noOfTestSamples;
    part1.done() = true;

    FIRFilter filterL(c_filterCooefs, c_noCooefs);
    FIRFilter filterR(c_filterCooefs, c_noCooefs);

    filterL.setOffset(c_offset);
    filterR.setOffset(c_offset);

    ASSERT_EQ(filterL.offset(), c_offset);
    ASSERT_EQ(filterR.offset(), c_offset);

    filterL.process(&dataA, 0, c_filterIndex, true, false);
    filterR.process(&dataA, 1, c_filterIndex, true, false);

    testActualToExpectedFIRFilter(dataA, expectA2Ch, c_filterIndex, c_noOfTestSamples, c_noChannels);
}

//-------------------------------------------------------------------------------------------

void expectDataNoNextPartNoComplete(const sample_t * sample, tint noSamples, tint offset, const sample_t *filterCooefs, tint noCooefs, sample_t *expect)
{
    tint i, j, idx;

    for(i = 0; i < noSamples; i++)
    {
        sample_t sL, sR, fL, fR;

        fL = 0.0;
        fR = 0.0;
        for(j = 0; j < noCooefs; j++)
        {
            idx = i + j - offset;
            if(idx < noSamples)
            {
                sL = (idx >= 0 && idx < noSamples) ? sample[(idx * 2) + 0] : 0.0;
                sR = (idx >= 0 && idx < noSamples) ? sample[(idx * 2) + 1] : 0.0;
                fL += sL * filterCooefs[noCooefs - (j + 1)];
                fR += sR * filterCooefs[noCooefs - (j + 1)];
            }
            else
            {
                fL = 0.0;
                fR = 0.0;
                break;
            }
        }
        expect[(i * 2) + 0] = fL;
        expect[(i * 2) + 1] = fR;
    }
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterNoOffsetNoCompleteNoNextPart)
{
    const tfloat64 c_tolerance = 0.0000001;
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 0;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};

    const sample_t *sampleA2Ch = testSampleFIRFilterA();
    sample_t expectA2Ch[c_noOfTestSamples * c_noChannels];

    expectDataNoNextPartNoComplete(sampleA2Ch, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expectA2Ch);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& part1 = dataA.nextPart();
    ::memcpy(dataA.partData(0), sampleA2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    part1.length() = c_noOfTestSamples;
    part1.done() = true;

    FIRFilter filterL(c_filterCooefs, c_noCooefs);
    FIRFilter filterR(c_filterCooefs, c_noCooefs);

    ASSERT_EQ(filterL.offset(), c_offset);
    ASSERT_EQ(filterR.offset(), c_offset);

    filterL.process(&dataA, 0, c_filterIndex, false, false);
    filterR.process(&dataA, 1, c_filterIndex, false, false);

    testActualToExpectedFIRFilter(dataA, expectA2Ch, c_filterIndex, c_noOfTestSamples, c_noChannels);
}

//-------------------------------------------------------------------------------------------

void expectDataTwoDataComplete(const sample_t * sampleA, const sample_t * sampleB, tint noSamples, tint offset, const sample_t *filterCooefs, tint noCooefs, sample_t *expect)
{
    tint i, j, idx;

    for(i = 0; i < noSamples * 2; i++)
    {
        sample_t sL, sR, fL, fR;

        fL = 0.0;
        fR = 0.0;
        for(j = 0; j < noCooefs; j++)
        {
            idx = i + j - offset;
            if(idx >= 0 && idx < noSamples)
            {
                sL = sampleA[(idx * 2) + 0];
                sR = sampleA[(idx * 2) + 1];
                fL += sL * filterCooefs[noCooefs - (j + 1)];
                fR += sR * filterCooefs[noCooefs - (j + 1)];
            }
            else if(idx >= noSamples && idx < (noSamples * 2))
            {
                idx -= noSamples;
                sL = sampleB[(idx * 2) + 0];
                sR = sampleB[(idx * 2) + 1];
                fL += sL * filterCooefs[noCooefs - (j + 1)];
                fR += sR * filterCooefs[noCooefs - (j + 1)];
            }
        }
        expect[(i * 2) + 0] = fL;
        expect[(i * 2) + 1] = fR;
    }
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterNoOffsetTwoData)
{
    const tfloat64 c_tolerance = 0.0000001;
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 0;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};

    const sample_t *sampleA2Ch = testSampleFIRFilterA();
    const sample_t *sampleB2Ch = testSampleFIRFilterB();
    sample_t expect2Ch[c_noOfTestSamples * c_noChannels * 2];

    expectDataTwoDataComplete(sampleA2Ch, sampleB2Ch, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expect2Ch);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& partA = dataA.nextPart();
    memcpy(dataA.partData(0), sampleA2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partA.length() = c_noOfTestSamples;
    partA.done() = true;

    RData dataB(20, c_noChannels, c_noChannels);
    RData::Part& partB = dataB.nextPart();
    memcpy(dataB.partData(0), sampleB2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partB.length() = c_noOfTestSamples;
    partB.done() = true;

    FIRFilter filterL(c_filterCooefs, c_noCooefs);
    FIRFilter filterR(c_filterCooefs, c_noCooefs);

    filterL.process(&dataA, 0, c_filterIndex, false, false);
    filterR.process(&dataA, 1, c_filterIndex, false, false);

    filterL.process(&dataB, 0, c_filterIndex, true, false);
    filterR.process(&dataB, 1, c_filterIndex, true, false);

    testActualToExpectedFIRFilter(dataA, expect2Ch, c_filterIndex, c_noOfTestSamples, c_noChannels);
    testActualToExpectedFIRFilter(dataB, &expect2Ch[c_noOfTestSamples * c_noChannels], c_filterIndex, c_noOfTestSamples, c_noChannels);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterOffsetTwoData)
{
    const tfloat64 c_tolerance = 0.0000001;
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 5;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};

    const sample_t *sampleA2Ch = testSampleFIRFilterA();
    const sample_t *sampleB2Ch = testSampleFIRFilterB();
    sample_t expect2Ch[c_noOfTestSamples * c_noChannels * 2];

    expectDataTwoDataComplete(sampleA2Ch, sampleB2Ch, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expect2Ch);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& partA = dataA.nextPart();
    memcpy(dataA.partData(0), sampleA2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partA.length() = c_noOfTestSamples;
    partA.done() = true;

    RData dataB(20, c_noChannels, c_noChannels);
    RData::Part& partB = dataB.nextPart();
    memcpy(dataB.partData(0), sampleB2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partB.length() = c_noOfTestSamples;
    partB.done() = true;

    FIRFilter filterL(c_filterCooefs, c_noCooefs);
    FIRFilter filterR(c_filterCooefs, c_noCooefs);

    filterL.setOffset(c_offset);
    filterR.setOffset(c_offset);

    filterL.process(&dataA, 0, c_filterIndex, false, false);
    filterR.process(&dataA, 1, c_filterIndex, false, false);

    filterL.process(&dataB, 0, c_filterIndex, true, false);
    filterR.process(&dataB, 1, c_filterIndex, true, false);

    testActualToExpectedFIRFilter(dataA, expect2Ch, c_filterIndex, c_noOfTestSamples, c_noChannels);
    testActualToExpectedFIRFilter(dataB, &expect2Ch[c_noOfTestSamples * c_noChannels], c_filterIndex, c_noOfTestSamples, c_noChannels);
}

//-------------------------------------------------------------------------------------------

void expectDataThreeDataComplete(const sample_t *sampleA, const sample_t *sampleB, const sample_t *sampleC, tint noSamples, tint offset, const sample_t *filterCooefs, tint noCooefs, sample_t *expect)
{
    tint i, j, idx;

    for(i = 0; i < noSamples * 3; i++)
    {
        sample_t sL, sR, fL, fR;

        fL = 0.0;
        fR = 0.0;
        for(j = 0; j < noCooefs; j++)
        {
            idx = i + j - offset;
            if(idx >= 0 && idx < noSamples)
            {
                sL = sampleA[(idx * 2) + 0];
                sR = sampleA[(idx * 2) + 1];
                fL += sL * filterCooefs[noCooefs - (j + 1)];
                fR += sR * filterCooefs[noCooefs - (j + 1)];
            }
            else if(idx >= noSamples && idx < (noSamples * 2))
            {
                idx -= noSamples;
                sL = sampleB[(idx * 2) + 0];
                sR = sampleB[(idx * 2) + 1];
                fL += sL * filterCooefs[noCooefs - (j + 1)];
                fR += sR * filterCooefs[noCooefs - (j + 1)];
            }
            else if(idx >= (noSamples * 2) && idx < (noSamples * 3))
            {
                idx -= noSamples * 2;
                sL = sampleC[(idx * 2) + 0];
                sR = sampleC[(idx * 2) + 1];
                fL += sL * filterCooefs[noCooefs - (j + 1)];
                fR += sR * filterCooefs[noCooefs - (j + 1)];
            }
        }
        expect[(i * 2) + 0] = fL;
        expect[(i * 2) + 1] = fR;
    }
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterNoOffsetThreeData)
{
    const tfloat64 c_tolerance = 0.0000001;
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 0;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};

    const sample_t *sampleA2Ch = testSampleFIRFilterA();
    const sample_t *sampleB2Ch = testSampleFIRFilterB();
    const sample_t *sampleC2Ch = testSampleFIRFilterC();
    sample_t expect2Ch[c_noOfTestSamples * c_noChannels * 3];

    expectDataThreeDataComplete(sampleA2Ch, sampleB2Ch, sampleC2Ch, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expect2Ch);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& partA = dataA.nextPart();
    memcpy(dataA.partData(0), sampleA2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partA.length() = c_noOfTestSamples;
    partA.done() = true;

    RData dataB(20, c_noChannels, c_noChannels);
    RData::Part& partB = dataB.nextPart();
    memcpy(dataB.partData(0), sampleB2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partB.length() = c_noOfTestSamples;
    partB.done() = true;

    RData dataC(20, c_noChannels, c_noChannels);
    RData::Part& partC = dataC.nextPart();
    memcpy(dataC.partData(0), sampleC2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partC.length() = c_noOfTestSamples;
    partC.done() = true;

    FIRFilter filterL(c_filterCooefs, c_noCooefs);
    FIRFilter filterR(c_filterCooefs, c_noCooefs);

    filterL.process(&dataA, 0, c_filterIndex, false, false);
    filterR.process(&dataA, 1, c_filterIndex, false, false);

    filterL.process(&dataB, 0, c_filterIndex, false, false);
    filterR.process(&dataB, 1, c_filterIndex, false, false);

    filterL.process(&dataC, 0, c_filterIndex, true, false);
    filterR.process(&dataC, 1, c_filterIndex, true, false);

    testActualToExpectedFIRFilter(dataA, expect2Ch, c_filterIndex, c_noOfTestSamples, c_noChannels);
    testActualToExpectedFIRFilter(dataB, &expect2Ch[c_noOfTestSamples * c_noChannels], c_filterIndex, c_noOfTestSamples, c_noChannels);
    testActualToExpectedFIRFilter(dataC, &expect2Ch[c_noOfTestSamples * c_noChannels * 2], c_filterIndex, c_noOfTestSamples, c_noChannels);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterOffsetThreeData)
{
    const tfloat64 c_tolerance = 0.0000001;
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 4;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};

    const sample_t *sampleA2Ch = testSampleFIRFilterA();
    const sample_t *sampleB2Ch = testSampleFIRFilterB();
    const sample_t *sampleC2Ch = testSampleFIRFilterC();
    sample_t expect2Ch[c_noOfTestSamples * c_noChannels * 3];

    expectDataThreeDataComplete(sampleA2Ch, sampleB2Ch, sampleC2Ch, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expect2Ch);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& partA = dataA.nextPart();
    memcpy(dataA.partData(0), sampleA2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partA.length() = c_noOfTestSamples;
    partA.done() = true;

    RData dataB(20, c_noChannels, c_noChannels);
    RData::Part& partB = dataB.nextPart();
    memcpy(dataB.partData(0), sampleB2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partB.length() = c_noOfTestSamples;
    partB.done() = true;

    RData dataC(20, c_noChannels, c_noChannels);
    RData::Part& partC = dataC.nextPart();
    memcpy(dataC.partData(0), sampleC2Ch, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partC.length() = c_noOfTestSamples;
    partC.done() = true;

    FIRFilter filterL(c_filterCooefs, c_noCooefs);
    FIRFilter filterR(c_filterCooefs, c_noCooefs);

    filterL.setOffset(c_offset);
    filterR.setOffset(c_offset);

    filterL.process(&dataA, 0, c_filterIndex, false, false);
    filterR.process(&dataA, 1, c_filterIndex, false, false);

    filterL.process(&dataB, 0, c_filterIndex, false, false);
    filterR.process(&dataB, 1, c_filterIndex, false, false);

    filterL.process(&dataC, 0, c_filterIndex, true, false);
    filterR.process(&dataC, 1, c_filterIndex, true, false);

    testActualToExpectedFIRFilter(dataA, expect2Ch, c_filterIndex, c_noOfTestSamples, c_noChannels);
    testActualToExpectedFIRFilter(dataB, &expect2Ch[c_noOfTestSamples * c_noChannels], c_filterIndex, c_noOfTestSamples, c_noChannels);
    testActualToExpectedFIRFilter(dataC, &expect2Ch[c_noOfTestSamples * c_noChannels * 2], c_filterIndex, c_noOfTestSamples, c_noChannels);
}

//-------------------------------------------------------------------------------------------

void expectLFEDataThreeDataComplete(const sample_t *sampleA, const sample_t *sampleB, const sample_t *sampleC, tint noSamples, tint offset, const sample_t *filterCooefs, tint noCooefs, sample_t *expect)
{
    tint i, j, idx;

    for(i = 0; i < noSamples * 3; i++)
    {
        sample_t sC, fC;

        fC = 0.0;
        for(j = 0; j < noCooefs; j++)
        {
            idx = i + j - offset;
            if(idx >= 0 && idx < noSamples)
            {
                sC = (sampleA[(idx * 2) + 0] + sampleA[(idx * 2) + 1]) / 2.0;
                fC += sC * filterCooefs[noCooefs - (j + 1)];
            }
            else if(idx >= noSamples && idx < (noSamples * 2))
            {
                idx -= noSamples;
                sC = (sampleB[(idx * 2) + 0] + sampleB[(idx * 2) + 1]) / 2.0;
                fC += sC * filterCooefs[noCooefs - (j + 1)];
            }
            else if(idx >= (noSamples * 2) && idx < (noSamples * 3))
            {
                idx -= noSamples * 2;
                sC = (sampleC[(idx * 2) + 0] + sampleC[(idx * 2) + 1]) / 2.0;
                fC += sC * filterCooefs[noCooefs - (j + 1)];
            }
        }
        expect[i] = fC;
    }
}

//-------------------------------------------------------------------------------------------

void testActualLFEToExpectedFIRFilter(RData& data, const sample_t *expect, tint filterIndex, tint noSamples)
{
    const tfloat64 c_tolerance = 0.0000001;
    tint i;

    sample_t *actual = data.filterData(filterIndex);
    ASSERT_TRUE(actual != NULL);
    for(i = 0; i < noSamples; i++)
    {
        EXPECT_NEAR(actual[i], expect[i], c_tolerance);
    }

    actual = data.partFilterData(0, filterIndex);
    ASSERT_TRUE(actual != NULL);
    for(i = 0; i < noSamples; i++)
    {
        EXPECT_NEAR(actual[i], expect[i], c_tolerance);
    }
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterLFENoOffsetThreeData)
{
    const tfloat64 c_tolerance = 0.0000001;
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 0;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = {-0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389};

    const sample_t *sampleA = testSampleFIRFilterA();
    const sample_t *sampleB = testSampleFIRFilterB();
    const sample_t *sampleC = testSampleFIRFilterC();
    sample_t expect[c_noOfTestSamples * 3];

    expectLFEDataThreeDataComplete(sampleA, sampleB, sampleC, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expect);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& partA = dataA.nextPart();
    memcpy(dataA.partData(0), sampleA, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partA.length() = c_noOfTestSamples;
    partA.done() = true;

    RData dataB(20, c_noChannels, c_noChannels);
    RData::Part& partB = dataB.nextPart();
    memcpy(dataB.partData(0), sampleB, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partB.length() = c_noOfTestSamples;
    partB.done() = true;

    RData dataC(20, c_noChannels, c_noChannels);
    RData::Part& partC = dataC.nextPart();
    memcpy(dataC.partData(0), sampleC, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partC.length() = c_noOfTestSamples;
    partC.done() = true;

    FIRFilter filterLFE(c_filterCooefs, c_noCooefs);

    filterLFE.process(&dataA, e_lfeChannelIndex, false, false);
    filterLFE.process(&dataB, e_lfeChannelIndex, false, false);
    filterLFE.process(&dataC, e_lfeChannelIndex, true, false);

    testActualLFEToExpectedFIRFilter(dataA, expect, e_lfeChannelIndex, c_noOfTestSamples);
    testActualLFEToExpectedFIRFilter(dataB, &expect[c_noOfTestSamples], e_lfeChannelIndex, c_noOfTestSamples);
    testActualLFEToExpectedFIRFilter(dataC, &expect[c_noOfTestSamples * 2], e_lfeChannelIndex, c_noOfTestSamples);
}

//-------------------------------------------------------------------------------------------

TEST(FIRFilter,filterLFEOffsetThreeData)
{
    const tfloat64 c_tolerance = 0.0000001;
    const tint c_noOfTestSamples = 20;
    const tint c_noChannels = 2;
    const tint c_noCooefs = 6;
    const tint c_offset = 4;
    const tint c_filterIndex = 0;

    const sample_t c_filterCooefs[c_noCooefs] = { -0.19062389, -0.16668996,  0.61590277,  0.61590277, -0.16668996, -0.19062389 };

    const sample_t* sampleA = testSampleFIRFilterA();
    const sample_t* sampleB = testSampleFIRFilterB();
    const sample_t* sampleC = testSampleFIRFilterC();
    sample_t expect[c_noOfTestSamples * 3];

    expectLFEDataThreeDataComplete(sampleA, sampleB, sampleC, c_noOfTestSamples, c_offset, c_filterCooefs, c_noCooefs, expect);

    RData dataA(20, c_noChannels, c_noChannels);
    RData::Part& partA = dataA.nextPart();
    memcpy(dataA.partData(0), sampleA, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partA.length() = c_noOfTestSamples;
    partA.done() = true;

    RData dataB(20, c_noChannels, c_noChannels);
    RData::Part& partB = dataB.nextPart();
    memcpy(dataB.partData(0), sampleB, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partB.length() = c_noOfTestSamples;
    partB.done() = true;

    RData dataC(20, c_noChannels, c_noChannels);
    RData::Part& partC = dataC.nextPart();
    memcpy(dataC.partData(0), sampleC, sizeof(sample_t) * c_noOfTestSamples * c_noChannels);
    partC.length() = c_noOfTestSamples;
    partC.done() = true;

    FIRFilter filterLFE(c_filterCooefs, c_noCooefs);
    filterLFE.setOffset(c_offset);

    filterLFE.process(&dataA, e_lfeChannelIndex, false, false);
    filterLFE.process(&dataB, e_lfeChannelIndex, false, false);
    filterLFE.process(&dataC, e_lfeChannelIndex, true, false);

    testActualLFEToExpectedFIRFilter(dataA, expect, e_lfeChannelIndex, c_noOfTestSamples);
    testActualLFEToExpectedFIRFilter(dataB, &expect[c_noOfTestSamples], e_lfeChannelIndex, c_noOfTestSamples);
    testActualLFEToExpectedFIRFilter(dataC, &expect[c_noOfTestSamples * 2], e_lfeChannelIndex, c_noOfTestSamples);
}

//-------------------------------------------------------------------------------------------
