#include "remote/inc/KeyAssignment.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/model/test/TrackDBTestEnviroment.h"
#include "common/test/XMLLibMockIF.h"
#include "common/inc/DiskIF.h"
#include "common/inc/DiskOps.h"

using namespace omega::remote;
using namespace omega::common;
using namespace testing;

//-------------------------------------------------------------------------------------------

class KeyAssignmentTest : public KeyAssignment
{
    public:
        KeyAssignmentTest();
        virtual ~KeyAssignmentTest();
        
        virtual KeyAssignment::Key testKeyFromCommandId(const QString& cmdId) const;
        virtual QString testCommandIdFromKey(KeyAssignment::Key k) const;
        virtual bool testStartBuildContainer(KeyAssignment::Key cmd);
        virtual void testAssignToCurrent(const KeyCode& k);
        virtual QByteArray testLoadInFile(const QString& fileName) const;
        virtual void testProcessNode(xmlNodePtr pNode);
        virtual bool testLoadFromXML(const QString& fileName);
        virtual bool testIsRootNode(xmlNodePtr pNode) const;
        virtual bool testWriteXMLKeyCode(xmlTextWriterPtr pWriter,const KeyCode& k) const;
        virtual bool testWriteXMLKeyContainer(xmlTextWriterPtr pWriter,const Key& keyCmd,const KeyCodesContainer& cont) const;
        virtual bool testWriteXMLKeyMap(xmlTextWriterPtr pWriter) const;
        virtual void testBuildAssignmentMap();
        virtual void testClearAssignmentMap();
        virtual QString testGetDisplayNameForKey(Key key) const;
};

//-------------------------------------------------------------------------------------------

KeyAssignmentTest::KeyAssignmentTest() : KeyAssignment()
{}

//-------------------------------------------------------------------------------------------

KeyAssignmentTest::~KeyAssignmentTest()
{}

//-------------------------------------------------------------------------------------------

KeyAssignment::Key KeyAssignmentTest::testKeyFromCommandId(const QString& cmdId) const
{
    return keyFromCommandId(cmdId);
}

//-------------------------------------------------------------------------------------------

QString KeyAssignmentTest::testCommandIdFromKey(KeyAssignment::Key k) const
{
    return commandIdFromKey(k);
}

//-------------------------------------------------------------------------------------------

bool KeyAssignmentTest::testStartBuildContainer(KeyAssignment::Key cmd)
{
    return startBuildContainer(cmd);
}

//-------------------------------------------------------------------------------------------

void KeyAssignmentTest::testAssignToCurrent(const KeyCode& k)
{
    assignToCurrent(k);
}

//-------------------------------------------------------------------------------------------

QByteArray KeyAssignmentTest::testLoadInFile(const QString& fileName) const
{
    return loadInFile(fileName);
}

//-------------------------------------------------------------------------------------------

void KeyAssignmentTest::testProcessNode(xmlNodePtr pNode)
{
    processNode(pNode);
}

//-------------------------------------------------------------------------------------------

bool KeyAssignmentTest::testLoadFromXML(const QString& fileName)
{
    return loadFromXML(fileName);
}

//-------------------------------------------------------------------------------------------

bool KeyAssignmentTest::testIsRootNode(xmlNodePtr pNode) const
{
    return isRootNode(pNode);
}

//-------------------------------------------------------------------------------------------

bool KeyAssignmentTest::testWriteXMLKeyCode(xmlTextWriterPtr pWriter,const KeyCode& k) const
{
    return writeXMLKeyCode(pWriter,k);
}

//-------------------------------------------------------------------------------------------

bool KeyAssignmentTest::testWriteXMLKeyContainer(xmlTextWriterPtr pWriter,const Key& keyCmd,const KeyCodesContainer& cont) const
{
    return writeXMLKeyContainer(pWriter,keyCmd,cont);
}

//-------------------------------------------------------------------------------------------

bool KeyAssignmentTest::testWriteXMLKeyMap(xmlTextWriterPtr pWriter) const
{
    return writeXMLKeyMap(pWriter);
}

//-------------------------------------------------------------------------------------------

void KeyAssignmentTest::testBuildAssignmentMap()
{
    buildAssignmentMap();
}

//-------------------------------------------------------------------------------------------

void KeyAssignmentTest::testClearAssignmentMap()
{
    clearAssignmentMap();
}

//-------------------------------------------------------------------------------------------

