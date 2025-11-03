#include "track/info/inc/ID3Info2.h"

#if QT_VERSION > 0x050000
typedef char16_t omegashort;
#else
typedef ushort omegashort;
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

ID3Info2::ID3Info2() : Info(),
    m_version(0),
    m_unsyncFlag(false),
    m_extendedFlag(false),
    m_experimentalFlag(false),
    m_footerFlag(false),
    m_imageMap(),
    m_tagBuffer(),
    m_tagSize(0),
    m_tagOffset(0),
    m_tagLength(0)
{
    m_type = Info::e_InfoID3Type2;
}

//-------------------------------------------------------------------------------------------

ID3Info2::~ID3Info2()
{
    QMap<IDTagImageType,QPair<ImageFormat,ImageInfoArray *> >::iterator ppI;

    for(ppI=m_imageMap.begin();ppI!=m_imageMap.end();ppI++)
    {
        ImageInfoArray *pArr = ppI.value().second;
        delete pArr;
    }
    m_imageMap.clear();
}

//-------------------------------------------------------------------------------------------

void ID3Info2::printError(const tchar *strR,const tchar *strE) const
{
    common::Log::g_Log << "ID3Info2::" << strR << " - " << strE << common::c_endl;
}

//-------------------------------------------------------------------------------------------

bool ID3Info2::read(common::BIOStream *input)
{
    tchar *data;
    tint size;
    tuint ID;
    bool res;

    if(input==0)
    {
        printError("read","No input to read from given");
        return false;
    }
    if(!readHeader(input))
    {
        printError("read","Failed to read ID3v2 tag header");
        return false;
    }
    if(!readTagBuffer(input))
    {
        printError("read","Failed to read ID3v2 tag data");
        return false;
    }
    readExtendedHeader();

    while(res = readFrame(ID,size,reinterpret_cast<tubyte **>(&data)),res && ID!=0)
    {
        if(data!=0)
        {
            if(ID == ID3_TAG('T','P','E','1') || ID == ID3_TAG('\0','T','P','1')) // Artist
            {
                frameToString(m_Artist,data,size);
            }
            else if(ID == ID3_TAG('T','I','T','2') || ID == ID3_TAG('\0','T','T','2')) // Title
            {
                frameToString(m_Title,data,size);
            }
            else if(ID == ID3_TAG('T','A','L','B') || ID == ID3_TAG('\0','T','A','L')) // Album
            {
                frameToString(m_Album,data,size);
            }
            else if(ID == ID3_TAG('T','Y','E','R') || ID == ID3_TAG('\0','T','Y','E')) // Year
            {
                frameToString(m_Year,data,size);
            }
            else if(ID == ID3_TAG('C','O','M','M') || ID == ID3_TAG('\0','C','O','M')) // Comment
            {
                frameToString(m_Comment,data,size);
            }
            else if(ID == ID3_TAG('T','C','O','N') || ID == ID3_TAG('\0','T','C','O')) // Genre
            {
                frameToString(m_Genre,data,size);
            }
            else if(ID == ID3_TAG('T','R','C','K') || ID == ID3_TAG('\0','T','R','K')) // Track
            {
                frameToString(m_Track,data,size);

                // Seek for positional information and remove it.
                {
                    tint i;

                    for(i=0;i<m_Track.length();++i)
                    {
                        if(m_Track.at(i)==QChar('/') || m_Track.at(i)==QChar('\\'))
                        {
                            m_Track = m_Track.mid(0,i);
                            break;
                        }
                    }
                }
            }
            else if(ID == ID3_TAG('T','C','O','M') || ID == ID3_TAG('\0','T','C','M')) // Composer
            {
                frameToString(m_Composer,data,size);
            }
            else if(ID == ID3_TAG('T','O','P','E') || ID == ID3_TAG('\0','T','O','A')) // Original Artist
            {
                frameToString(m_OriginalArtist,data,size);
            }
            else if(ID == ID3_TAG('T','C','O','P') || ID == ID3_TAG('\0','T','C','R')) // Copyright
            {
                frameToString(m_Copyright,data,size);
            }
            else if(ID == ID3_TAG('T','E','N','C') || ID == ID3_TAG('\0','T','E','N')) // Encoder
            {
                frameToString(m_Encoder,data,size);
            }
            else if(ID == ID3_TAG('A','P','I','C') || ID == ID3_TAG('\0','P','I','C')) // Picture
            {
                frameToImage(data,size);
            }
        }

        if(m_tagOffset >= m_tagLength)
        {
            break;
        }
    }
    return true;
}

