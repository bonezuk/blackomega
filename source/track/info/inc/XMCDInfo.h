//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_XMCDINFO_H
#define __OMEGA_TRACK_INFO_XMCDINFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"
#include <QPair>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT XMCDInfo : public Info
{
    public:
        XMCDInfo();
        virtual ~XMCDInfo();
    
        static QVector<InfoSPtr> readXMCD(common::BIOStream *input);
        
    protected:
};

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT XMCDParser
{
    public:

        typedef enum
        {
            e_NoComment = 0,
            e_CommentPlain = 1,
            e_CommentXMCD,
            e_CommentTrackHeader,
            e_CommentNumericOffset,
            e_CommentDiscLength
        } CommentState;

    public:
        XMCDParser();
        virtual ~XMCDParser();
        
        virtual bool read(const QString& fileName,const QByteArray& array,QVector<InfoSPtr>& list);
        
        static bool isUtf8(const tubyte *mem,tint len);

    protected:
        
        virtual CommentState parseLineComment(const QString& line,int& number);
        virtual bool parseLineData(const QString& line,QString& keyword,QString& data) const;
        
        virtual tuint32 processDiscID8ByteSequence(const QString& s) const;
        virtual tuint32 processDiscID(const QString& line) const;
        virtual QPair<QString,QString> processTitle(const QString& line) const;
        virtual tint processYear(const QString& line) const;
        virtual QString processGenre(const QString& line) const;
        
        virtual int getTitleNumber(const QString& keyword) const;
        
        virtual QStringList findLines(const QString& text) const;
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

