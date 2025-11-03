#include "common/inc/XMLReadOps.h"
#include "common/test/XMLLibMockIF.h"

using namespace omega::common;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfNodeWithNullPointer)
{
    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfNode(0).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfNodeWithNonTextPointer)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    node.type = XML_ELEMENT_NODE;

    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfNode(&node).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfNodeWhenNullTextIsGiven)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    node.type = XML_TEXT_NODE;

    EXPECT_CALL(pAPI,xmlNodeGetContent(&node)).Times(1).WillOnce(Return(reinterpret_cast<unsigned char *>(0)));

    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfNode(&node).isEmpty());

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfNodeWhenEmptyTextIsGiven)
{
    char txt[] = "";

    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    node.type = XML_TEXT_NODE;

    EXPECT_CALL(pAPI,xmlNodeGetContent(&node)).Times(1).WillOnce(Return(reinterpret_cast<unsigned char *>(txt)));
    EXPECT_CALL(pAPI,xmlFree(_)).Times(1);

    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfNode(&node).isEmpty());

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfNodeWhenTextIsGiven)
{
    char txt[] = "Expect String";

    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    node.type = XML_TEXT_NODE;

    EXPECT_CALL(pAPI,xmlNodeGetContent(&node)).Times(1).WillOnce(Return(reinterpret_cast<unsigned char *>(txt)));
    EXPECT_CALL(pAPI,xmlFree(_)).Times(1);

    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfNode(&node)=="Expect String");

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfElementWhenElementIsNull)
{
    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfElement(0).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfElementWhenElementIsNotAnElement)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    node.type = XML_TEXT_NODE;

    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfElement(&node).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfElementWhenElementHasNoChildren)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    node.type = XML_ELEMENT_NODE;

    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfElement(&node).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfElementWhenHasChildrenButNoText)
{
    xmlNode pNode;
    ::memset(&pNode,0,sizeof(xmlNode));
    pNode.type = XML_ELEMENT_NODE;

    xmlNode cNode1;
    ::memset(&cNode1,0,sizeof(xmlNode));
    cNode1.type = XML_ATTRIBUTE_NODE;

    xmlNode cNode2;
    ::memset(&cNode2,0,sizeof(xmlNode));
    cNode2.type = XML_ATTRIBUTE_NODE;

    cNode1.next = &cNode2;
    pNode.children = &cNode1;

    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfElement(&pNode).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getTextOfElementWhenElementHasTextChild)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode pNode;
    ::memset(&pNode,0,sizeof(xmlNode));
    pNode.type = XML_ELEMENT_NODE;

    xmlNode cNode1;
    ::memset(&cNode1,0,sizeof(xmlNode));
    cNode1.type = XML_ATTRIBUTE_NODE;

    xmlNode cNode2;
    ::memset(&cNode2,0,sizeof(xmlNode));
    cNode2.type = XML_TEXT_NODE;

    cNode1.next = &cNode2;
    pNode.children = &cNode1;

    char txt[] = "Expect String";

    EXPECT_CALL(pAPI,xmlNodeGetContent(&cNode2)).Times(1).WillOnce(Return(reinterpret_cast<unsigned char *>(txt)));
    EXPECT_CALL(pAPI,xmlFree(_)).Times(1);

    XMLReadOps ops;
    EXPECT_TRUE(ops.getTextOfElement(&pNode)=="Expect String");

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getNameOfNodeWhenNull)
{
    XMLReadOps ops;
    EXPECT_TRUE(ops.getNameOfNode(0).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getNameOfNodeWhenNoNameIsSpecifed)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    XMLReadOps ops;
    EXPECT_TRUE(ops.getNameOfNode(&node).isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getNameOfNodeWhenNameIsGiven)
{
    const char *txt = "Name";
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    node.name = reinterpret_cast<const xmlChar *>(txt);
    XMLReadOps ops;
    EXPECT_TRUE(ops.getNameOfNode(&node)=="Name");
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,isElementNode)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    XMLReadOps ops;
    EXPECT_FALSE(ops.isElementNode(0));
    node.type = XML_TEXT_NODE;
    EXPECT_FALSE(ops.isElementNode(&node));
    node.type = XML_ELEMENT_NODE;
    EXPECT_TRUE(ops.isElementNode(&node));
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,comparisonKey)
{
    XMLReadOps ops;
    EXPECT_TRUE(ops.comparisonKey("")=="");
    EXPECT_TRUE(ops.comparisonKey("key")=="key");
    EXPECT_TRUE(ops.comparisonKey("Key")=="key");
    EXPECT_TRUE(ops.comparisonKey("KEY")=="key");
    EXPECT_FALSE(ops.comparisonKey("Key")=="Key");
}

