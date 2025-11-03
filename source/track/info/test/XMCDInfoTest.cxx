#include "network/inc/Resource.h"
#include "common/test/UPnPProviderTestEnviroment.h"
#include "common/inc/DiskLayerIF.h"

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "track/info/inc/XMCDInfo.h"

using namespace omega::track::info;
using namespace omega;
using namespace testing;

//-------------------------------------------------------------------------------------------

class XMCDParserTest : public XMCDParser
{
    public:
        XMCDParserTest();
        virtual ~XMCDParserTest();
        XMCDParser::CommentState testParseLineComment(const QString& line,int& number);
        bool testIsUtf8(const tubyte *mem,tint len) const;
        bool testParserLineData(const QString& line,QString& keyword,QString& data);
        tuint32 testProcessDiscID(const QString& line) const;
        QPair<QString,QString> testProcessTitle(const QString& line) const;
        tint testProcessYear(const QString& line) const;
        QString testProcessGenre(const QString& line) const;
        int testGetTitleNumber(const QString& keyword) const;
        QStringList testFindLines(const QString& text) const;
};

//-------------------------------------------------------------------------------------------

XMCDParserTest::XMCDParserTest()
{}

//-------------------------------------------------------------------------------------------

XMCDParserTest::~XMCDParserTest()
{}

//-------------------------------------------------------------------------------------------

XMCDParser::CommentState XMCDParserTest::testParseLineComment(const QString& line,int& number)
{
    return parseLineComment(line,number);
}

//-------------------------------------------------------------------------------------------

bool XMCDParserTest::testIsUtf8(const tubyte *mem,tint len) const
{
    return isUtf8(mem,len);
}

//-------------------------------------------------------------------------------------------

bool XMCDParserTest::testParserLineData(const QString& line,QString& keyword,QString& data)
{
    return parseLineData(line,keyword,data);
}

//-------------------------------------------------------------------------------------------

tuint32 XMCDParserTest::testProcessDiscID(const QString& line) const
{
    return processDiscID(line);
}

//-------------------------------------------------------------------------------------------

QPair<QString,QString> XMCDParserTest::testProcessTitle(const QString& line) const
{
    return processTitle(line);
}

//-------------------------------------------------------------------------------------------

tint XMCDParserTest::testProcessYear(const QString& line) const
{
    return processYear(line);
}

//-------------------------------------------------------------------------------------------

QString XMCDParserTest::testProcessGenre(const QString& line) const
{
    return processGenre(line);
}

//-------------------------------------------------------------------------------------------

int XMCDParserTest::testGetTitleNumber(const QString& keyword) const
{
    return getTitleNumber(keyword);
}

//-------------------------------------------------------------------------------------------

