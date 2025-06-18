//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_AIFFINFO_H
#define __OMEGA_TRACK_INFO_AIFFINFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/EmbeddedID3Info.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT AIFFInfo : public EmbeddedID3Info
{
    public:
        AIFFInfo();
        virtual ~AIFFInfo();
        
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

