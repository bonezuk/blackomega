//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_AUDIOIO_WASCHANNEKMAPPING_H
#define __OMEGA_AUDIOIO_WASCHANNEKMAPPING_H
//-------------------------------------------------------------------------------------------
/* At present different speaker arrangement is defined in Black Omega as following a
   surround sound cinema setup. The number of speakers define the surround setup as
   follows

    static const char *c_speaker1 = "1 Speaker : Center";
    static const char *c_speaker2 = "2 Speaker : Stereo";
    static const char *c_speaker3 = "3 Speakers : Stereo + Center";
    static const char *c_speaker4 = "4 Speakers : Quadrophonic";
    static const char *c_speaker5 = "5 Speakers : Surround";
    static const char *c_speaker6 = "5.1 Speakers : Surround + Subwoofer";
    static const char *c_speaker7 = "7 Speakers : Full Surround";
    static const char *c_speaker8 = "7.1 Speakers : Full Surround + Subwoofer";

    The Windows WasAPI defines a large number of speaker channels following speaker
    arrangement but is more verbose.
// Speaker Positions for dwChannelMask in WAVEFORMATEXTENSIBLE:
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000

    The purpose of WasChannelMapping is to map a given channel mask, as reported in the
    WAVEFORMATEXTENSIBLE, when querying the WasAPI for what output format a given device
    supports.
*/

#include "audioio/inc/WasAPIIF.h"
#include "audioio/inc/WasAPICOMInterfaces.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

class AUDIOIO_EXPORT WasChannelMapping
{
    public:
        WasChannelMapping();
        virtual ~WasChannelMapping();
};

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "audioio/inc/WasChannelMapping.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

// Initial breakdown

#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40

#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80

#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_CENTER              0x800

#define SPEAKER_LOW_FREQUENCY           0x8

#define SPEAKER_SIDE_LEFT               0x200

#define SPEAKER_SIDE_RIGHT              0x400

#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_TOP_BACK_LEFT           0x8000

#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_TOP_BACK_RIGHT          0x20000

#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_TOP_BACK_CENTER         0x10000

    static const char *c_speaker1 = "1 Speaker : Center";
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_LEFT           0x8000
#define SPEAKER_TOP_BACK_CENTER         0x10000
#define SPEAKER_TOP_BACK_RIGHT          0x20000


    static const char *c_speaker2 = "2 Speaker : Stereo";
#define SPEAKER_FRONT_LEFT              0x1
#define SPEAKER_BACK_LEFT               0x10
#define SPEAKER_FRONT_LEFT_OF_CENTER    0x40
#define SPEAKER_SIDE_LEFT               0x200
#define SPEAKER_TOP_FRONT_LEFT          0x1000
#define SPEAKER_TOP_BACK_LEFT           0x8000

#define SPEAKER_FRONT_RIGHT             0x2
#define SPEAKER_BACK_RIGHT              0x20
#define SPEAKER_FRONT_RIGHT_OF_CENTER   0x80
#define SPEAKER_SIDE_RIGHT              0x400
#define SPEAKER_TOP_FRONT_RIGHT         0x4000
#define SPEAKER_TOP_BACK_RIGHT          0x20000

/*
#define SPEAKER_FRONT_CENTER            0x4
#define SPEAKER_LOW_FREQUENCY           0x8
#define SPEAKER_BACK_CENTER             0x100
#define SPEAKER_TOP_CENTER              0x800
#define SPEAKER_TOP_FRONT_CENTER        0x2000
#define SPEAKER_TOP_BACK_CENTER         0x10000
*/

    static const char *c_speaker3 = "3 Speakers : Stereo + Center";
    static const char *c_speaker4 = "4 Speakers : Quadrophonic";
    static const char *c_speaker5 = "5 Speakers : Surround";
    static const char *c_speaker6 = "5.1 Speakers : Surround + Subwoofer";
    static const char *c_speaker7 = "7 Speakers : Full Surround";
    static const char *c_speaker8 = "7.1 Speakers : Full Surround + Subwoofer";


//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