QStringList XMCDParserTest::testFindLines(const QString& text) const
{
    return findLines(text);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentEmptyLine)
{
    int num = 0;
    QString line = QString::fromLatin1("");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_NoComment);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentWhitespace)
{
    int num = 0;
    QString line = QString::fromLatin1("                  ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_NoComment);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentAttributeLine)
{
    int num = 0;
    QString line = QString::fromLatin1("DGENRE=Classical");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_NoComment);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentAttributeLineWithComment)
{
    int num = 0;
    QString line = QString::fromLatin1("DGENRE=Classical # comment");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_NoComment);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentBlankCommentLine)
{
    int num = 0;
    QString line = QString::fromLatin1("#");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentBlankCommentLineWithLeadingWhitespace)
{
    int num = 0;
    QString line = QString::fromLatin1("# \t ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentWhitespaceBeforeComment)
{
    int num = 0;
    QString line = QString::fromLatin1("    #");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentNonWhitespaceBeforeComment)
{
    int num = 0;
    QString line = QString::fromLatin1("a #");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_NoComment);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithLowerCaseXMCDTag)
{
    int num = 0;
    QString line = QString::fromLatin1("# xmcd");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentXMCD);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithUpperCaseXMCDTag)
{
    int num = 0;
    QString line = QString::fromLatin1("#XMCD");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentXMCD);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithMixedCaseXMCDTag)
{
    int num = 0;
    QString line = QString::fromLatin1("# \t XmCd");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentXMCD);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithXMCDTagSubstring)
{
    int num = 0;
    QString line = QString::fromLatin1("# axmcdb");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithXMCDTagSubstringBefore)
{
    int num = 0;
    QString line = QString::fromLatin1("# a xmcd");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithXMCDTagWithStringAfter)
{
    int num = 0;
    QString line = QString::fromLatin1("# xmcd big in");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentXMCD);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithXMCDTagAsSubstringAfter)
{
    int num = 0;
    QString line = QString::fromLatin1("# xmcdb");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithXMCDTagWithWhitespaceAfter)
{
    int num = 0;
    QString line = QString::fromLatin1("# xmcd  \t ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentXMCD);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackHeaderTextSpecificationCase)
{
    int num = 0;
    QString line = QString::fromLatin1("# Track frame offsets:");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentTrackHeader);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackHeaderTextLowerCase)
{
    int num = 0;
    QString line = QString::fromLatin1("#track frame offsets:");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentTrackHeader);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackHeaderTextUpperCase)
{
    int num = 0;
    QString line = QString::fromLatin1("#  TRACK FRAME OFFSETS: ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentTrackHeader);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackHeaderTextMixedCase)
{
    int num = 0;
    QString line = QString::fromLatin1("#\tTrAcK fRaMe OfFsEtS:\t");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentTrackHeader);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackHeaderTextAsSubstring)
{
    int num = 0;
    QString line = QString::fromLatin1("#AbcTrack frame offsets:Abc");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackHeaderTextAsSubstringBefore)
{
    int num = 0;
    QString line = QString::fromLatin1("# A Track frame offsets:");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackHeaderTextAsSubstringAfter)
{
    int num = 0;
    QString line = QString::fromLatin1("#  Track frame offsets: A");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetOnly)
{
    int num = 0;
    QString line = QString::fromLatin1("#1234");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentNumericOffset);
    EXPECT_TRUE(num==1234);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetWhitespaceBefore)
{
    int num = 0;
    QString line = QString::fromLatin1("# \t 1234");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentNumericOffset);
    EXPECT_TRUE(num==1234);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetWhitespaceAfter)
{
    int num = 0;
    QString line = QString::fromLatin1("#1234 \t ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentNumericOffset);
    EXPECT_TRUE(num==1234);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetWhitespaceBothSides)
{
    int num = 0;
    QString line = QString::fromLatin1("# 1234 ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentNumericOffset);
    EXPECT_TRUE(num==1234);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetAndAdditionalTextAround)
{
    int num = 0;
    QString line = QString::fromLatin1("#a1234a");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetAndAdditionalTextBeforeNoWhitespace)
{
    int num = 0;
    QString line = QString::fromLatin1("# a1234");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetAndAdditionalTextBeforeWithWhitespace)
{
    int num = 0;
    QString line = QString::fromLatin1("# a 1234");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetAndAdditionalTextAfterNoWhitespace)
{
    int num = 0;
    QString line = QString::fromLatin1("# 1234a");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithTrackOffsetAndAdditionalTextAfterWithWhitespace)
{
    int num = 0;
    QString line = QString::fromLatin1("# 1234 a");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthButNoLength)
{
    int num = 0;
    QString line = QString::fromLatin1("# Disc length:");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthNonDigitLengthAfter)
{
    int num = 0;
    QString line = QString::fromLatin1("# Disc length: 1234a");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthNonDigitLengthBefore)
{
    int num = 0;
    QString line = QString::fromLatin1("# Disc length: a1234");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthWithSpecificationCase)
{
    int num = 0;
    QString line = QString::fromLatin1("# Disc length: 2952");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentDiscLength);
    EXPECT_TRUE(num==2952);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthWithUpperCase)
{
    int num = 0;
    QString line = QString::fromLatin1("#DISC LENGTH: 2952");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentDiscLength);
    EXPECT_TRUE(num==2952);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthWithLowerCase)
{
    int num = 0;
    QString line = QString::fromLatin1("# \t disc length : \t 2952 \t ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentDiscLength);
    EXPECT_TRUE(num==2952);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthWithMixedCase)
{
    int num = 0;
    QString line = QString::fromLatin1("# \t DiSc LeNgTh : \t 2952 \t ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentDiscLength);
    EXPECT_TRUE(num==2952);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthWithUnitsNoWhitespace)
{
    int num = 0;
    QString line = QString::fromLatin1("# Disc length: 2952seconds");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthWithUnitsWhitespace)
{
    int num = 0;
    QString line = QString::fromLatin1("# Disc length: 2952 seconds");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentDiscLength);
    EXPECT_TRUE(num==2952);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthAsSubstringBefore)
{
    int num = 0;
    QString line = QString::fromLatin1("# as Disc length: 2952 seconds");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthAsSubstringAfter)
{
    int num = 0;
    QString line = QString::fromLatin1("# Disc length: as 2952 seconds");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineCommentGivenCommentWithDiscLengthAsSubstringSurround)
{
    int num = 0;
    QString line = QString::fromLatin1("# as Disc length: as 2952 seconds");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParseLineComment(line,num)==XMCDParser::e_CommentPlain);
    EXPECT_TRUE(num==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8GivenNoBuffer)
{
    XMCDParserTest parse;
    EXPECT_TRUE(parse.testIsUtf8(0,10));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8GivenNoBufferLength)
{
    tubyte mem[1] = { 0x00 };
    XMCDParserTest parse;
    EXPECT_TRUE(parse.testIsUtf8(mem,0));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8GivenASCIIText)
{
    tbyte mem[5] = { 'a', 'b', 'c', 'd', 'e' };
    XMCDParserTest parse;
    EXPECT_TRUE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(mem),5));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given2ByteUTF8With1)
{
    // 1101 1111 1011 1111
    tubyte mem[2] = { 0xdf, 0xbf };
    XMCDParserTest parse;
    EXPECT_TRUE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(mem),2));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given3ByteUTF8With1)
{
    // 1110 1111 1011 1111 1011 1111
    tubyte mem[3] = { 0xef, 0xbf, 0xbf };
    XMCDParserTest parse;
    EXPECT_TRUE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(mem),3));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given2ByteUTF8With0)
{
    // 1101 0000 1000 0000
    tubyte mem[2] = { 0xc0, 0x80 };
    XMCDParserTest parse;
    EXPECT_TRUE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(mem),2));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given3ByteUTF8With0)
{
    // 1110 0000 1000 0000 1000 0000
    tubyte mem[3] = { 0xe0, 0x80, 0x80 };
    XMCDParserTest parse;
    EXPECT_TRUE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(mem),3));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given1ByteLatinEndCase)
{
    // 1101 1111
    tubyte mem[1] = { 0xdf };
    XMCDParserTest parse;
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(mem),1));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given2ByteLatinEndCase)
{
    // 1110 1111 1011 1111
    tubyte mem[2] = { 0xef, 0xbf };
    XMCDParserTest parse;
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(mem),2));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given3ByteUnderformedUtf8)
{
    // 1110 1111 1011 1111 0111 1111
    tubyte memA[3] = { 0xef, 0xbf, 0x7f }; // 0111 - 7
    tubyte memB[3] = { 0xef, 0xbf, 0x7f }; // 0011 - 4
    tubyte memC[3] = { 0xef, 0xbf, 0x7f }; // 1111 - f
    XMCDParserTest parse;
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memA),3));
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memB),3));
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memC),3));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given2ByteUnderformedUtf8)
{
    // 1110 1111 1011 1111
    tubyte memA[2] = { 0xdf, 0x7f }; // 0111 - 7
    tubyte memB[2] = { 0xdf, 0x4f }; // 0011 - 4
    tubyte memC[2] = { 0xdf, 0xff }; // 1111 - f
    XMCDParserTest parse;
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memA),2));
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memB),2));
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memC),2));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given3ByteUTF8WithFalseStart)
{
    // 1110 1111 1011 1111 1011 1111
    tubyte memA[3] = { 0xff, 0xbf, 0xbf }; // 1111 - f
    tubyte memB[3] = { 0x8f, 0xbf, 0xbf }; // 1000 - 8
    tubyte memC[3] = { 0xaf, 0xbf, 0xbf }; // 1010 - a
    XMCDParserTest parse;
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memA),3));
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memB),3));
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memC),3));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,isUtf8Given2ByteUTF8WithFalseStart)
{
    // 1101 1111 1011 1111
    tubyte memA[2] = { 0xff, 0xbf };
    tubyte memB[2] = { 0x8f, 0xbf };
    tubyte memC[2] = { 0xaf, 0xbf };
    XMCDParserTest parse;
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memA),2));
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memB),2));
    EXPECT_FALSE(parse.testIsUtf8(reinterpret_cast<const tubyte *>(memC),2));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenNoData)
{
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("");
    XMCDParserTest parser;
    EXPECT_FALSE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="OLDWORD");
    EXPECT_TRUE(data=="data");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenNonDataLine)
{
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("This a piece of text");
    XMCDParserTest parser;
    EXPECT_FALSE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="OLDWORD");
    EXPECT_TRUE(data=="data");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenASCIIDataLine)
{
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("DTITLE=The Invisible Man");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="dtitle");
    EXPECT_TRUE(data=="The Invisible Man");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenASCIIDataLineWithNewlineTabAndBackslash)
{
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("DTITLE=The\\nInvisible\\tMan\\\\Appears");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="dtitle");
    EXPECT_TRUE(data=="The\nInvisible\tMan\\Appears");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenLatin1DataLine)
{
    const tubyte latinTxt[] = { 0x54, 0x68, 0xe9, 0x20, 0xcd, 0x6e, 0xec, 0x73, 0xed, 0x62, 0x6c, 0xe8, 0x20, 0xa2, 0x00 };
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("DTITLE=");
    line += QString::fromLatin1(reinterpret_cast<const char *>(latinTxt));
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="dtitle");
    EXPECT_TRUE(data==QString::fromLatin1(reinterpret_cast<const char *>(latinTxt)));
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenLatin1DataLineWithNewlineTabAndBackslash)
{
    const tubyte latinTxt[] = { 0x54, 0x68, 0xe9, 0x20, 0xcd, 0x6e, 0xec, 0x73, 0xed, 0x62, 0x6c, 0xe8, 0x20, 0xa2, 0x00 };
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("DTITLE=\\n\\t\\\\");
    line += QString::fromLatin1(reinterpret_cast<const char *>(latinTxt));
    QString expectLine = "\n\t\\" + QString::fromLatin1(reinterpret_cast<const char *>(latinTxt));
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="dtitle");
    EXPECT_TRUE(data==expectLine);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenUTF8DataLineWithNewlineTabAndBackslash)
{
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("DTITLE=\\n\\t\\\\");

    QString expectLine;
    expectLine += QChar(0x50c0);
    expectLine += QChar(0x3060);
    expectLine += QChar(0x2f21);
    line += expectLine;
    expectLine = QString::fromLatin1("\n\t\\") + expectLine;

    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="dtitle");
    EXPECT_TRUE(data==expectLine);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenEmptyDataLine)
{
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("DTITLE=");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="dtitle");
    EXPECT_TRUE(data=="");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,parseLineDataGivenDataLineWithSpaceOnKeyWord)
{
    QString keyword = QString::fromLatin1("OLDWORD");
    QString data = QString::fromLatin1("data");
    QString line = QString::fromLatin1("    DTITLE   =");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testParserLineData(line,keyword,data));
    EXPECT_TRUE(keyword=="dtitle");
    EXPECT_TRUE(data=="");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGivenEmptyLine)
{
    QString discIDData = QString::fromLatin1("");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGiven7ByteSequence)
{
    QString discIDData = QString::fromLatin1("a02a203");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0x0a02a203);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGivenInvalid7ByteSequence)
{
    QString discIDData = QString::fromLatin1("a02r203");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGivenSingle8ByteSequenceLowerCase)
{
    QString discIDData = QString::fromLatin1("abcdefab");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0xabcdefab);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGivenSingle8ByteSequenceUpperCase)
{
    QString discIDData = QString::fromLatin1("ABCDEFAB");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0xabcdefab);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGivenSingle8ByteSequenceMixedCase)
{
    QString discIDData = QString::fromLatin1("aBcDeFaB");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0xabcdefab);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGivenInvalid8ByteSequence)
{
    XMCDParserTest parser;
    QString discIDDataA = QString::fromLatin1(";40aa70c");
    EXPECT_TRUE(parser.testProcessDiscID(discIDDataA)==0);
    QString discIDDataB = QString::fromLatin1("40aa70c;");
    EXPECT_TRUE(parser.testProcessDiscID(discIDDataB)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGivenSingle9ByteSequence)
{
    QString discIDData = QString::fromLatin1("940aa70cd");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGivenInvalid9ByteSequence)
{
    QString discIDData = QString::fromLatin1(";40aa70cd");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGiven3SeparateCommaSeparatedIds)
{
    QString discIDData = QString::fromLatin1("940aa70c,1a02a203,1a01e103");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0x940aa70c);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGiven3SeparateCommaSeparatedIdsWithWhiteSpace)
{
    QString discIDData = QString::fromLatin1("   940aa70c   , \t 1a02a203,  1a01e103\t ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0x940aa70c);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGiven3SeparateCommaSeparatedIdsWithFirst2Invalid)
{
    QString discIDData = QString::fromLatin1("940aa7sc,ta02a203,1a01e103");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0x1a01e103);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGiven3SeparateCommaSeparatedIdsWithFirst2InvalidWithWhiteSpace)
{
    QString discIDData = QString::fromLatin1("   s40aa70c   , \t 1a0.a203,  1a01e103\t ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0x1a01e103);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processDiscIDGiven3SeparateCommaSeparatedWithEmptyEntries)
{
    QString discIDData = QString::fromLatin1(",,, , \t , 1a02a203, ");
    XMCDParserTest parser;
    EXPECT_TRUE(parser.testProcessDiscID(discIDData)==0x1a02a203);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenEmpty)
{
    QString dtitle = QString::fromLatin1("");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="");
    EXPECT_TRUE(title=="");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenArtistTitleSameLatin1)
{
    QString dtitle = QString::fromLatin1("Travis");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="Travis");
    EXPECT_TRUE(title=="Travis");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenArtistTitleSameWhiteSpaceLatin1)
{
    QString dtitle = QString::fromLatin1(" \t  Travis \t ");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="Travis");
    EXPECT_TRUE(title=="Travis");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenOnlySingleSlash)
{
    QString dtitle = QString::fromLatin1(" /");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="/");
    EXPECT_TRUE(title=="/");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenOnlySlashIdentifyingDifferentArtistAndTitle)
{
    QString dtitle = QString::fromLatin1(" / ");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="");
    EXPECT_TRUE(title=="");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenSeparateArtistAndTitleLatin1)
{
    QString dtitle = QString::fromLatin1("Travis / The Invisible Band");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="Travis");
    EXPECT_TRUE(title=="The Invisible Band");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenSeparateArtistAndTitleLatin1WhiteSpace)
{
    QString dtitle = QString::fromLatin1(" \t Travis  /  The Invisible Band  \t");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="Travis");
    EXPECT_TRUE(title=="The Invisible Band");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenArtistAndTitleSameUTF8)
{
    QString expectLine;
    expectLine += QChar(0x50c0);
    expectLine += QChar(0x3060);
    expectLine += QChar(0x2f21);

    QString dtitle = expectLine;
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist==expectLine);
    EXPECT_TRUE(title==expectLine);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenArtistAndTitleSameUTF8WhiteSpace)
{
    QString expectLine;
    expectLine += QChar(0x50c0);
    expectLine += QChar(0x3060);
    expectLine += QChar(0x2f21);

    QString dtitle = " \t " + expectLine + "  \t ";
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist==expectLine);
    EXPECT_TRUE(title==expectLine);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenDifferentArtistAndTitleUTF8)
{
    QString expectLineA;
    expectLineA += QChar(0x50c0);
    expectLineA += QChar(0x3060);
    expectLineA += QChar(0x2f21);

    QString expectLineB;
    expectLineB += QChar(0x50c2);
    expectLineB += QChar(0x3062);
    expectLineB += QChar(0x2f23);

    QString dtitle = expectLineA + " / " + expectLineB;
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist==expectLineA);
    EXPECT_TRUE(title==expectLineB);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenDifferentArtistAndTitleUTF8WhiteSpace)
{
    QString expectLineA;
    expectLineA += QChar(0x50c0);
    expectLineA += QChar(0x3060);
    expectLineA += QChar(0x2f21);

    QString expectLineB;
    expectLineB += QChar(0x50c2);
    expectLineB += QChar(0x3062);
    expectLineB += QChar(0x2f23);

    QString dtitle = "\t" + expectLineA + "   / \t" + expectLineB + " ";
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist==expectLineA);
    EXPECT_TRUE(title==expectLineB);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenSingleVarious)
{
    QString dtitle = QString::fromLatin1("\"Various\"");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="Various");
    EXPECT_TRUE(title=="\"Various\"");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenTitleAndArtistAsVariousLatin1)
{
    QString dtitle = QString::fromLatin1("\"Various\" / The Invisible");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="Various");
    EXPECT_TRUE(title=="The Invisible");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenTitleAndArtistAsVariousUTF8)
{
    QString expectLineA;
    expectLineA += QChar(0x50c0);
    expectLineA += QChar(0x3060);
    expectLineA += QChar(0x2f21);

    QString dtitle = QString::fromLatin1("\"Various\" / ");
    dtitle += expectLineA;

    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="Various");
    EXPECT_TRUE(title==expectLineA);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processTitleGivenTitleAsVarisouAndArtist)
{
    QString dtitle = QString::fromLatin1("Travis / \"Various\"");
    XMCDParserTest parser;
    QPair<QString,QString> artistTitlePair = parser.testProcessTitle(dtitle);
    QString artist = artistTitlePair.first;
    QString title  = artistTitlePair.second;
    EXPECT_TRUE(artist=="Travis");
    EXPECT_TRUE(title=="\"Various\"");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processYearGivenEmpty)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("");
    EXPECT_TRUE(parser.testProcessYear(txtA)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processYearGivenInvalid4DigitsCombinations)
{
    XMCDParserTest parser;

    QString txtA = QString::fromLatin1("a123");
    EXPECT_TRUE(parser.testProcessYear(txtA)==0);

    QString txtB = QString::fromLatin1("a1234");
    EXPECT_TRUE(parser.testProcessYear(txtB)==0);

    QString txtC = QString::fromLatin1("1234a");
    EXPECT_TRUE(parser.testProcessYear(txtC)==0);

    QString txtD = QString::fromLatin1("12a34");
    EXPECT_TRUE(parser.testProcessYear(txtD)==0);

    QString txtE = QString::fromLatin1("12a3456");
    EXPECT_TRUE(parser.testProcessYear(txtE)==0);

    QString txtF = QString::fromLatin1("12a3456");
    EXPECT_TRUE(parser.testProcessYear(txtF)==0);

    QString txtG = QString::fromLatin1("a  3456");
    EXPECT_TRUE(parser.testProcessYear(txtG)==0);

    QString txtH = QString::fromLatin1("3456  9");
    EXPECT_TRUE(parser.testProcessYear(txtH)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processYearGiven2Digits)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("93");
    EXPECT_TRUE(parser.testProcessYear(txtA)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processYearGiven3Digits)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("392");
    EXPECT_TRUE(parser.testProcessYear(txtA)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processYearGiven4Digits)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("2014");
    EXPECT_TRUE(parser.testProcessYear(txtA)==2014);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processYearGiven5Digits)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("10245");
    EXPECT_TRUE(parser.testProcessYear(txtA)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processYearGiven4DigitsWhitespace)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1(" \t 2014  \t ");
    EXPECT_TRUE(parser.testProcessYear(txtA)==2014);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenEmptyLine)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenWhitespaceLine)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("\t    \t  ");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenSingleLetters)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("a b c");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="A B C");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenSingleLowerCase)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("word");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="Word");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenSingleUpperCase)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("WORD");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="Word");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenSingleMixedCase)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("WoRd");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="Word");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenMultipleLowerCase)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("new age gothic");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="New Age Gothic");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenMultipleCorrect)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("New Age Gothic");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="New Age Gothic");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenMultipleCorrectWithSymbols)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("N;e'w A^g$e G,o.t<h>i@c");
    QString txtB = parser.testProcessGenre(txtA);
    EXPECT_TRUE(txtA==txtB);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenMultipleUpperCase)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("NEW AGE GOTHIC");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="New Age Gothic");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenMultipleMixedCase)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("NeW aGe GoThIc");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="New Age Gothic");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,processGenreGivenMultipleMixedCaseWhiteSpace)
{
    XMCDParserTest parser;
    QString txtA = QString::fromLatin1("   NeW   aGe\tGoThIc    ");
    EXPECT_TRUE(parser.testProcessGenre(txtA)=="New Age Gothic");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenEmpty)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenNonTitle)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("DISC");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenTitleWithoutNumber)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("TTITLE");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==-1);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenSingleDigit)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("TTITLE3");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==3);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenDoubleDigits)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("TTITLE67");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==67);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenThreeDigits)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("TTITLE854");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==854);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenLowerCase)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("ttitle67");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==67);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenMixedCase)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("tTiTlE67");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==67);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,getTitleNumberGivenSingleDigitWhiteSpace)
{
    XMCDParserTest parser;
    QString keyword = QString::fromLatin1("  \t  TTITLE0   \t  ");
    EXPECT_TRUE(parser.testGetTitleNumber(keyword)==0);
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,findLinesNoText)
{
    XMCDParserTest parser;
    QString line = QString::fromLatin1("");
    QStringList lines = parser.testFindLines(line);
    EXPECT_TRUE(lines.isEmpty());
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,findLinesSingleLineWithNoEOL)
{
    XMCDParserTest parser;
    QString line = QString::fromLatin1("abc");
    QStringList lines = parser.testFindLines(line);
    EXPECT_TRUE(lines.size()==1);
    EXPECT_TRUE(lines.at(0)=="abc");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,findLinesSingleLineWithEOL)
{
    XMCDParserTest parser;
    QString line = QString::fromLatin1("abc\n");
    QStringList lines = parser.testFindLines(line);
    EXPECT_TRUE(lines.size()==1);
    EXPECT_TRUE(lines.at(0)=="abc");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,findLinesTwoLinesWithNoEOL)
{
    XMCDParserTest parser;
    QString line = QString::fromLatin1("a\r\n b ");
    QStringList lines = parser.testFindLines(line);
    EXPECT_TRUE(lines.size()==2);
    EXPECT_TRUE(lines.at(0)=="a");
    EXPECT_TRUE(lines.at(1)==" b ");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,findLinesTwoLinesWithEOL)
{
    XMCDParserTest parser;
    QString line = QString::fromLatin1("a\r\n b \n");
    QStringList lines = parser.testFindLines(line);
    EXPECT_TRUE(lines.size()==2);
    EXPECT_TRUE(lines.at(0)=="a");
    EXPECT_TRUE(lines.at(1)==" b ");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,findLinesThreeLinesWithNoEOL)
{
    XMCDParserTest parser;
    QString line = QString::fromLatin1("#Line 1\r\n#\r\nDISCID=id");
    QStringList lines = parser.testFindLines(line);
    EXPECT_TRUE(lines.size()==3);
    EXPECT_TRUE(lines.at(0)=="#Line 1");
    EXPECT_TRUE(lines.at(1)=="#");
    EXPECT_TRUE(lines.at(2)=="DISCID=id");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,findLinesThreeLinesWithEOL)
{
    XMCDParserTest parser;
    QString line = QString::fromLatin1("#Line 1\n#\nDISCID=id\n");
    QStringList lines = parser.testFindLines(line);
    EXPECT_TRUE(lines.size()==3);
    EXPECT_TRUE(lines.at(0)=="#Line 1");
    EXPECT_TRUE(lines.at(1)=="#");
    EXPECT_TRUE(lines.at(2)=="DISCID=id");
}

