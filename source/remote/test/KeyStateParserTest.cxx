#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/KeyStateParser.h"

using namespace omega::remote;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(KeyStateParserItem,constructor)
{
    KeyStateParser::Item item;
    EXPECT_TRUE(item.map().isEmpty());
    EXPECT_TRUE(item.value()==0);
}

//-------------------------------------------------------------------------------------------

class KeyStateParserItemFreeTest : public KeyStateParser::Item
{
    public:
        MOCK_METHOD1(deleteItem,void(KeyStateParser::Item *item));
        void testFreeMap(QMap<int,Item *>& cMap);
};

void KeyStateParserItemFreeTest::testFreeMap(QMap<int,Item *>& cMap)
{
    freeMap(cMap);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParserItem,isNode)
{
    KeyStateParser::Item item;
    EXPECT_TRUE(item.isNode());
    KeyStateParser::Item *cItem = new KeyStateParser::Item;
    item.map().insert(1,cItem);
    EXPECT_FALSE(item.isNode());
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParserItem,setValue)
{
    KeyStateParser::Item item;
    item.setValue(3);
    EXPECT_TRUE(item.value()==3);
    item.setValue(8);
    EXPECT_TRUE(item.value()==8);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParserItem,find)
{
    KeyStateParser::Item pItem;
    KeyStateParser::Item *cItemA = new KeyStateParser::Item;
    KeyStateParser::Item *cItemB = new KeyStateParser::Item;
    KeyStateParser::Item *cItemC = new KeyStateParser::Item;

    pItem.map().insert(2,cItemA);
    pItem.map().insert(4,cItemB);
    pItem.map().insert(6,cItemC);

    EXPECT_TRUE(pItem.find(1)==0);
    EXPECT_TRUE(pItem.find(2)==cItemA);
    EXPECT_TRUE(pItem.find(3)==0);
    EXPECT_TRUE(pItem.find(4)==cItemB);
    EXPECT_TRUE(pItem.find(5)==0);
    EXPECT_TRUE(pItem.find(6)==cItemC);
    EXPECT_TRUE(pItem.find(7)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParserItem,freeMapEmptyMap)
{
    QMap<int,KeyStateParser::Item *> cMap;
    KeyStateParserItemFreeTest pItem;
    pItem.testFreeMap(pItem.map());
    EXPECT_TRUE(pItem.map().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParserItem,freeMapWithContent)
{
    QMap<int,KeyStateParser::Item *> cMap;
    KeyStateParserItemFreeTest pItem;
    KeyStateParser::Item cItemA;
    KeyStateParser::Item cItemB;
    KeyStateParser::Item cItemC;

    cMap.insert(1,&cItemA);
    cMap.insert(2,&cItemB);
    cMap.insert(3,&cItemC);

    EXPECT_CALL(pItem,deleteItem(&cItemA)).Times(1);
    EXPECT_CALL(pItem,deleteItem(&cItemB)).Times(1);
    EXPECT_CALL(pItem,deleteItem(&cItemC)).Times(1);

    pItem.testFreeMap(cMap);

    EXPECT_TRUE(cMap.isEmpty());
}

//-------------------------------------------------------------------------------------------

class KeyStateParserItemMock : public KeyStateParser::Item
{
    public:
        MOCK_CONST_METHOD0(isNode,bool());
        MOCK_CONST_METHOD0(value,int());
        MOCK_METHOD1(setValue,void(int v));
        MOCK_CONST_METHOD1(find,const KeyStateParser::Item *(int entry));
        MOCK_METHOD0(map,QMap<int,Item *>&());
};

class KeyStateParserTest : public KeyStateParser
{
    public:
        MOCK_METHOD0(getTopItems,QMap<int,KeyStateParser::Item *>&());
        MOCK_CONST_METHOD0(getTopItemsConst,const QMap<int,KeyStateParser::Item *>&());
        MOCK_METHOD0(getNextSequenceID,int());
        MOCK_METHOD0(getSequenceLengthMap,QMap<int,int>&());
        MOCK_CONST_METHOD0(getSequenceLengthMapConst,const QMap<int,int>&());
        MOCK_CONST_METHOD4(findRecursiveIter,int(const int *mem,int len,const Item *parent,int index));

        int testFindRecursive(const int *mem,int len,const Item *parent,int index) const;
        int testGetNextSequenceID();
};

int KeyStateParserTest::testFindRecursive(const int *mem,int len,const Item *parent,int index) const
{
    return findRecursive(mem,len,parent,index);
}

int KeyStateParserTest::testGetNextSequenceID()
{
    return KeyStateParser::getNextSequenceID();
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findRecursiveEqualToLength)
{
    KeyStateParserItemMock pItem;

    int mem[] = { 2, 8, 10, 4 };
    KeyStateParserTest parser;
    EXPECT_TRUE(parser.testFindRecursive(mem,4,&pItem,4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findRecursiveGreaterThanLength)
{
    KeyStateParserItemMock pItem;

    int mem[] = { 2, 8, 10, 4 };
    KeyStateParserTest parser;
    EXPECT_TRUE(parser.testFindRecursive(mem,4,&pItem,5)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findRecursiveNoItemNextInSequence)
{
    KeyStateParserItemMock pItem;
    EXPECT_CALL(pItem,find(8)).Times(1).WillOnce(Return((KeyStateParser::Item *)0));

    int mem[] = { 2, 8, 10, 4 };
    KeyStateParserTest parser;
    EXPECT_TRUE(parser.testFindRecursive(mem,4,&pItem,1)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findRecursiveGetNextInSequenceWhenNotEndOfSequence)
{
    KeyStateParserItemMock cItem;
    EXPECT_CALL(cItem,isNode()).Times(1).WillOnce(Return(false));

    KeyStateParserItemMock pItem;
    EXPECT_CALL(pItem,find(8)).Times(1).WillOnce(Return(&cItem));

    int mem[] = { 2, 8, 10, 4 };
    KeyStateParserTest parser;
    EXPECT_CALL(parser,findRecursiveIter(mem,4,&cItem,2)).Times(1).WillOnce(Return(6));

    EXPECT_TRUE(parser.testFindRecursive(mem,4,&pItem,1)==6);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findRecursiveEndOfSequence)
{
    KeyStateParserItemMock cItem;
    EXPECT_CALL(cItem,isNode()).Times(1).WillOnce(Return(true));
    EXPECT_CALL(cItem,value()).Times(1).WillOnce(Return(5));

    KeyStateParserItemMock pItem;
    EXPECT_CALL(pItem,find(10)).Times(1).WillOnce(Return(&cItem));

    int mem[] = { 2, 8, 10, 4 };
    KeyStateParserTest parser;
    EXPECT_TRUE(parser.testFindRecursive(mem,4,&pItem,2)==5);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findNoMemory)
{
    KeyStateParserTest parser;

    int offset = 0;
    EXPECT_TRUE(parser.find(0,10,offset)==0);
    EXPECT_TRUE(offset==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findNoLength)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    QMap<int,KeyStateParser::Item *> topItemMap;

    KeyStateParserTest parser;

    int offset = 0;
    EXPECT_TRUE(parser.find(mem,0,offset)==0);
    EXPECT_TRUE(offset==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findOffsetEqualToLength)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserTest parser;

    int offset = 10;
    EXPECT_TRUE(parser.find(mem,10,offset)==0);
    EXPECT_TRUE(offset==10);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findOffsetGreaterThanLength)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserTest parser;

    int offset = 11;
    EXPECT_TRUE(parser.find(mem,10,offset)==0);
    EXPECT_TRUE(offset==11);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findNoSequenceFound)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    QMap<int,KeyStateParser::Item *> topItemMap;

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));

    int offset = 0;
    EXPECT_TRUE(parser.find(mem,10,offset)==0);
    EXPECT_TRUE(offset==10);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findSingleSequenceAtBegining)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserItemMock itemD; // 8 = 5
    EXPECT_CALL(itemD,isNode()).WillRepeatedly(Return(true));
    EXPECT_CALL(itemD,value()).WillRepeatedly(Return(5));

    QMap<int,KeyStateParser::Item *> topItemMap;
    topItemMap.insert(8,&itemD);

    QMap<int,int> sequenceMap;
    sequenceMap.insert(5,1);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(sequenceMap));

    int offset = 5;
    EXPECT_TRUE(parser.find(mem,10,offset)==5);
    EXPECT_TRUE(offset==6);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findSingleSequenceAfterBegining)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserItemMock itemD; // 8 = 5
    EXPECT_CALL(itemD,isNode()).WillRepeatedly(Return(true));
    EXPECT_CALL(itemD,value()).WillRepeatedly(Return(5));

    QMap<int,KeyStateParser::Item *> topItemMap;
    topItemMap.insert(8,&itemD);

    QMap<int,int> sequenceMap;
    sequenceMap.insert(5,1);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(sequenceMap));

    int offset = 0;
    EXPECT_TRUE(parser.find(mem,10,offset)==5);
    EXPECT_TRUE(offset==6);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findSingleSequenceAfterSomeHeaders)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserItemMock itemA; // 9, 3, 2, 4, 5 = 1
    EXPECT_CALL(itemA,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemB; // 2, 7, 3 = 2 && 2, 9 = 3
    EXPECT_CALL(itemB,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemC; // 3, 2, 10, 1 = 4
    EXPECT_CALL(itemC,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemD; // 8 = 5
    EXPECT_CALL(itemD,isNode()).WillRepeatedly(Return(true));
    EXPECT_CALL(itemD,value()).Times(1).WillOnce(Return(5));

    QMap<int,KeyStateParser::Item *> topItemMap;
    topItemMap.insert(9,&itemA);
    topItemMap.insert(2,&itemB);
    topItemMap.insert(3,&itemC);
    topItemMap.insert(8,&itemD);

    QMap<int,int> sequenceMap;
    sequenceMap.insert(1,5);
    sequenceMap.insert(2,3);
    sequenceMap.insert(3,2);
    sequenceMap.insert(4,4);
    sequenceMap.insert(5,1);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(sequenceMap));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemA,2)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemC,3)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemB,4)).Times(1).WillOnce(Return(0));

    int offset = 0;
    EXPECT_TRUE(parser.find(mem,10,offset)==5);
    EXPECT_TRUE(offset==6);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findSequenceAtBeginningOfGiven)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserItemMock itemA; // 9, 3, 2, 10, 8 = 1
    EXPECT_CALL(itemA,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemB; // 2, 7, 3 = 2 && 2, 9 = 3
    KeyStateParserItemMock itemC; // 3, 2, 10, 1 = 4

    QMap<int,KeyStateParser::Item *> topItemMap;
    topItemMap.insert(9,&itemA);
    topItemMap.insert(2,&itemB);
    topItemMap.insert(3,&itemC);

    QMap<int,int> sequenceMap;
    sequenceMap.insert(1,5);
    sequenceMap.insert(2,3);
    sequenceMap.insert(3,2);
    sequenceMap.insert(4,4);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(sequenceMap));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemA,2)).Times(1).WillOnce(Return(1));

    int offset = 1;
    EXPECT_TRUE(parser.find(mem,10,offset)==1);
    EXPECT_TRUE(offset==6);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findSequenceAfterBeginingOfGiven)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserItemMock itemA; // 9, 3, 2, 10, 8 = 1
    EXPECT_CALL(itemA,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemB; // 2, 7, 3 = 2 && 2, 9 = 3
    KeyStateParserItemMock itemC; // 3, 2, 10, 1 = 4

    QMap<int,KeyStateParser::Item *> topItemMap;
    topItemMap.insert(9,&itemA);
    topItemMap.insert(3,&itemB);
    topItemMap.insert(2,&itemC);

    QMap<int,int> sequenceMap;
    sequenceMap.insert(1,5);
    sequenceMap.insert(2,3);
    sequenceMap.insert(3,2);
    sequenceMap.insert(4,4);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(sequenceMap));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemA,2)).Times(1).WillOnce(Return(1));

    int offset = 0;
    EXPECT_TRUE(parser.find(mem,10,offset)==1);
    EXPECT_TRUE(offset==6);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findSequenceHeadersButNoFullSequence)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserItemMock itemA; // 9, 3, 2, 4, 5 = 1
    EXPECT_CALL(itemA,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemB; // 2, 7, 3 = 2 && 2, 9, 4 = 3
    EXPECT_CALL(itemB,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemC; // 3, 2, 10, 1 = 4
    EXPECT_CALL(itemC,isNode()).WillRepeatedly(Return(false));

    QMap<int,KeyStateParser::Item *> topItemMap;
    topItemMap.insert(9,&itemA);
    topItemMap.insert(2,&itemB);
    topItemMap.insert(3,&itemC);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemA,2)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemC,3)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemB,4)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemB,7)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemA,8)).Times(1).WillOnce(Return(0));

    int offset = 0;
    EXPECT_TRUE(parser.find(mem,10,offset)==0);
    EXPECT_TRUE(offset==10);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findSequenceAfterSomeHeaders)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserItemMock itemA; // 9, 3, 2, 4, 5 = 1
    EXPECT_CALL(itemA,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemB; // 2, 7, 3 = 2 && 2, 9 = 3
    EXPECT_CALL(itemB,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemC; // 3, 2, 10, 1 = 4
    EXPECT_CALL(itemC,isNode()).WillRepeatedly(Return(false));

    QMap<int,KeyStateParser::Item *> topItemMap;
    topItemMap.insert(9,&itemA);
    topItemMap.insert(2,&itemB);
    topItemMap.insert(3,&itemC);

    QMap<int,int> sequenceMap;
    sequenceMap.insert(1,5);
    sequenceMap.insert(2,3);
    sequenceMap.insert(3,2);
    sequenceMap.insert(4,4);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(sequenceMap));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemA,2)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemC,3)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemB,4)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemB,7)).Times(1).WillOnce(Return(3));

    int offset = 0;
    EXPECT_TRUE(parser.find(mem,10,offset)==3);
    EXPECT_TRUE(offset==8);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,findSequenceAfterSomeHeadersWithOffset)
{
    const int mem[] = { 4, 9, 3, 2, 10, 8, 2, 9, 5, 7 };

    KeyStateParserItemMock itemA; // 9, 3, 2, 4, 5 = 1

    KeyStateParserItemMock itemB; // 2, 7, 3 = 2 && 2, 9 = 3
    EXPECT_CALL(itemB,isNode()).WillRepeatedly(Return(false));

    KeyStateParserItemMock itemC; // 3, 2, 10, 1 = 4
    EXPECT_CALL(itemC,isNode()).WillRepeatedly(Return(false));

    QMap<int,KeyStateParser::Item *> topItemMap;
    topItemMap.insert(9,&itemA);
    topItemMap.insert(2,&itemB);
    topItemMap.insert(3,&itemC);

    QMap<int,int> sequenceMap;
    sequenceMap.insert(1,5);
    sequenceMap.insert(2,3);
    sequenceMap.insert(3,2);
    sequenceMap.insert(4,4);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItemsConst()).WillRepeatedly(ReturnRef(topItemMap));
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(sequenceMap));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemC,3)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemB,4)).Times(1).WillOnce(Return(0));
    EXPECT_CALL(parser,findRecursiveIter(mem,10,&itemB,7)).Times(1).WillOnce(Return(3));

    int offset = 2;
    EXPECT_TRUE(parser.find(mem,10,offset)==3);
    EXPECT_TRUE(offset==8);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,getNextSequenceID)
{
    KeyStateParserTest parser;
    EXPECT_TRUE(parser.testGetNextSequenceID()==1);
    EXPECT_TRUE(parser.testGetNextSequenceID()==2);
    EXPECT_TRUE(parser.testGetNextSequenceID()==3);
    EXPECT_TRUE(parser.testGetNextSequenceID()==4);
    EXPECT_TRUE(parser.testGetNextSequenceID()==5);
    EXPECT_TRUE(parser.testGetNextSequenceID()==6);
    EXPECT_TRUE(parser.testGetNextSequenceID()==7);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceWhenNoMemoryGiven)
{
    KeyStateParserTest parser;
    EXPECT_TRUE(parser.sequence(0,10)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceWhenNoLengthGiven)
{
    const int seq[3] = { 6 };

    KeyStateParserTest parser;
    EXPECT_TRUE(parser.sequence(seq,0)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddOneDigitSequenceNotExisting)
{
    const int seq[3] = { 6 };

    QMap<int,int> seqMap;
    QMap<int,KeyStateParser::Item *> topMap;

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));
    EXPECT_CALL(parser,getNextSequenceID()).Times(1).WillOnce(Return(2));
    EXPECT_CALL(parser,getSequenceLengthMap()).WillRepeatedly(ReturnRef(seqMap));

    EXPECT_TRUE(parser.sequence(seq,1)==2);
    EXPECT_TRUE(seqMap.find(2).value()==1);

    EXPECT_TRUE(topMap.size()==1);
    EXPECT_TRUE(topMap.find(6)!=topMap.end());
    KeyStateParser::Item *itemZ = topMap.find(6).value();

    EXPECT_TRUE(itemZ->isNode());
    EXPECT_TRUE(itemZ->map().isEmpty());
    EXPECT_TRUE(itemZ->value()==2);

    topMap.erase(topMap.find(6));
    delete itemZ;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddOneDigitSequenceAlreadyExistingAsSingle)
{
    const int seq[3] = { 6 };

    QMap<int,int> seqMap;

    QMap<int,KeyStateParser::Item *> topMap;
    KeyStateParser::Item *itemA = new KeyStateParser::Item;

    topMap.insert(6,itemA);
    itemA->setValue(2);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));

    EXPECT_TRUE(parser.sequence(seq,1)==2);

    topMap.erase(topMap.find(6));
    delete itemA;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddOneDigitSequenceAlreadyExistingAsSubsetOfLongerSequence)
{
    const int seq[1] = { 6 };

    QMap<int,KeyStateParser::Item *> topMap;
    KeyStateParser::Item *itemA = new KeyStateParser::Item;
    KeyStateParser::Item *itemB = new KeyStateParser::Item;

    topMap.insert(6,itemA);
    itemA->map().insert(2,itemB);
    itemB->setValue(1);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));

    EXPECT_TRUE(parser.sequence(seq,1)==0);

    topMap.erase(topMap.find(6));
    delete itemA;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddThreeDigitSequenceNotExisting)
{
    const int seq[3] = { 6, 5, 4 };

    QMap<int,int> seqMap;

    QMap<int,KeyStateParser::Item *> topMap;
    KeyStateParser::Item *itemA = new KeyStateParser::Item;
    KeyStateParser::Item *itemB = new KeyStateParser::Item;
    KeyStateParser::Item *itemC = new KeyStateParser::Item;

    topMap.insert(6,itemA);
    itemA->map().insert(2,itemB);
    itemB->map().insert(3,itemC);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));
    EXPECT_CALL(parser,getNextSequenceID()).Times(1).WillOnce(Return(6));
    EXPECT_CALL(parser,getSequenceLengthMap()).WillRepeatedly(ReturnRef(seqMap));

    EXPECT_TRUE(parser.sequence(seq,3)==6);
    EXPECT_TRUE(seqMap.find(6).value()==3);

    EXPECT_TRUE(topMap.size()==1);
    EXPECT_TRUE(topMap.find(6)!=topMap.end());
    KeyStateParser::Item *itemZ = topMap.find(6).value();

    EXPECT_FALSE(itemZ->isNode());
    EXPECT_TRUE(itemZ->map().size()==2);
    EXPECT_TRUE(itemZ->map().find(5)!=itemZ->map().end());
    KeyStateParser::Item *itemY = itemZ->map().find(5).value();

    EXPECT_FALSE(itemY->isNode());
    EXPECT_TRUE(itemY->map().size()==1);
    EXPECT_TRUE(itemY->map().find(4)!=itemY->map().end());
    KeyStateParser::Item *itemX = itemY->map().find(4).value();

    EXPECT_TRUE(itemX->isNode());
    EXPECT_TRUE(itemX->map().isEmpty());
    EXPECT_TRUE(itemX->value()==6);

    topMap.erase(topMap.find(6));
    delete itemA;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddThreeDigitSequenceFirstDigitExists)
{
    const int seq[3] = { 6, 5, 4 };

    QMap<int,int> seqMap;

    QMap<int,KeyStateParser::Item *> topMap;
    KeyStateParser::Item *itemA = new KeyStateParser::Item;
    KeyStateParser::Item *itemB = new KeyStateParser::Item;
    KeyStateParser::Item *itemC = new KeyStateParser::Item;

    topMap.insert(6,itemA);
    itemA->map().insert(2,itemB);
    itemB->map().insert(3,itemC);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));
    EXPECT_CALL(parser,getNextSequenceID()).Times(1).WillOnce(Return(6));
    EXPECT_CALL(parser,getSequenceLengthMap()).WillRepeatedly(ReturnRef(seqMap));

    EXPECT_TRUE(parser.sequence(seq,3)==6);
    EXPECT_TRUE(seqMap.find(6).value()==3);

    EXPECT_TRUE(topMap.size()==1);
    EXPECT_TRUE(topMap.find(6)!=topMap.end());
    KeyStateParser::Item *itemZ = topMap.find(6).value();

    EXPECT_FALSE(itemZ->isNode());
    EXPECT_TRUE(itemZ->map().size()==2);
    EXPECT_TRUE(itemZ->map().find(5)!=itemZ->map().end());
    KeyStateParser::Item *itemY = itemZ->map().find(5).value();

    EXPECT_FALSE(itemY->isNode());
    EXPECT_TRUE(itemY->map().size()==1);
    EXPECT_TRUE(itemY->map().find(4)!=itemY->map().end());
    KeyStateParser::Item *itemX = itemY->map().find(4).value();

    EXPECT_TRUE(itemX->isNode());
    EXPECT_TRUE(itemX->map().isEmpty());
    EXPECT_TRUE(itemX->value()==6);

    topMap.erase(topMap.find(6));
    delete itemA;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddThreeDigitSequenceFirstTwoDigitsExists)
{
    const int seq[3] = { 6, 5, 4 };

    QMap<int,int> seqMap;

    QMap<int,KeyStateParser::Item *> topMap;
    KeyStateParser::Item *itemA = new KeyStateParser::Item;
    KeyStateParser::Item *itemB = new KeyStateParser::Item;
    KeyStateParser::Item *itemC = new KeyStateParser::Item;

    topMap.insert(6,itemA);
    itemA->map().insert(5,itemB);
    itemB->map().insert(3,itemC);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));
    EXPECT_CALL(parser,getNextSequenceID()).Times(1).WillOnce(Return(6));
    EXPECT_CALL(parser,getSequenceLengthMap()).WillRepeatedly(ReturnRef(seqMap));

    EXPECT_TRUE(parser.sequence(seq,3)==6);
    EXPECT_TRUE(seqMap.find(6).value()==3);
    EXPECT_TRUE(itemB->map().find(4).value()->isNode());
    EXPECT_TRUE(itemB->map().find(4).value()->value()==6);

    topMap.erase(topMap.find(6));
    delete itemA;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddThreeDigitSequenceAndSequenceAlreadyExists)
{
    const int seq[3] = { 6, 5, 4 };

    QMap<int,KeyStateParser::Item *> topMap;
    KeyStateParser::Item *itemA = new KeyStateParser::Item;
    KeyStateParser::Item *itemB = new KeyStateParser::Item;
    KeyStateParser::Item *itemC = new KeyStateParser::Item;

    topMap.insert(6,itemA);
    itemA->map().insert(5,itemB);
    itemB->map().insert(4,itemC);
    itemC->setValue(1);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));

    EXPECT_TRUE(parser.sequence(seq,3)==1);

    topMap.erase(topMap.find(6));
    delete itemA;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddThreeDigitSequenceFirstTwoDigitsExistsAsCompleteSequence)
{
    const int seq[3] = { 6, 5, 4 };

    QMap<int,KeyStateParser::Item *> topMap;
    KeyStateParser::Item *itemA = new KeyStateParser::Item;
    KeyStateParser::Item *itemB = new KeyStateParser::Item;

    topMap.insert(6,itemA);
    itemA->map().insert(5,itemB);
    itemB->setValue(2);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));

    EXPECT_TRUE(parser.sequence(seq,3)==0);

    topMap.erase(topMap.find(6));
    delete itemA;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceAddThreeDigitSequenceAndAlreadyExistingAsSubsetOfLongerSequence)
{
    const int seq[3] = { 6, 5, 4 };

    QMap<int,KeyStateParser::Item *> topMap;
    KeyStateParser::Item *itemA = new KeyStateParser::Item;
    KeyStateParser::Item *itemB = new KeyStateParser::Item;
    KeyStateParser::Item *itemC = new KeyStateParser::Item;

    topMap.insert(6,itemA);
    itemA->map().insert(5,itemB);
    itemB->map().insert(4,itemC);
    itemC->setValue(3);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getTopItems()).WillRepeatedly(ReturnRef(topMap));

    EXPECT_TRUE(parser.sequence(seq,4)==0);

    topMap.erase(topMap.find(6));
    delete itemA;
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,sequenceBuildExpectedMap)
{
    KeyStateParser parser;

    const int seqA[4] = {6, 5, 4, 3};
    EXPECT_TRUE(parser.sequence(seqA,4)==1);
    EXPECT_TRUE(parser.sequence(seqA,3)==0);
    EXPECT_TRUE(parser.sequence(seqA,2)==0);
    EXPECT_TRUE(parser.sequence(seqA,1)==0);
    EXPECT_TRUE(parser.sequence(seqA,4)==1);

    const int seqB[3] = {1, 2, 3};
    EXPECT_TRUE(parser.sequence(seqB,3)==2);
    EXPECT_TRUE(parser.sequence(seqB,2)==0);
    EXPECT_TRUE(parser.sequence(seqB,1)==0);
    EXPECT_TRUE(parser.sequence(seqB,3)==2);

    const int seqC[1] = {3};
    EXPECT_TRUE(parser.sequence(seqC,1)==3);

    const int seqD[2] = {3,2};
    EXPECT_TRUE(parser.sequence(seqD,2)==0);

    const int seqE[3] = {1, 2, 4};
    EXPECT_TRUE(parser.sequence(seqE,2)==0);
    EXPECT_TRUE(parser.sequence(seqE,3)==4);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,lengthNoSequence)
{
    QMap<int,int> seqLengthMap;
    seqLengthMap.insert(3,5);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(seqLengthMap));

    EXPECT_TRUE(parser.length(4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,lengthWhenSequence)
{
    QMap<int,int> seqLengthMap;
    seqLengthMap.insert(3,5);

    KeyStateParserTest parser;
    EXPECT_CALL(parser,getSequenceLengthMapConst()).WillRepeatedly(ReturnRef(seqLengthMap));

    EXPECT_TRUE(parser.length(3)==5);
}

//-------------------------------------------------------------------------------------------

TEST(KeyStateParser,defineAndFindUsingRemote10_6Codes)
{
    const int c_remotePlusKey[]         = {33, 31, 30, 21 ,20,  2}; // 1 - 6
    const int c_remoteMinusKey[]        = {33, 32, 30, 21, 20,  2}; // 2 - 6
    const int c_remoteMenuKey[]         = {33, 22, 21, 20,  2, 33, 22, 21, 20,  2}; // 3 - 10
    const int c_remotePlayKey[]         = {33, 23, 21, 20,  2, 33, 23, 21, 20,  2}; // 4 - 10
    const int c_remoteRightKey[]        = {33, 24, 21, 20,  2, 33, 24, 21, 20,  2}; // 5 - 10
    const int c_remoteLeftKey[]         = {33, 25, 21, 20,  2, 33, 25, 21, 20,  2}; // 6 - 10
    const int c_remoteRightHoldKey[]    = {33, 21, 20, 14, 12,  2}; // 7 - 6
    const int c_remoteLeftHoldKey[]     = {33, 21, 20, 13, 12,  2}; // 8 - 6
    const int c_remoteMenuHoldKey[]     = {33, 21, 20,  2, 33, 21, 20,  2}; // 9 - 8
    const int c_remotePlayHoldKey[]     = {37, 33, 21, 20,  2, 37, 33, 21, 20,  2}; // 10 - 10
    const int c_remoteControlSwitched[] = {19}; // 11 - 1
    const int c_remotePlayAKey[]        = {33, 21, 20,  8,  2, 33, 21, 20,  8,  2}; // 12 - 10
    const int c_remotePlayBKey[]        = {33, 21, 20,  3,  2, 33, 21, 20,  3,  2}; // 13 - 10
    const int c_remotePlayBHoldKey[]    = {33, 21, 20, 11,  2, 33, 21, 20, 11,  2}; // 14 - 10

    int offset;
    KeyStateParser parser;

    EXPECT_TRUE(parser.isEmpty());
    EXPECT_TRUE(parser.size()==0);

    int entryPlusKey            = parser.sequence(c_remotePlusKey,6);
    EXPECT_TRUE(entryPlusKey==1);
    EXPECT_TRUE(parser.length(entryPlusKey)==6);

    int entryMinusKey           = parser.sequence(c_remoteMinusKey,6);
    EXPECT_TRUE(entryMinusKey==2);
    EXPECT_TRUE(parser.length(entryMinusKey)==6);

    int entryMenuKey            = parser.sequence(c_remoteMenuKey,10);
    EXPECT_TRUE(entryMenuKey==3);
    EXPECT_TRUE(parser.length(entryMenuKey)==10);

    int entryPlayKey            = parser.sequence(c_remotePlayKey,10);
    EXPECT_TRUE(entryPlayKey==4);
    EXPECT_TRUE(parser.length(entryPlayKey)==10);

    int entryRightKey           = parser.sequence(c_remoteRightKey,10);
    EXPECT_TRUE(entryRightKey==5);
    EXPECT_TRUE(parser.length(entryRightKey)==10);

    int entryLeftKey            = parser.sequence(c_remoteLeftKey,10);
    EXPECT_TRUE(entryLeftKey==6);
    EXPECT_TRUE(parser.length(entryLeftKey)==10);

    int entryRightHoldKey       = parser.sequence(c_remoteRightHoldKey,6);
    EXPECT_TRUE(entryRightHoldKey==7);
    EXPECT_TRUE(parser.length(entryRightHoldKey)==6);

    int entryLeftHoldKey        = parser.sequence(c_remoteLeftHoldKey,6);
    EXPECT_TRUE(entryLeftHoldKey==8);
    EXPECT_TRUE(parser.length(entryLeftHoldKey)==6);

    int entryMenuHoldKey        = parser.sequence(c_remoteMenuHoldKey,8);
    EXPECT_TRUE(entryMenuHoldKey==9);
    EXPECT_TRUE(parser.length(entryMenuHoldKey)==8);

    int entryPlayHoldKey        = parser.sequence(c_remotePlayHoldKey,10);
    EXPECT_TRUE(entryPlayHoldKey==10);
    EXPECT_TRUE(parser.length(entryPlayHoldKey)==10);

    int entryControlSwitchedKey = parser.sequence(c_remoteControlSwitched,1);
    EXPECT_TRUE(entryControlSwitchedKey==11);
    EXPECT_TRUE(parser.length(entryControlSwitchedKey)==1);

    int entryPlayAKey           = parser.sequence(c_remotePlayAKey,10);
    EXPECT_TRUE(entryPlayAKey==12);
    EXPECT_TRUE(parser.length(entryPlayAKey)==10);

    int entryPlayBKey           = parser.sequence(c_remotePlayBKey,10);
    EXPECT_TRUE(entryPlayBKey==13);
    EXPECT_TRUE(parser.length(entryPlayBKey)==10);

    int entryPlayBHoldKey       = parser.sequence(c_remotePlayBHoldKey,10);
    EXPECT_TRUE(entryPlayBHoldKey==14);
    EXPECT_TRUE(parser.length(entryPlayBHoldKey)==10);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remotePlusKey,6,offset)==1);
    EXPECT_TRUE(offset==6);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remoteMinusKey,6,offset)==2);
    EXPECT_TRUE(offset==6);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remoteMenuKey,10,offset)==3);
    EXPECT_TRUE(offset==10);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remotePlayKey,10,offset)==4);
    EXPECT_TRUE(offset==10);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remoteRightKey,10,offset)==5);
    EXPECT_TRUE(offset==10);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remoteLeftKey,10,offset)==6);
    EXPECT_TRUE(offset==10);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remoteRightHoldKey,6,offset)==7);
    EXPECT_TRUE(offset==6);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remoteLeftHoldKey,6,offset)==8);
    EXPECT_TRUE(offset==6);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remoteMenuHoldKey,8,offset)==9);
    EXPECT_TRUE(offset==8);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remotePlayHoldKey,10,offset)==10);
    EXPECT_TRUE(offset==10);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remoteControlSwitched,1,offset)==11);
    EXPECT_TRUE(offset==1);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remotePlayAKey,10,offset)==12);
    EXPECT_TRUE(offset==10);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remotePlayBKey,10,offset)==13);
    EXPECT_TRUE(offset==10);

    offset = 0;
    EXPECT_TRUE(parser.find(c_remotePlayBHoldKey,10,offset)==14);
    EXPECT_TRUE(offset==10);

    offset = 0;
    EXPECT_TRUE(parser.find(0,10,offset)==0);
    EXPECT_TRUE(offset==0);

    const int testSeqF[] = {33, 21, 20, 14, 12,  2, 19, 33, 32, 30, 21, 20,  2};
    offset = 0;
    EXPECT_TRUE(parser.find(testSeqF,0,offset)==0);
    EXPECT_TRUE(offset==0);
    EXPECT_TRUE(parser.find(testSeqF,13,offset)==7);
    EXPECT_TRUE(offset==6);
    EXPECT_TRUE(parser.find(testSeqF,13,offset)==11);
    EXPECT_TRUE(offset==7);
    EXPECT_TRUE(parser.find(testSeqF,13,offset)==2);
    EXPECT_TRUE(offset==13);
    EXPECT_TRUE(parser.find(testSeqF,13,offset)==0);
    EXPECT_TRUE(offset==13);

    const int testSeqB[] = {33, 21, 20, 14, 12, 19};
    offset = 0;
    EXPECT_TRUE(parser.find(testSeqB,6,offset)==11);
    EXPECT_TRUE(offset==6);

    const int testSeqC[] = {33, 33, 33, 21, 33, 21, 20,  8,  2, 33, 21, 20,  8,  2};
    offset = 0;
    EXPECT_TRUE(parser.find(testSeqC,14,offset)==12);
    EXPECT_TRUE(offset==14);
    offset = 0;
    EXPECT_TRUE(parser.find(testSeqC,1,offset)==0);

    const int testSeqD[] = {33, 33, 33, 21, 33, 21, 20,  8,  2, 33, 21, 20,  8,  37, 35};
    offset = 0;
    EXPECT_TRUE(parser.find(testSeqD,15,offset)==0);
    EXPECT_TRUE(offset==15);

    const int testSeqE[] = {33, 21, 33, 21, 20, 14, 12,  2,  33, 21, 33, 21, 20, 13, 12,  2, 33};
    offset = 0;
    EXPECT_TRUE(parser.find(testSeqE,17,offset)==7);
    EXPECT_TRUE(offset==8);
    EXPECT_TRUE(parser.find(testSeqE,17,offset)==8);
    EXPECT_TRUE(offset==16);
    EXPECT_TRUE(parser.find(testSeqE,17,offset)==0);
    EXPECT_TRUE(offset==17);
}

//-------------------------------------------------------------------------------------------
