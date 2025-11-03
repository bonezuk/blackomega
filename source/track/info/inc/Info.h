//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_INFO_H
#define __OMEGA_TRACK_INFO_INFO_H
//-------------------------------------------------------------------------------------------

#include <QMap>
#include <QImage>
#include <QString>
#include <QObject>

#include "common/inc/Log.h"
#include "common/inc/Exception.h"
#include "common/inc/BString.h"
#include "common/inc/BIOStream.h"
#include "common/inc/Array.h"
#include "common/inc/TimeStamp.h"
#include "common/inc/Factory.h"
#include "track/info/inc/InfoDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

#define ID3_TAG(a,b,c,d) (((static_cast<tuint>(a) << 24) & 0xff000000) | ((static_cast<tuint>(b) << 16) & 0x00ff0000) | ((static_cast<tuint>(c) << 8) & 0x0000ff00) | (static_cast<tuint>(d) & 0x000000ff))

//-------------------------------------------------------------------------------------------

typedef enum
{
    e_TagInfo_Artist = 0,
    e_TagInfo_Title,
    e_TagInfo_Album,
    e_TagInfo_Year,
    e_TagInfo_Comment,
    e_TagInfo_Genre,
    e_TagInfo_Track,
    e_TagInfo_Composer,
    e_TagInfo_OriginalArtist,
    e_TagInfo_Copyright,
    e_TagInfo_Encoder,
    e_TagInfo_FileName,
    e_TagInfo_DirectoryName,
    e_TagInfo_Size,
    e_TagInfo_Disc
} IDTagInformation;

//-------------------------------------------------------------------------------------------

typedef enum
{
    e_TagImage_Other        = 0x00,
    e_TagImage_Icon32x32    = 0x01,
    e_TagImage_OtherIcon    = 0x02,
    e_TagImage_CoverFront   = 0x03,
    e_TagImage_CoverBack    = 0x04,
    e_TagImage_Leaflet      = 0x05,
    e_TagImage_Media        = 0x06,
    e_TagImage_LeadArtist   = 0x07,
    e_TagImage_Artist       = 0x08,
    e_TagImage_Conductor    = 0x09,
    e_TagImage_Band         = 0x0A,
    e_TagImage_Composer     = 0x0B,
    e_TagImage_Lyricist     = 0x0C,
    e_TagImage_RecLocation  = 0x0D,
    e_TagImage_Recording    = 0x0E,
    e_TagImage_Performance  = 0x0F,
    e_TagImage_VidCaptute   = 0x10,
    e_TagImage_Fish         = 0x11,
    e_TagImage_Illustration = 0x12,
    e_TagImage_BandLogo     = 0x13,
    e_TagImage_StudioLogo   = 0x14
} IDTagImageType;

//-------------------------------------------------------------------------------------------