//-------------------------------------------------------------------------------------------

bool ID3Info2::readHeader(common::BIOStream *input)
{
    tubyte mem[10];
    bool res = false;

    if(input->read(mem,10)==10)
    {
        if(mem[0]=='I' && mem[1]=='D' && mem[2]=='3')
        {
            m_version = static_cast<tint>((static_cast<tuint>(mem[3]) << 8) | static_cast<tuint>(mem[4]));
            if(m_version>=0x0200 && m_version<0x04ff)
            {
                if(m_version>=0x400)
                {
                    m_footerFlag = (mem[5] & 0x10) ? true : false;
                }
                else
                {
                    m_footerFlag = false;
                }
                m_unsyncFlag = (mem[5] & 0x80) ? true : false;
                m_extendedFlag = (mem[5] & 0x40) ? true : false;
                m_experimentalFlag = (mem[5] & 0x20) ? true : false;

                m_tagSize = static_cast<tint>( ((static_cast<tuint>(mem[6]) << 21) & 0x0fe00000) |
                                               ((static_cast<tuint>(mem[7]) << 14) & 0x001fc000) |
                                               ((static_cast<tuint>(mem[8]) <<  7) & 0x00003f80) |
                                               ((static_cast<tuint>(mem[9])      ) & 0x0000007f) );

                res = true;
            }
            else
            {
                printError("readHeader","Unsupported ID3 version tag");
            }
        }
        else
        {
            printError("readHeader","Not ID3 tag");
        }
    }
    else
    {
        printError("readHeader","Failed to read in ID3 tag header");
    }
    return res;
}

//-------------------------------------------------------------------------------------------

bool ID3Info2::readTagBuffer(common::BIOStream *input)
{
    bool res = true;

    m_tagBuffer.SetSize(m_tagSize);
    m_tagOffset = 0;
    m_tagLength = m_tagSize;

    if(input->read(m_tagBuffer.GetData(),m_tagSize)==m_tagSize)
    {
        if(m_unsyncFlag && m_version>=0x0300 && m_version<=0x03ff)
        {
            m_tagLength = unsync(m_tagBuffer.GetData(),m_tagSize);
        }
    }
    else
    {
        printError("readHeader","Failed to read ID3 tag data from file");
        res = false;
    }
    return res;
}

//-------------------------------------------------------------------------------------------

void ID3Info2::readExtendedHeader()
{
    if(m_extendedFlag)
    {
        tint size;
        tubyte *mem = m_tagBuffer.GetData();

        mem += m_tagOffset;
        size = static_cast<tint>( ((static_cast<tuint>(mem[0]) << 21) & 0x0fe00000) |
                                  ((static_cast<tuint>(mem[1]) << 14) & 0x001fc000) |
                                  ((static_cast<tuint>(mem[2]) <<  7) & 0x00003f80) |
                                  ((static_cast<tuint>(mem[3])      ) & 0x0000007f) );
        m_tagOffset = 4 + size;
    }
}

//-------------------------------------------------------------------------------------------

