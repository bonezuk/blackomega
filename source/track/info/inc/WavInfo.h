//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_TRACK_INFO_WAVINFO_H
#define __OMEGA_TRACK_INFO_WAVINFO_H
//-------------------------------------------------------------------------------------------

#include "track/info/inc/EmbeddedID3Info.h"
#include "engine/blueomega/inc/WaveInformation.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace track
{
namespace info
{
//-------------------------------------------------------------------------------------------

#define WAVINFO_LIST_ID BLUEOMEGA_ID('L','I','S','T')
#define WAVINFO_INFO_ID BLUEOMEGA_ID('I','N','F','O')
#define WAVINFO_IART_ID BLUEOMEGA_ID('I','A','R','T')
#define WAVINFO_INAM_ID BLUEOMEGA_ID('I','N','A','M')
#define WAVINFO_IPRD_ID BLUEOMEGA_ID('I','P','R','D')
#define WAVINFO_ITRK_ID BLUEOMEGA_ID('I','T','R','K')
#define WAVINFO_ICRD_ID BLUEOMEGA_ID('I','C','R','D')
#define WAVINFO_IGNR_ID BLUEOMEGA_ID('I','G','N','R')
#define WAVINFO_ICMT_ID BLUEOMEGA_ID('I','C','M','T')
#define WAVINFO_ICOP_ID BLUEOMEGA_ID('I','C','O','P')
#define WAVINFO_ISFT_ID BLUEOMEGA_ID('I','S','F','T')
#define WAVINFO_ID3A_ID BLUEOMEGA_ID('i','d','3',' ')

//-------------------------------------------------------------------------------------------

class TRACK_INFO_EXPORT WavInfo : public EmbeddedID3Info
{
    public:
        WavInfo();
        virtual ~WavInfo();
        
        virtual bool read(common::BIOStream *input);
        
    protected:
    
        virtual void printError(const tchar *strR,const tchar *strE) const;

        virtual tint intFromMemory(const tchar *mem) const;
        virtual bool readInfoBlock(const tchar *mem,tint len);
};

//-------------------------------------------------------------------------------------------
} // namespace info
} // namespace track
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
