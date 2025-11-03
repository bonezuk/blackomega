//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_EMBEDDEDID3INFO_H
#define __OMEGA_TRACK_INFO_EMBEDDEDID3INFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/CueInfo.h"
#include "track/info/inc/ID3Info2.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT EmbeddedID3Info : public CueInfo
{
    public:
        EmbeddedID3Info();
        virtual ~EmbeddedID3Info();

        virtual bool isImage() const;
        virtual ImageInfoArray *getImageData(ImageFormat& format) const;
        virtual ImageInfoArray *getImageData(IDTagImageType type,ImageFormat& format) const;
        
    protected:
    
        ID3Info2 *m_id3Tag;
    
        virtual void readID3(common::BIOStream *file);
        virtual void copyInfo(Info *pInfo);
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
