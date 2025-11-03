#include "network/inc/Resource.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "common/inc/BIOMemory.h"

using namespace omega::common;
using namespace testing;

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,constructWithNullArray)
{
    QByteArray array;
    BIOMemory file(array);
    EXPECT_TRUE(file.name().isEmpty());
    EXPECT_TRUE(file.sof());
    EXPECT_TRUE(file.eof());
    EXPECT_TRUE(file.size()==0);
    EXPECT_TRUE(file.offset()==0);
    EXPECT_TRUE(file.readable());
    EXPECT_FALSE(file.writeable());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,constructWithNonNullArray)
{
    const tubyte mem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray array(reinterpret_cast<const char *>(mem),4);
    BIOMemory file(array);
    EXPECT_TRUE(file.name().isEmpty());
    EXPECT_TRUE(file.sof());
    EXPECT_FALSE(file.eof());
    EXPECT_TRUE(file.size()==array.size());
    EXPECT_TRUE(file.offset()==0);
    EXPECT_TRUE(file.readable());
    EXPECT_FALSE(file.writeable());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,openConstCharIsFalseGivenNullArray)
{
    QByteArray array;
    BIOMemory file(array);
    EXPECT_FALSE(file.open("name"));
    EXPECT_TRUE(file.name().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,openConstCharIsTrueGivenNonNullArray)
{
    const tubyte mem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray array(reinterpret_cast<const char *>(mem),4);
    BIOMemory file(array);
    EXPECT_TRUE(file.open("name"));
    EXPECT_TRUE(file.name()=="name");
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,openBStringIsFalseGivenNullArray)
{
    QByteArray array;
    BIOMemory file(array);
    BString name("name");
    EXPECT_FALSE(file.open(name));
    EXPECT_TRUE(file.name().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,openBStringIsTrueGivenNonNullArray)
{
    const tubyte mem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray array(reinterpret_cast<const char *>(mem),4);
    BIOMemory file(array);
    BString name("name");
    EXPECT_TRUE(file.open(name));
    EXPECT_TRUE(file.name()=="name");
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,openQStringIsFalseGivenNullArray)
{
    QByteArray array;
    BIOMemory file(array);
    QString name = QString::fromUtf8("name");
    EXPECT_FALSE(file.open(name));
    EXPECT_TRUE(file.name().isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,openQStringIsTrueGivenNonNullArray)
{
    const tubyte mem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray array(reinterpret_cast<const char *>(mem),4);
    BIOMemory file(array);
    QString name = QString::fromUtf8("name");
    EXPECT_TRUE(file.open(name));
    EXPECT_TRUE(file.name()=="name");
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,close)
{
    const tubyte mem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray array(reinterpret_cast<const char *>(mem),4);
    BIOMemory file(array);

    EXPECT_TRUE(file.open("name"));
    file.close();

    EXPECT_TRUE(file.name().isEmpty());
    EXPECT_TRUE(file.sof());
    EXPECT_FALSE(file.eof());
    EXPECT_TRUE(file.size()==array.size());
    EXPECT_TRUE(file.offset()==0);
    EXPECT_TRUE(file.readable());
    EXPECT_FALSE(file.writeable());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,readUByteGivenNullArray)
{
    tubyte mem[1];
    QByteArray array;
    BIOMemory file(array);
    EXPECT_TRUE(file.read(mem,1)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,readUByteAllArray)
{
    tubyte rMem[4];

    const tubyte mem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray array(reinterpret_cast<const char *>(mem),4);

    BIOMemory file(array);

    EXPECT_TRUE(file.sof());
    EXPECT_TRUE(file.offset()==0);

    EXPECT_TRUE(file.read(rMem,4)==4);
    EXPECT_TRUE(file.offset()==4);
    EXPECT_TRUE(!file.sof());
    EXPECT_TRUE(file.eof());

    EXPECT_TRUE(file.read(rMem,1)==0);
    EXPECT_TRUE(file.offset()==4);
    EXPECT_TRUE(!file.sof());
    EXPECT_TRUE(file.eof());

    EXPECT_TRUE(::memcmp(mem,rMem,4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,readUByteSequenceRead)
{
    tubyte rMemA[4],rMemB[2],rMemC[6],rMemD[1];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.sof());
    EXPECT_TRUE(file.offset()==0);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_TRUE(file.offset()==4);
    EXPECT_TRUE(!file.sof());
    EXPECT_TRUE(!file.eof());

    EXPECT_TRUE(file.read(rMemB,2)==2);
    EXPECT_TRUE(file.offset()==6);
    EXPECT_TRUE(!file.sof());
    EXPECT_TRUE(!file.eof());

    EXPECT_TRUE(file.read(rMemC,6)==4);
    EXPECT_TRUE(file.offset()==10);
    EXPECT_TRUE(!file.sof());
    EXPECT_TRUE(file.eof());

    EXPECT_TRUE(file.read(rMemD,1)==0);
    EXPECT_TRUE(file.offset()==10);
    EXPECT_TRUE(!file.sof());
    EXPECT_TRUE(file.eof());

    EXPECT_TRUE(::memcmp(rMemA,mem,4)==0);
    EXPECT_TRUE(::memcmp(rMemB,&mem[4],2)==0);
    EXPECT_TRUE(::memcmp(rMemC,&mem[6],4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,readByteGivenNullArray)
{
    tbyte mem[1];
    QByteArray array;
    BIOMemory file(array);
    EXPECT_TRUE(file.read(mem,1)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,readByteAllArray)
{
    tbyte rMem[4];

    const tubyte mem[4] = { 0x01, 0x02, 0x03, 0x04 };
    QByteArray array(reinterpret_cast<const char *>(mem),4);

    BIOMemory file(array);

    EXPECT_TRUE(file.sof());
    EXPECT_TRUE(file.offset()==0);

    EXPECT_TRUE(file.read(rMem,4)==4);
    EXPECT_TRUE(file.offset()==4);
    EXPECT_TRUE(!file.sof());
    EXPECT_TRUE(file.eof());

    EXPECT_TRUE(file.read(rMem,1)==0);
    EXPECT_TRUE(file.offset()==4);
    EXPECT_TRUE(!file.sof());
    EXPECT_TRUE(file.eof());

    EXPECT_TRUE(::memcmp(mem,rMem,4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,readByteSequenceRead)
{
    tbyte rMemA[4],rMemB[2],rMemC[6],rMemD[1];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.sof());
    EXPECT_TRUE(file.offset()==0);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_TRUE(file.offset()==4);
    EXPECT_TRUE(!file.eof());
    EXPECT_TRUE(!file.sof());

    EXPECT_TRUE(file.read(rMemB,2)==2);
    EXPECT_TRUE(file.offset()==6);
    EXPECT_TRUE(!file.eof());
    EXPECT_TRUE(!file.sof());

    EXPECT_TRUE(file.read(rMemC,6)==4);
    EXPECT_TRUE(file.offset()==10);
    EXPECT_TRUE(file.eof());
    EXPECT_TRUE(!file.sof());

    EXPECT_TRUE(file.read(rMemD,1)==0);
    EXPECT_TRUE(file.offset()==10);
    EXPECT_TRUE(file.eof());
    EXPECT_TRUE(!file.sof());

    EXPECT_TRUE(::memcmp(rMemA,mem,4)==0);
    EXPECT_TRUE(::memcmp(rMemB,&mem[4],2)==0);
    EXPECT_TRUE(::memcmp(rMemC,&mem[6],4)==0);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,writeByteWithNullArray)
{
    const tubyte memB[4] = { 0x05, 0x06, 0x07, 0x08 };
    QByteArray array;
    BIOMemory file(array);
    EXPECT_TRUE(file.write(reinterpret_cast<const char *>(memB),4)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,writeByteWithNonNullArray)
{
    const tubyte memA[4] = { 0x01, 0x02, 0x03, 0x04 };
    const tubyte memB[4] = { 0x05, 0x06, 0x07, 0x08 };

    QByteArray array(reinterpret_cast<const char *>(memA),4);
    BIOMemory file(array);

    EXPECT_TRUE(file.write(reinterpret_cast<const char *>(memB),4)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,writeUByteWithNullArray)
{
    const tubyte memB[4] = { 0x05, 0x06, 0x07, 0x08 };
    QByteArray array;
    BIOMemory file(array);
    EXPECT_TRUE(file.write(reinterpret_cast<const char *>(memB),4)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,writeUByteWithNonNullArray)
{
    const tubyte memA[4] = { 0x01, 0x02, 0x03, 0x04 };
    const tubyte memB[4] = { 0x05, 0x06, 0x07, 0x08 };

    QByteArray array(reinterpret_cast<const char *>(memA),4);
    BIOMemory file(array);

    EXPECT_TRUE(file.write(memB,4)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekAndReadFromStartPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_TRUE(file.seek(6,e_Seek_Start));
    EXPECT_TRUE(file.offset()==6);
    EXPECT_TRUE(file.read(rMemA,4)==4);

    EXPECT_TRUE(::memcmp(&mem[6],rMemA,4)==0);

    EXPECT_TRUE(file.seek(0,e_Seek_Start));
    EXPECT_TRUE(file.sof());

    EXPECT_TRUE(file.seek(10,e_Seek_Start));
    EXPECT_TRUE(file.eof());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekFromStartPositionBeforeStartPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_FALSE(file.seek(-1,e_Seek_Start));

    EXPECT_TRUE(file.offset()==4);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekFromStartPositionBeyondEndPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_FALSE(file.seek(11,e_Seek_Start));

    EXPECT_TRUE(file.offset()==4);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekAndReadFromCurrentPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_TRUE(file.seek(2,e_Seek_Current));
    EXPECT_TRUE(file.offset()==6);
    EXPECT_TRUE(file.read(rMemA,4)==4);

    EXPECT_TRUE(::memcmp(&mem[6],rMemA,4)==0);

    EXPECT_TRUE(file.seek(-10,e_Seek_Current));
    EXPECT_TRUE(file.sof());

    EXPECT_TRUE(file.seek(10,e_Seek_Current));
    EXPECT_TRUE(file.eof());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekFromCurrentPositionBeforeStartPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_FALSE(file.seek(-5,e_Seek_Current));

    EXPECT_TRUE(file.offset()==4);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekFromCurrentPositionBeyondEndPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_FALSE(file.seek(7,e_Seek_Current));

    EXPECT_TRUE(file.offset()==4);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekAndReadFromEndPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_TRUE(file.seek(-4,e_Seek_End));
    EXPECT_TRUE(file.offset()==6);
    EXPECT_TRUE(file.read(rMemA,4)==4);

    EXPECT_TRUE(::memcmp(&mem[6],rMemA,4)==0);

    EXPECT_TRUE(file.seek(-10,e_Seek_End));
    EXPECT_TRUE(file.sof());

    EXPECT_TRUE(file.seek(0,e_Seek_End));
    EXPECT_TRUE(file.eof());
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekFromEndPositionBeforeStartPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_FALSE(file.seek(-11,e_Seek_End));

    EXPECT_TRUE(file.offset()==4);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,seekFromEndPositionBeyondEndPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_FALSE(file.seek(1,e_Seek_End));

    EXPECT_TRUE(file.offset()==4);
}

//-------------------------------------------------------------------------------------------

TEST(BIOMemory,bookmarkAndPosition)
{
    tbyte rMemA[4];

    const tubyte mem[10] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a };
    QByteArray array(reinterpret_cast<const char *>(mem),10);

    BIOMemory file(array);
    EXPECT_TRUE(file.seek(1,e_Seek_Start));

    int bk = file.bookmark(3);
    EXPECT_TRUE(bk>=0);

    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_TRUE(::memcmp(&mem[1],rMemA,4)==0);

    EXPECT_TRUE(file.position(bk));
    EXPECT_TRUE(file.read(rMemA,4)==4);
    EXPECT_TRUE(::memcmp(&mem[4],rMemA,4)==0);

    file.deleteBookmark(bk);
    EXPECT_FALSE(file.position(bk));
}

//-------------------------------------------------------------------------------------------
