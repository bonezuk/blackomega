#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/JaroWinklerDistance.h"

using namespace omega::common;
using namespace testing;

//-------------------------------------------------------------------------------------------

class JaroWinklerDistanceTest : public JaroWinklerDistance
{
    public:
        JaroWinklerDistanceTest(const QString& s2);
        virtual void testBuildIndexMap(const QString& s2,QVector<QMultiHash<QChar,int> >& iMapList,int noEntries);
        virtual const QVector<QMultiHash<QChar,int> >& testGetIndexMap(const QString& s1,bool caseSensitive);
        virtual int testFindMatches(const QString& s,int *match,bool caseSensitive);
        
        static int testMinLength(int s1Len,int s2Len);
        static int testMaxLength(int s1Len,int s2Len);
        static int testWindow(int s1Len,int s2Len);
        static int testFindMatchesBothCaseSensitive(const QString& s1,const QString& s2,int *match);
        static int testFindMatchesBothCaseInsensitive(const QString& s1,const QString& s2,int *match);
        static int testTranspositionsCaseSensitive(const QString& s1,const QString& s2,int *match);
        static int testTranspositionsCaseInsensitive(const QString& s1,const QString& s2,int *match);
        static int testWinklerScalarCaseSensitive(const QString& s1,const QString& s2);
        static int testWinklerScalarCaseInsensitive(const QString& s1,const QString& s2);
        
        static tfloat64 testCalculateDistance(int m,int t,int l,int s1Len,int s2Len);
};

//-------------------------------------------------------------------------------------------

JaroWinklerDistanceTest::JaroWinklerDistanceTest(const QString& s2) : JaroWinklerDistance(s2)
{}

//-------------------------------------------------------------------------------------------