//-------------------------------------------------------------------------------------------

TEST(XMCDParser,findLinesThreeLinesWithEOLGivenTwoEmptyLines)
{
    XMCDParserTest parser;
    QString line = QString::fromLatin1("\r\nDISCID=id\n\r\n");
    QStringList lines = parser.testFindLines(line);
    EXPECT_TRUE(lines.size()==1);
    EXPECT_TRUE(lines.at(0)=="DISCID=id");
}

//-------------------------------------------------------------------------------------------
// Travis - The Invisible Band
// 01. Sing    [0:03:48.60]
// 02. Dear Diary    [0:02:57.07]
// 03. Side    [0:03:59.18]
// 04. Pipe Dreams    [0:04:05.62]
// 05. Flowers in the Window    [0:03:41.50]
// 06. The Cage    [0:03:05.60]
// 07. Safe    [0:04:23.23]
// 08. Follow the Light    [0:03:08.67]
// 09. Last Train    [0:03:16.00]
// 10. Afterglow    [0:04:05.70]
// 11. Indefinitely    [0:03:52.60]
// 12. The Humpty Dumpty Love Song    [0:05:02.00]
//-------------------------------------------------------------------------------------------

TEST(XMCDInfo,readXMCDIntegrationTravis)
{
    common::test::UPnPProviderTestEnviroment *env = common::test::UPnPProviderTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(env->root(2),"travis.txt");
    common::BIOStream fileIO(common::e_BIOStream_FileRead);

    const char *trackNameRaw[12] = {
        "Sing", // 1
        "Dear Diary", // 2
        "Side", // 3
        "Pipe Dreams", // 4
        "Flowers in the Window", // 5
        "The Cage", // 6
        "Safe", // 7
        "Follow the Light", // 8
        "Last Train", // 9
        "Afterglow", // 10
        "Indefinitely", // 11
        "The Humpty Dumpty Love Song"  // 12
    };

    int trackTimesRaw[12][3] = {
        {3,48,60}, // 1 - [0:03:48.60]
        {2,57, 7}, // 2 - [0:02:57.07]
        {3,59,18}, // 3 - [0:03:59.18]
        {4, 5,62}, // 4 - [0:04:05.62]
        {3,41,50}, // 5 - [0:03:41.50]
        {3, 5,60}, // 6 - [0:03:05.60]
        {4,23,23}, // 7 - [0:04:23.23]
        {3, 8,67}, // 8 - [0:03:08.67]
        {3,16, 0}, // 9 - [0:03:16.00]
        {4, 5,70}, // 10 - [0:04:05.70]
        {3,52,60}, // 11 - [0:03:52.60]
        {5, 1,48}  // 12 - [0:05:02.00]
    };

    QVector<QPair<QString,common::TimeStamp> > expectInfoList;
    for(int i=0;i<12;i++)
    {
        double min = static_cast<double>(trackTimesRaw[i][0]);
        double sec = static_cast<double>(trackTimesRaw[i][1]);
        double fra = static_cast<double>(trackTimesRaw[i][2]);
        double timeF = (min * 60.0) + sec + (fra / 75.0);
        common::TimeStamp trackT(timeF);
        QString trackN = QString::fromLatin1(trackNameRaw[i]);
        expectInfoList.append(QPair<QString,common::TimeStamp>(trackN,trackT));
    }

    QString expectAlbum = QString::fromLatin1("The Invisible Band");
    QString expectArtist = QString::fromLatin1("Travis");
    QString expectGenre = QString::fromLatin1("Alternative");
    QString expectYear = QString::fromLatin1("2001");

    EXPECT_TRUE(fileIO.open(fileName));

    QVector<InfoSPtr> trackList = XMCDInfo::readXMCD(&fileIO);

    EXPECT_TRUE(trackList.size()==12);
    for(int i=0;i<trackList.size();i++)
    {
        InfoSPtr info = trackList[i];
        EXPECT_TRUE(info->title()==expectInfoList.at(i).first);
        EXPECT_TRUE(info->artist()==expectArtist);
        EXPECT_TRUE(info->album()==expectAlbum);
        EXPECT_TRUE(info->year()==expectYear);
        EXPECT_TRUE(info->comment().isEmpty());
        EXPECT_TRUE(info->genre()==expectGenre);
        EXPECT_TRUE(info->track().toInt()==(i+1));
        EXPECT_TRUE(info->disc().toInt()==1);
        EXPECT_TRUE(info->composer()==expectArtist);
        EXPECT_TRUE(info->originalArtist()==expectArtist);
        EXPECT_TRUE(info->copyright().isEmpty());
        EXPECT_TRUE(info->encoder().isEmpty());
        EXPECT_FALSE(info->isChildren());

        common::TimeStamp trackS = expectInfoList.at(i).second - (1.0 / 75.0);
        common::TimeStamp trackE = expectInfoList.at(i).second + (1.0 / 75.0);

        EXPECT_TRUE(trackS<=info->length() && info->length()<=trackE);

        EXPECT_FALSE(info->isImage());
    }

    fileIO.close();
}