QString KeyAssignmentTest::testGetDisplayNameForKey(Key key) const
{
    return getDisplayNameForKey(key);
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentAssignedToTest : public KeyAssignmentTest
{
    public:
        MOCK_CONST_METHOD0(mapConst,const QMap<Key,KeyCodesContainer>&());
        MOCK_CONST_METHOD0(assignMapConst,const QMap<QString,Key>&());
        MOCK_METHOD0(assignMap,QMap<QString,Key>&());
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignedToGivenEmptyMapNoAssignMap)
{
    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    QMap<QString,KeyAssignment::Key> aMap;

    KeyCode k(1234);
    KeyAssignmentAssignedToTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMapConst()).WillRepeatedly(ReturnRef(aMap));
    
    EXPECT_TRUE(kAssign.assignedTo(k)==KeyAssignment::e_keyUnassigned);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignedToGivenEmptyMapWithAssignMap)
{
    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    QMap<QString,KeyAssignment::Key> aMap;

    KeyCode k(1234);
    KeyAssignmentAssignedToTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMapConst()).WillRepeatedly(ReturnRef(aMap));
    EXPECT_CALL(kAssign,assignMap()).WillRepeatedly(ReturnRef(aMap));

    kAssign.testBuildAssignmentMap();
    EXPECT_TRUE(kAssign.assignedTo(k)==KeyAssignment::e_keyUnassigned);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignedToGivenMappingNotAssignedNoAssignMap)
{
    KeyCodesContainer kContA;
    KeyCode kA1(12);
    kContA.list().append(kA1);
    KeyCode kA2(13);
    kContA.list().append(kA2);
    KeyCode kA3(14);
    kContA.list().append(kA3);
    
    KeyCodesContainer kContB;
    KeyCode kB1(15);
    kContB.list().append(kB1);
    KeyCode kB2(16);
    kContB.list().append(kB2);
    
    KeyCodesContainer kContC;
    KeyCode kC1(17);
    kContC.list().append(kC1);

    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,kContA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,kContB);
    kMap.insert(KeyAssignment::e_keyVolumeUp,kContC);
    QMap<QString,KeyAssignment::Key> aMap;

    KeyAssignmentAssignedToTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMapConst()).WillRepeatedly(ReturnRef(aMap));
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(11))==KeyAssignment::e_keyUnassigned);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(18))==KeyAssignment::e_keyUnassigned);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignedToGivenMappingNotAssignedWithAssignMap)
{
    KeyCodesContainer kContA;
    KeyCode kA1(12);
    kContA.list().append(kA1);
    KeyCode kA2(13);
    kContA.list().append(kA2);
    KeyCode kA3(14);
    kContA.list().append(kA3);
    
    KeyCodesContainer kContB;
    KeyCode kB1(15);
    kContB.list().append(kB1);
    KeyCode kB2(16);
    kContB.list().append(kB2);
    
    KeyCodesContainer kContC;
    KeyCode kC1(17);
    kContC.list().append(kC1);

    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,kContA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,kContB);
    kMap.insert(KeyAssignment::e_keyVolumeUp,kContC);

    QMap<QString,KeyAssignment::Key> aMap;

    KeyAssignmentAssignedToTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMapConst()).WillRepeatedly(ReturnRef(aMap));
    EXPECT_CALL(kAssign,assignMap()).WillRepeatedly(ReturnRef(aMap));
    
    kAssign.testBuildAssignmentMap();
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(11))==KeyAssignment::e_keyUnassigned);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(18))==KeyAssignment::e_keyUnassigned);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignedToGivenMappingHasBeenCreatedNoAssignMap)
{
    KeyCodesContainer kContA;
    KeyCode kA1(12);
    kContA.list().append(kA1);
    KeyCode kA2(13);
    kContA.list().append(kA2);
    KeyCode kA3(14);
    kContA.list().append(kA3);
    
    KeyCodesContainer kContB;
    KeyCode kB1(15);
    kContB.list().append(kB1);
    KeyCode kB2(16);
    kContB.list().append(kB2);
    
    KeyCodesContainer kContC;
    KeyCode kC1(17);
    kContC.list().append(kC1);

    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,kContA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,kContB);
    kMap.insert(KeyAssignment::e_keyVolumeUp,kContC);

    QMap<QString,KeyAssignment::Key> aMap;

    KeyAssignmentAssignedToTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMapConst()).WillRepeatedly(ReturnRef(aMap));
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(12))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(13))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(14))==KeyAssignment::e_keyPlay);
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(15))==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(16))==KeyAssignment::e_keyPreviousTrack);
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(17))==KeyAssignment::e_keyVolumeUp);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignedToGivenMappingHasBeenCreatedWithAssignMap)
{
    KeyCodesContainer kContA;
    KeyCode kA1(12);
    kContA.list().append(kA1);
    KeyCode kA2(13);
    kContA.list().append(kA2);
    KeyCode kA3(14);
    kContA.list().append(kA3);
    
    KeyCodesContainer kContB;
    KeyCode kB1(15);
    kContB.list().append(kB1);
    KeyCode kB2(16);
    kContB.list().append(kB2);
    
    KeyCodesContainer kContC;
    KeyCode kC1(17);
    kContC.list().append(kC1);

    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,kContA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,kContB);
    kMap.insert(KeyAssignment::e_keyVolumeUp,kContC);
    
    QMap<QString,KeyAssignment::Key> aMap;

    KeyAssignmentAssignedToTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMapConst()).WillRepeatedly(ReturnRef(aMap));
    EXPECT_CALL(kAssign,assignMap()).WillRepeatedly(ReturnRef(aMap));
    
    kAssign.testBuildAssignmentMap();
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(12))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(13))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(14))==KeyAssignment::e_keyPlay);
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(15))==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(16))==KeyAssignment::e_keyPreviousTrack);
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(17))==KeyAssignment::e_keyVolumeUp);
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentBuildAssignmentMapTest : public KeyAssignmentTest
{
    public:
        MOCK_CONST_METHOD0(mapConst,const QMap<Key,KeyCodesContainer>&());
        MOCK_METHOD0(assignMap,QMap<QString,Key>&());
        MOCK_METHOD0(clearAssignmentMap,void());
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,buildAssignmentWhenNoContainers)
{
    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    QMap<QString,KeyAssignment::Key> aMap;

    KeyAssignmentBuildAssignmentMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMap()).WillRepeatedly(ReturnRef(aMap));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    
    kAssign.testBuildAssignmentMap();
    
    EXPECT_TRUE(aMap.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,buildAssignmentWhenContainersAreEmpty)
{
    KeyCodesContainer kContA;
    KeyCodesContainer kContB;
    KeyCodesContainer kContC;

    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,kContA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,kContB);
    kMap.insert(KeyAssignment::e_keyVolumeUp,kContC);
    
    QMap<QString,KeyAssignment::Key> aMap;

    KeyAssignmentBuildAssignmentMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMap()).WillRepeatedly(ReturnRef(aMap));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    
    kAssign.testBuildAssignmentMap();
    
    EXPECT_TRUE(aMap.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,buildAssignmentGivenUniqueMapping)
{
    KeyCodesContainer kContA;
    KeyCode kA1(12);
    kContA.list().append(kA1);
    KeyCode kA2(13);
    kContA.list().append(kA2);
    KeyCode kA3(14);
    kContA.list().append(kA3);
    
    KeyCodesContainer kContB;
    KeyCode kB1(15);
    kContB.list().append(kB1);
    KeyCode kB2(16);
    kContB.list().append(kB2);
    
    KeyCodesContainer kContC;
    KeyCode kC1(17);
    kContC.list().append(kC1);

    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,kContA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,kContB);
    kMap.insert(KeyAssignment::e_keyVolumeUp,kContC);
    
    QMap<QString,KeyAssignment::Key> aMap;

    KeyAssignmentBuildAssignmentMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMap()).WillRepeatedly(ReturnRef(aMap));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    
    kAssign.testBuildAssignmentMap();
    
    EXPECT_TRUE(aMap.size()==6);
    EXPECT_TRUE(aMap.find(kA1.variant().toString()).value()==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(aMap.find(kA2.variant().toString()).value()==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(aMap.find(kA3.variant().toString()).value()==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(aMap.find(kB1.variant().toString()).value()==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(aMap.find(kB2.variant().toString()).value()==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(aMap.find(kC1.variant().toString()).value()==KeyAssignment::e_keyVolumeUp);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,buildAssignmentGivenNonUniqueMapping)
{
    KeyCodesContainer kContA;
    KeyCode kA1(12);
    kContA.list().append(kA1);
    KeyCode kA2(13);
    kContA.list().append(kA2);
    KeyCode kA3(13);
    kContA.list().append(kA3);
    
    KeyCodesContainer kContB;
    KeyCode kB1(12);
    kContB.list().append(kB1);
    KeyCode kB2(16);
    kContB.list().append(kB2);
    
    KeyCodesContainer kContC;
    KeyCode kC1(16);
    kContC.list().append(kC1);
    
    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,kContA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,kContB);
    kMap.insert(KeyAssignment::e_keyVolumeUp,kContC);
    
    QMap<QString,KeyAssignment::Key> aMap;

    KeyAssignmentBuildAssignmentMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMap()).WillRepeatedly(ReturnRef(aMap));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    
    kAssign.testBuildAssignmentMap();
    
    EXPECT_TRUE(aMap.size()==3);
    EXPECT_TRUE(aMap.find(kA3.variant().toString()).value()==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(aMap.find(kB1.variant().toString()).value()==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(aMap.find(kC1.variant().toString()).value()==KeyAssignment::e_keyVolumeUp);
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentClearAssignmentMapTest : public KeyAssignmentTest
{
    public:
        MOCK_METHOD0(assignMap,QMap<QString,Key>&());
        MOCK_CONST_METHOD0(mapConst,const QMap<Key,KeyCodesContainer>&());
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,clearAssignmentMap)
{
    KeyCodesContainer kContA;
    KeyCode kA1(12);
    kContA.list().append(kA1);
    KeyCode kA2(13);
    kContA.list().append(kA2);
    KeyCode kA3(14);
    kContA.list().append(kA3);
    
    KeyCodesContainer kContB;
    KeyCode kB1(15);
    kContB.list().append(kB1);
    KeyCode kB2(16);
    kContB.list().append(kB2);
    
    KeyCodesContainer kContC;
    KeyCode kC1(17);
    kContC.list().append(kC1);

    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,kContA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,kContB);
    kMap.insert(KeyAssignment::e_keyVolumeUp,kContC);
    
    QMap<QString,KeyAssignment::Key> aMap;
    
    KeyAssignmentClearAssignmentMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(kAssign,assignMap()).WillRepeatedly(ReturnRef(aMap));
    
    kAssign.testBuildAssignmentMap();
    EXPECT_FALSE(aMap.isEmpty());
    
    kAssign.testClearAssignmentMap();
    EXPECT_TRUE(aMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,keyFromCommandIdWhenPlay)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testKeyFromCommandId("play")==KeyAssignment::e_keyPlay);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,keyFromCommandIdWhenPreviousTrack)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testKeyFromCommandId("previous_track")==KeyAssignment::e_keyPreviousTrack);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,keyFromCommandIdWhenNextTrack)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testKeyFromCommandId("next_track")==KeyAssignment::e_keyNextTrack);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,keyFromCommandIdWhenVolumeDown)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testKeyFromCommandId("volume_down")==KeyAssignment::e_keyVolumeDown);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,keyFromCommandIdWhenVolumeUp)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testKeyFromCommandId("volume_up")==KeyAssignment::e_keyVolumeUp);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,keyFromCommandIdWhenUnknown)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testKeyFromCommandId("not_a_key")==KeyAssignment::e_keyUnassigned);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,commandIdFromKeyWhenPlay)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testCommandIdFromKey(KeyAssignment::e_keyPlay)=="play");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,commandIdFromKeyWhenPreviousTrack)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testCommandIdFromKey(KeyAssignment::e_keyPreviousTrack)=="previous_track");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,commandIdFromKeyWhenNextTrack)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testCommandIdFromKey(KeyAssignment::e_keyNextTrack)=="next_track");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,commandIdFromKeyWhenVolumeDown)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testCommandIdFromKey(KeyAssignment::e_keyVolumeDown)=="volume_down");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,commandIdFromKeyWhenVolumeUp)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testCommandIdFromKey(KeyAssignment::e_keyVolumeUp)=="volume_up");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,commandIdFromKeyWhenUnassigned)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testCommandIdFromKey(KeyAssignment::e_keyUnassigned)=="unassigned");
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentStartBuildContainerTest : public KeyAssignmentTest
{
    public:
        MOCK_METHOD0(map,QMap<Key,KeyCodesContainer>&());
        MOCK_METHOD0(currentBuildKey,KeyAssignment::Key&());
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,startBuildContainerGivenUnassigned)
{
    KeyAssignment::Key key = KeyAssignment::e_keyPlay;
    
    KeyAssignmentStartBuildContainerTest kAssign;
    EXPECT_CALL(kAssign,currentBuildKey()).WillRepeatedly(ReturnRef(key));
    
    EXPECT_FALSE(kAssign.testStartBuildContainer(KeyAssignment::e_keyUnassigned));
    EXPECT_TRUE(key==KeyAssignment::e_keyUnassigned);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,startBuildContainerGivenNoContainerExists)
{
    KeyAssignment::Key key = KeyAssignment::e_keyUnassigned;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    
    KeyAssignmentStartBuildContainerTest kAssign;
    EXPECT_CALL(kAssign,map()).WillRepeatedly(ReturnRef(map));
    EXPECT_CALL(kAssign,currentBuildKey()).WillRepeatedly(ReturnRef(key));
    
    EXPECT_TRUE(kAssign.testStartBuildContainer(KeyAssignment::e_keyPlay));
    EXPECT_TRUE(map.find(KeyAssignment::e_keyPlay)!=map.end());
    EXPECT_TRUE(map.find(KeyAssignment::e_keyPlay).value().isEmpty());
    EXPECT_TRUE(key==KeyAssignment::e_keyPlay);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,startBuildContainerGivenContainerAlreadyExists)
{
    KeyCodesContainer kCont;
    kCont.list().append(KeyCode(12));
    
    KeyAssignment::Key key = KeyAssignment::e_keyUnassigned;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    map.insert(KeyAssignment::e_keyPlay,kCont);
    
    KeyAssignmentStartBuildContainerTest kAssign;
    EXPECT_CALL(kAssign,map()).WillRepeatedly(ReturnRef(map));
    EXPECT_CALL(kAssign,currentBuildKey()).WillRepeatedly(ReturnRef(key));
    
    EXPECT_TRUE(kAssign.testStartBuildContainer(KeyAssignment::e_keyPlay));
    EXPECT_TRUE(map.find(KeyAssignment::e_keyPlay)!=map.end());
    EXPECT_TRUE(map.find(KeyAssignment::e_keyPlay).value().isEmpty());
    EXPECT_TRUE(key==KeyAssignment::e_keyPlay);
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentAssignToCurrentTest : public KeyAssignmentTest
{
    public:
        MOCK_CONST_METHOD0(currentBuildKeyConst,const KeyAssignment::Key&());
        MOCK_METHOD0(map,QMap<Key,KeyCodesContainer>&());
        MOCK_CONST_METHOD1(assignedTo,KeyAssignment::Key(const KeyCode& k));
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignToCurrentGivenCurrentKeyIsUnassigned)
{
    KeyAssignment::Key cmd = KeyAssignment::e_keyUnassigned;
    KeyCode newKey(13);
    
    KeyAssignmentAssignToCurrentTest kAssign;
    EXPECT_CALL(kAssign,currentBuildKeyConst()).Times(1).WillOnce(ReturnRef(cmd));

    kAssign.testAssignToCurrent(newKey);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignToCurrentGivenThatKeyIsAlreadyAssigned)
{
    KeyAssignment::Key cmd = KeyAssignment::e_keyPlay;
    KeyCode newKey(13);
    
    KeyAssignmentAssignToCurrentTest kAssign;
    EXPECT_CALL(kAssign,currentBuildKeyConst()).WillRepeatedly(ReturnRef(cmd));
    EXPECT_CALL(kAssign,assignedTo(newKey)).Times(1).WillOnce(Return(KeyAssignment::e_keyPlay));
    
    kAssign.testAssignToCurrent(newKey);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignToCurrentGivenThatContainerDoesNotExist)
{
    KeyCodesContainer kCont;
    kCont.list().append(KeyCode(12));
    
    KeyAssignment::Key cmd = KeyAssignment::e_keyPlay;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    map.insert(KeyAssignment::e_keyVolumeDown,kCont);
    
    KeyCode newKey(13);
    
    KeyAssignmentAssignToCurrentTest kAssign;
    EXPECT_CALL(kAssign,currentBuildKeyConst()).WillRepeatedly(ReturnRef(cmd));
    EXPECT_CALL(kAssign,assignedTo(newKey)).Times(1).WillOnce(Return(KeyAssignment::e_keyUnassigned));
    EXPECT_CALL(kAssign,map()).WillRepeatedly(ReturnRef(map));
    
    kAssign.testAssignToCurrent(newKey);
    
    EXPECT_TRUE(map.find(KeyAssignment::e_keyVolumeDown).value().size()==1);
    EXPECT_TRUE(map.find(KeyAssignment::e_keyVolumeDown).value().at(0)==KeyCode(12));
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,assignToCurrentGivenThatContainerExists)
{
    KeyCodesContainer kCont;
    kCont.list().append(KeyCode(12));
    
    KeyAssignment::Key cmd = KeyAssignment::e_keyPlay;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    map.insert(KeyAssignment::e_keyPlay,kCont);
    
    KeyCode newKey(13);
    
    KeyAssignmentAssignToCurrentTest kAssign;
    EXPECT_CALL(kAssign,currentBuildKeyConst()).WillRepeatedly(ReturnRef(cmd));
    EXPECT_CALL(kAssign,assignedTo(newKey)).Times(1).WillOnce(Return(KeyAssignment::e_keyUnassigned));
    EXPECT_CALL(kAssign,map()).WillRepeatedly(ReturnRef(map));
    
    kAssign.testAssignToCurrent(newKey);
    
    EXPECT_TRUE(map.find(KeyAssignment::e_keyPlay).value().size()==2);
    EXPECT_TRUE(map.find(KeyAssignment::e_keyPlay).value().at(0)==KeyCode(12));
    EXPECT_TRUE(map.find(KeyAssignment::e_keyPlay).value().at(1)==KeyCode(13));
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadInFileWithSuccess)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testLoadInFile(":/remote/Resources/remote/keyassignment.xml").size()>0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadInFileWithFailure)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testLoadInFile(":/remote/Resources/remote/keyassignment_notexist.xml").size()==0);
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentProcessNodeTest : public KeyAssignmentTest
{
    public:
        MOCK_CONST_METHOD1(getNameOfNode,QString(xmlNodePtr pNode));
        MOCK_CONST_METHOD0(getXMLParseState,tint());
        MOCK_METHOD1(setXMLParseState,void(tint s));
        MOCK_CONST_METHOD2(isAttribute,bool(xmlNodePtr pNode,const QString& name));
        MOCK_CONST_METHOD2(getAttribute,QString(xmlNodePtr pNode,const QString& name));
        MOCK_METHOD1(startBuildContainer,bool(Key cmd));
        MOCK_CONST_METHOD1(getTextOfElement,QString(xmlNodePtr pElement));
        MOCK_METHOD1(assignToCurrent,void(const KeyCode& k));
        MOCK_METHOD1(parse,void(xmlNodePtr pNode));
        MOCK_CONST_METHOD1(keyFromCommandId,KeyAssignment::Key(const QString& cmdId));
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeWithNonCommandNode)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("notcommand");

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(0));
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeWithCommandNodeGivenNoCommandId)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("command");

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(kAssign,isAttribute(&node,QString::fromLatin1(KeyAssignment::c_attributeId))).Times(1).WillOnce(Return(false));
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeWithCommandNodeGivenUnknownCommandId)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("command");
    QString cmdId("play");

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(kAssign,isAttribute(&node,QString::fromLatin1(KeyAssignment::c_attributeId))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,getAttribute(&node,QString::fromLatin1(KeyAssignment::c_attributeId))).Times(1).WillOnce(Return(cmdId));
    EXPECT_CALL(kAssign,keyFromCommandId(cmdId)).Times(1).WillOnce(Return(KeyAssignment::e_keyPlay));
    EXPECT_CALL(kAssign,startBuildContainer(KeyAssignment::e_keyPlay)).Times(1).WillOnce(Return(false));
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeWithValidCommandNode)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("command");
    QString cmdId("play");

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(kAssign,isAttribute(&node,QString::fromLatin1(KeyAssignment::c_attributeId))).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,getAttribute(&node,QString::fromLatin1(KeyAssignment::c_attributeId))).Times(1).WillOnce(Return(cmdId));
    EXPECT_CALL(kAssign,keyFromCommandId(cmdId)).Times(1).WillOnce(Return(KeyAssignment::e_keyPlay));
    EXPECT_CALL(kAssign,startBuildContainer(KeyAssignment::e_keyPlay)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,setXMLParseState(1)).Times(1);
    EXPECT_CALL(kAssign,parse(&node)).Times(1);
    EXPECT_CALL(kAssign,setXMLParseState(0)).Times(1);
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeWithNonKeyOrRemoteNode)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("nonode");

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(1));
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeOnRemoteNodeWithNoEntry)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("remote");
    QString remoteString("");

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(kAssign,getTextOfElement(&node)).Times(1).WillOnce(Return(remoteString));
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeOnRemoteNodeWithEntry)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("remote");
    QString remoteString("play");
    KeyCode newKey(remoteString);

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(kAssign,getTextOfElement(&node)).Times(1).WillOnce(Return(remoteString));
    EXPECT_CALL(kAssign,assignToCurrent(newKey)).Times(1);
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeOnNonNumericKeyNodeWithX)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("key");
    QString hexElement("asxgfd34sj");

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(kAssign,getTextOfElement(&node)).Times(1).WillOnce(Return(hexElement));
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeOnDecimalKeyNode)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("key");
    QString hexElement("12345678");
    KeyCode newKey(12345678);

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(kAssign,getTextOfElement(&node)).Times(1).WillOnce(Return(hexElement));
    EXPECT_CALL(kAssign,assignToCurrent(Eq(newKey))).Times(1);
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,processNodeOnHexidecimalKeyNode)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));

    QString cNameKey("key");
    QString hexElement("0xfedc1234");
    KeyCode newKey(0xfedc1234);

    KeyAssignmentProcessNodeTest kAssign;
    EXPECT_CALL(kAssign,getNameOfNode(&node)).Times(1).WillOnce(Return(cNameKey));
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(kAssign,getTextOfElement(&node)).Times(1).WillOnce(Return(hexElement));
    EXPECT_CALL(kAssign,assignToCurrent(newKey)).Times(1);
    
    kAssign.testProcessNode(&node);
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentIsRootNodeTest : public KeyAssignmentTest
{
    public:
        MOCK_CONST_METHOD0(getXMLParseState,tint());
        MOCK_CONST_METHOD2(isSpecifiedNode,bool(xmlNodePtr pNode,const tchar *name));
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,isRootNodeWhenTopNodeAndKeyAssignmentMap)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    
    KeyAssignmentIsRootNodeTest kAssign;
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(kAssign,isSpecifiedNode(&node,KeyAssignment::c_keyKeyAssignmentMap)).Times(1).WillOnce(Return(true));
    
    EXPECT_TRUE(kAssign.testIsRootNode(&node));
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,isRootNodeWhenTopNodeAndNotKeyAssignmentMap)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    
    KeyAssignmentIsRootNodeTest kAssign;
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(0));
    EXPECT_CALL(kAssign,isSpecifiedNode(&node,KeyAssignment::c_keyKeyAssignmentMap)).Times(1).WillOnce(Return(false));
    
    EXPECT_FALSE(kAssign.testIsRootNode(&node));
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,isRootNodeWhenCommandNodeAndCommand)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    
    KeyAssignmentIsRootNodeTest kAssign;
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(kAssign,isSpecifiedNode(&node,KeyAssignment::c_keyCommand)).Times(1).WillOnce(Return(true));
    
    EXPECT_TRUE(kAssign.testIsRootNode(&node));
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,isRootNodeWhenCommandNodeAndNotCommand)
{
    xmlNode node;
    ::memset(&node,0,sizeof(xmlNode));
    
    KeyAssignmentIsRootNodeTest kAssign;
    EXPECT_CALL(kAssign,getXMLParseState()).Times(1).WillOnce(Return(1));
    EXPECT_CALL(kAssign,isSpecifiedNode(&node,KeyAssignment::c_keyCommand)).Times(1).WillOnce(Return(false));
    
    EXPECT_FALSE(kAssign.testIsRootNode(&node));
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentLoadFromXMLTest : public KeyAssignmentTest
{
    public:
        MOCK_METHOD0(map,QMap<Key,KeyCodesContainer>&());
        MOCK_CONST_METHOD1(loadInFile,QByteArray(const QString& fileName));
        MOCK_CONST_METHOD1(isRootNode,bool(xmlNodePtr pNode));
        MOCK_METHOD1(parse,void(xmlNodePtr pNode));
        MOCK_METHOD0(buildAssignmentMap,void());
        MOCK_METHOD0(clearAssignmentMap,void());
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadFromXMLGivenNoFileIsLoaded)
{
    KeyCodesContainer kCont;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    map.insert(KeyAssignment::e_keyPlay,kCont);
    QByteArray xmlMem;

    QString fileName("key_assignment.xml");

    KeyAssignmentLoadFromXMLTest kAssign;
    EXPECT_CALL(kAssign,map()).Times(1).WillOnce(ReturnRef(map));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    EXPECT_CALL(kAssign,loadInFile(fileName)).Times(1).WillOnce(Return(xmlMem));
    
    EXPECT_FALSE(kAssign.loadFromXML(fileName));
    EXPECT_TRUE(map.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadFromXMLGivenNoXMLDocument)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    KeyCodesContainer kCont;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    map.insert(KeyAssignment::e_keyPlay,kCont);

    QString xmlString("<?xml version=\"1.0\"?>");
    QByteArray xmlMem(xmlString.toUtf8().constData(),xmlString.toUtf8().size());

    QString fileName("key_assignment.xml");

    KeyAssignmentLoadFromXMLTest kAssign;
    EXPECT_CALL(kAssign,map()).Times(1).WillOnce(ReturnRef(map));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    EXPECT_CALL(kAssign,loadInFile(fileName)).Times(1).WillOnce(Return(xmlMem));
    EXPECT_CALL(pAPI,xmlParseMemory(xmlMem.constData(),xmlMem.size())).Times(1).WillOnce(Return((xmlDocPtr)0));
    
    EXPECT_FALSE(kAssign.loadFromXML(fileName));
    EXPECT_TRUE(map.isEmpty());

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadFromXMLGivenNoXMLRoot)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    KeyCodesContainer kCont;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    map.insert(KeyAssignment::e_keyPlay,kCont);

    QString xmlString("<?xml version=\"1.0\"?>");
    QByteArray xmlMem(xmlString.toUtf8().constData(),xmlString.toUtf8().size());

    xmlDoc xDoc;

    QString fileName("key_assignment.xml");

    KeyAssignmentLoadFromXMLTest kAssign;
    EXPECT_CALL(kAssign,map()).Times(1).WillOnce(ReturnRef(map));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    EXPECT_CALL(kAssign,loadInFile(fileName)).Times(1).WillOnce(Return(xmlMem));
    EXPECT_CALL(pAPI,xmlParseMemory(xmlMem.constData(),xmlMem.size())).Times(1).WillOnce(Return(&xDoc));
    EXPECT_CALL(pAPI,xmlDocGetRootElement(&xDoc)).Times(1).WillOnce(Return((xmlNodePtr)0));
    EXPECT_CALL(pAPI,xmlFreeDoc(&xDoc)).Times(1);
    
    EXPECT_FALSE(kAssign.loadFromXML(fileName));
    EXPECT_TRUE(map.isEmpty());

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadFromXMLGivenInvalidKeyAssignmentXML)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    KeyCodesContainer kCont;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    map.insert(KeyAssignment::e_keyPlay,kCont);

    QString xmlString("<?xml version=\"1.0\"?>");
    QByteArray xmlMem(xmlString.toUtf8().constData(),xmlString.toUtf8().size());

    xmlDoc xDoc;
    xmlNode xRoot;

    QString fileName("key_assignment.xml");

    KeyAssignmentLoadFromXMLTest kAssign;
    EXPECT_CALL(kAssign,map()).Times(1).WillOnce(ReturnRef(map));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    EXPECT_CALL(kAssign,loadInFile(fileName)).Times(1).WillOnce(Return(xmlMem));
    EXPECT_CALL(pAPI,xmlParseMemory(xmlMem.constData(),xmlMem.size())).Times(1).WillOnce(Return(&xDoc));
    EXPECT_CALL(pAPI,xmlDocGetRootElement(&xDoc)).Times(1).WillOnce(Return(&xRoot));
    EXPECT_CALL(kAssign,isRootNode(&xRoot)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,xmlFreeDoc(&xDoc)).Times(1);
    
    EXPECT_FALSE(kAssign.loadFromXML(fileName));
    EXPECT_TRUE(map.isEmpty());

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadFromXMLGivenValidKeyAssignmentXML)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    KeyCodesContainer kCont;
    QMap<KeyAssignment::Key,KeyCodesContainer> map;
    map.insert(KeyAssignment::e_keyPlay,kCont);

    QString xmlString("<?xml version=\"1.0\"?>");
    QByteArray xmlMem(xmlString.toUtf8().constData(),xmlString.toUtf8().size());

    xmlDoc xDoc;
    xmlNode xRoot;

    QString fileName("key_assignment.xml");

    KeyAssignmentLoadFromXMLTest kAssign;
    EXPECT_CALL(kAssign,map()).Times(1).WillOnce(ReturnRef(map));
    EXPECT_CALL(kAssign,clearAssignmentMap()).Times(1);
    EXPECT_CALL(kAssign,loadInFile(fileName)).Times(1).WillOnce(Return(xmlMem));
    EXPECT_CALL(pAPI,xmlParseMemory(xmlMem.constData(),xmlMem.size())).Times(1).WillOnce(Return(&xDoc));
    EXPECT_CALL(pAPI,xmlDocGetRootElement(&xDoc)).Times(1).WillOnce(Return(&xRoot));
    EXPECT_CALL(kAssign,isRootNode(&xRoot)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,parse(&xRoot)).Times(1);
    EXPECT_CALL(kAssign,buildAssignmentMap()).Times(1);
    EXPECT_CALL(pAPI,xmlFreeDoc(&xDoc)).Times(1);
    
    EXPECT_TRUE(kAssign.loadFromXML(fileName));
    EXPECT_TRUE(map.isEmpty());

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyCodeWithKeyboardSuccess)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));
    
    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCode k(0x1234);
    QString kText = "0x" + QString::number(k.keyCode(),16);
    
    KeyAssignmentTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterWriteElement(writer,StrEq(KeyAssignment::c_keyKey),StrEq(kText.toUtf8().constData()))).Times(1).WillOnce(Return(100));
    
    EXPECT_TRUE(kAssign.testWriteXMLKeyCode(writer,k));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyCodeWithKeyboardFail)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCode k(0x1234);
    QString kText = "0x" + QString::number(k.keyCode(),16);
    
    KeyAssignmentTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterWriteElement(writer,StrEq(KeyAssignment::c_keyKey),StrEq(kText.toUtf8().constData()))).Times(1).WillOnce(Return(-1));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyCode(writer,k));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyCodeWithRemoteSuccess)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCode k(QString::fromLatin1("play"));
    
    KeyAssignmentTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterWriteElement(writer,StrEq(KeyAssignment::c_keyRemote),StrEq("play"))).Times(1).WillOnce(Return(100));
    
    EXPECT_TRUE(kAssign.testWriteXMLKeyCode(writer,k));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyCodeWithRemoteFail)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCode k(QString::fromLatin1("play"));
    
    KeyAssignmentTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterWriteElement(writer,StrEq(KeyAssignment::c_keyRemote),StrEq("play"))).Times(1).WillOnce(Return(-1));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyCode(writer,k));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentWriteXMLKeyContainerTest : public KeyAssignmentTest
{
    public:
        MOCK_CONST_METHOD2(writeXMLKeyCode,bool(xmlTextWriterPtr pWriter,const KeyCode& k));
        MOCK_CONST_METHOD1(commandIdFromKey,QString(KeyAssignment::Key k));
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyContainerWhenStartElementFails)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer cont;
    KeyCode kA(123),kB(456),kC(789);
    cont.list().append(kA);
    cont.list().append(kB);
    cont.list().append(kC);
    
    KeyAssignmentWriteXMLKeyContainerTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("command"))).Times(1).WillOnce(Return(-1));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyContainer(writer,KeyAssignment::e_keyPlay,cont));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyContainerWhenWriteAttributeFails)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer cont;
    KeyCode kA(123),kB(456),kC(789);
    cont.list().append(kA);
    cont.list().append(kB);
    cont.list().append(kC);
    
    KeyAssignmentWriteXMLKeyContainerTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("command"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(pAPI,xmlTextWriterWriteAttribute(writer,StrEq("id"),StrEq("play"))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(kAssign,commandIdFromKey(KeyAssignment::e_keyPlay)).Times(1).WillOnce(Return(QString::fromLatin1("play")));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyContainer(writer,KeyAssignment::e_keyPlay,cont));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyContainerWhenFirstKeyCodeFails)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer cont;
    KeyCode kA(123),kB(456),kC(789);
    cont.list().append(kA);
    cont.list().append(kB);
    cont.list().append(kC);
    
    KeyAssignmentWriteXMLKeyContainerTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("command"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(pAPI,xmlTextWriterWriteAttribute(writer,StrEq("id"),StrEq("play"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(kAssign,commandIdFromKey(KeyAssignment::e_keyPlay)).Times(1).WillOnce(Return(QString::fromLatin1("play")));
    EXPECT_CALL(kAssign,writeXMLKeyCode(writer,kA)).Times(1).WillOnce(Return(false));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyContainer(writer,KeyAssignment::e_keyPlay,cont));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyContainerWhenEndElementFails)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer cont;
    KeyCode kA(123),kB(456),kC(789);
    cont.list().append(kA);
    cont.list().append(kB);
    cont.list().append(kC);
    
    KeyAssignmentWriteXMLKeyContainerTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("command"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(pAPI,xmlTextWriterWriteAttribute(writer,StrEq("id"),StrEq("play"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(kAssign,commandIdFromKey(KeyAssignment::e_keyPlay)).Times(1).WillOnce(Return(QString::fromLatin1("play")));
    EXPECT_CALL(kAssign,writeXMLKeyCode(writer,kA)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,writeXMLKeyCode(writer,kB)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,writeXMLKeyCode(writer,kC)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,xmlTextWriterEndElement(writer)).Times(1).WillOnce(Return(-1));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyContainer(writer,KeyAssignment::e_keyPlay,cont));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyContainerWhenNoKeyCodesAndSuccess)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer cont;
    
    KeyAssignmentWriteXMLKeyContainerTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("command"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(pAPI,xmlTextWriterWriteAttribute(writer,StrEq("id"),StrEq("play"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(kAssign,commandIdFromKey(KeyAssignment::e_keyPlay)).Times(1).WillOnce(Return(QString::fromLatin1("play")));
    EXPECT_CALL(pAPI,xmlTextWriterEndElement(writer)).Times(1).WillOnce(Return(10));
    
    EXPECT_TRUE(kAssign.testWriteXMLKeyContainer(writer,KeyAssignment::e_keyPlay,cont));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyContainerWhenThreeKeyCodesAndSuccess)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer cont;
    KeyCode kA(123),kB(456),kC(789);
    cont.list().append(kA);
    cont.list().append(kB);
    cont.list().append(kC);
    
    KeyAssignmentWriteXMLKeyContainerTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("command"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(pAPI,xmlTextWriterWriteAttribute(writer,StrEq("id"),StrEq("play"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(kAssign,commandIdFromKey(KeyAssignment::e_keyPlay)).Times(1).WillOnce(Return(QString::fromLatin1("play")));
    EXPECT_CALL(kAssign,writeXMLKeyCode(writer,kA)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,writeXMLKeyCode(writer,kB)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,writeXMLKeyCode(writer,kC)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,xmlTextWriterEndElement(writer)).Times(1).WillOnce(Return(10));
    
    EXPECT_TRUE(kAssign.testWriteXMLKeyContainer(writer,KeyAssignment::e_keyPlay,cont));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentWriteXMLKeyMapTest : public KeyAssignmentTest
{
    public:
        MOCK_CONST_METHOD3(writeXMLKeyContainer,bool(xmlTextWriterPtr pWriter,const Key& keyCmd,const KeyCodesContainer& cont));
        MOCK_CONST_METHOD0(mapConst,const QMap<Key,KeyCodesContainer>&());
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyMapGivenStartElementFails)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyAssignmentWriteXMLKeyMapTest kAssign;
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("KeyAssignmentMap"))).Times(1).WillOnce(Return(-1));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyMap(writer));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyMapGivenFirstContainerFails)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer contA,contB,contC;
    KeyCode kA(123),kB(456),kC(789);
    contA.list().append(kA);
    contB.list().append(kB);
    contC.list().append(kC);
    
    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,contA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,contB);
    kMap.insert(KeyAssignment::e_keyNextTrack,contC);
    
    KeyAssignmentWriteXMLKeyMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("KeyAssignmentMap"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(kAssign,writeXMLKeyContainer(writer,Eq(KeyAssignment::e_keyPlay),Eq(contA))).Times(1).WillOnce(Return(false));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyMap(writer));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyMapGivenEndElementFails)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer contA,contB,contC;
    KeyCode kA(123),kB(456),kC(789);
    contA.list().append(kA);
    contB.list().append(kB);
    contC.list().append(kC);
    
    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,contA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,contB);
    kMap.insert(KeyAssignment::e_keyNextTrack,contC);
    
    KeyAssignmentWriteXMLKeyMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("KeyAssignmentMap"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(kAssign,writeXMLKeyContainer(writer,KeyAssignment::e_keyPlay,contA)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,writeXMLKeyContainer(writer,KeyAssignment::e_keyPreviousTrack,contB)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,writeXMLKeyContainer(writer,KeyAssignment::e_keyNextTrack,contC)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,xmlTextWriterEndElement(writer)).Times(1).WillOnce(Return(-1));
    
    EXPECT_FALSE(kAssign.testWriteXMLKeyMap(writer));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyMapSuccessWithNoContainers)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
        
    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    
    KeyAssignmentWriteXMLKeyMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("KeyAssignmentMap"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(pAPI,xmlTextWriterEndElement(writer)).Times(1).WillOnce(Return(10));
    
    EXPECT_TRUE(kAssign.testWriteXMLKeyMap(writer));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,writeXMLKeyMapSuccessWithThreeContainers)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    
    KeyCodesContainer contA,contB,contC;
    KeyCode kA(123),kB(456),kC(789);
    contA.list().append(kA);
    contB.list().append(kB);
    contC.list().append(kC);
    
    QMap<KeyAssignment::Key,KeyCodesContainer> kMap;
    kMap.insert(KeyAssignment::e_keyPlay,contA);
    kMap.insert(KeyAssignment::e_keyPreviousTrack,contB);
    kMap.insert(KeyAssignment::e_keyNextTrack,contC);
    
    KeyAssignmentWriteXMLKeyMapTest kAssign;
    EXPECT_CALL(kAssign,mapConst()).WillRepeatedly(ReturnRef(kMap));
    EXPECT_CALL(pAPI,xmlTextWriterStartElement(writer,StrEq("KeyAssignmentMap"))).Times(1).WillOnce(Return(10));
    EXPECT_CALL(kAssign,writeXMLKeyContainer(writer,KeyAssignment::e_keyPlay,contA)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,writeXMLKeyContainer(writer,KeyAssignment::e_keyPreviousTrack,contB)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(kAssign,writeXMLKeyContainer(writer,KeyAssignment::e_keyNextTrack,contC)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,xmlTextWriterEndElement(writer)).Times(1).WillOnce(Return(10));
    
    EXPECT_TRUE(kAssign.testWriteXMLKeyMap(writer));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

class KeyAssignmentSaveToXMLTest : public KeyAssignmentTest
{
    public:
        MOCK_CONST_METHOD1(writeXMLKeyMap,bool(xmlTextWriterPtr pWriter));
};

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,saveToXMLGivenFailureToCreateWriter)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    QString fileName("key.xml");
    
    KeyAssignmentSaveToXMLTest kAssign;
    EXPECT_CALL(pAPI,xmlNewTextWriterDoc(A<xmlDocPtr *>(),Eq(0))).Times(1).WillOnce(Return((xmlTextWriterPtr)0));
    
    EXPECT_FALSE(kAssign.saveToXML(fileName));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,saveToXMLGivenFailureToWriteXML)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    char eMem[] = "mock_doc";
    xmlDocPtr doc = reinterpret_cast<xmlDocPtr>(&eMem);
    
    QString fileName("key.xml");
    
    KeyAssignmentSaveToXMLTest kAssign;
    EXPECT_CALL(pAPI,xmlNewTextWriterDoc(A<xmlDocPtr *>(),Eq(0))).Times(1)
        .WillOnce(DoAll(SetArgPointee<0>(doc),Return(writer)));
    EXPECT_CALL(kAssign,writeXMLKeyMap(writer)).Times(1).WillOnce(Return(false));
    EXPECT_CALL(pAPI,xmlFreeTextWriter(writer)).Times(1);
    EXPECT_CALL(pAPI,xmlFreeDoc(doc)).Times(1);
    
    EXPECT_FALSE(kAssign.saveToXML(fileName));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,saveToXMLGivenFailureToWriteFile)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    char eMem[] = "mock_doc";
    xmlDocPtr doc = reinterpret_cast<xmlDocPtr>(&eMem);
    
    QString fileName("key.xml");
    
    KeyAssignmentSaveToXMLTest kAssign;
    EXPECT_CALL(pAPI,xmlNewTextWriterDoc(A<xmlDocPtr *>(),Eq(0))).Times(1)
        .WillOnce(DoAll(SetArgPointee<0>(doc),Return(writer)));
    EXPECT_CALL(kAssign,writeXMLKeyMap(writer)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,xmlSaveFileEnc(StrEq("key.xml"),doc,StrEq("utf-8"))).Times(1).WillOnce(Return(-1));
    EXPECT_CALL(pAPI,xmlFreeTextWriter(writer)).Times(1);
    EXPECT_CALL(pAPI,xmlFreeDoc(doc)).Times(1);
    
    EXPECT_FALSE(kAssign.saveToXML(fileName));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,saveToXMLWhenSuccessful)
{
    XMLLibIFSPtr pMockAPI = XMLLibIF::instance("mock");
    XMLLibMockIF& pAPI = dynamic_cast<XMLLibMockIF&>(*(pMockAPI.data()));

    char dMem[] = "mock_writer";
    xmlTextWriterPtr writer = reinterpret_cast<xmlTextWriterPtr>(&dMem);
    char eMem[] = "mock_doc";
    xmlDocPtr doc = reinterpret_cast<xmlDocPtr>(&eMem);
    
    QString fileName("key.xml");
    
    KeyAssignmentSaveToXMLTest kAssign;
    EXPECT_CALL(pAPI,xmlNewTextWriterDoc(A<xmlDocPtr *>(),Eq(0))).Times(1)
        .WillOnce(DoAll(SetArgPointee<0>(doc),Return(writer)));
    EXPECT_CALL(kAssign,writeXMLKeyMap(writer)).Times(1).WillOnce(Return(true));
    EXPECT_CALL(pAPI,xmlSaveFileEnc(StrEq("key.xml"),Eq(doc),StrEq("utf-8"))).Times(1).WillOnce(Return(40));
    EXPECT_CALL(pAPI,xmlFreeTextWriter(writer)).Times(1);
    EXPECT_CALL(pAPI,xmlFreeDoc(doc)).Times(1);
    
    EXPECT_TRUE(kAssign.saveToXML(fileName));
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,getDisplayNameForKeyWithPlay)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testGetDisplayNameForKey(KeyAssignment::e_keyPlay)=="Play / Pause");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,getDisplayNameForKeyWithPreviousTrack)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testGetDisplayNameForKey(KeyAssignment::e_keyPreviousTrack)=="Previous Track");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,getDisplayNameForKeyWithNextTrack)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testGetDisplayNameForKey(KeyAssignment::e_keyNextTrack)=="Next Track");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,getDisplayNameForKeyWithVolumeDown)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testGetDisplayNameForKey(KeyAssignment::e_keyVolumeDown)=="Volume Down");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,getDisplayNameForKeyWithVolumeUp)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testGetDisplayNameForKey(KeyAssignment::e_keyVolumeUp)=="Volume Up");
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,getDisplayNameForKeyWithUnassigned)
{
    KeyAssignmentTest kAssign;
    EXPECT_TRUE(kAssign.testGetDisplayNameForKey(KeyAssignment::e_keyUnassigned)=="Unassigned");
}

