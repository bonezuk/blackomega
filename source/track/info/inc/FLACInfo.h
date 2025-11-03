//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_FLACINFO_H
#define __OMEGA_TRACK_INFO_FLACINFO_H
//-------------------------------------------------------------------------------------------

#include <QPair>

#include "engine/inc/IOFile.h"
#include "engine/inc/BitStream.h"
#include "engine/inc/Sequence.h"
#include "engine/greenomega/inc/FLACFramework.h"
#include "track/info/inc/CueInfo.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT FLACInfo : public CueInfo
{
    public:
        FLACInfo();
        virtual ~FLACInfo();

        virtual bool read(common::BIOStream *input);

        virtual bool isImage() const;
        virtual ImageInfoArray *getImageData(ImageFormat& format) const;
        virtual ImageInfoArray *getImageData(IDTagImageType type,ImageFormat& format) const;

    protected:

        QMap<IDTagImageType,QPair<ImageFormat,ImageInfoArray *> > m_imageMap;

        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual void readCuesheet(engine::greenomega::FLACFramework *frameWork);
        virtual void readImages(engine::greenomega::FLACFramework *frameWork);

        virtual bool canGetTrackLength() const;
        virtual common::TimeStamp getTrackLength();
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
