//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_APEINFO_H
#define __OMEGA_TRACK_INFO_APEINFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/CueInfo.h"
#include "track/info/inc/APETag.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT APEInfo : public CueInfo
{
    public:
        APEInfo();
        virtual ~APEInfo();

        virtual bool read(common::BIOStream *input);

        virtual bool isImage() const;
        virtual ImageInfoArray *getImageData(ImageFormat& format) const;
        virtual ImageInfoArray *getImageData(IDTagImageType type,ImageFormat& format) const;

    protected:

        ImageInfoArray *m_imageArray;

        virtual bool isMusepack(const QString& name) const;
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