//-------------------------------------------------------------------------------------------
//<?xml version="1.0"?>
//<KeyAssignmentMap>
//    <command id="play">
//        <key>0x00000020</key>
//        <key>0x04000050</key>
//        <remote>play</remote>
//        <remote>pause</remote>
//    </command>
//    <command id="previous_track">
//        <key>0x05000012</key>
//        <remote>search&lt;&lt;</remote>
//    </command>
//    <command id="next_track">
//        <key>0x05000014</key>
//        <remote>search&gt;&gt;</remote>
//    </command>
//    <command id="volume_down">
//        <key>0x0000002d</key>
//        <remote>ch-</remote>
//    </command>
//    <command id="volume_up">
//        <key>0x0000003d</key>
//        <remote>ch+</remote>
//    </command>
// </KeyAssignmentMap>
//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadFromXMLIntegration)
{
    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");

    KeyAssignment kAssign;
    EXPECT_TRUE(kAssign.loadFromXML(":/remote/Resources/remote/keyassignment.xml"));

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Space)))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_P) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("play")))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("pause")))==KeyAssignment::e_keyPlay);
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Left) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("search<<")))==KeyAssignment::e_keyPreviousTrack);
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Right) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyNextTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("search>>")))==KeyAssignment::e_keyNextTrack);
    
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint>(Qt::Key_Minus)))==KeyAssignment::e_keyVolumeDown);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("ch-")))==KeyAssignment::e_keyVolumeDown);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint>(Qt::Key_Equal)))==KeyAssignment::e_keyVolumeUp);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("ch+")))==KeyAssignment::e_keyVolumeUp);

    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,saveToXMLIntegration)
{
    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");
    
    KeyAssignment kLoadAssign;
    EXPECT_TRUE(kLoadAssign.loadFromXML(":/remote/Resources/remote/keyassignment.xml"));

    QString fileName = omega::common::DiskOps::mergeName(omega::track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"key_save_test.xml");
    EXPECT_TRUE(kLoadAssign.saveToXML(fileName));
    
    KeyAssignment kAssign;
    EXPECT_TRUE(kAssign.loadFromXML(fileName));

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Space)))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_P) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("play")))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("pause")))==KeyAssignment::e_keyPlay);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Left) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("search<<")))==KeyAssignment::e_keyPreviousTrack);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Right) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyNextTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("search>>")))==KeyAssignment::e_keyNextTrack);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint>(Qt::Key_Minus)))==KeyAssignment::e_keyVolumeDown);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("ch-")))==KeyAssignment::e_keyVolumeDown);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint>(Qt::Key_Equal)))==KeyAssignment::e_keyVolumeUp);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("ch+")))==KeyAssignment::e_keyVolumeUp);
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------


