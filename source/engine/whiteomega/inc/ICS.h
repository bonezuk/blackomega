//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_ICS_H
#define __OMEGA_WHITEOMEGA_ICS_H
//-------------------------------------------------------------------------------------------

#include "engine/whiteomega/inc/ProgramConfig.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

#define MAX_CHANNELS        64
#define MAX_SYNTAX_ELEMENTS 48
#define MAX_WINDOW_GROUPS    8
#define MAX_SFB             51
#define MAX_LTP_SFB         40
#define MAX_LTP_SFB_S        8

#define ONLY_LONG_SEQUENCE   0
#define LONG_START_SEQUENCE  1
#define EIGHT_SHORT_SEQUENCE 2
#define LONG_STOP_SEQUENCE   3

#define ZERO_HCB       0
#define FIRST_PAIR_HCB 5
#define ESC_HCB        11
#define QUAD_LEN       4
#define PAIR_LEN       2
#define NOISE_HCB      13
#define INTENSITY_HCB2 14
#define INTENSITY_HCB  15

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint limit;
    bool predictorDataPresent;
    bool predictorReset;
    tint predictorResetGroupNumber;
    tint predictionUsed[MAX_SFB];
} PredictionInfo;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint lastBand;
    bool dataPresent;
    bool lagUpdate;
    tint lag;
    tint coefficient;
    tint longUsed[MAX_SFB];
} LTPInfo;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint numberPulse;
    tint pulseStartSfb;
    tint pulseOffset[4];
    tint pulseAmp[4];
} PulseData;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint nFilt[8];
    tint coefRes[8];
    tint length[8][4];
    tint order[8][4];
    tint direction[8][4];
    tint coefCompress[8][4];
    tint coef[8][4][32];
} TNSData;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint maxBand;
    tint adjustNum[4][8];
    tint alevcode[4][8][8];
    tint aloccode[4][8][8];
} SSRInfo;

//-------------------------------------------------------------------------------------------

typedef struct
{
    tint maxSfb; // ics_info

    tint numSwb;
    tint numWindowGroups;
    tint numWindows;
    tint windowSequence; // ics_info
    tint windowGroupLength[8];
    tint windowShape; // ics_info
    tint scaleFactorGrouping; // ics_info
    tint sectSfbOffset[8][15 * 8];
    tint swbOffset[52];
    tint swbOffsetMax;

    tint sectCb[8][15 * 8];
    tint sectStart[8][15 * 8];
    tint sectEnd[8][15 * 8];
    tint sfbCb[8][15 * 8];
    tint numSec[8];

    bool noiseFlag;
    bool intensityFlag;

    tint globalGain;
    tint scaleFactors[8][51];
    tint escapeSF[8 * 51];

    tint sfConcealment;
    tint revGlobalGain;
    tint lengthOfRvlcSf;
    tint lengthOfRvlcEscapes;

    tint msMaskPresent;
    tint msUser[MAX_WINDOW_GROUPS][MAX_SFB];

    PredictionInfo pred; // ics_info

    bool ltpDataPresent;
    LTPInfo ltp1; // ics_info
    LTPInfo ltp2; // ics_info

    bool pulseDataPresent;
    PulseData pulse;

    bool tnsDataPresent;
    TNSData tns;

    bool gainControlDataPresent;
    SSRInfo ssr;

} ICSInfo;


//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

