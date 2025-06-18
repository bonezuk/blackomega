//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_APETAG_H
#define __OMEGA_TRACK_INFO_APETAG_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/Info.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class APETag;

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT APETagItem
{
    public:
        APETagItem();
        APETagItem(const APETagItem& rhs);
        virtual ~APETagItem();
        
        const APETagItem& operator = (const APETagItem& rhs);
        
        virtual const QString& key() const;
        
        virtual bool isString() const;
        virtual bool isData() const;
        virtual bool isLink() const;
        
        virtual const QByteArray& data() const;
        virtual QString text() const;
        
        virtual int read(const tbyte *mem,int len);
        
        static tuint32 unsignedIntFromMemory(const tbyte *mem);

    protected:
    
        tuint32 m_flags;
        QString m_key;
        QByteArray m_data;
        
        void copy(const APETagItem& rhs);
};

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT APETagImage
{
    public:
        APETagImage(const APETagItem& item);
        virtual ~APETagImage();
        
        virtual bool isImage();
        
        virtual QString name();
        virtual QImage image();
        virtual ImageInfoArray *imageData();
        
    protected:
    
        const APETagItem& m_item;
        QImage m_image;
        
        int offset() const;
};

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT APETag
{
    public:
        APETag();
        virtual ~APETag();
        
        virtual const QVector<APETagItem>& items() const;
        
        virtual bool read(common::BIOStream *file);
        
    protected:
    
        QVector<APETagItem> m_items;
        
        virtual bool readHeader(const tbyte *mem,tint len,tint& tagVersion,tint& tagSize,tint& noItems,tint& flags);
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