TEST(KeyAssignment,saveToXMLOverwriteIntegration)
{
    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");

    KeyAssignment kLoadAssign;
    EXPECT_TRUE(kLoadAssign.loadFromXML(":/remote/Resources/remote/keyassignment.xml"));

    QString fileName = omega::common::DiskOps::mergeName(omega::track::model::TrackDBTestEnviroment::instance()->getDBDirectory(),"key_save_test.xml");
    EXPECT_TRUE(kLoadAssign.saveToXML(fileName));
    EXPECT_TRUE(kLoadAssign.saveToXML(fileName));
    EXPECT_TRUE(kLoadAssign.saveToXML(fileName));

    KeyAssignment kAssign;
    EXPECT_TRUE(kAssign.loadFromXML(fileName));

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Space)))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_P) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("play")))==KeyAssignment::e_keyPlay);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("pause")))==KeyAssignment::e_keyPlay);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Left) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("search<<")))==KeyAssignment::e_keyPreviousTrack);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Right) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyNextTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("search>>")))==KeyAssignment::e_keyNextTrack);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint>(Qt::Key_Minus)))==KeyAssignment::e_keyVolumeDown);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("ch-")))==KeyAssignment::e_keyVolumeDown);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint>(Qt::Key_Equal)))==KeyAssignment::e_keyVolumeUp);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("ch+")))==KeyAssignment::e_keyVolumeUp);
    
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,loadFromModelIntegration)
{
    QStandardItemModel model(5,1);
    
    KeyCodesContainer playKeys;
    playKeys.list().append(KeyCode(static_cast<tuint32>(Qt::Key_Space)));
    playKeys.list().append(KeyCode(static_cast<tuint32>(Qt::Key_P) | static_cast<tuint32>(Qt::ControlModifier)));
    playKeys.list().append(KeyCode(QString::fromLatin1("play")));
    playKeys.list().append(KeyCode(QString::fromLatin1("pause")));
    
    KeyCodesContainer prevKeys;
    prevKeys.list().append(KeyCode(static_cast<tuint32>(Qt::Key_Left) | static_cast<tuint32>(Qt::ControlModifier)));
    prevKeys.list().append(KeyCode(QString::fromLatin1("search<<")));
    
    KeyCodesContainer nextKeys;
    nextKeys.list().append(KeyCode(static_cast<tuint32>(Qt::Key_Right) | static_cast<tuint32>(Qt::ControlModifier)));
    nextKeys.list().append(KeyCode(QString::fromLatin1("search>>")));
    
    KeyCodesContainer vDownKeys;
    vDownKeys.list().append(KeyCode(static_cast<tuint>(Qt::Key_Minus)));
    vDownKeys.list().append(KeyCode(QString::fromLatin1("ch-")));
    
    KeyCodesContainer vUpKeys;
    vUpKeys.list().append(KeyCode(static_cast<tuint>(Qt::Key_Equal)));
    vUpKeys.list().append(KeyCode(QString::fromLatin1("ch+")));

    model.setData(model.index(0,0,QModelIndex()),playKeys.variant(),Qt::EditRole);
    model.setData(model.index(1,0,QModelIndex()),prevKeys.variant(),Qt::EditRole);
    model.setData(model.index(2,0,QModelIndex()),nextKeys.variant(),Qt::EditRole);
    model.setData(model.index(3,0,QModelIndex()),vDownKeys.variant(),Qt::EditRole);
    model.setData(model.index(4,0,QModelIndex()),vUpKeys.variant(),Qt::EditRole);
    
    KeyAssignment kAssign;
    EXPECT_TRUE(kAssign.loadFromModel(&model));

    KeyAssignment::Key k;
    k = kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Space)));
    EXPECT_TRUE(k==KeyAssignment::e_keyPlay);
    k = kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_P) | static_cast<tuint32>(Qt::ControlModifier)));
    EXPECT_TRUE(k==KeyAssignment::e_keyPlay);
    k = kAssign.assignedTo(KeyCode(QString::fromLatin1("play")));
    EXPECT_TRUE(k==KeyAssignment::e_keyPlay);
    k = kAssign.assignedTo(KeyCode(QString::fromLatin1("pause")));
    EXPECT_TRUE(k==KeyAssignment::e_keyPlay);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Left) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyPreviousTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("search<<")))==KeyAssignment::e_keyPreviousTrack);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint32>(Qt::Key_Right) | static_cast<tuint32>(Qt::ControlModifier)))==KeyAssignment::e_keyNextTrack);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("search>>")))==KeyAssignment::e_keyNextTrack);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint>(Qt::Key_Minus)))==KeyAssignment::e_keyVolumeDown);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("ch-")))==KeyAssignment::e_keyVolumeDown);

    EXPECT_TRUE(kAssign.assignedTo(KeyCode(static_cast<tuint>(Qt::Key_Equal)))==KeyAssignment::e_keyVolumeUp);
    EXPECT_TRUE(kAssign.assignedTo(KeyCode(QString::fromLatin1("ch+")))==KeyAssignment::e_keyVolumeUp);
}

