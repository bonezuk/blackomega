#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/model/inc/AlbumModelKey.h"

using namespace testing;
using namespace omega::track::model;

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,constructorDefault)
{
    AlbumModelKey key;
    EXPECT_TRUE(key.isAlbum());
    EXPECT_TRUE(key.isAll());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,constructorStdPair)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,2));
    EXPECT_TRUE(keyA.isAlbum());
    EXPECT_TRUE(keyA.id()==2);

    AlbumModelKey keyB(std::pair<bool,int>(true,3));
    EXPECT_TRUE(keyB.isGroup());
    EXPECT_TRUE(keyB.id()==3);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,constructorCopy)
{
    AlbumModelKey keyAC(std::pair<bool,int>(false,2));
    AlbumModelKey keyA(keyAC);
    EXPECT_TRUE(keyA.isAlbum());
    EXPECT_TRUE(keyA.id()==2);

    AlbumModelKey keyBC(std::pair<bool,int>(true,3));
    AlbumModelKey keyB(keyBC);
    EXPECT_TRUE(keyB.isGroup());
    EXPECT_TRUE(keyB.id()==3);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,constructorVariant)
{
    QVariant vA;
    AlbumModelKey keyA(vA);
    EXPECT_TRUE(keyA.isAlbum());
    EXPECT_TRUE(keyA.isAll());

    QVariant vB(false);
    AlbumModelKey keyB(vB);
    EXPECT_TRUE(keyB.isAlbum());
    EXPECT_TRUE(keyB.isAll());

    QVariant vC(3);
    AlbumModelKey keyC(vC);
    EXPECT_TRUE(keyC.isAlbum());
    EXPECT_TRUE(keyC.isAll());

    QList<QVariant> listD;
    listD.append(QVariant(false));
    listD.append(QVariant(2));
    QVariant vD(listD);
    AlbumModelKey keyD(vD);
    EXPECT_TRUE(keyD.isAlbum());
    EXPECT_TRUE(keyD.id()==2);

    QList<QVariant> listE;
    listE.append(QVariant(true));
    listE.append(QVariant(3));
    QVariant vE(listE);
    AlbumModelKey keyE(vE);
    EXPECT_TRUE(keyE.isGroup());
    EXPECT_TRUE(keyE.id()==3);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,equalVariant)
{
    QVariant vA;
    AlbumModelKey keyA;
    keyA = vA;
    EXPECT_TRUE(keyA.isAlbum());
    EXPECT_TRUE(keyA.isAll());

    QVariant vB(false);
    AlbumModelKey keyB;
    keyB = vB;
    EXPECT_TRUE(keyB.isAlbum());
    EXPECT_TRUE(keyB.isAll());

    QVariant vC(3);
    AlbumModelKey keyC;
    keyC = vC;
    EXPECT_TRUE(keyC.isAlbum());
    EXPECT_TRUE(keyC.isAll());

    QList<QVariant> listD;
    listD.append(QVariant(false));
    listD.append(QVariant(2));
    QVariant vD(listD);
    AlbumModelKey keyD;
    keyD = vD;
    EXPECT_TRUE(keyD.isAlbum());
    EXPECT_TRUE(keyD.id()==2);

    QList<QVariant> listE;
    listE.append(QVariant(true));
    listE.append(QVariant(3));
    QVariant vE(listE);
    AlbumModelKey keyE;
    keyE = vE;
    EXPECT_TRUE(keyE.isGroup());
    EXPECT_TRUE(keyE.id()==3);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,variant)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,2));
    QVariant vA = keyA.variant();
    AlbumModelKey keyAV(vA);
    EXPECT_TRUE(keyAV.isAlbum());
    EXPECT_TRUE(keyAV.id()==2);

    AlbumModelKey keyB(std::pair<bool,int>(true,3));
    QVariant vB = keyB.variant();
    AlbumModelKey keyBV(vB);
    EXPECT_TRUE(keyBV.isGroup());
    EXPECT_TRUE(keyBV.id()==3);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,equalOperator)
{
    AlbumModelKey keyA;
    AlbumModelKey keyAC(std::pair<bool,int>(false,2));
    keyA = keyAC;
    EXPECT_TRUE(keyA.isAlbum());
    EXPECT_TRUE(keyA.id()==2);

    AlbumModelKey keyB;
    AlbumModelKey keyBC(std::pair<bool,int>(true,3));
    keyB = keyBC;
    EXPECT_TRUE(keyB.isGroup());
    EXPECT_TRUE(keyB.id()==3);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,isAll)
{
    AlbumModelKey keyA(std::pair<bool,int>(true,-1));
    EXPECT_TRUE(keyA.isAll());

    AlbumModelKey keyB(std::pair<bool,int>(false,-1));
    EXPECT_TRUE(keyB.isAll());

    AlbumModelKey keyC(std::pair<bool,int>(false,0));
    EXPECT_FALSE(keyC.isAll());

    AlbumModelKey keyD(std::pair<bool,int>(false,1));
    EXPECT_FALSE(keyD.isAll());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,setAsGroup)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,2));
    EXPECT_FALSE(keyA.isGroup());
    keyA.setAsGroup();
    EXPECT_TRUE(keyA.isGroup());

    AlbumModelKey keyB(std::pair<bool,int>(false,-1));
    EXPECT_FALSE(keyB.isGroup());
    keyB.setAsGroup();
    EXPECT_TRUE(keyB.isGroup());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,setAsAlbum)
{
    AlbumModelKey keyA(std::pair<bool,int>(true,2));
    EXPECT_FALSE(keyA.isAlbum());
    keyA.setAsAlbum();
    EXPECT_TRUE(keyA.isAlbum());

    AlbumModelKey keyB(std::pair<bool,int>(true,-1));
    EXPECT_FALSE(keyB.isAlbum());
    keyB.setAsAlbum();
    EXPECT_TRUE(keyB.isAlbum());
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,compareEqual)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,1));
    AlbumModelKey keyB(std::pair<bool,int>(false,2));
    AlbumModelKey keyC(std::pair<bool,int>(true,1));
    AlbumModelKey keyD(std::pair<bool,int>(true,2));
    AlbumModelKey keyE(std::pair<bool,int>(false,1));
    AlbumModelKey keyF(std::pair<bool,int>(false,2));

    EXPECT_TRUE(keyA==keyA);
    EXPECT_FALSE(keyA==keyB);
    EXPECT_FALSE(keyA==keyC);
    EXPECT_FALSE(keyA==keyD);
    EXPECT_TRUE(keyA==keyE);
    EXPECT_FALSE(keyA==keyF);

    EXPECT_FALSE(keyB==keyA);
    EXPECT_TRUE(keyB==keyB);
    EXPECT_FALSE(keyB==keyC);
    EXPECT_FALSE(keyB==keyD);
    EXPECT_FALSE(keyB==keyE);
    EXPECT_TRUE(keyB==keyF);
}

//-------------------------------------------------------------------------------------------

TEST(AlbumModelKey,compareNotEqual)
{
    AlbumModelKey keyA(std::pair<bool,int>(false,1));
    AlbumModelKey keyB(std::pair<bool,int>(false,2));
    AlbumModelKey keyC(std::pair<bool,int>(true,1));
    AlbumModelKey keyD(std::pair<bool,int>(true,2));
    AlbumModelKey keyE(std::pair<bool,int>(false,1));
    AlbumModelKey keyF(std::pair<bool,int>(false,2));

    EXPECT_FALSE(keyA!=keyA);
    EXPECT_TRUE(keyA!=keyB);
    EXPECT_TRUE(keyA!=keyC);
    EXPECT_TRUE(keyA!=keyD);
    EXPECT_FALSE(keyA!=keyE);
    EXPECT_TRUE(keyA!=keyF);

    EXPECT_TRUE(keyB!=keyA);
    EXPECT_FALSE(keyB!=keyB);
    EXPECT_TRUE(keyB!=keyC);
    EXPECT_TRUE(keyB!=keyD);
    EXPECT_TRUE(keyB!=keyE);
    EXPECT_FALSE(keyB!=keyF);
}

//-------------------------------------------------------------------------------------------