bool ID3Info2::readFrame(tuint& ID,tint& size,tubyte **dataPtr)
{
    tint flags;
    tubyte *mem = m_tagBuffer.GetData();

    mem += m_tagOffset;

    if(m_version>=0x0300)
    {
        if((m_tagOffset + 10) > m_tagLength)
        {
            return false;
        }
        m_tagOffset += 10;


        ID = ((static_cast<tuint>(mem[0]) << 24) & 0xff000000) |
             ((static_cast<tuint>(mem[1]) << 16) & 0x00ff0000) |
             ((static_cast<tuint>(mem[2]) <<  8) & 0x0000ff00) |
             ( static_cast<tuint>(mem[3])        & 0x000000ff);
    }
    else
    {
        if((m_tagOffset + 6) > m_tagLength)
        {
            return false;
        }
        m_tagOffset += 6;

        ID = ((static_cast<tuint>(mem[0]) << 16) & 0x00ff0000) |
             ((static_cast<tuint>(mem[1]) <<  8) & 0x0000ff00) |
             ( static_cast<tuint>(mem[2])        & 0x000000ff);
    }

    if(ID==0)
    {
        *dataPtr = 0;
        return true;
    }

    if(m_version>=0x0400 && m_version<=0x04ff)
    {
        size = static_cast<tint>( ((static_cast<tuint>(mem[4]) << 21) & 0x0fe00000) |
                                  ((static_cast<tuint>(mem[5]) << 14) & 0x001fc000) |
                                  ((static_cast<tuint>(mem[6]) <<  7) & 0x00003f80) |
                                  ((static_cast<tuint>(mem[7])      ) & 0x0000007f) );

        if((m_tagOffset + size) > m_tagLength)
        {
            return false;
        }

        flags = static_cast<tint>( ((static_cast<tuint>(mem[8]) << 8) & 0x0000ff00) | (static_cast<tuint>(mem[9]) & 0x000000ff) );

        if((flags & ID3_4_FRAME_FLAG_COMPRESSION) || (flags & ID3_4_FRAME_FLAG_ENCRYPTION))
        {
            m_tagOffset += size;
            *dataPtr = 0;
            return true;
        }

        *dataPtr = &mem[10];
        m_tagOffset += size;

        if(m_unsyncFlag || (flags & ID3_4_FRAME_FLAG_UNSYNC))
        {
            size = unsync(&mem[10],size);
        }
    }
    else if(m_version >=0x0300 && m_version<=0x03ff)
    {
        size = static_cast<tint>( ((static_cast<tuint>(mem[4]) << 24) & 0xff000000) |
                                  ((static_cast<tuint>(mem[5]) << 16) & 0x00ff0000) |
                                  ((static_cast<tuint>(mem[6]) <<  8) & 0x0000ff00) |
                                  ((static_cast<tuint>(mem[7])      ) & 0x000000ff) );

        if((m_tagOffset + size) > m_tagLength)
        {
            return false;
        }

        flags = static_cast<tint>( ((static_cast<tuint>(mem[8]) << 8) & 0x0000ff00) | (static_cast<tuint>(mem[9]) & 0x000000ff) );

        if((flags & ID3_3_FRAME_FLAG_COMPRESSION) || (flags & ID3_3_FRAME_FLAG_ENCRYPTION))
        {
            m_tagOffset += size;
            *dataPtr = 0;
            return true;
        }

        *dataPtr = &mem[10];
        m_tagOffset += size;
    }
    else if(m_version>=0x0200 && m_version<=0x02ff)
    {
        size = static_cast<tint>( ((static_cast<tuint>(mem[3]) << 16) & 0x00ff0000) |
                                  ((static_cast<tuint>(mem[4]) <<  8) & 0x0000ff00) |
                                  ((static_cast<tuint>(mem[5])      ) & 0x000000ff) );

        if((m_tagOffset + size) > m_tagLength)
        {
            return false;
        }

        *dataPtr = &mem[6];
        m_tagOffset += size;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

tint ID3Info2::unsync(tubyte *mem,tint len)
{
    tint a = 0,b = 0,state = 0;

    while(b < len)
    {
        tubyte x = mem[b++];

        if(!state)
        {
            mem[a++] = x;
            if(x==0xff)
            {
                state = 1;
            }
        }
        else
        {
            if(x!=0x00)
            {
                mem[a++] = x;
            }
            state = 0;
        }
    }
    return a;
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToString(QString& str,const tchar *mem,tint size)
{
    common::UString t;

    frameToString(t,mem,size);
    str = QString::fromUtf16(reinterpret_cast<const omegashort *>(t.Ptr()));
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToString(common::UString& str,const tchar *mem,tint size)
{
    const tuchar *m = reinterpret_cast<const tuchar *>(mem);

    str = "";

    if(m!=0)
    {
        if(m[0]==0x00)
        {
            frameToStringISO8859(str,mem,size);
        }
        else if(m[0]==0x01)
        {
            frameToStringUTF16(str,mem,size);
        }
        else if(m[0]==0x02)
        {
            frameToStringUTF16BE(str,mem,size);
        }
        else if(m[0]==0x03)
        {
            frameToStringUTF8(str,mem,size);
        }
    }
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToStringISO8859(common::UString& str,const tchar *mem,tint size)
{
    tint i,j;
    const tuchar *m = reinterpret_cast<const tuchar *>(mem);
    tushort *x = new tushort [size + 1];

    for(i=1,j=0;i<size;++i)
    {
        if(m[i]>=0x20 && m[i]<=0x7e)
        {
            x[j] = static_cast<tushort>(m[i]);
            ++j;
        }
        else if(m[i]>=0xa0 && m[i]<=0xff)
        {
            x[j] = static_cast<tushort>(m[i]);
            ++j;
        }
        else if(m[i]==0x0a)
        {
            x[j] = static_cast<tushort>(m[i]);
            ++j;
        }
    }
    x[j] = 0;
    str = x;
    delete [] x;
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToStringUTF16(common::UString& str,const tchar *mem,tint size)
{
    const tuchar *m = reinterpret_cast<const tuchar *>(mem);

    if(size>3)
    {
        if(m[1]==0xff && m[2]==0xfe)
        {
            frameToStringUTF16LittleEndian(str,mem,size,3);
        }
        else
        {
            frameToStringUTF16BigEndian(str,mem,size,3);
        }
    }
    else
    {
        str = "";
    }
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToStringUTF16BE(common::UString& str,const tchar *mem,tint size)
{
    frameToStringUTF16LittleEndian(str,mem,size,1);
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToStringUTF8(common::UString& str,const tchar *mem,tint size)
{
    common::BString tmp(&mem[1],size - 1);
    common::UString::DecodeUTF8(tmp,str);
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToStringUTF16BigEndian(common::UString& str,const tchar *mem,tint size,tint offset)
{
    tint i,j;
    tushort *x = new tushort [(size>>1) + 2];

    for(i=offset,j=0;i<size;i+=2,++j)
    {
        x[j] = (static_cast<tushort>(mem[i+1]) & 0x00ff) | ((static_cast<tushort>(mem[i]) << 8) & 0xff00);
    }
    x[j] = 0;
    str = x;
    delete [] x;
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToStringUTF16LittleEndian(common::UString& str,const tchar *mem,tint size,tint offset)
{
    tint i,j;
    tushort *x = new tushort [(size>>1) + 2];

    for(i=offset,j=0;i<size;i+=2,++j)
    {
        x[j] = (static_cast<tushort>(mem[i]) & 0x00ff) | ((static_cast<tushort>(mem[i+1]) << 8) & 0xff00);
    }
    x[j] = 0;
    str = x;
    delete [] x;
}
//-------------------------------------------------------------------------------------------

void ID3Info2::decodeString(QString& str,const tchar *mem,tint size,tint& offset)
{
    common::UString t;

    decodeString(t,mem,size,offset);
    str = QString::fromUtf16(reinterpret_cast<const omegashort *>(t.Ptr()));
}

//-------------------------------------------------------------------------------------------

void ID3Info2::decodeString(common::UString& str,const tchar *mem,tint size,tint& offset)
{
    const tuchar *m = reinterpret_cast<const tuchar *>(mem);

    if(offset < size)
    {
        offset++;

        if(m[0]==0x00)
        {
            decodeStringISO18859(str,mem,size,offset);
        }
        else if(m[1]==0x01)
        {
            decodeStringUTF16(str,mem,size,offset);
        }
        else if(m[2]==0x02)
        {
            decodeStringUTF16BE(str,mem,size,offset);
        }
        else if(m[3]==0x03)
        {
            decodeStringUTF8(str,mem,size,offset);
        }
    }
}

//-------------------------------------------------------------------------------------------

void ID3Info2::decodeStringISO18859(common::UString& str,const tchar *mem,tint size,tint& offset)
{
    tint i,j;
    const tuchar *m = reinterpret_cast<const tuchar *>(mem);
    tushort x[128];

    for(i=offset,j=0;i<size && mem[i]!='\0';++i)
    {
        if(m[i]>=0x20 && m[i]<=0x7e)
        {
            x[j] = static_cast<tushort>(m[i]);
            ++j;
        }
        else if(m[i]>=0xa0 && m[i]<=0xff)
        {
            x[j] = static_cast<tushort>(m[i]);
            ++j;
        }
        else if(m[i]==0x0a)
        {
            x[j] = static_cast<tushort>(m[i]);
            ++j;
        }

        if(j>=127)
        {
            x[j] = 0;
            str += x;
            j = 0;
        }
    }
    offset = i + 1;

    if(j>0)
    {
        x[j] = 0;
        str += x;
    }
}

//-------------------------------------------------------------------------------------------

void ID3Info2::decodeStringUTF16(common::UString& str,const tchar *mem,tint size,tint& offset)
{
    const tuchar *m = reinterpret_cast<const tuchar *>(mem);

    if((offset+2) < size)
    {
        if(m[offset]==0xff && m[offset+1]==0xfe)
        {
            offset += 2;
            decodeStringUTF16LittleEndian(str,mem,size,offset);
        }
        else
        {
            offset += 2;
            decodeStringUTF16BigEndian(str,mem,size,offset);
        }
    }
}

//-------------------------------------------------------------------------------------------

void ID3Info2::decodeStringUTF16BE(common::UString& str,const tchar *mem,tint size,tint& offset)
{
    decodeStringUTF16BigEndian(str,mem,size,offset);
}

//-------------------------------------------------------------------------------------------

void ID3Info2::decodeStringUTF8(common::UString& str,const tchar *mem,tint size,tint& offset)
{
    tint i = offset;
    common::BString tmp;

    while(offset<size && mem[offset]!='\0')
    {
        offset++;
    }
    offset++;
    tmp = &mem[i];
    common::UString::DecodeUTF8(tmp,str);
}

//-------------------------------------------------------------------------------------------

void ID3Info2::decodeStringUTF16BigEndian(common::UString& str,const tchar *mem,tint size,int& offset)
{
    tint i,j;
    tushort x[128];

    for(i=offset,j=0;i<size;i+=2)
    {
        if(mem[i]==0 && mem[i+1]==0)
        {
            i += 2;
            break;
        }
        else
        {
            x[j] = (static_cast<tushort>(mem[i+1]) & 0x00ff) | ((static_cast<tushort>(mem[i]) << 8) & 0xff00);
            j++;
            if(j>=127)
            {
                x[j] = 0;
                str += x;
                j = 0;
            }
        }
    }
    if(j>0)
    {
        x[j] = 0;
        str += x;
    }
    offset = i;
}

//-------------------------------------------------------------------------------------------

void ID3Info2::decodeStringUTF16LittleEndian(common::UString& str,const tchar *mem,tint size,int& offset)
{
    tint i,j;
    tushort x[128];

    for(i=offset,j=0;i<size;i+=2)
    {
        if(mem[i]==0 && mem[i+1]==0)
        {
            i += 2;
            break;
        }
        else
        {
            x[j] = (static_cast<tushort>(mem[i]) & 0x00ff) | ((static_cast<tushort>(mem[i+1]) << 8) & 0xff00);
            j++;
            if(j>=127)
            {
                x[j] = 0;
                str += x;
                j = 0;
            }
        }
    }
    if(j>0)
    {
        x[j] = 0;
        str += x;
    }
    offset = i;
}

//-------------------------------------------------------------------------------------------

void ID3Info2::frameToImage(const tchar *mem,tint size)
{
    if(mem!=0)
    {
        ImageFormat iFormat = e_imageUnknown;
        tint offset = 1,txtEncoding,picType;
        common::UString mimeType,description;

        if(size>0)
        {
            txtEncoding = static_cast<tint>(mem[0]);
            switch(txtEncoding)
            {
                case 1:
                    decodeStringUTF16(mimeType,mem,size,offset);
                    break;

                case 2:
                    decodeStringUTF16BE(mimeType,mem,size,offset);
                    break;

                case 3:
                    decodeStringUTF8(mimeType,mem,size,offset);
                    break;

                default:
                    decodeStringISO18859(mimeType,mem,size,offset);
                    break;
            }

            {
                QString mType(QString::fromUtf16(reinterpret_cast<const omegashort *>(mimeType.getString()),mimeType.length()));

                if(mType.contains("jpeg",Qt::CaseInsensitive) || mType.contains("jpg",Qt::CaseInsensitive))
                {
                    iFormat = e_imageJPEG;
                }
                else if(mType.contains("gif",Qt::CaseInsensitive))
                {
                    iFormat = e_imageGIF;
                }
                else if(mType.contains("png",Qt::CaseInsensitive))
                {
                    iFormat = e_imagePNG;
                }
            }

            if(offset<size)
            {
                ImageInfoArray *arr = new ImageInfoArray;

                picType = static_cast<tint>(mem[offset]);
                offset++;
                switch(txtEncoding)
                {
                    case 1:
                        decodeStringUTF16(description,mem,size,offset);
                        break;

                    case 2:
                        decodeStringUTF16BE(description,mem,size,offset);
                        break;

                    case 3:
                        decodeStringUTF8(description,mem,size,offset);
                        break;

                    default:
                        decodeStringISO18859(description,mem,size,offset);
                        break;
                }

                if((offset<size) && picType>=0x00 && picType<=0x14)
                {
                    arr->AppendRaw(reinterpret_cast<const tbyte *>(&mem[offset]),size - offset);
                    m_imageMap.insert(static_cast<IDTagImageType>(picType),QPair<ImageFormat,ImageInfoArray *>(iFormat,arr));
                }
                else
                {
                    delete arr;
                }
            }
        }
    }
}

//-------------------------------------------------------------------------------------------

bool ID3Info2::isImage() const
{
    if(m_imageMap.size()>0)
    {
        return true;
    }
    else
    {
        return Info::isImage();
    }
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *ID3Info2::getImageData(ImageFormat& format) const
{
    static IDTagImageType typeOrder[13] = {
        e_TagImage_CoverFront,
        e_TagImage_LeadArtist,
        e_TagImage_Artist,
        e_TagImage_Conductor,
        e_TagImage_Band,
        e_TagImage_Lyricist,
        e_TagImage_Illustration,
        e_TagImage_BandLogo,
        e_TagImage_StudioLogo,
        e_TagImage_CoverBack,
        e_TagImage_Composer,
        e_TagImage_Performance,
        e_TagImage_Other};

    tint i;
    ImageInfoArray *pArr = 0;

    for(i=0;i<13 && pArr==0;i++)
    {
        pArr = getImageData(typeOrder[i],format);
    }
    if(pArr==0)
    {
        pArr = Info::getImageData(format);
    }
    return pArr;
}

//-------------------------------------------------------------------------------------------

ImageInfoArray *ID3Info2::getImageData(IDTagImageType type,ImageFormat& format) const
{
    ImageInfoArray *pArr = 0;
    QMap<IDTagImageType,QPair<ImageFormat,ImageInfoArray *> >::const_iterator ppI;

    ppI = m_imageMap.find(type);
    if(ppI!=m_imageMap.end())
    {
        const QPair<ImageFormat,ImageInfoArray *>& p = ppI.value();
        format = p.first;
        pArr = p.second;
    }
    if(pArr==0 && type==e_TagImage_CoverFront)
    {
        pArr = Info::getImageData(type,format);
    }
    return pArr;
}

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
