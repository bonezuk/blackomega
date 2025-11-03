#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/XMLNodeParser.h"

using namespace omega::common;
using namespace testing;

//-------------------------------------------------------------------------------------------

class XMLNodeParserTest : public omega::common::XMLNodeParser
{
    public:
        MOCK_METHOD1(processNode,void(xmlNodePtr pNode));
        MOCK_CONST_METHOD1(isRootNode,bool(xmlNodePtr pNode));
        MOCK_CONST_METHOD1(isElementNode,bool(xmlNodePtr pNode));
        MOCK_CONST_METHOD1(getNameOfNode,QString(xmlNodePtr pNode));

        bool testIsSpecifiedNode(xmlNodePtr pNode,const tchar *name) const;
};

//-------------------------------------------------------------------------------------------

bool XMLNodeParserTest::testIsSpecifiedNode(xmlNodePtr pNode,const tchar *name) const
{
    return isSpecifiedNode(pNode,name);
}

//-------------------------------------------------------------------------------------------

TEST(XMLNodeParser,isSpecifiedNodeNotElement)
{
    xmlNode parentNode;
    ::memset(&parentNode,0,sizeof(xmlNode));

    XMLNodeParserTest p;
    EXPECT_CALL(p,isElementNode(&parentNode)).Times(1).WillOnce(Return(false));

    EXPECT_FALSE(p.testIsSpecifiedNode(&parentNode,"Name"));
}

//-------------------------------------------------------------------------------------------

TEST(XMLNodeParser,isSpecifiedNodeNotGivenName)
{
    xmlNode parentNode;
    ::memset(&parentNode,0,sizeof(xmlNode));

    XMLNodeParserTest p;
    EXPECT_CALL(p,isElementNode(&parentNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(p,getNameOfNode(&parentNode)).Times(1).WillOnce(Return("NotName"));

    EXPECT_FALSE(p.testIsSpecifiedNode(&parentNode,"Name"));
}

//-------------------------------------------------------------------------------------------

TEST(XMLNodeParser,isSpecifiedNodeHasGivenName)
{
    xmlNode parentNode;
    ::memset(&parentNode,0,sizeof(xmlNode));

    XMLNodeParserTest p;
    EXPECT_CALL(p,isElementNode(&parentNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(p,getNameOfNode(&parentNode)).Times(1).WillOnce(Return("Name"));

    EXPECT_TRUE(p.testIsSpecifiedNode(&parentNode,"Name"));
}

//-------------------------------------------------------------------------------------------

TEST(XMLNodeParser,parseWhenNotRoot)
{
    xmlNode parentNode;
    ::memset(&parentNode,0,sizeof(xmlNode));

    XMLNodeParserTest p;
    EXPECT_CALL(p,isRootNode(&parentNode)).Times(1).WillOnce(Return(false));

    p.parse(&parentNode);
}

//-------------------------------------------------------------------------------------------

TEST(XMLNodeParser,parseWhenNoChildren)
{
    xmlNode parentNode;
    ::memset(&parentNode,0,sizeof(xmlNode));

    XMLNodeParserTest p;
    EXPECT_CALL(p,isRootNode(&parentNode)).Times(1).WillOnce(Return(true));

    p.parse(&parentNode);
}

//-------------------------------------------------------------------------------------------

TEST(XMLNodeParser,parseWhenOneChild)
{
    xmlNode parentNode,childNode1;
    ::memset(&parentNode,0,sizeof(xmlNode));
    ::memset(&childNode1,0,sizeof(xmlNode));
    parentNode.children = &childNode1;

    XMLNodeParserTest p;
    EXPECT_CALL(p,isRootNode(&parentNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(p,isElementNode(&childNode1)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(p,processNode(&childNode1)).Times(1);

    p.parse(&parentNode);
}

//-------------------------------------------------------------------------------------------

TEST(XMLNodeParser,parseWhenThreeChildren)
{
    xmlNode parentNode,childNode1,childNode2,childNode3;
    ::memset(&parentNode,0,sizeof(xmlNode));
    ::memset(&childNode1,0,sizeof(xmlNode));
    ::memset(&childNode2,0,sizeof(xmlNode));
    ::memset(&childNode3,0,sizeof(xmlNode));
    parentNode.children = &childNode1;
    childNode1.next = &childNode2;
    childNode2.next = &childNode3;

    XMLNodeParserTest p;
    EXPECT_CALL(p,isRootNode(&parentNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(p,isElementNode(&childNode1)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(p,isElementNode(&childNode2)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(p,isElementNode(&childNode3)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(p,processNode(&childNode1)).Times(1);
    EXPECT_CALL(p,processNode(&childNode2)).Times(1);
    EXPECT_CALL(p,processNode(&childNode3)).Times(1);

    p.parse(&parentNode);
}

//-------------------------------------------------------------------------------------------