//-------------------------------------------------------------------------------------------

TEST(XMCDInfo,readXMCDIntegrationKanon)
{
    common::test::UPnPProviderTestEnviroment *env = common::test::UPnPProviderTestEnviroment::instance();
    QString fileName = common::DiskOps::mergeName(env->root(2),"kanon.txt");
    common::BIOStream fileIO(common::e_BIOStream_FileRead);

    const char *trackNameRaw[3] = {
        "Kammerton a",
        "Kanon",
        "Gigue"
    };

    const char *trackArtistRaw[3] = {
        "Artist A",
        "Artist B",
        "Johann Pachelbel"
    };

    int trackTimesRaw[3][3] = {
        {0,21,50},
        {5,56, 0},
        {1,43,25}
    };

    QVector<QPair<QString,common::TimeStamp> > expectInfoList;
    for(int i=0;i<3;i++)
    {
        double min = static_cast<double>(trackTimesRaw[i][0]);
        double sec = static_cast<double>(trackTimesRaw[i][1]);
        double fra = static_cast<double>(trackTimesRaw[i][2]);
        double timeF = (min * 60.0) + sec + (fra / 75.0);
        common::TimeStamp trackT(timeF);
        QString trackN = QString::fromLatin1(trackNameRaw[i]);
        expectInfoList.append(QPair<QString,common::TimeStamp>(trackN,trackT));
    }

    QString expectAlbum = QString::fromLatin1("Kanon und Gigue (Begleitung zur Solostimme)");
    QString expectGenre = QString::fromLatin1("Classical");
    QString expectComposer = QString::fromLatin1("Johann Pachelbel");

    EXPECT_TRUE(fileIO.open(fileName));

    QVector<InfoSPtr> trackList = XMCDInfo::readXMCD(&fileIO);

    EXPECT_TRUE(trackList.size()==3);
    for(int i=0;i<trackList.size();i++)
    {
        QString expectArtist = QString::fromLatin1(trackArtistRaw[i]);
        InfoSPtr info = trackList[i];
        EXPECT_TRUE(info->title()==expectInfoList.at(i).first);
        EXPECT_TRUE(info->artist()==expectArtist);
        EXPECT_TRUE(info->album()==expectAlbum);
        EXPECT_TRUE(info->comment().isEmpty());
        EXPECT_TRUE(info->genre()==expectGenre);
        EXPECT_TRUE(info->track().toInt()==(i+1));
        EXPECT_TRUE(info->disc().toInt()==1);
        EXPECT_TRUE(info->composer()==expectComposer);
        EXPECT_TRUE(info->originalArtist()==expectArtist);
        EXPECT_TRUE(info->copyright().isEmpty());
        EXPECT_TRUE(info->encoder().isEmpty());
        EXPECT_FALSE(info->isChildren());

        common::TimeStamp trackS = expectInfoList.at(i).second - (1.0 / 75.0);
        common::TimeStamp trackE = expectInfoList.at(i).second + (1.0 / 75.0);

        EXPECT_TRUE(trackS<=info->length() && info->length()<=trackE);

        EXPECT_FALSE(info->isImage());
    }

    fileIO.close();
}

//-------------------------------------------------------------------------------------------