void JaroWinklerDistanceTest::testBuildIndexMap(const QString& s2,QVector<QMultiHash<QChar,int> >& iMapList,int noEntries)
{
    buildIndexMap(s2,iMapList,noEntries);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testFindMatches(const QString& s,int *match,bool caseSensitive)
{
    return findMatches(s,match,caseSensitive);
}

//-------------------------------------------------------------------------------------------

const QVector<QMultiHash<QChar,int> >& JaroWinklerDistanceTest::testGetIndexMap(const QString& s1,bool caseSensitive)
{
    return getIndexMap(s1,caseSensitive);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testMinLength(int s1Len,int s2Len)
{
    return minLength(s1Len,s2Len);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testMaxLength(int s1Len,int s2Len)
{
    return maxLength(s1Len,s2Len);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testWindow(int s1Len,int s2Len)
{
    return window(s1Len,s2Len);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(const QString& s1,const QString& s2,int *match)
{
    return findMatchesBothCaseSensitive(s1,s2,match);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(const QString& s1,const QString& s2,int *match)
{
    return findMatchesBothCaseInsensitive(s1,s2,match);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testTranspositionsCaseSensitive(const QString& s1,const QString& s2,int *match)
{
    return transpositionsCaseSensitive(s1,s2,match);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testTranspositionsCaseInsensitive(const QString& s1,const QString& s2,int *match)
{
    return transpositionsCaseInsensitive(s1,s2,match);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(const QString& s1,const QString& s2)
{
    return winklerScalarCaseSensitive(s1,s2);
}

//-------------------------------------------------------------------------------------------

int JaroWinklerDistanceTest::testWinklerScalarCaseInsensitive(const QString& s1,const QString& s2)
{
    return winklerScalarCaseInsensitive(s1,s2);
}

//-------------------------------------------------------------------------------------------

tfloat64 JaroWinklerDistanceTest::testCalculateDistance(int m,int t,int l,int s1Len,int s2Len)
{
    return calculateDistance(m,t,l,s1Len,s2Len);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceStaticExampleMarthaVsMarhta)
{
    QString s1 = "martha";
    QString s2 = "marhta";
    ASSERT_NEAR(JaroWinklerDistance::distance(s1,s2),0.961,0.001);
    ASSERT_NEAR(JaroWinklerDistance::distance(s2,s1),0.961,0.001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceStaticExampleMarthaVsMarhtaCaseInsensitive)
{
    QString s1 = "mArThA";
    QString s2 = "MarhTa";
    ASSERT_NEAR(JaroWinklerDistance::distance(s1,s2,false),0.961,0.001);
    ASSERT_NEAR(JaroWinklerDistance::distance(s2,s1,false),0.961,0.001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceExampleMarthaVsMarhta)
{
    QString s1 = "martha";
    QString s2 = "marhta";
    JaroWinklerDistance d(s2);
    ASSERT_NEAR(d.distance(s1),0.961,0.001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceExampleMarthaVsMarhtaCaseInsensitive)
{
    QString s1 = "mArThA";
    QString s2 = "MarhTa";
    JaroWinklerDistance d(s2);
    ASSERT_NEAR(d.distance(s1,false),0.961,0.001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceStaticExampleDwayneVsDuane)
{
    QString s1 = "dwayne";
    QString s2 = "duane";
    ASSERT_NEAR(JaroWinklerDistance::distance(s1,s2),0.84,0.001);
    ASSERT_NEAR(JaroWinklerDistance::distance(s2,s1),0.84,0.001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceStaticExampleDwayneVsDuaneCaseInsensitive)
{
    QString s1 = "DwAynE";
    QString s2 = "dUaNe";
    ASSERT_NEAR(JaroWinklerDistance::distance(s1,s2,false),0.84,0.001);
    ASSERT_NEAR(JaroWinklerDistance::distance(s2,s1,false),0.84,0.001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceExampleDwayneVsDuane)
{
    QString s1 = "dwayne";
    QString s2 = "duane";
    JaroWinklerDistance d(s2);
    ASSERT_NEAR(d.distance(s1),0.84,0.001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceExampleDwayneVsDuaneCaseInsensitive)
{
    QString s1 = "DwAynE";
    QString s2 = "dUaNe";
    JaroWinklerDistance d(s2);
    ASSERT_NEAR(d.distance(s1,false),0.84,0.001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceStaticGivenBothEmpty)
{
    QString s1 = "";
    QString s2 = "";
    ASSERT_DOUBLE_EQ(JaroWinklerDistance::distance(s1,s2),1.0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceStaticGivenFirstIsOnlyEmpty)
{
    QString s1 = "";
    QString s2 = "text";
    ASSERT_DOUBLE_EQ(JaroWinklerDistance::distance(s1,s2),0.0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceStaticGivenSecondIsOnlyEmpty)
{
    QString s1 = "text";
    QString s2 = "";
    ASSERT_DOUBLE_EQ(JaroWinklerDistance::distance(s1,s2),0.0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceGivenBothEmpty)
{
    QString s1 = "";
    QString s2 = "";
    JaroWinklerDistance d(s2);
    ASSERT_DOUBLE_EQ(d.distance(s1),1.0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceGivenFirstIsOnlyEmpty)
{
    QString s1 = "";
    QString s2 = "text";
    JaroWinklerDistance d(s2);
    ASSERT_DOUBLE_EQ(d.distance(s1),0.0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,distanceGivenSecondIsOnlyEmpty)
{
    QString s1 = "text";
    QString s2 = "";
    JaroWinklerDistance d(s2);
    ASSERT_DOUBLE_EQ(d.distance(s1),0.0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesEmptyString)
{
    QString s1("");
    QString s2("rtuxv");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==0);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesSingleCharStringAsInput)
{
    QString s1("t");
    QString s2("rtuxv");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringNoMatch)
{
    QString s1("names");
    QString s2("rtuxv");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==0);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasSingleMatchOnSame)
{
    QString s1("names");
    QString s2("rtmuv");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasSingleMatchBefore)
{
    QString s1("names");
    QString s2("rmutv");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasSingleMatchAfter)
{
    QString s1("names");
    QString s2("rtumv");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasSingleMatchesNotOnSame)
{
    QString s1("names");
    QString s2("anest");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==4);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 1);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasSingleMatchesOnSame)
{
    QString s1("aaeet");
    QString s2("anser");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==2);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasMultipleMatchesNotSame)
{
    QString s1("names");
    QString s2("aaaee");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==2);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasMultipleMatchesOnSame)
{
    QString s1("aaaeree");
    QString s2("baaeett");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [7];
    ::memset(match,0,7 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==4);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    EXPECT_TRUE((match[5] & 0x00000001) == 1);
    EXPECT_TRUE((match[6] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 2);
    EXPECT_TRUE((match[5] & 0x00000002) == 0);
    EXPECT_TRUE((match[6] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasSingleMatchesOnSameAndCaseSensitive)
{
    QString s1("aAEEt");
    QString s2("AneEr");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==2);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasSingleMatchesOnSameAndCaseInsensitive)
{
    QString s1("AAEEt");
    QString s2("anser");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,false)==2);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasMultipleMatchesOnSameAndCaseSensitive)
{
    QString s1("aaaErEE");
    QString s2("aaAtEEe");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [7];
    ::memset(match,0,7 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,true)==4);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    EXPECT_TRUE((match[5] & 0x00000001) == 1);
    EXPECT_TRUE((match[6] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 2);
    EXPECT_TRUE((match[5] & 0x00000002) == 2);
    EXPECT_TRUE((match[6] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesStringHasMultipleMatchesOnSameAndCaseInsensitive)
{
    QString s1("aaaErEE");
    QString s2("aaAtEEe");
    
    JaroWinklerDistanceTest d(s2);
    
    int *match = new int [7];
    ::memset(match,0,7 * sizeof(int));
    
    EXPECT_TRUE(d.testFindMatches(s1,match,false)==6);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    EXPECT_TRUE((match[5] & 0x00000001) == 1);
    EXPECT_TRUE((match[6] & 0x00000001) == 1);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 2);
    EXPECT_TRUE((match[5] & 0x00000002) == 2);
    EXPECT_TRUE((match[6] & 0x00000002) == 2);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,buildIndexMap)
{
    QString s2("teststrin");

    QVector<QMultiHash<QChar,int> > mapList;
    JaroWinklerDistanceTest jaro(s2);
    jaro.testBuildIndexMap(s2,mapList,7);
    
    EXPECT_TRUE(mapList.size()==7);
    
    QList<int> mPos;
    
    // 0 -     t-est (0,1,2,3)
    const QMultiHash<QChar,int>& mapA = mapList.at(0);
    EXPECT_TRUE(mapA.size()==4);
    EXPECT_TRUE(mapA.count(QChar('t'))==2);
    mPos = mapA.values(QChar('t'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==0);
    EXPECT_TRUE(mPos.at(1)==3);
    EXPECT_TRUE(mapA.count(QChar('e'))==1);
    EXPECT_TRUE(mapA.value(QChar('e'))==1);
    EXPECT_TRUE(mapA.count(QChar('s'))==1);
    EXPECT_TRUE(mapA.value(QChar('s'))==2);
    
    // 1 -   t-e-sts (0,1,2,3,4)
    const QMultiHash<QChar,int>& mapB = mapList.at(1);
    EXPECT_TRUE(mapB.size()==5);
    EXPECT_TRUE(mapB.count(QChar('t'))==2);
    mPos = mapB.values(QChar('t'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==0);
    EXPECT_TRUE(mPos.at(1)==3);
    EXPECT_TRUE(mapB.count(QChar('e'))==1);
    EXPECT_TRUE(mapB.value(QChar('e'))==1);
    EXPECT_TRUE(mapB.count(QChar('s'))==2);
    mPos = mapB.values(QChar('s'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==2);
    EXPECT_TRUE(mPos.at(1)==4);
    
    // 2 -  te-s-tst (0,1,2,3,4,5)
    const QMultiHash<QChar,int>& mapC = mapList.at(2);
    EXPECT_TRUE(mapC.size()==6);
    EXPECT_TRUE(mapC.count(QChar('t'))==3);
    mPos = mapC.values(QChar('t'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==0);
    EXPECT_TRUE(mPos.at(1)==3);
    EXPECT_TRUE(mPos.at(2)==5);
    EXPECT_TRUE(mapC.count(QChar('e'))==1);
    EXPECT_TRUE(mapC.value(QChar('e'))==1);
    EXPECT_TRUE(mapC.count(QChar('s'))==2);
    mPos = mapC.values(QChar('s'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==2);
    EXPECT_TRUE(mPos.at(1)==4);
    
    // 3 - tes-t-str (0,1,2,3,4,5,6)
    const QMultiHash<QChar,int>& mapD = mapList.at(3);
    EXPECT_TRUE(mapD.size()==7);
    EXPECT_TRUE(mapD.count(QChar('t'))==3);
    mPos = mapD.values(QChar('t'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==0);
    EXPECT_TRUE(mPos.at(1)==3);
    EXPECT_TRUE(mPos.at(2)==5);
    EXPECT_TRUE(mapD.count(QChar('e'))==1);
    EXPECT_TRUE(mapD.value(QChar('e'))==1);
    EXPECT_TRUE(mapD.count(QChar('s'))==2);
    mPos = mapD.values(QChar('s'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==2);
    EXPECT_TRUE(mPos.at(1)==4);
    EXPECT_TRUE(mapD.count(QChar('r'))==1);
    EXPECT_TRUE(mapD.value(QChar('r'))==6);
    
    // 4 - est-s-tri (1,2,3,4,5,6,7)
    const QMultiHash<QChar,int>& mapE = mapList.at(4);
    EXPECT_TRUE(mapE.size()==7);
    EXPECT_TRUE(mapE.count(QChar('t'))==2);
    mPos = mapE.values(QChar('t'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==3);
    EXPECT_TRUE(mPos.at(1)==5);
    EXPECT_TRUE(mapE.count(QChar('e'))==1);
    EXPECT_TRUE(mapE.value(QChar('e'))==1);
    EXPECT_TRUE(mapE.count(QChar('s'))==2);
    mPos = mapE.values(QChar('s'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==2);
    EXPECT_TRUE(mPos.at(1)==4);
    EXPECT_TRUE(mapE.count(QChar('r'))==1);
    EXPECT_TRUE(mapE.value(QChar('r'))==6);
    EXPECT_TRUE(mapE.count(QChar('i'))==1);
    EXPECT_TRUE(mapE.value(QChar('i'))==7);
    
    // 5 - sts-t-rin (2,3,4,5,6,7,8)
    const QMultiHash<QChar,int>& mapF = mapList.at(5);
    EXPECT_TRUE(mapF.size()==7);
    EXPECT_TRUE(mapF.count(QChar('t'))==2);
    mPos = mapF.values(QChar('t'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==3);
    EXPECT_TRUE(mPos.at(1)==5);
    EXPECT_TRUE(mapF.count(QChar('s'))==2);
    mPos = mapF.values(QChar('s'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==2);
    EXPECT_TRUE(mPos.at(1)==4);
    EXPECT_TRUE(mapF.count(QChar('r'))==1);
    EXPECT_TRUE(mapF.value(QChar('r'))==6);
    EXPECT_TRUE(mapF.count(QChar('i'))==1);
    EXPECT_TRUE(mapF.value(QChar('i'))==7);
    EXPECT_TRUE(mapF.count(QChar('n'))==1);
    EXPECT_TRUE(mapF.value(QChar('n'))==8);
    
    // 6 - tst-r-in (3,4,5,6,7,8,--)
    const QMultiHash<QChar,int>& mapG = mapList.at(6);
    EXPECT_TRUE(mapG.size()==6);
    EXPECT_TRUE(mapG.count(QChar('t'))==2);
    mPos = mapG.values(QChar('t'));
    std::sort(mPos.begin(),mPos.end());
    EXPECT_TRUE(mPos.at(0)==3);
    EXPECT_TRUE(mPos.at(1)==5);
    EXPECT_TRUE(mapG.count(QChar('s'))==1);
    EXPECT_TRUE(mapG.value(QChar('s'))==4);
    EXPECT_TRUE(mapG.count(QChar('r'))==1);
    EXPECT_TRUE(mapG.value(QChar('r'))==6);
    EXPECT_TRUE(mapG.count(QChar('i'))==1);
    EXPECT_TRUE(mapG.value(QChar('i'))==7);
    EXPECT_TRUE(mapG.count(QChar('n'))==1);
    EXPECT_TRUE(mapG.value(QChar('n'))==8);
}

//-------------------------------------------------------------------------------------------

class JaroWinklerDistanceGetIndexMapTest : public JaroWinklerDistanceTest
{
    public:
        JaroWinklerDistanceGetIndexMapTest(const QString& s2);
        MOCK_CONST_METHOD1(getComparisonString,const QString& (bool caseSensitive));
        MOCK_METHOD1(getLookupIndexMap,QMap<int,QVector<QMultiHash<QChar,int> > >& (bool caseSensitive));
    protected:
        virtual void buildIndexMap(const QString& s2,QVector<QMultiHash<QChar,int> >& iMapList,int noEntries);
};

//-------------------------------------------------------------------------------------------

JaroWinklerDistanceGetIndexMapTest::JaroWinklerDistanceGetIndexMapTest(const QString& s2) : JaroWinklerDistanceTest(s2)
{}

//-------------------------------------------------------------------------------------------

void JaroWinklerDistanceGetIndexMapTest::buildIndexMap(const QString& s2,QVector<QMultiHash<QChar,int> >& iMapList,int noEntries)
{
    QMultiHash<QChar,int> map;
    map.insert(QChar('a'),1);
    iMapList.append(map);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,getIndexMapGivenIndexOfLengthNotBuiltCaseSensitive)
{
    QString s2("String"),s1("abc");
    
    QMap<int,QVector<QMultiHash<QChar,int> > > lookupMap;
    
    JaroWinklerDistanceGetIndexMapTest jaro(s2);
    EXPECT_CALL(jaro,getComparisonString(true)).Times(1).WillOnce(ReturnRef(s2));
    EXPECT_CALL(jaro,getLookupIndexMap(true)).WillRepeatedly(ReturnRef(lookupMap));
    
    const QVector<QMultiHash<QChar,int> >& indexMap = jaro.testGetIndexMap(s1,true);
    EXPECT_TRUE(indexMap.size()==1);
    EXPECT_TRUE(indexMap.at(0).size()==1);
    EXPECT_TRUE(indexMap.at(0).value(QChar('a'))==1);

    EXPECT_TRUE(lookupMap.size()==1);
    EXPECT_TRUE(lookupMap.find(3).value().size()==1);
    EXPECT_TRUE(lookupMap.find(3).value().at(0).size()==1);
    EXPECT_TRUE(lookupMap.find(3).value().at(0).value(QChar('a'))==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,getIndexMapGivenIndexOfLengthBuiltCaseSensitive)
{
    QString s2("String"),s1("abc");
    
    QMultiHash<QChar,int> dict;
    dict.insert(QChar('b'),2);
    QVector<QMultiHash<QChar,int> > dictList;
    dictList.append(dict);
    QMap<int,QVector<QMultiHash<QChar,int> > > lookupMap;
    lookupMap.insert(3,dictList);
    
    JaroWinklerDistanceGetIndexMapTest jaro(s2);
    EXPECT_CALL(jaro,getLookupIndexMap(true)).WillRepeatedly(ReturnRef(lookupMap));
    
    const QVector<QMultiHash<QChar,int> >& indexMap = jaro.testGetIndexMap(s1,true);
    EXPECT_TRUE(indexMap.size()==1);
    EXPECT_TRUE(indexMap.at(0).size()==1);
    EXPECT_TRUE(indexMap.at(0).value(QChar('b'))==2);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,getIndexMapGivenIndexOfLengthNotBuiltCaseInsensitive)
{
    QString s2("string"),s1("abc");
    
    QMap<int,QVector<QMultiHash<QChar,int> > > lookupMap;
    
    JaroWinklerDistanceGetIndexMapTest jaro(s2);
    EXPECT_CALL(jaro,getComparisonString(false)).Times(1).WillOnce(ReturnRef(s2));
    EXPECT_CALL(jaro,getLookupIndexMap(false)).WillRepeatedly(ReturnRef(lookupMap));
    
    const QVector<QMultiHash<QChar,int> >& indexMap = jaro.testGetIndexMap(s1,false);
    EXPECT_TRUE(indexMap.size()==1);
    EXPECT_TRUE(indexMap.at(0).size()==1);
    EXPECT_TRUE(indexMap.at(0).value(QChar('a'))==1);

    EXPECT_TRUE(lookupMap.size()==1);
    EXPECT_TRUE(lookupMap.find(3).value().size()==1);
    EXPECT_TRUE(lookupMap.find(3).value().at(0).size()==1);
    EXPECT_TRUE(lookupMap.find(3).value().at(0).value(QChar('a'))==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,getIndexMapGivenIndexOfLengthBuiltCaseInsensitive)
{
    QString s2("String"),s1("abc");
    
    QMultiHash<QChar,int> dict;
    dict.insert(QChar('b'),2);
    QVector<QMultiHash<QChar,int> > dictList;
    dictList.append(dict);
    QMap<int,QVector<QMultiHash<QChar,int> > > lookupMap;
    lookupMap.insert(3,dictList);
    
    JaroWinklerDistanceGetIndexMapTest jaro(s2);
    EXPECT_CALL(jaro,getLookupIndexMap(false)).WillRepeatedly(ReturnRef(lookupMap));
    
    const QVector<QMultiHash<QChar,int> >& indexMap = jaro.testGetIndexMap(s1,false);
    EXPECT_TRUE(indexMap.size()==1);
    EXPECT_TRUE(indexMap.at(0).size()==1);
    EXPECT_TRUE(indexMap.at(0).value(QChar('b'))==2);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,minLengthBothEqual)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testMinLength(5,5)==5);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,minLengthFirstIsLeast)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testMinLength(5,6)==5);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,minLengthLastIsLeast)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testMinLength(6,5)==5);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,maxLengthBothEqual)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testMaxLength(5,5)==5);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,maxLengthFirstIsMost)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testMaxLength(5,4)==5);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,maxLengthLastIsMost)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testMaxLength(4,5)==5);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,windowOnEmpty)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(0,0)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,windowOneCharacter)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(1,0)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,windowSeveralCharacters)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(0,2)==0);
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(3,2)==0);
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(3,4)==1);
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(5,4)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,windowGivenBothEqualLength)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(8,8)==3);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,windowGivenFirstIsLonger)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(8,3)==3);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,windowGivenLastIsLonger)
{
    EXPECT_TRUE(JaroWinklerDistanceTest::testWindow(3,8)==3);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveEmptyString)
{
    QString s1("");
    QString s2("rtuxv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==0);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveSingleCharStringAsInput)
{
    QString s1("t");
    QString s2("rtuxv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveStringNoMatch)
{
    QString s1("names");
    QString s2("rtuxv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==0);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveStringHasSingleMatchOnSame)
{
    QString s1("names");
    QString s2("rtmuv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveStringHasSingleMatchBefore)
{
    QString s1("names");
    QString s2("rmutv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveStringHasSingleMatchAfter)
{
    QString s1("names");
    QString s2("rtumv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveStringHasSingleMatchesNotOnSame)
{
    QString s1("names");
    QString s2("anest");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==4);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 1);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveStringHasSingleMatchesOnSame)
{
    QString s1("aaeet");
    QString s2("anser");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==2);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveStringHasMultipleMatchesNotSame)
{
    QString s1("names");
    QString s2("aaaee");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==2);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveStringHasMultipleMatchesOnSame)
{
    QString s1("aaaeree");
    QString s2("baaeett");
    
    int *match = new int [7];
    ::memset(match,0,7 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==4);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    EXPECT_TRUE((match[5] & 0x00000001) == 1);
    EXPECT_TRUE((match[6] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 2);
    EXPECT_TRUE((match[5] & 0x00000002) == 0);
    EXPECT_TRUE((match[6] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseSensitiveWithDifferentCase)
{
    QString s1("aaaeree");
    QString s2("bAAEEtt");
    
    int *match = new int [7];
    ::memset(match,0,7 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseSensitive(s1,s2,match)==0);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    EXPECT_TRUE((match[5] & 0x00000001) == 0);
    EXPECT_TRUE((match[6] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    EXPECT_TRUE((match[5] & 0x00000002) == 0);
    EXPECT_TRUE((match[6] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveEmptyString)
{
    QString s1("");
    QString s2("rtuxv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==0);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveSingleCharStringAsInput)
{
    QString s1("t");
    QString s2("rTuxv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveStringNoMatch)
{
    QString s1("names");
    QString s2("RtUxV");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==0);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveStringHasSingleMatchOnSame)
{
    QString s1("names");
    QString s2("rtMuv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveStringHasSingleMatchBefore)
{
    QString s1("naMes");
    QString s2("rmutv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 0);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveStringHasSingleMatchAfter)
{
    QString s1("naMes");
    QString s2("rtuMv");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==1);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveStringHasSingleMatchesNotOnSame)
{
    QString s1("names");
    QString s2("ANEST");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==4);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 1);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveStringHasSingleMatchesOnSame)
{
    QString s1("aaeet");
    QString s2("AnSeR");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==2);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 0);
    EXPECT_TRUE((match[2] & 0x00000001) == 1);
    EXPECT_TRUE((match[3] & 0x00000001) == 0);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveStringHasMultipleMatchesNotSame)
{
    QString s1("names");
    QString s2("AAAEE");
    
    int *match = new int [5];
    ::memset(match,0,5 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==2);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 0);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 2);
    EXPECT_TRUE((match[1] & 0x00000002) == 0);
    EXPECT_TRUE((match[2] & 0x00000002) == 0);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,findMatchesBothCaseInsensitiveStringHasMultipleMatchesOnSame)
{
    QString s1("aAaEreE");
    QString s2("BAAEETT");
    
    int *match = new int [7];
    ::memset(match,0,7 * sizeof(int));
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testFindMatchesBothCaseInsensitive(s1,s2,match)==4);
    
    EXPECT_TRUE((match[0] & 0x00000001) == 1);
    EXPECT_TRUE((match[1] & 0x00000001) == 1);
    EXPECT_TRUE((match[2] & 0x00000001) == 0);
    EXPECT_TRUE((match[3] & 0x00000001) == 1);
    EXPECT_TRUE((match[4] & 0x00000001) == 0);
    EXPECT_TRUE((match[5] & 0x00000001) == 1);
    EXPECT_TRUE((match[6] & 0x00000001) == 0);
    
    EXPECT_TRUE((match[0] & 0x00000002) == 0);
    EXPECT_TRUE((match[1] & 0x00000002) == 2);
    EXPECT_TRUE((match[2] & 0x00000002) == 2);
    EXPECT_TRUE((match[3] & 0x00000002) == 2);
    EXPECT_TRUE((match[4] & 0x00000002) == 2);
    EXPECT_TRUE((match[5] & 0x00000002) == 0);
    EXPECT_TRUE((match[6] & 0x00000002) == 0);
    
    delete [] match;
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseSensitiveNoMatches)
{
    QString s1("abcd");
    QString s2("efgh");
    
    int match[4];
    match[0] = 0;
    match[1] = 0;
    match[2] = 0;
    match[3] = 0;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseSensitive(s1,s2,match)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseSensitiveGivenNoTranspositionsAndAllSame)
{
    QString s1("abcd");
    QString s2("abc");
    
    int match[4];
    match[0] = 3;
    match[1] = 3;
    match[2] = 3;
    match[3] = 0;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseSensitive(s1,s2,match)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseSensitiveGivenNoTranspositionsAndAllNotSame)
{
    QString s1("abcd");
    QString s2("acd");
    
    int match[4];
    match[0] = 3;
    match[1] = 2;
    match[2] = 3;
    match[3] = 1;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseSensitive(s1,s2,match)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseSensitiveGivenOneTransposition)
{
    QString s1("abc");
    QString s2("cb");
    
    int match[3];
    match[0] = 2;
    match[1] = 3;
    match[2] = 1;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseSensitive(s1,s2,match)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseSensitiveGivenCrossedNonMatchingTransposition)
{
    QString s1("abcdefghij");
    QString s2("bcdafghjei");
    
    int match[10];
    for(int i=0;i<10;i++)
    {
        match[i] = 3;
    }
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseSensitive(s1,s2,match)==5);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseSensitiveGivenMixedTransposition)
{
    QString s1("abcdefghij");
    QString s2("dkbcakke");
    
    int match[10];
    match[0] = 3;
    match[1] = 1;
    match[2] = 3;
    match[3] = 3;
    match[4] = 3;
    match[5] = 0;
    match[6] = 2;
    match[7] = 0;
    match[8] = 0;
    match[9] = 0;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseSensitive(s1,s2,match)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseSensitiveGivenMixedTranspositionAndCase)
{
    QString s1("abcdefghij");
    QString s2("dkBcakkE");
    
    int match[10];
    match[0] = 3;
    match[1] = 0;
    match[2] = 1;
    match[3] = 3;
    match[4] = 2;
    match[5] = 0;
    match[6] = 0;
    match[7] = 0;
    match[8] = 0;
    match[9] = 0;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseSensitive(s1,s2,match)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseInsensitiveNoMatches)
{
    QString s1("abcd");
    QString s2("EFGH");
    
    int match[4];
    match[0] = 0;
    match[1] = 0;
    match[2] = 0;
    match[3] = 0;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseInsensitive(s1,s2,match)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseInsensitiveGivenNoTranspositionsAndAllSame)
{
    QString s1("abcd");
    QString s2("AbC");
    
    int match[4];
    match[0] = 3;
    match[1] = 3;
    match[2] = 3;
    match[3] = 0;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseInsensitive(s1,s2,match)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseInsensitiveGivenNoTranspositionsAndAllNotSame)
{
    QString s1("abcd");
    QString s2("AcD");
    
    int match[4];
    match[0] = 3;
    match[1] = 2;
    match[2] = 3;
    match[3] = 1;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseInsensitive(s1,s2,match)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseInsensitiveGivenOneTransposition)
{
    QString s1("ABC");
    QString s2("cB");
    
    int match[3];
    match[0] = 2;
    match[1] = 3;
    match[2] = 1;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseInsensitive(s1,s2,match)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseInsensitiveGivenCrossedNonMatchingTransposition)
{
    QString s1("abcdefghij");
    QString s2("BcDaFgHjEi");
    
    int match[10];
    for(int i=0;i<10;i++)
    {
        match[i] = 3;
    }
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseInsensitive(s1,s2,match)==5);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,transpositionsCaseInsensitiveGivenMixedTransposition)
{
    QString s1("aBcDeFgHiJ");
    QString s2("dkbcakke");
    
    int match[10];
    match[0] = 3;
    match[1] = 1;
    match[2] = 3;
    match[3] = 3;
    match[4] = 3;
    match[5] = 0;
    match[6] = 2;
    match[7] = 0;
    match[8] = 0;
    match[9] = 0;
    
    EXPECT_TRUE(JaroWinklerDistanceTest::testTranspositionsCaseInsensitive(s1,s2,match)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveEmptyFirst)
{
    QString s1("");
    QString s2("abc");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveEmptySecond)
{
    QString s1("abc");
    QString s2("");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveSingleCharacterFirst)
{
    QString s1("a");
    QString s2("abcd");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveSingleCharacterSecond)
{
    QString s1("abcd");
    QString s2("a");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveNoMatch)
{
    QString s1("abcdefgh");
    QString s2("ijklmnop");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveFirstMatch)
{
    QString s1("ahijkl");
    QString s2("abcdefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveThreeMatch)
{
    QString s1("abcjkl");
    QString s2("abcdefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==3);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveFourMatch)
{
    QString s1("abcdefkl");
    QString s2("abcdefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==4);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveAllMatch)
{
    QString s1("abcdefg");
    QString s2("abcdefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==4);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseSensitiveAllMatchDifferentCase)
{
    QString s1("abCDefg");
    QString s2("abcdefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==2);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveEmptyFirst)
{
    QString s1("");
    QString s2("aBC");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveEmptySecond)
{
    QString s1("ABc");
    QString s2("");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseSensitive(s1,s2)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveSingleCharacterFirst)
{
    QString s1("a");
    QString s2("Abcd");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseInsensitive(s1,s2)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveSingleCharacterSecond)
{
    QString s1("abcd");
    QString s2("A");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseInsensitive(s1,s2)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveNoMatch)
{
    QString s1("ABCDEFGH");
    QString s2("ijklmnop");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseInsensitive(s1,s2)==0);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveFirstMatch)
{
    QString s1("AhIjKl");
    QString s2("abcdefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseInsensitive(s1,s2)==1);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveThreeMatch)
{
    QString s1("Abcjkl");
    QString s2("abCdefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseInsensitive(s1,s2)==3);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveFourMatch)
{
    QString s1("AbCdefkl");
    QString s2("aBcDefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseInsensitive(s1,s2)==4);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,winklerScalarCaseInsensitiveAllMatch)
{
    QString s1("abCDefg");
    QString s2("abCDefg");
    EXPECT_TRUE(JaroWinklerDistanceTest::testWinklerScalarCaseInsensitive(s1,s2)==4);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,calculateDistanceGivenNoMatches)
{
    int m = 0;
    int t = 0;
    int l = 0;
    int s1Len = 6;
    int s2Len = 8;
    ASSERT_NEAR(JaroWinklerDistanceTest::testCalculateDistance(m,t,l,s1Len,s2Len),0.0,0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,calculateDistanceJaroNoScalarAndTransposition)
{
    const tfloat64 cM = 5.0;
    const tfloat64 cT = 0.0;
    const tfloat64 cL = 0.0;
    const tfloat64 cS1Len = 6.0;
    const tfloat64 cS2Len = 8.0;

    int m = 5;
    int t = 0;
    int l = 0;
    int s1Len = 6;
    int s2Len = 8;
    
    tfloat64 dJ = ((cM / cS1Len) + (cM / cS2Len) + ((cM - cT) / cM)) / 3.0;
    tfloat64 dW = dJ + (cL * 0.1 * (1.0 - dJ));
    
    ASSERT_NEAR(JaroWinklerDistanceTest::testCalculateDistance(m,t,l,s1Len,s2Len),dW,0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,calculateDistanceJaroWithTranspositionNoScalar)
{
    const tfloat64 cM = 5.0;
    const tfloat64 cT = 2.0;
    const tfloat64 cL = 0.0;
    const tfloat64 cS1Len = 6.0;
    const tfloat64 cS2Len = 8.0;

    int m = 5;
    int t = 2;
    int l = 0;
    int s1Len = 6;
    int s2Len = 8;
    
    tfloat64 dJ = ((cM / cS1Len) + (cM / cS2Len) + ((cM - cT) / cM)) / 3.0;
    tfloat64 dW = dJ + (cL * 0.1 * (1.0 - dJ));
    
    ASSERT_NEAR(JaroWinklerDistanceTest::testCalculateDistance(m,t,l,s1Len,s2Len),dW,0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,calculateDistanceJaroWithScalarNoTransposition)
{
    const tfloat64 cM = 5.0;
    const tfloat64 cT = 0.0;
    const tfloat64 cL = 3.0;
    const tfloat64 cS1Len = 6.0;
    const tfloat64 cS2Len = 8.0;

    int m = 5;
    int t = 0;
    int l = 3;
    int s1Len = 6;
    int s2Len = 8;
    
    tfloat64 dJ = ((cM / cS1Len) + (cM / cS2Len) + ((cM - cT) / cM)) / 3.0;
    tfloat64 dW = dJ + (cL * 0.1 * (1.0 - dJ));
    
    ASSERT_NEAR(JaroWinklerDistanceTest::testCalculateDistance(m,t,l,s1Len,s2Len),dW,0.000001);
}

//-------------------------------------------------------------------------------------------

TEST(JaroWinklerDistance,calculateDistanceJaroWithScalarAndTransposition)
{
    const tfloat64 cM = 5.0;
    const tfloat64 cT = 2.0;
    const tfloat64 cL = 3.0;
    const tfloat64 cS1Len = 6.0;
    const tfloat64 cS2Len = 8.0;

    int m = 5;
    int t = 2;
    int l = 3;
    int s1Len = 6;
    int s2Len = 8;
    
    tfloat64 dJ = ((cM / cS1Len) + (cM / cS2Len) + ((cM - cT) / cM)) / 3.0;
    tfloat64 dW = dJ + (cL * 0.1 * (1.0 - dJ));
    
    ASSERT_NEAR(JaroWinklerDistanceTest::testCalculateDistance(m,t,l,s1Len,s2Len),dW,0.000001);
}

//-------------------------------------------------------------------------------------------