typedef common::Array<tubyte,tubyte> ImageInfoArray;

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT Info : public QObject
{
    public:
        Q_OBJECT

    public:
        class ChildInfo;
        
        typedef enum
        {
            e_InfoUnknown = 0,
            e_InfoID3Type1,
            e_InfoID3Type2,
            e_InfoMP4Atom,
            e_InfoVorbis,
            e_InfoFLAC,
            e_InfoXKCD,
            e_InfoAPE,
            e_InfoAIFF,
            e_InfoWAV
        } Type;
        
        typedef enum
        {
            e_imageUnknown = 0,
            e_imageJPEG,
            e_imageGIF,
            e_imagePNG,
            e_imageBMP
        } ImageFormat;

    public:
        Info(QObject *parent = 0);
        virtual ~Info();
        
        static bool isSupported(const QString& name);
        static QSharedPointer<Info> readInfo(common::BIOStream *input);
        
        Type type() const;
        
        virtual bool read(common::BIOStream *input);
        
        virtual bool get(IDTagInformation info,QString& str) const;
        virtual bool get(IDTagInformation info,common::BString& str) const;

        virtual bool isImage() const;
        virtual ImageInfoArray *getImageData(ImageFormat& format) const;
        virtual ImageInfoArray *getImageData(IDTagImageType type,ImageFormat& format) const;
        
        virtual QString getName() const;
        virtual QString getName(const QString& scheme,bool sortFlag = false) const;
        virtual QString getName(const QString& scheme,bool& contentFlag,bool sortFlag = false) const;

        virtual const QString& getFilename() const;
        virtual void setFilename(const QString& name);
        
        virtual QString directory() const;
        virtual QString filename() const;
        virtual QString albumName() const;
        virtual QString directoryGroup() const;
        
        virtual common::TimeStamp& length();
        virtual const common::TimeStamp& length() const;
        
        virtual tint bitrate() const;
        virtual tint noChannels() const;
        virtual tint frequency() const;
        
        QString& artist();
        const QString& artist() const;
        QString& title();
        const QString& title() const;
        QString& album();
        const QString& album() const;
        QString& year();
        const QString& year() const;
        QString& comment();
        const QString& comment() const;
        QString& genre();
        const QString& genre() const;
        QString& track();
        const QString& track() const;
        QString& disc();
        const QString& disc() const;
        QString& composer();
        const QString& composer() const;
        QString& originalArtist();
        const QString& originalArtist() const;
        QString& copyright();
        const QString& copyright() const;
        QString& encoder();
        const QString& encoder() const;
        
        virtual bool isChildren() const;
        virtual tint noChildren() const;
        virtual const ChildInfo& child(tint idx) const;

        static bool playPreGap();
        static void setPlayPreGap(bool flag);
        
        virtual tuint64 hashID() const;
        
    protected:
    
        static ChildInfo m_nullChild;
        
        Type m_type;
        
        QString m_Artist;
        QString m_Title;
        QString m_Album;
        QString m_Year;
        QString m_Comment;
        QString m_Genre;
        QString m_Track;
        QString m_Disc;
        QString m_Composer;
        QString m_OriginalArtist;
        QString m_Copyright;
        QString m_Encoder;
        QString m_fileName;
        
        common::TimeStamp m_length;
        
        tint m_bitrate;
        tint m_frequency;
        tint m_noChannels;
        
        ImageFormat m_dirImageFormat;
        ImageInfoArray *m_dirImageArray;
        
        tuint64 m_hash;
    
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual void blank();

        static bool seekType1(common::BIOStream *input);
        static bool seekType2(common::BIOStream *input);
        
        virtual void setLength(const common::TimeStamp& l);
        virtual void setBitrate(tint v);
        virtual void setFrequency(tint v);
        virtual void setNoChannels(tint v);

        static bool isAppleFinderFile(const QString& name,common::BIOStream *reader);

        static tuint64 calculateELFHash(common::BIOStream *input);
        virtual void calcHashID(common::BIOStream *input);
};

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(TRACK_INFO_EXPORT,InfoFactory,Info);

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT Info::ChildInfo
{
    public:
        ChildInfo();
        ChildInfo(const ChildInfo& rhs);
        virtual ~ChildInfo();

        virtual const ChildInfo& operator = (const ChildInfo& rhs);

        QString& name();
        const QString& name() const;
        common::TimeStamp& startTime();
        const common::TimeStamp& startTime() const;
        common::TimeStamp& length();
        const common::TimeStamp& length() const;

    protected:

        QString m_name;
        common::TimeStamp m_startTime;
        common::TimeStamp m_lengthTime;

        virtual void copy(const ChildInfo& rhs);
};

//-------------------------------------------------------------------------------------------

typedef Info *InfoPtr;
typedef QSharedPointer<Info> InfoSPtr;

//-------------------------------------------------------------------------------------------

inline Info::Type Info::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::artist()
{
    return m_Artist;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::artist() const
{
    if(!m_Artist.isEmpty())
    {
        return m_Artist;
    }
    else if(!m_OriginalArtist.isEmpty())
    {
        return m_OriginalArtist;
    }
    else
    {
        return m_Composer;
    }
}

//-------------------------------------------------------------------------------------------

inline QString& Info::title()
{
    return m_Title;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::title() const
{
    return m_Title;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::album()
{
    return m_Album;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::album() const
{
    return m_Album;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::year()
{
    return m_Year;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::year() const
{
    return m_Year;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::comment()
{
    return m_Comment;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::comment() const
{
    return m_Comment;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::genre()
{
    return m_Genre;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::genre() const
{
    return m_Genre;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::track()
{
    return m_Track;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::track() const
{
    return m_Track;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::composer()
{
    return m_Composer;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::composer() const
{
    return m_Composer;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::originalArtist()
{
    return m_OriginalArtist;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::originalArtist() const
{
    return m_OriginalArtist;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::copyright()
{
    return m_Copyright;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::copyright() const
{
    return m_Copyright;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::encoder()
{
    return m_Encoder;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::encoder() const
{
    return m_Encoder;
}

//-------------------------------------------------------------------------------------------

inline QString& Info::disc()
{
    return m_Disc;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::disc() const
{
    return m_Disc;
}

//-------------------------------------------------------------------------------------------
// Info::ChildInfo
//-------------------------------------------------------------------------------------------

inline QString& Info::ChildInfo::name()
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

inline const QString& Info::ChildInfo::name() const
{
    return m_name;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& Info::ChildInfo::startTime()
{
    return m_startTime;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& Info::ChildInfo::startTime() const
{
    return m_startTime;
}

//-------------------------------------------------------------------------------------------

inline common::TimeStamp& Info::ChildInfo::length()
{
    return m_lengthTime;
}

//-------------------------------------------------------------------------------------------

inline const common::TimeStamp& Info::ChildInfo::length() const
{
    return m_lengthTime;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