//-------------------------------------------------------------------------------------------

TEST(KeyAssignment,saveToModelIntegration)
{
    XMLLibIFSPtr pAPI = XMLLibIF::instance("xml");

    KeyAssignment kLoadAssign;
    EXPECT_TRUE(kLoadAssign.loadFromXML(":/remote/Resources/remote/keyassignment.xml"));

    QStandardItemModel *model = kLoadAssign.saveToModel();
    EXPECT_TRUE(model!=0);
    
    EXPECT_TRUE(model->rowCount()==5);
    EXPECT_TRUE(model->columnCount()==1);
    
    KeyCodesContainer playKeys(model->data(model->index(0,0,QModelIndex()),Qt::EditRole));
    EXPECT_TRUE(playKeys.list().indexOf(KeyCode(static_cast<tuint32>(Qt::Key_Space)))>=0);
    EXPECT_TRUE(playKeys.list().indexOf(KeyCode(static_cast<tuint32>(Qt::Key_P) | static_cast<tuint32>(Qt::ControlModifier)))>=0);
    EXPECT_TRUE(playKeys.list().indexOf(KeyCode(QString::fromLatin1("play")))>=0);
    EXPECT_TRUE(playKeys.list().indexOf(KeyCode(QString::fromLatin1("pause")))>=0);
    
    KeyCodesContainer prevKeys(model->data(model->index(1,0,QModelIndex()),Qt::EditRole));
    EXPECT_TRUE(prevKeys.list().indexOf(KeyCode(static_cast<tuint32>(Qt::Key_Left) | static_cast<tuint32>(Qt::ControlModifier)))>=0);
    EXPECT_TRUE(prevKeys.list().indexOf(KeyCode(QString::fromLatin1("search<<")))>=0);
    
    KeyCodesContainer nextKeys(model->data(model->index(2,0,QModelIndex()),Qt::EditRole));
    EXPECT_TRUE(nextKeys.list().indexOf(KeyCode(static_cast<tuint32>(Qt::Key_Right) | static_cast<tuint32>(Qt::ControlModifier)))>=0);
    EXPECT_TRUE(nextKeys.list().indexOf(KeyCode(QString::fromLatin1("search>>")))>=0);
    
    KeyCodesContainer vDownKeys(model->data(model->index(3,0,QModelIndex()),Qt::EditRole));
    EXPECT_TRUE(vDownKeys.list().indexOf(KeyCode(static_cast<tuint>(Qt::Key_Minus)))>=0);
    EXPECT_TRUE(vDownKeys.list().indexOf(KeyCode(QString::fromLatin1("ch-")))>=0);
    
    KeyCodesContainer vUpKeys(model->data(model->index(4,0,QModelIndex()),Qt::EditRole));
    EXPECT_TRUE(vUpKeys.list().indexOf(KeyCode(static_cast<tuint>(Qt::Key_Equal)))>=0);
    EXPECT_TRUE(vUpKeys.list().indexOf(KeyCode(QString::fromLatin1("ch+")))>=0);
    
    EXPECT_TRUE(model->data(model->index(0,0,QModelIndex()),Qt::UserRole).toString()=="Play / Pause");
    EXPECT_TRUE(model->data(model->index(1,0,QModelIndex()),Qt::UserRole).toString()=="Previous Track");
    EXPECT_TRUE(model->data(model->index(2,0,QModelIndex()),Qt::UserRole).toString()=="Next Track");
    EXPECT_TRUE(model->data(model->index(3,0,QModelIndex()),Qt::UserRole).toString()=="Volume Down");
    EXPECT_TRUE(model->data(model->index(4,0,QModelIndex()),Qt::UserRole).toString()=="Volume Up");
    
    EXPECT_TRUE(model->headerData(0,Qt::Vertical).toString()=="Play / Pause");
    EXPECT_TRUE(model->headerData(1,Qt::Vertical).toString()=="Previous Track");
    EXPECT_TRUE(model->headerData(2,Qt::Vertical).toString()=="Next Track");
    EXPECT_TRUE(model->headerData(3,Qt::Vertical).toString()=="Volume Down");
    EXPECT_TRUE(model->headerData(4,Qt::Vertical).toString()=="Volume Up");

    delete model;
    XMLLibIF::release();
}

//-------------------------------------------------------------------------------------------
