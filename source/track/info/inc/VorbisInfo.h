//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_VORBISINFO_H
#define __OMEGA_TRACK_INFO_VORBISINFO_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/IOFile.h"
#include "engine/inc/BitStream.h"
#include "engine/inc/Sequence.h"
#include "engine/silveromega/inc/VSilverContainer.h"
#include "engine/silveromega/inc/VSilverOgg.h"
#include "track/info/inc/Info.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT VorbisInfo : public Info
{
    public:
        VorbisInfo();
        virtual ~VorbisInfo();

        virtual bool read(common::BIOStream *input);

    protected:

        virtual void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
