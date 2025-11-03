#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "remote/inc/KeyCodesContainer.h"

#include <QStringList>

using namespace omega::remote;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorDefault)
{
    KeyCode k;
    EXPECT_TRUE(k.isEmpty());
    EXPECT_FALSE(k.isKeyboard());
    EXPECT_FALSE(k.isRemote());
    EXPECT_TRUE(k.keyCode()==0);
    EXPECT_TRUE(k.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorWithKeyInteger)
{
    KeyCode k(12);
    EXPECT_FALSE(k.isEmpty());
    EXPECT_TRUE(k.isKeyboard());
    EXPECT_TRUE(k.keyCode()==12);
    EXPECT_FALSE(k.isRemote());
    EXPECT_TRUE(k.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorWithRemoteString)
{
    KeyCode k(QString::fromLatin1("play"));
    EXPECT_FALSE(k.isEmpty());
    EXPECT_FALSE(k.isKeyboard());
    EXPECT_TRUE(k.keyCode()==0);
    EXPECT_TRUE(k.isRemote());
    EXPECT_TRUE(k.remote()=="play");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorVariantWithNone)
{
    QVariant v;
    KeyCode k(v);
    EXPECT_TRUE(k.isEmpty());
    EXPECT_FALSE(k.isKeyboard());
    EXPECT_FALSE(k.isRemote());
    EXPECT_TRUE(k.keyCode()==0);
    EXPECT_TRUE(k.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorVariantWithKey)
{
    QVariant v;

    KeyCode kA;
    kA.setKeyCode(0x01000032);
    v = kA.variant();

    KeyCode kB(v);
    EXPECT_FALSE(kB.isEmpty());
    EXPECT_TRUE(kB.isKeyboard());
    EXPECT_FALSE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0x01000032);
    EXPECT_TRUE(kB.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorVariantWithRemote)
{
    QVariant v;

    KeyCode kA;
    kA.setRemote("fastforward");
    v = kA.variant();

    KeyCode kB(v);
    EXPECT_FALSE(kB.isEmpty());
    EXPECT_FALSE(kB.isKeyboard());
    EXPECT_TRUE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0);
    EXPECT_TRUE(kB.remote()=="fastforward");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorCopyWithNone)
{
    KeyCode kA;
    KeyCode k(kA);
    EXPECT_TRUE(k.isEmpty());
    EXPECT_FALSE(k.isKeyboard());
    EXPECT_FALSE(k.isRemote());
    EXPECT_TRUE(k.keyCode()==0);
    EXPECT_TRUE(k.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorCopyWithKey)
{
    KeyCode kA;
    kA.setKeyCode(0x01000032);

    KeyCode kB(kA);
    EXPECT_FALSE(kB.isEmpty());
    EXPECT_TRUE(kB.isKeyboard());
    EXPECT_FALSE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0x01000032);
    EXPECT_TRUE(kB.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,constructorCopyWithRemote)
{
    KeyCode kA;
    kA.setRemote("fastforward");

    KeyCode kB(kA);
    EXPECT_FALSE(kB.isEmpty());
    EXPECT_FALSE(kB.isKeyboard());
    EXPECT_TRUE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0);
    EXPECT_TRUE(kB.remote()=="fastforward");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,operatorEqualVariantWithNone)
{
    QVariant v;
    KeyCode k;
    k.setKeyCode(0x32);
    k = v;
    EXPECT_TRUE(k.isEmpty());
    EXPECT_FALSE(k.isKeyboard());
    EXPECT_FALSE(k.isRemote());
    EXPECT_TRUE(k.keyCode()==0);
    EXPECT_TRUE(k.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,operatorEqualVariantWithKey)
{
    QVariant v;

    KeyCode kA;
    kA.setKeyCode(0x01000032);
    v = kA.variant();

    KeyCode kB;
    kB = v;

    EXPECT_FALSE(kB.isEmpty());
    EXPECT_TRUE(kB.isKeyboard());
    EXPECT_FALSE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0x01000032);
    EXPECT_TRUE(kB.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,operatorEqualVariantWithRemote)
{
    QVariant v;

    KeyCode kA;
    kA.setRemote("fastforward");
    v = kA.variant();

    KeyCode kB;
    kB = v;

    EXPECT_FALSE(kB.isEmpty());
    EXPECT_FALSE(kB.isKeyboard());
    EXPECT_TRUE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0);
    EXPECT_TRUE(kB.remote()=="fastforward");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,operatorEqualCopyWithNone)
{
    KeyCode kA;

    KeyCode k;
    k.setKeyCode(0x32);
    k = kA;

    EXPECT_TRUE(k.isEmpty());
    EXPECT_FALSE(k.isKeyboard());
    EXPECT_FALSE(k.isRemote());
    EXPECT_TRUE(k.keyCode()==0);
    EXPECT_TRUE(k.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,operatorEqualCopyWithKey)
{
    KeyCode kA;
    kA.setKeyCode(0x01000032);

    KeyCode kB;
    kB = kA;

    EXPECT_FALSE(kB.isEmpty());
    EXPECT_TRUE(kB.isKeyboard());
    EXPECT_FALSE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0x01000032);
    EXPECT_TRUE(kB.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,operatorEqualCopyWithRemote)
{
    KeyCode kA;
    kA.setRemote("fastforward");

    KeyCode kB;
    kB = kA;

    EXPECT_FALSE(kB.isEmpty());
    EXPECT_FALSE(kB.isKeyboard());
    EXPECT_TRUE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0);
    EXPECT_TRUE(kB.remote()=="fastforward");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,keyCode)
{
    KeyCode kA;
    kA.setKeyCode(0x1fffffff);
    EXPECT_FALSE(kA.isEmpty());
    EXPECT_TRUE(kA.isKeyboard());
    EXPECT_FALSE(kA.isRemote());
    EXPECT_TRUE(kA.keyCode()==0x1fffffff);

    KeyCode kB;
    kB.setKeyCode(0xffffffff);
    EXPECT_FALSE(kB.isEmpty());
    EXPECT_TRUE(kB.isKeyboard());
    EXPECT_FALSE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0x1fffffff);

    KeyCode kC;
    kC.setRemote("remote");
    kC.setKeyCode(345);
    EXPECT_FALSE(kC.isEmpty());
    EXPECT_TRUE(kC.isKeyboard());
    EXPECT_FALSE(kC.isRemote());
    EXPECT_TRUE(kC.keyCode()==345);
    EXPECT_TRUE(kC.remote().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,remote)
{
    KeyCode kA;
    kA.setRemote("remote");
    EXPECT_FALSE(kA.isEmpty());
    EXPECT_FALSE(kA.isKeyboard());
    EXPECT_TRUE(kA.isRemote());
    EXPECT_TRUE(kA.keyCode()==0);
    EXPECT_TRUE(kA.remote()=="remote");

    KeyCode kB;
    kB.setKeyCode(345);
    kB.setRemote("play");
    EXPECT_FALSE(kB.isEmpty());
    EXPECT_FALSE(kB.isKeyboard());
    EXPECT_TRUE(kB.isRemote());
    EXPECT_TRUE(kB.keyCode()==0);
    EXPECT_TRUE(kB.remote()=="play");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenNull)
{
    QVariant v;
    KeyCode k(v);
    EXPECT_TRUE(k.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenEmptyString)
{
    QVariant v("");
    KeyCode k(v);
    EXPECT_TRUE(k.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenInt)
{
    QVariant v(1234);
    KeyCode k(v);
    EXPECT_TRUE(k.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenList)
{
    QVariant vA("k1234");
    QVariant vB("rplay");
    QList<QVariant> list;
    list.append(vA);
    list.append(vB);

    KeyCode k(list);
    EXPECT_TRUE(k.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenInvalidString)
{
    QVariant v("anystring");
    KeyCode k(v);
    EXPECT_TRUE(k.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenInvalidKey)
{
    QVariant v("k");
    KeyCode k(v);
    EXPECT_TRUE(k.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenOverflowKey)
{
    tuint32 kCode = 0xff001234;
    QString kStr = "k" + QString::number(kCode);
    QVariant v(kStr);
    KeyCode k(v);
    EXPECT_TRUE(k.keyCode()==0x1f001234);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenCorrectKey)
{
    int kCode = 0x1f001234;
    QString kStr = "k" + QString::number(kCode);
    QVariant v(kStr);
    KeyCode k(v);
    EXPECT_TRUE(k.keyCode()==kCode);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenInvalidRemote)
{
    QVariant v("r");
    KeyCode k(v);
    EXPECT_TRUE(k.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,copyFromVariantWhenCorrectRemote)
{
    QVariant v("rplay");
    KeyCode k(v);
    EXPECT_TRUE(k.remote()=="play");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,constructorDefault)
{
    KeyCodesContainer c;
    EXPECT_TRUE(c.isEmpty());
    EXPECT_TRUE(c.size()==0);
    EXPECT_TRUE(c.list().size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,copyVariantWithNull)
{
    QVariant v;
    KeyCodesContainer c(v);
    EXPECT_TRUE(c.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,copyVariantWithStringType)
{
    QString str = "k234";
    QVariant vList(str);
    KeyCodesContainer c(vList);
    EXPECT_TRUE(c.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,copyVariantWithEmptyList)
{
    QStringList l;
    QVariant vList(l);
    KeyCodesContainer c(vList);
    EXPECT_TRUE(c.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,copyVariantWithValidListOneElement)
{
    QStringList l;
    l << "k1234";

    QVariant vList(l);
    KeyCodesContainer c(vList);

    EXPECT_TRUE(c.size()==1);
    EXPECT_TRUE(c.at(0).keyCode()==1234);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,copyVariantWithValidListThreeElements)
{
    QStringList l;
    l << "k1234";
    l << "rplay";
    l << "k4567";

    QVariant vList(l);
    KeyCodesContainer c(vList);

    EXPECT_TRUE(c.size()==3);
    EXPECT_TRUE(c.at(0).keyCode()==1234);
    EXPECT_TRUE(c.at(1).remote()=="play");
    EXPECT_TRUE(c.at(2).keyCode()==4567);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,copyVariantWithPartlyValidStringList)
{
    QStringList l;
    l << "k1234";
    l << "not";
    l << "rplay";
    l << "k4567";
    l << "";
    l << "in";
    l << "rstop";

    QVariant vList(l);
    KeyCodesContainer c(vList);

    EXPECT_TRUE(c.size()==4);
    EXPECT_TRUE(c.at(0).keyCode()==1234);
    EXPECT_TRUE(c.at(1).remote()=="play");
    EXPECT_TRUE(c.at(2).keyCode()==4567);
    EXPECT_TRUE(c.at(3).remote()=="stop");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,copyVariantOverwritesPreviousList)
{
    QStringList l;
    l << "k1234";
    l << "rplay";

    QVariant vList(l);
    KeyCodesContainer c(vList);

    QStringList lB;
    lB << "k5678";
    lB << "k90";
    lB << "rstop";
    QVariant vListB(lB);

    c = vListB;

    EXPECT_TRUE(c.size()==3);
    EXPECT_TRUE(c.at(0).keyCode()==5678);
    EXPECT_TRUE(c.at(1).keyCode()==90);
    EXPECT_TRUE(c.at(2).remote()=="stop");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,constructorCopyEmpty)
{
    KeyCodesContainer cA;
    KeyCodesContainer cB(cA);
    EXPECT_TRUE(cB.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,constructorCopyThreeElements)
{
    QStringList lB;
    lB << "k5678";
    lB << "k90";
    lB << "rstop";
    QVariant vListB(lB);

    KeyCodesContainer cA(vListB);
    KeyCodesContainer cB(cA);

    EXPECT_TRUE(cB.size()==3);
    EXPECT_TRUE(cB.at(0).keyCode()==5678);
    EXPECT_TRUE(cB.at(1).keyCode()==90);
    EXPECT_TRUE(cB.at(2).remote()=="stop");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,operatorEqualsVariant)
{
    QStringList lA;
    lA << "k1234";
    lA << "rplay";
    QVariant vListA(lA);

    QStringList lB;
    lB << "k5678";
    lB << "k90";
    lB << "rstop";
    QVariant vListB(lB);

    KeyCodesContainer cB(vListA);
    cB = vListB;

    EXPECT_TRUE(cB.size()==3);
    EXPECT_TRUE(cB.at(0).keyCode()==5678);
    EXPECT_TRUE(cB.at(1).keyCode()==90);
    EXPECT_TRUE(cB.at(2).remote()=="stop");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,operatorEqualsEmpty)
{
    QStringList lA;
    lA << "k1234";
    lA << "rplay";
    QVariant vListA(lA);

    KeyCodesContainer cA;
    KeyCodesContainer cB(vListA);
    cB = cA;

    EXPECT_TRUE(cB.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,operatorEqualsThreeElements)
{
    QStringList lA;
    lA << "k1234";
    lA << "rplay";
    QVariant vListA(lA);

    QStringList lB;
    lB << "k5678";
    lB << "k90";
    lB << "rstop";
    QVariant vListB(lB);

    KeyCodesContainer cA(vListB);
    KeyCodesContainer cB(vListA);
    cB = cA;

    EXPECT_TRUE(cB.size()==3);
    EXPECT_TRUE(cB.at(0).keyCode()==5678);
    EXPECT_TRUE(cB.at(1).keyCode()==90);
    EXPECT_TRUE(cB.at(2).remote()=="stop");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,list)
{
    KeyCodesContainer cA;
    KeyCode kA;
    kA.setKeyCode(1234);
    KeyCode kB;
    kB.setRemote("play");
    cA.list().append(kA);
    cA.list().append(kB);

    EXPECT_TRUE(cA.size()==2);
    EXPECT_TRUE(cA.at(0).keyCode()==1234);
    EXPECT_TRUE(cA.at(1).remote()=="play");
    EXPECT_TRUE(cA.list().at(0).keyCode()==1234);
    EXPECT_TRUE(cA.list().at(1).remote()=="play");
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,variantWhenEmpty)
{
    KeyCodesContainer cA;
    QVariant v = cA.variant();
    KeyCodesContainer cB(v);
    EXPECT_TRUE(cB.isEmpty());
    EXPECT_TRUE(cB.size()==0);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,variantOneElement)
{
    KeyCodesContainer cA;
    KeyCode kA;
    kA.setKeyCode(1234);
    cA.list().append(kA);

    QVariant v = cA.variant();
    KeyCodesContainer cB(v);

    EXPECT_FALSE(cB.isEmpty());
    EXPECT_TRUE(cB.size()==1);
    EXPECT_TRUE(cB.at(0).keyCode()==1234);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,variantThreeElements)
{
    KeyCodesContainer cA;
    KeyCode kA;
    kA.setKeyCode(1234);
    cA.list().append(kA);
    KeyCode kB;
    kB.setKeyCode(5678);
    cA.list().append(kB);
    KeyCode kC;
    kC.setKeyCode(90);
    cA.list().append(kC);

    QVariant v = cA.variant();
    KeyCodesContainer cB(v);

    EXPECT_FALSE(cB.isEmpty());
    EXPECT_TRUE(cB.size()==3);
    EXPECT_TRUE(cB.at(0).keyCode()==1234);
    EXPECT_TRUE(cB.at(1).keyCode()==5678);
    EXPECT_TRUE(cB.at(2).keyCode()==90);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenBothAreEmpty)
{
    KeyCode a,b;
    EXPECT_TRUE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenKeyboardAndRemote)
{
    KeyCode a,b;
    a.setKeyCode(12);
    b.setRemote("play");
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenRemoteAndKeyboard)
{
    KeyCode a,b;
    a.setRemote("play");
    b.setKeyCode(12);
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenKeyboardAndEmpty)
{
    KeyCode a,b;
    a.setKeyCode(12);
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenEmptyAndKeyboard)
{
    KeyCode a,b;
    b.setKeyCode(12);
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenRemoteAndEmpty)
{
    KeyCode a,b;
    a.setRemote("play");
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenEmptyAndRemote)
{
    KeyCode a,b;
    b.setRemote("play");
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenBothKeyboardButDifferentCodes)
{
    KeyCode a,b;
    a.setKeyCode(12);
    b.setKeyCode(13);
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenBothKeyboardWithSameCode)
{
    KeyCode a,b;
    a.setKeyCode(12);
    b.setKeyCode(12);
    EXPECT_TRUE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivemBothRemoteButDifferentCodes)
{
    KeyCode a,b;
    a.setRemote("play");
    b.setRemote("pause");
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isEqualGivenBothRemoteWithSameCode)
{
    KeyCode a,b;
    a.setRemote("play");
    b.setRemote("play");
    EXPECT_TRUE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenBothAreEmpty)
{
    KeyCode a,b;
    EXPECT_FALSE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenKeyboardAndRemote)
{
    KeyCode a,b;
    a.setKeyCode(12);
    b.setRemote("play");
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenRemoteAndKeyboard)
{
    KeyCode a,b;
    a.setRemote("play");
    b.setKeyCode(12);
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenKeyboardAndEmpty)
{
    KeyCode a,b;
    a.setKeyCode(12);
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenEmptyAndKeyboard)
{
    KeyCode a,b;
    b.setKeyCode(12);
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenRemoteAndEmpty)
{
    KeyCode a,b;
    a.setRemote("play");
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenEmptyAndRemote)
{
    KeyCode a,b;
    b.setRemote("play");
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenBothKeyboardButDifferentCodes)
{
    KeyCode a,b;
    a.setKeyCode(12);
    b.setKeyCode(13);
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenBothKeyboardWithSameCode)
{
    KeyCode a,b;
    a.setKeyCode(12);
    b.setKeyCode(12);
    EXPECT_FALSE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivemBothRemoteButDifferentCodes)
{
    KeyCode a,b;
    a.setRemote("play");
    b.setRemote("pause");
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isNotEqualGivenBothRemoteWithSameCode)
{
    KeyCode a,b;
    a.setRemote("play");
    b.setRemote("play");
    EXPECT_FALSE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenBothAreEmpty)
{
    KeyCode a,b;
    EXPECT_FALSE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenKeyboardAndRemote)
{
    KeyCode a(1234);
    KeyCode b(QString::fromLatin1("play"));
    EXPECT_TRUE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenRemoteAndKeyboard)
{
    KeyCode a(QString::fromLatin1("play"));
    KeyCode b(1234);
    EXPECT_FALSE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenKeyboardAndEmpty)
{
    KeyCode a(1234);
    KeyCode b;
    EXPECT_FALSE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenEmptyAndKeyboard)
{
    KeyCode a;
    KeyCode b(1234);
    EXPECT_TRUE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenRemoteAndEmpty)
{
    KeyCode a(QString::fromLatin1("play"));
    KeyCode b;
    EXPECT_FALSE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenEmptyAndRemote)
{
    KeyCode a;
    KeyCode b(QString::fromLatin1("play"));
    EXPECT_TRUE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenBothKeyboardButLeftCodeLessThanRightCode)
{
    KeyCode a(1234);
    KeyCode b(1235);
    EXPECT_TRUE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenBothKeyboardWithSameCode)
{
    KeyCode a(1234);
    KeyCode b(1234);
    EXPECT_FALSE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenBothKeyboardButLeftCodeGreaterThanRightCode)
{
    KeyCode a(1235);
    KeyCode b(1234);
    EXPECT_FALSE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenBothRemoteButLeftCodeLessThanRightCode)
{
    KeyCode a(QString::fromLatin1("a"));
    KeyCode b(QString::fromLatin1("b"));
    EXPECT_TRUE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenBothRemoteWithSameCode)
{
    KeyCode a(QString::fromLatin1("a"));
    KeyCode b(QString::fromLatin1("a"));
    EXPECT_FALSE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanGivenBothRemoteButLeftCodeGreaterThanRightCode)
{
    KeyCode a(QString::fromLatin1("b"));
    KeyCode b(QString::fromLatin1("a"));
    EXPECT_FALSE(a < b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenBothAreEmpty)
{
    KeyCode a,b;
    EXPECT_TRUE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenKeyboardAndRemote)
{
    KeyCode a(1234);
    KeyCode b(QString::fromLatin1("play"));
    EXPECT_TRUE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenRemoteAndKeyboard)
{
    KeyCode a(QString::fromLatin1("play"));
    KeyCode b(1234);
    EXPECT_FALSE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenKeyboardAndEmpty)
{
    KeyCode a(1234);
    KeyCode b;
    EXPECT_FALSE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenEmptyAndKeyboard)
{
    KeyCode a;
    KeyCode b(1234);
    EXPECT_TRUE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenRemoteAndEmpty)
{
    KeyCode a(QString::fromLatin1("play"));
    KeyCode b;
    EXPECT_FALSE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenEmptyAndRemote)
{
    KeyCode a;
    KeyCode b(QString::fromLatin1("play"));
    EXPECT_TRUE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenBothKeyboardButLeftCodeLessThanRightCode)
{
    KeyCode a(1234);
    KeyCode b(1235);
    EXPECT_TRUE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenBothKeyboardWithSameCode)
{
    KeyCode a(1234);
    KeyCode b(1234);
    EXPECT_TRUE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenBothKeyboardButLeftCodeGreaterThanRightCode)
{
    KeyCode a(1235);
    KeyCode b(1234);
    EXPECT_FALSE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenBothRemoteButLeftCodeLessThanRightCode)
{
    KeyCode a(QString::fromLatin1("a"));
    KeyCode b(QString::fromLatin1("b"));
    EXPECT_TRUE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenBothRemoteWithSameCode)
{
    KeyCode a(QString::fromLatin1("a"));
    KeyCode b(QString::fromLatin1("a"));
    EXPECT_TRUE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isLessThanAndEqualToGivenBothRemoteButLeftCodeGreaterThanRightCode)
{
    KeyCode a(QString::fromLatin1("b"));
    KeyCode b(QString::fromLatin1("a"));
    EXPECT_FALSE(a <= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenBothAreEmpty)
{
    KeyCode a,b;
    EXPECT_FALSE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenKeyboardAndRemote)
{
    KeyCode a(1234);
    KeyCode b(QString::fromLatin1("play"));
    EXPECT_FALSE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenRemoteAndKeyboard)
{
    KeyCode a(QString::fromLatin1("play"));
    KeyCode b(1234);
    EXPECT_TRUE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenKeyboardAndEmpty)
{
    KeyCode a(1234);
    KeyCode b;
    EXPECT_TRUE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenEmptyAndKeyboard)
{
    KeyCode a;
    KeyCode b(1234);
    EXPECT_FALSE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenRemoteAndEmpty)
{
    KeyCode a(QString::fromLatin1("play"));
    KeyCode b;
    EXPECT_TRUE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenEmptyAndRemote)
{
    KeyCode a;
    KeyCode b(QString::fromLatin1("play"));
    EXPECT_FALSE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenBothKeyboardButLeftCodeLessThanRightCode)
{
    KeyCode a(1234);
    KeyCode b(1235);
    EXPECT_FALSE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenBothKeyboardWithSameCode)
{
    KeyCode a(1234);
    KeyCode b(1234);
    EXPECT_FALSE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenBothKeyboardButLeftCodeGreaterThanRightCode)
{
    KeyCode a(1235);
    KeyCode b(1234);
    EXPECT_TRUE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenBothRemoteButLeftCodeLessThanRightCode)
{
    KeyCode a(QString::fromLatin1("a"));
    KeyCode b(QString::fromLatin1("b"));
    EXPECT_FALSE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenBothRemoteWithSameCode)
{
    KeyCode a(QString::fromLatin1("a"));
    KeyCode b(QString::fromLatin1("a"));
    EXPECT_FALSE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanGivenBothRemoteButLeftCodeGreaterThanRightCode)
{
    KeyCode a(QString::fromLatin1("b"));
    KeyCode b(QString::fromLatin1("a"));
    EXPECT_TRUE(a > b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenBothAreEmpty)
{
    KeyCode a,b;
    EXPECT_TRUE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenKeyboardAndRemote)
{
    KeyCode a(1234);
    KeyCode b(QString::fromLatin1("play"));
    EXPECT_FALSE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenRemoteAndKeyboard)
{
    KeyCode a(QString::fromLatin1("play"));
    KeyCode b(1234);
    EXPECT_TRUE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenKeyboardAndEmpty)
{
    KeyCode a(1234);
    KeyCode b;
    EXPECT_TRUE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenEmptyAndKeyboard)
{
    KeyCode a;
    KeyCode b(1234);
    EXPECT_FALSE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenRemoteAndEmpty)
{
    KeyCode a(QString::fromLatin1("play"));
    KeyCode b;
    EXPECT_TRUE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenEmptyAndRemote)
{
    KeyCode a;
    KeyCode b(QString::fromLatin1("play"));
    EXPECT_FALSE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenBothKeyboardButLeftCodeLessThanRightCode)
{
    KeyCode a(1234);
    KeyCode b(1235);
    EXPECT_FALSE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenBothKeyboardWithSameCode)
{
    KeyCode a(1234);
    KeyCode b(1234);
    EXPECT_TRUE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenBothKeyboardButLeftCodeGreaterThanRightCode)
{
    KeyCode a(1235);
    KeyCode b(1234);
    EXPECT_TRUE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenBothRemoteButLeftCodeLessThanRightCode)
{
    KeyCode a(QString::fromLatin1("a"));
    KeyCode b(QString::fromLatin1("b"));
    EXPECT_FALSE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenBothRemoteWithSameCode)
{
    KeyCode a(QString::fromLatin1("a"));
    KeyCode b(QString::fromLatin1("a"));
    EXPECT_TRUE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCode,isGreaterThanAndEqualToGivenBothRemoteButLeftCodeGreaterThanRightCode)
{
    KeyCode a(QString::fromLatin1("b"));
    KeyCode b(QString::fromLatin1("a"));
    EXPECT_TRUE(a >= b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,equalityOperatorGivenBothEmpty)
{
    KeyCodesContainer a;
    KeyCodesContainer b;
    EXPECT_TRUE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,equalityOperatorGivenLeftEmptyRightNotEmpty)
{
    KeyCodesContainer a;
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,equalityOperatorGivenLeftNotEmptyRightEmpty)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    KeyCodesContainer b;
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,equalityOperatorGivenNotEmptyButLeftMoreCodesThanRight)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    b.list().append(KeyCode(34));
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,equalityOperatorGivenNotEmptyButRightMoreCodesThanLeft)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    a.list().append(KeyCode(34));
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,equalityOperatorGivenBothTwoCodesFirstDifferent)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    a.list().append(KeyCode(34));
    KeyCodesContainer b;
    b.list().append(KeyCode(13));
    b.list().append(KeyCode(34));
    EXPECT_FALSE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,equalityOperatorGivenBothTwoCodesSecondDifferent)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    a.list().append(KeyCode(34));
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    b.list().append(KeyCode(35));
    EXPECT_FALSE(a==b);

}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,equalityOperatorGivenBothTwoCodesBothSame)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    a.list().append(KeyCode(34));
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    b.list().append(KeyCode(34));
    EXPECT_TRUE(a==b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,notEqualityOperatorGivenBothEmpty)
{
    KeyCodesContainer a;
    KeyCodesContainer b;
    EXPECT_FALSE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,notEqualityOperatorGivenLeftEmptyRightNotEmpty)
{
    KeyCodesContainer a;
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,notEqualityOperatorGivenLeftNotEmptyRightEmpty)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    KeyCodesContainer b;
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,notEqualityOperatorGivenNotEmptyButLeftMoreCodesThanRight)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    b.list().append(KeyCode(34));
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,notEqualityOperatorGivenNotEmptyButRightMoreCodesThanLeft)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    a.list().append(KeyCode(34));
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,notEqualityOperatorGivenBothTwoCodesFirstDifferent)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    a.list().append(KeyCode(34));
    KeyCodesContainer b;
    b.list().append(KeyCode(13));
    b.list().append(KeyCode(34));
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,notEqualityOperatorGivenBothTwoCodesSecondDifferent)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    a.list().append(KeyCode(34));
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    b.list().append(KeyCode(35));
    EXPECT_TRUE(a!=b);
}

//-------------------------------------------------------------------------------------------

TEST(KeyCodesContainer,notEqualityOperatorGivenBothTwoCodesBothSame)
{
    KeyCodesContainer a;
    a.list().append(KeyCode(12));
    a.list().append(KeyCode(34));
    KeyCodesContainer b;
    b.list().append(KeyCode(12));
    b.list().append(KeyCode(34));
    EXPECT_FALSE(a!=b);
}

//-------------------------------------------------------------------------------------------
