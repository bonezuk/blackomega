//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_IDTAG_ID3TAG2_H
#define __OMEGA_TRACK_IDTAG_ID3TAG2_H
//-------------------------------------------------------------------------------------------

#include <QPair>

#include "common/inc/BOTree.h"
#include "common/inc/UString.h"
#include "track/info/inc/Info.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

#define ID3_4_FRAME_FLAG_COMPRESSION 0x00000008
#define ID3_4_FRAME_FLAG_ENCRYPTION  0x00000004
#define ID3_4_FRAME_FLAG_UNSYNC      0x00000002

#define ID3_3_FRAME_FLAG_COMPRESSION 0x00000080
#define ID3_3_FRAME_FLAG_ENCRYPTION  0x00000040

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT ID3Info2 : public Info
{
    public:
        ID3Info2();
        virtual ~ID3Info2();
        
        virtual bool read(common::BIOStream *input);

        virtual bool isImage() const;
        virtual ImageInfoArray *getImageData(ImageFormat& format) const;
        virtual ImageInfoArray *getImageData(IDTagImageType type,ImageFormat& format) const;
        
    protected:
            
        tint m_version;
        bool m_unsyncFlag;
        bool m_extendedFlag;
        bool m_experimentalFlag;
        bool m_footerFlag;
        
        QMap<IDTagImageType,QPair<ImageFormat,ImageInfoArray *> > m_imageMap;
        
        common::Array<tubyte,tubyte> m_tagBuffer;
        tint m_tagSize;
        tint m_tagOffset;
        tint m_tagLength;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
                        
        virtual bool readHeader(common::BIOStream *input);
        virtual bool readTagBuffer(common::BIOStream *input);
        virtual void readExtendedHeader();
        virtual bool readFrame(tuint& ID,tint& size,tubyte **mem);
        
        virtual tint unsync(tubyte *mem,tint len);

        virtual void frameToString(QString& str,const tchar *mem,tint size);
        virtual void frameToString(common::UString& str,const tchar *mem,tint size);
        virtual void frameToStringISO8859(common::UString& str,const tchar *mem,tint size);
        virtual void frameToStringUTF16(common::UString& str,const tchar *mem,tint size);
        virtual void frameToStringUTF16BE(common::UString& str,const tchar *mem,tint size);
        virtual void frameToStringUTF8(common::UString& str,const tchar *mem,tint size);
        virtual void frameToStringUTF16BigEndian(common::UString& str,const tchar *mem,tint size,tint offset);
        virtual void frameToStringUTF16LittleEndian(common::UString& str,const tchar *mem,tint size,tint offset);

        virtual void frameToImage(const tchar *mem,tint size);

        virtual void decodeString(QString& str,const tchar *mem,tint size,tint& offset);
        virtual void decodeString(common::UString& str,const tchar *mem,tint size,tint& offset);
        virtual void decodeStringISO18859(common::UString& str,const tchar *mem,tint size,tint& offset);
        virtual void decodeStringUTF16(common::UString& str,const tchar *mem,tint size,tint& offset);
        virtual void decodeStringUTF16BE(common::UString& str,const tchar *mem,tint size,tint& offset);
        virtual void decodeStringUTF8(common::UString& str,const tchar *mem,tint size,tint& offset);
        virtual void decodeStringUTF16BigEndian(common::UString& str,const tchar *mem,tint size,int& offset);
        virtual void decodeStringUTF16LittleEndian(common::UString& str,const tchar *mem,tint size,int& offset);
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
