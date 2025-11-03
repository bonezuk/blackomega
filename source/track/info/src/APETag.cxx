#include "track/info/inc/APETag.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------
// APETagItem
//-------------------------------------------------------------------------------------------

APETagItem::APETagItem() : m_flags(0),
    m_key(),
    m_data()
{}

//-------------------------------------------------------------------------------------------

APETagItem::APETagItem(const APETagItem& rhs) : m_flags(0),
    m_key(),
    m_data()
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

APETagItem::~APETagItem()
{}

//-------------------------------------------------------------------------------------------

const APETagItem& APETagItem::operator = (const APETagItem& rhs)
{
    if(this!=&rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

void APETagItem::copy(const APETagItem& rhs)
{
    m_flags = rhs.m_flags;
    m_key = rhs.m_key;
    m_data = rhs.m_data;
}

//-------------------------------------------------------------------------------------------

const QString& APETagItem::key() const
{
    return m_key;
}

//-------------------------------------------------------------------------------------------

bool APETagItem::isString() const
{
    return ((m_flags & 0x00000006)==0 || isLink());
}

//-------------------------------------------------------------------------------------------

bool APETagItem::isData() const
{
    return ((m_flags & 0x00000006)==2);
}

//-------------------------------------------------------------------------------------------

bool APETagItem::isLink() const
{
    return ((m_flags & 0x00000006)==4);
}

//-------------------------------------------------------------------------------------------

const QByteArray& APETagItem::data() const
{
    return m_data;
}

//-------------------------------------------------------------------------------------------

QString APETagItem::text() const
{
    return QString::fromUtf8(m_data);
}

//-------------------------------------------------------------------------------------------

int APETagItem::read(const tbyte *mem,int len)
{
    int amount;
    
    if(mem!=0 && len>8)
    {
        tint sLen;
        bool res = true;
        
        sLen = static_cast<tint>(unsignedIntFromMemory(mem));
        m_flags = unsignedIntFromMemory(&mem[4]);
        
        for(amount=8;amount<len && res && mem[amount]!='\0';amount++)
        {
            tubyte c = static_cast<tubyte>(mem[amount]);
            if(!(c>=0x20 && c<=0x7E))
            {
                res = false;
            }
        }
        
        if(res && amount<len && mem[amount]=='\0')
        {
            m_key = QString::fromLatin1(&mem[8],amount - 8);
            
            if((amount + sLen) < len)
            {
                amount++;
                if(sLen > 0)
                {
                    m_data.clear();
                    m_data.append(&mem[amount],sLen);
                    amount += sLen;
                }
            }
            else
            {
                amount = -1;
            }
        }
        else
        {
            amount = -1;
        }
    }
    else
    {
        amount = -1;
    }
    return amount;
}

//-------------------------------------------------------------------------------------------

tuint32 APETagItem::unsignedIntFromMemory(const tbyte *mem)
{
    tuint32 y;
    const tubyte *x = reinterpret_cast<const tubyte *>(mem);
    
    y = ((static_cast<tuint32>(x[3]) << 24) & 0xff000000) |
        ((static_cast<tuint32>(x[2]) << 16) & 0x00ff0000) |
        ((static_cast<tuint32>(x[1]) <<  8) & 0x0000ff00) |
        ((static_cast<tuint32>(x[0])      ) & 0x000000ff);
    return y;
}

//-------------------------------------------------------------------------------------------
// APETag
//-------------------------------------------------------------------------------------------

APETag::APETag() : m_items()
{}

//-------------------------------------------------------------------------------------------

APETag::~APETag()
{}

//-------------------------------------------------------------------------------------------

const QVector<APETagItem>& APETag::items() const
{
    return m_items;
}

//-------------------------------------------------------------------------------------------

bool APETag::read(common::BIOStream *file)
{
    bool res = false;
    
    if(file!=0)
    {
        int version,size,noItems,flags;
        int cPos = file->offset();
        
        if(file->seek(-32,common::e_Seek_End))
        {
            tbyte hdr[32];
            
            if(file->read(hdr,32)==32)
            {
                if(readHeader(hdr,32,version,size,noItems,flags))
                {
                    tbyte *mem = new tbyte [size];
                    
                    if(file->seek(-size,common::e_Seek_End))
                    {
                        if(file->read(mem,size)==size)
                        {
                            tint i;
                            tint pos = 0;
                        
                            res = true;
                            m_items.clear();
                    
                            for(i=0;i<noItems && res;i++)
                            {
                                tint amount;
                                APETagItem item;
                            
                                amount = item.read(&mem[pos],size - pos);
                                if(amount > 0)
                                {
                                    m_items.append(item);
                                    pos += amount;
                                }
                                else
                                {
                                    res = false;
                                }
                            }
                        
                            if(i != noItems)
                            {
                                res = false;
                            }
                        }
                    }
                    delete [] mem;
                }
            }
        }
        file->seek(cPos,common::e_Seek_Start);
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool APETag::readHeader(const tbyte *mem,tint len,tint& tagVersion,tint& tagSize,tint& noItems,tint& flags)
{
    bool res = false;

    if(mem!=0 && len==32)
    {
        const tbyte apeID[8] = {'A','P','E','T','A','G','E','X'};
        
        if(::memcmp(mem,apeID,8)==0)
        {
            tagVersion = static_cast<tint>(APETagItem::unsignedIntFromMemory(&mem[8]));
            if(tagVersion==1000 || tagVersion==2000)
            {
                tagSize = static_cast<tint>(APETagItem::unsignedIntFromMemory(&mem[12]));
                noItems = static_cast<tint>(APETagItem::unsignedIntFromMemory(&mem[16]));
                flags = static_cast<tint>(APETagItem::unsignedIntFromMemory(&mem[20]));
                res = true;
            }
        }
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// APETagImage
//-------------------------------------------------------------------------------------------

APETagImage::APETagImage(const APETagItem& item) : m_item(item),
    m_image()
{}

//-------------------------------------------------------------------------------------------

APETagImage::~APETagImage()
{}

//-------------------------------------------------------------------------------------------

bool APETagImage::isImage()
{
    return (!image().isNull());
}

//-------------------------------------------------------------------------------------------

int APETagImage::offset() const
{
    int pos = -1;
    
    if(m_item.isData())
    {
        int i,len = m_item.data().size();
        const char *x = m_item.data().constData();
        
        for(i=0;i<len && x[i]!='\0';i++) ;
        if(i<len)
        {
            pos = i;
        }
    }
    return pos;
}

//-------------------------------------------------------------------------------------------

QString APETagImage::name()
{
    int pos;
    QString n;

    pos = offset();
    if(pos>0)
    {
        n = QString::fromLatin1(m_item.data().constData(),pos);
    }
    return n;
}

//-------------------------------------------------------------------------------------------

QImage APETagImage::image()
{
    if(m_image.isNull())
    {
        int pos = offset();
        if(pos>=0)
        {
            const char *mem = m_item.data().constData();
            m_image = QImage::fromData(reinterpret_cast<const tubyte *>(&mem[pos+1]),m_item.data().size() - (pos+1));
        }
    }
    return m_image;
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *APETagImage::imageData()
{
    int pos;
    ImageInfoArray *array = 0;
    
    pos = offset();
    if(pos>=0)
    {
        const char *mem = m_item.data().constData();
        array = new ImageInfoArray;
        array->AppendRaw(&mem[pos+1],m_item.data().size() - (pos+1));
    }
    return array;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