//-------------------------------------------------------------------------------------------

void invokeRemoveChildrenFromNodeXmlUnlinkNode(xmlNodePtr cur)
{
    xmlNodePtr pNode = cur->parent;
    pNode->children = cur->next;
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,removeChildrenFromNodeWithNoNode)
{
    XMLLibIF::instance("mock");

    XMLReadOps ops;
    ops.removeChildrenFromNode(0);

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,removeChildrenFromNodeWithNoChildren)
{
    XMLLibIF::instance("mock");

    xmlNode pNode;
    ::memset(&pNode,0,sizeof(xmlNode));

    pNode.type = XML_ELEMENT_NODE;

    XMLReadOps ops;
    ops.removeChildrenFromNode(&pNode);

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,removeChildrenFromNodeWithOneChild)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode pNode,cNodeA;
    ::memset(&pNode,0,sizeof(xmlNode));
    ::memset(&cNodeA,0,sizeof(xmlNode));

    pNode.children = &cNodeA;
    pNode.type = XML_ELEMENT_NODE;
    cNodeA.parent = &pNode;

    EXPECT_CALL(pAPI,xmlUnlinkNode(&cNodeA)).Times(1).WillOnce(Invoke(invokeRemoveChildrenFromNodeXmlUnlinkNode));
    EXPECT_CALL(pAPI,xmlFreeNode(&cNodeA)).Times(1);

    XMLReadOps ops;
    ops.removeChildrenFromNode(&pNode);

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,removeChildrenFromNodeWithThreeChildren)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode pNode,cNodeA,cNodeB,cNodeC;
    ::memset(&pNode,0,sizeof(xmlNode));
    ::memset(&cNodeA,0,sizeof(xmlNode));
    ::memset(&cNodeB,0,sizeof(xmlNode));
    ::memset(&cNodeC,0,sizeof(xmlNode));

    pNode.children = &cNodeA;
    pNode.type = XML_ELEMENT_NODE;
    cNodeA.next = &cNodeB;
    cNodeA.parent = &pNode;
    cNodeB.next = &cNodeC;
    cNodeB.parent = &pNode;
    cNodeC.parent = &pNode;

    EXPECT_CALL(pAPI,xmlUnlinkNode(&cNodeA)).Times(1).WillOnce(Invoke(invokeRemoveChildrenFromNodeXmlUnlinkNode));
    EXPECT_CALL(pAPI,xmlFreeNode(&cNodeA)).Times(1);
    EXPECT_CALL(pAPI,xmlUnlinkNode(&cNodeB)).Times(1).WillOnce(Invoke(invokeRemoveChildrenFromNodeXmlUnlinkNode));
    EXPECT_CALL(pAPI,xmlFreeNode(&cNodeB)).Times(1);
    EXPECT_CALL(pAPI,xmlUnlinkNode(&cNodeC)).Times(1).WillOnce(Invoke(invokeRemoveChildrenFromNodeXmlUnlinkNode));
    EXPECT_CALL(pAPI,xmlFreeNode(&cNodeC)).Times(1);

    XMLReadOps ops;
    ops.removeChildrenFromNode(&pNode);

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,removeChildrenFromNodeXMLLib)
{
    QString s;
    s  = "<?xml version=\"1.0\"?>";
    s += "<service>";
    s += "<serviceType>urn:schemas-upnp-org:service:tvcontrol:1</serviceType>";
    s += "<serviceId>urn:upnp-org:serviceId:tvcontrol1</serviceId>";
    s += "<controlURL>/upnp/control/tvcontrol1</controlURL>";
    s += "<eventSubURL>/upnp/event/tvcontrol1</eventSubURL>";
    s += "<SCPDURL>/tvcontrolSCPD.xml</SCPDURL>";
    s += "</service>";

    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");

    QByteArray inputArray = s.toUtf8().data();
    xmlDocPtr doc = ::xmlParseMemory(inputArray.data(),inputArray.size());
    ASSERT_TRUE(doc!=0);
    xmlNodePtr root = ::xmlDocGetRootElement(doc);
    ASSERT_TRUE(root!=0);

    XMLReadOps ops;
    ops.removeChildrenFromNode(root);

    EXPECT_TRUE(root->children==0);

    ::xmlFreeDoc(doc);
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

class XMLReadOpsReplaceTextInNodeMock : public XMLReadOps
{
    public:
        MOCK_CONST_METHOD1(removeChildrenFromNode,void(xmlNodePtr pNode));
};

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInNodeWhenNoNode)
{
    XMLLibIF::instance("mock");

    XMLReadOpsReplaceTextInNodeMock ops;
    ops.replaceTextInNode(0,QString::fromUtf8("Text"));

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInNodeWhenElementHasNoText)
{
    const tchar *str = "";

    XMLLibIF::instance("mock");

    xmlNode pNode;
    ::memset(&pNode,0,sizeof(xmlNode));
    pNode.type = XML_ELEMENT_NODE;

    XMLReadOpsReplaceTextInNodeMock ops;

    EXPECT_CALL(ops,removeChildrenFromNode(&pNode)).Times(1);

    ops.replaceTextInNode(&pNode,str);

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInNodeWhenElementHasText)
{
    const tchar *str = "Text";

    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode pNode;
    ::memset(&pNode,0,sizeof(xmlNode));
    pNode.type = XML_ELEMENT_NODE;

    XMLReadOpsReplaceTextInNodeMock ops;

    EXPECT_CALL(ops,removeChildrenFromNode(&pNode)).Times(1);

    xmlNode tNode;
    ::memset(&tNode,0,sizeof(xmlNode));

    EXPECT_CALL(pAPI,xmlNewText((const xmlChar *)(str))).Times(1).WillOnce(Return(&tNode));
    EXPECT_CALL(pAPI,xmlAddChild(&pNode,&tNode)).Times(1);

    ops.replaceTextInNode(&pNode,str);

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInNodeXMLLib)
{
    QString s;
    s  = "<?xml version=\"1.0\"?>";
    s += "<service>";
    s += "<serviceType>urn:schemas-upnp-org:service:tvcontrol:1</serviceType>";
    s += "<serviceId>urn:upnp-org:serviceId:tvcontrol1</serviceId>";
    s += "<controlURL>/upnp/control/tvcontrol1</controlURL>";
    s += "<eventSubURL>/upnp/event/tvcontrol1</eventSubURL>";
    s += "<SCPDURL>/tvcontrolSCPD.xml</SCPDURL>";
    s += "</service>";

    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");

    QByteArray inputArray = s.toUtf8().data();
    xmlDocPtr doc = ::xmlParseMemory(inputArray.data(),inputArray.size());
    ASSERT_TRUE(doc!=0);
    xmlNodePtr root = ::xmlDocGetRootElement(doc);
    ASSERT_TRUE(root!=0);

    XMLReadOps ops;
    ops.replaceTextInNode(root,QString::fromUtf8("Expected Text"));

    EXPECT_TRUE(ops.getTextOfElement(root)=="Expected Text");

    ::xmlFreeDoc(doc);
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

class XMLReadOpsReplaceTextInAllNodesMock : public XMLReadOps
{
    public:
        MOCK_CONST_METHOD1(getNameOfNode,QString(xmlNodePtr pNode));
        MOCK_CONST_METHOD2(replaceTextInNode,void(xmlNodePtr pNode,const QString& text));
        MOCK_CONST_METHOD1(removeChildrenFromNode,void(xmlNodePtr pNode));
        MOCK_CONST_METHOD1(isElementNode,bool(xmlNodePtr pNode));
};

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInAllNodesWhenNoNode)
{
    xmlNode aNode;
    ::memset(&aNode,0,sizeof(xmlNode));

    QString replaceNodeName = "Root";
    QString replaceText = "Replacement Text";

    XMLReadOpsReplaceTextInAllNodesMock ops;

    EXPECT_CALL(ops,isElementNode(&aNode)).Times(1).WillOnce(Return(false));

    ops.replaceTextInAllNodes(&aNode,replaceNodeName,replaceText);
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInAllNodesWhenNoNodeName)
{
    xmlNode aNode;
    ::memset(&aNode,0,sizeof(xmlNode));

    QString replaceNodeName = "";
    QString replaceText = "Replacement Text";

    XMLReadOpsReplaceTextInAllNodesMock ops;
    ops.replaceTextInAllNodes(&aNode,replaceNodeName,replaceText);
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInAllNodesWhenGivenNodeIsReplacementNode)
{
    xmlNode aNode;
    ::memset(&aNode,0,sizeof(xmlNode));

    QString replaceNodeName = "Root";
    QString replaceText = "Replacement Text";

    XMLReadOpsReplaceTextInAllNodesMock ops;

    EXPECT_CALL(ops,isElementNode(&aNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&aNode)).Times(1).WillOnce(Return(QString::fromUtf8("Root")));
    EXPECT_CALL(ops,replaceTextInNode(&aNode,replaceText)).Times(1);

    ops.replaceTextInAllNodes(&aNode,replaceNodeName,replaceText);
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInAllNodesWhenGivenNodeHasNoChildren)
{
    xmlNode aNode;
    ::memset(&aNode,0,sizeof(xmlNode));

    QString replaceNodeName = "ChildC";
    QString replaceText = "Replacement Text";

    XMLReadOpsReplaceTextInAllNodesMock ops;

    EXPECT_CALL(ops,isElementNode(&aNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&aNode)).Times(1).WillOnce(Return(QString::fromUtf8("Root")));

    ops.replaceTextInAllNodes(&aNode,replaceNodeName,replaceText);
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInAllNodesWhenGivenNodeHasSingularChildWithoutReplacementNode)
{
    xmlNode aNode,bNode1;
    ::memset(&aNode,0,sizeof(xmlNode));
    ::memset(&bNode1,0,sizeof(xmlNode));

    aNode.children = &bNode1;

    QString replaceNodeName = "ChildA";
    QString replaceText = "Replacement Text";

    XMLReadOpsReplaceTextInAllNodesMock ops;

    EXPECT_CALL(ops,isElementNode(&aNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&aNode)).Times(1).WillOnce(Return(QString::fromUtf8("Root")));

    EXPECT_CALL(ops,isElementNode(&bNode1)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&bNode1)).Times(1).WillOnce(Return(QString::fromUtf8("ChildA")));
    EXPECT_CALL(ops,replaceTextInNode(&bNode1,replaceText)).Times(1);

    ops.replaceTextInAllNodes(&aNode,replaceNodeName,replaceText);
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInAllNodesWhenGivenNodeHasSingularChildWithReplacementNode)
{
    xmlNode aNode,bNode1;
    ::memset(&aNode,0,sizeof(xmlNode));
    ::memset(&bNode1,0,sizeof(xmlNode));

    aNode.children = &bNode1;

    QString replaceNodeName = "ChildC";
    QString replaceText = "Replacement Text";

    XMLReadOpsReplaceTextInAllNodesMock ops;

    EXPECT_CALL(ops,isElementNode(&aNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&aNode)).Times(1).WillOnce(Return(QString::fromUtf8("Root")));

    EXPECT_CALL(ops,isElementNode(&bNode1)).Times(2).WillRepeatedly(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&bNode1)).Times(2).WillRepeatedly(Return(QString::fromUtf8("ChildA")));

    ops.replaceTextInAllNodes(&aNode,replaceNodeName,replaceText);
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInAllNodesWhenGivenNodeHasMultipleChildren)
{
    xmlNode aNode;
    ::memset(&aNode,0,sizeof(xmlNode));

    xmlNode bNode1,bNode2,bNode3;
    ::memset(&bNode1,0,sizeof(xmlNode));
    ::memset(&bNode2,0,sizeof(xmlNode));
    ::memset(&bNode3,0,sizeof(xmlNode));

    aNode.children = &bNode1;
    bNode1.next = &bNode2;
    bNode2.next = &bNode3;

    QString replaceNodeName = "ChildC";
    QString replaceText = "Replacement Text";

    XMLReadOpsReplaceTextInAllNodesMock ops;

    EXPECT_CALL(ops,isElementNode(&aNode)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&aNode)).Times(1).WillOnce(Return(QString::fromUtf8("Root")));

    EXPECT_CALL(ops,isElementNode(&bNode1)).Times(2).WillRepeatedly(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&bNode1)).Times(2).WillRepeatedly(Return(QString::fromUtf8("ChildA")));

    EXPECT_CALL(ops,isElementNode(&bNode2)).Times(2).WillRepeatedly(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&bNode2)).Times(2).WillRepeatedly(Return(QString::fromUtf8("ChildB")));

    EXPECT_CALL(ops,isElementNode(&bNode3)).Times(1).WillRepeatedly(Return(true));
    EXPECT_CALL(ops,getNameOfNode(&bNode3)).Times(1).WillRepeatedly(Return(QString::fromUtf8("ChildC")));
    EXPECT_CALL(ops,replaceTextInNode(&bNode3,replaceText)).Times(1);

    ops.replaceTextInAllNodes(&aNode,replaceNodeName,replaceText);
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,replaceTextInAllNodesLibXML)
{
    QString s;
    s  = "<?xml version=\"1.0\"?>";
    s += "<root>";
    s += "<node><replace>Original</replace></node>";
    s += "<replace>Original</replace>";
    s += "<node>Original</node>";
    s += "<replace>Normal</replace>";
    s += "</root>";

    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");

    QByteArray inputArray = s.toUtf8().data();
    xmlDocPtr doc = ::xmlParseMemory(inputArray.data(),inputArray.size());
    ASSERT_TRUE(doc!=0);
    xmlNodePtr root = ::xmlDocGetRootElement(doc);
    ASSERT_TRUE(root!=0);

    XMLReadOps ops;
    ops.replaceTextInAllNodes(root,"replace","New");

    xmlNodePtr node = root->children;
    ASSERT_TRUE(node!=0);
    EXPECT_TRUE(ops.getTextOfElement(node->children)=="New");
    node = node->next;
    ASSERT_TRUE(node!=0);
    EXPECT_TRUE(ops.getTextOfElement(node)=="New");
    node = node->next;
    ASSERT_TRUE(node!=0);
    EXPECT_TRUE(ops.getTextOfElement(node)=="Original");
    node = node->next;
    ASSERT_TRUE(node!=0);
    EXPECT_TRUE(ops.getTextOfElement(node)=="New");

    ::xmlFreeDoc(doc);
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,isAttributeNoNodeGiven)
{
    XMLLibIF::instance("mock");

    XMLReadOps ops;
    EXPECT_FALSE(ops.isAttribute(0,"name"));

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,isAttributeNoNameGiven)
{
    XMLLibIF::instance("mock");

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    XMLReadOps ops;
    EXPECT_FALSE(ops.isAttribute(&node,""));

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,isAttributeWhenPropGivesFalse)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    EXPECT_CALL(pAPI,xmlHasProp(&node,A<const xmlChar *>())).Times(1).WillOnce(Return(reinterpret_cast<xmlAttr *>(0)));

    XMLReadOps ops;
    EXPECT_FALSE(ops.isAttribute(&node,"name"));

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,isAttributeWhenPropGivesTrue)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    xmlAttr attr;
    ::memset(&attr,0,sizeof(xmlAttr));

    EXPECT_CALL(pAPI,xmlHasProp(&node,A<const xmlChar *>())).Times(1).WillOnce(Return(&attr));

    XMLReadOps ops;
    EXPECT_TRUE(ops.isAttribute(&node,"name"));

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,isAttributeIntegrate)
{
    QString s;
    s  = "<?xml version=\"1.0\"?>";
    s += "<service name=\"value\" />";

    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");

    QByteArray inputArray = s.toUtf8().data();
    xmlDocPtr doc = ::xmlParseMemory(inputArray.data(),inputArray.size());
    ASSERT_TRUE(doc!=0);
    xmlNodePtr root = ::xmlDocGetRootElement(doc);
    ASSERT_TRUE(root!=0);

    XMLReadOps ops;
    EXPECT_TRUE(ops.isAttribute(root,"name"));
    EXPECT_FALSE(ops.isAttribute(root,"notName"));

    ::xmlFreeDoc(doc);
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

class XMLReadOpsGetAttributeTest : public XMLReadOps
{
    public:
        MOCK_CONST_METHOD2(isAttribute,bool(xmlNodePtr pNode,const QString& name));
};

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getAttributeNoAttributeFound)
{
    XMLLibIF::instance("mock");

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString attributeName("name");
    XMLReadOpsGetAttributeTest ops;
    EXPECT_CALL(ops,isAttribute(&node,attributeName)).Times(1).WillOnce(Return(false));

    EXPECT_TRUE(ops.getAttribute(&node,attributeName).isEmpty());

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getAttributeNoAttributePropertyNode)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString attributeName("name");
    XMLReadOpsGetAttributeTest ops;
    EXPECT_CALL(ops,isAttribute(&node,attributeName)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,xmlGetProp(&node,A<const xmlChar *>())).Times(1).WillOnce(Return(reinterpret_cast<xmlChar *>(0)));
    EXPECT_TRUE(ops.getAttribute(&node,attributeName).isEmpty());

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getAttributeWhenAttributeValueExists)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    xmlChar *pValue = (xmlChar *)("value");

    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString attributeName("name");
    XMLReadOpsGetAttributeTest ops;
    EXPECT_CALL(ops,isAttribute(&node,attributeName)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,xmlGetProp(&node,A<const xmlChar *>())).Times(1).WillOnce(Return(pValue));
    EXPECT_CALL(pAPI,xmlFree(pValue)).Times(1);

    EXPECT_TRUE(ops.getAttribute(&node,attributeName)=="value");

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(XMLReadOps,getAttributeIntegrate)
{
    QString s;
    s  = "<?xml version=\"1.0\"?>";
    s += "<service name=\"value\" />";

    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");

    QByteArray inputArray = s.toUtf8().data();
    xmlDocPtr doc = ::xmlParseMemory(inputArray.data(),inputArray.size());
    ASSERT_TRUE(doc!=0);
    xmlNodePtr root = ::xmlDocGetRootElement(doc);
    ASSERT_TRUE(root!=0);

    XMLReadOps ops;
    EXPECT_TRUE(ops.getAttribute(root,"name")=="value");
    EXPECT_TRUE(ops.getAttribute(root,"notName").isEmpty());

    ::xmlFreeDoc(doc);
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------
