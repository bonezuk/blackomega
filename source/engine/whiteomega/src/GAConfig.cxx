#include "engine/whiteomega/inc/GAConfig.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

GAConfig::GAConfig() : m_audioObjectType(e_audioUnknown),
    m_samplingFrequencyIndex(0),
    m_samplingFrequency(0),
    m_channelConfiguration(0),
    m_sbrPresentFlag(false),
    m_extAudioObjectType(e_audioUnknown),
    m_extSamplingFrequencyIndex(0),
    m_extSamplingFrequency(0),
    m_syncExtensionType(0),
    m_frameLength(1024),
    m_frameLengthFlag(false),
    m_dependsOnCoreCoder(false),
    m_coreCoderDelay(0),
    m_extensionFlag(false),
    m_layerNr(0),
    m_numOfSubFrame(0),
    m_layerLength(0),
    m_aacSectionDataResilienceFlag(false),
    m_aacScalefactorDataResilienceFlag(false),
    m_aacSpectralDataResilienceFlag(false),
    m_programConfig()
{}

//-------------------------------------------------------------------------------------------

GAConfig::~GAConfig()
{}

//-------------------------------------------------------------------------------------------

tint GAConfig::getSampleRate(int idx)
{
    static const tint rates[] = {
        96000, 88200, 64000, 48000, 44100, 32000,
        24000, 22050, 16000, 12000, 11025, 8000
    };

    if(idx < 12)
    {
        return rates[idx];
    }
    else
    {
        return 0;
    }
}

//-------------------------------------------------------------------------------------------

tint GAConfig::getSampleRateIndex(tint rate)
{
    if(rate>=92017)
    {
        return 0;
    }
    else if(rate>=75132)
    {
        return 1;
    }
    else if(rate>=55426)
    {
        return 2;
    }
    else if(rate>=46009)
    {
        return 3;
    }
    else if(rate>=37566)
    {
        return 4;
    }
    else if(rate>=27713)
    {
        return 5;
    }
    else if(rate>=23004)
    {
        return 6;
    }
    else if(rate>=18783)
    {
        return 7;
    }
    else if(rate>=13856)
    {
        return 8;
    }
    else if(rate>=11502)
    {
        return 9;
    }
    else if(rate>=9391)
    {
        return 10;
    }
    else
    {
        return 11;
    }
}

//-------------------------------------------------------------------------------------------
// 1.6.2.1 - Table 1.14
//-------------------------------------------------------------------------------------------

bool GAConfig::readObjectType(Sequence *seq,AudioType& type)
{
    tint n;
    bool res;

    n = seq->readBitsI(5);
    if(n==31)
    {
        n = 32 + seq->readBitsI(6);
    }
    switch(n)
    {
        case 1:
            type = e_audioAACMain;
            res = true;
            break;

        case 2:
            type = e_audioAACLC;
            res = true;
            break;

        case 3:
            type = e_audioAACSSR;
            res = true;
            break;

        case 4:
            type = e_audioAACLTP;
            res = true;
            break;

        case 5:
            type = e_audioSBR;
            res = true;
            break;

        case 6:
            type = e_audioAACScalable;
            res = false;
            break;

        case 7:
            type = e_audioTwinVQ;
            res = false;
            break;

        case 8:
            type = e_audioCELP;
            res = false;
            break;

        case 9:
            type = e_audioHVXC;
            res = false;
            break;

        case 12:
            type = e_audioTTSI;
            res = false;
            break;

        case 13:
            type = e_audioMainSynthetic;
            res = false;
            break;

        case 14:
            type = e_audioWaveSynthesis;
            res = false;
            break;

        case 15:
            type = e_audioGeneralMIDI;
            res = false;
            break;

        case 16:
            type = e_audioAudioFX;
            res = false;
            break;

        case 17:
            type = e_audioERAACLC;
            res = true;
            break;

        case 19:
            type = e_audioERAACLTP;
            res = true;
            break;

        case 20:
            type = e_audioERAACScalable;
            res = false;
            break;

        case 21:
            type = e_audioERTwinVQ;
            res = false;
            break;

        case 22:
            type = e_audioERBSAC;
            res = false;
            break;

        case 23:
            type = e_audioERAACLD;
            res = true;
            break;

        case 24:
            type = e_audioERCELP;
            res = false;
            break;

        case 25:
            type = e_audioERHVXC;
            res = false;
            break;

        case 26:
            type = e_audioERHILN;
            res = false;
            break;

        case 27:
            type = e_audioERParametric;
            res = false;
            break;

        case 28:
            type = e_audioSSC;
            res = false;
            break;

        case 32:
            type = e_audioLayer1;
            res = false;
            break;

        case 33:
            type = e_audioLayer2;
            res = false;
            break;

        case 34:
            type = e_audioLayer3;
            res = false;
            break;

        case 35:
            type = e_audioDST;
            res = false;
            break;

        default:
            type = e_audioUnknown;
            res = false;
            break;
    }
    return res;
}

//-------------------------------------------------------------------------------------------
// 1.6.2.1 - Table 1.13
//-------------------------------------------------------------------------------------------

bool GAConfig::readAudioSpecificConfig(Sequence *seq)
{
    if(!readObjectType(seq,m_audioObjectType))
    {
        return false;
    }

    m_samplingFrequencyIndex = seq->readBitsI(4);
    if(m_samplingFrequencyIndex==15)
    {
        m_samplingFrequency = seq->readBitsI(24);
    }
    else
    {
        m_samplingFrequency = getSampleRate(m_samplingFrequencyIndex);
    }
    m_channelConfiguration = seq->readBitsI(4);

    if(m_audioObjectType==e_audioSBR)
    {
        m_extAudioObjectType = m_audioObjectType;
        m_sbrPresentFlag = true;
        m_extSamplingFrequencyIndex = seq->readBits(4);
        if(m_extSamplingFrequencyIndex==15)
        {
            m_extSamplingFrequency = seq->readBits(24);
        }
        else
        {
            m_extSamplingFrequency = getSampleRate(m_extSamplingFrequencyIndex);
        }
        if(!readObjectType(seq,m_audioObjectType))
        {
            return false;
        }
    }
    else
    {
        m_extAudioObjectType = e_audioNULL;
        m_sbrPresentFlag = false;
    }

    switch(m_audioObjectType)
    {
        case e_audioAACMain:
        case e_audioAACLC:
        case e_audioAACSSR:
        case e_audioAACLTP:
        case e_audioAACScalable:
        case e_audioTwinVQ:
            if(!readGAConfig(seq))
            {
                return false;
            }
            break;

        case e_audioERAACLC:
        case e_audioERAACLTP:
        case e_audioERAACScalable:
        case e_audioERTwinVQ:
        case e_audioERBSAC:
        case e_audioERAACLD:
            if(!readGAConfig(seq))
            {
                return false;
            }
            // epConfig
            if(seq->readBitsI(2)!=0)
            {
                return false;
            }
            break;

        default:
            return false;
    }

    if(m_extAudioObjectType!=e_audioSBR && seq->remain()>=16)
    {
        m_syncExtensionType = seq->readBitsI(11);
        if(m_syncExtensionType==0x000002b7)
        {
            readObjectType(seq,m_extAudioObjectType);
            if(m_extAudioObjectType==e_audioSBR)
            {
                m_sbrPresentFlag = (seq->readBitI()) ? true : false;
                if(m_sbrPresentFlag)
                {
                    m_extSamplingFrequencyIndex = seq->readBitsI(4);
                    if(m_extSamplingFrequencyIndex==15)
                    {
                        m_extSamplingFrequency = seq->readBits(24);
                    }
                    else
                    {
                        m_extSamplingFrequency = getSampleRate(m_extSamplingFrequencyIndex);
                    }
                }
            }
        }
    }

    m_frameLength = (!m_frameLengthFlag) ? 1024 : 960;
    if(m_audioObjectType==e_audioERAACLD)
    {
        m_frameLength >>= 1;
    }

    return true;
}

//-------------------------------------------------------------------------------------------

bool GAConfig::readGAConfig(Sequence *seq)
{
    m_frameLengthFlag = (seq->readBitI()) ? true : false;
    m_dependsOnCoreCoder = (seq->readBitI()) ? true : false;
    if(m_dependsOnCoreCoder)
    {
        m_coreCoderDelay = seq->readBitsI(14);
    }
    m_extensionFlag = (seq->readBitI()) ? true : false;
    if(!m_channelConfiguration)
    {
        if(!m_programConfig.read(seq))
        {
            return false;
        }
    }
    if(m_audioObjectType==e_audioAACScalable || m_audioObjectType==e_audioERAACScalable)
    {
        m_layerNr = seq->readBitsI(3);
    }
    if(m_extensionFlag)
    {
        if(m_audioObjectType==e_audioERBSAC)
        {
            m_numOfSubFrame = seq->readBitsI(5);
            m_layerLength = seq->readBitsI(11);
        }
        if(m_audioObjectType==e_audioERAACLC || m_audioObjectType==e_audioERAACLTP || m_audioObjectType==e_audioAACScalable || m_audioObjectType==e_audioERAACLD)
        {
            m_aacSectionDataResilienceFlag = (seq->readBitI()) ? true : false;
            m_aacScalefactorDataResilienceFlag = (seq->readBitI()) ? true : false;
            m_aacSpectralDataResilienceFlag = (seq->readBitI()) ? true : false;
        }
        else
        {
            m_aacSectionDataResilienceFlag = false;
            m_aacScalefactorDataResilienceFlag = false;
            m_aacSpectralDataResilienceFlag = false;
        }
        m_extensionFlag3 = (seq->readBitI()) ? true : false;
    }
    return true;
}

//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbWindowSize_1024[12] = { 41, 41, 47, 49, 49, 51, 47, 47, 43, 43, 43, 40 };
tint GAConfig::m_swbWindowSize_960[12]  = { 40, 40, 45, 49, 49, 49, 46, 46, 42, 42, 42, 40 };
tint GAConfig::m_swbWindowSize_512[12]  = {  0,  0,  0, 36, 36, 37, 31, 31,  0,  0,  0,  0 };
tint GAConfig::m_swbWindowSize_480[12]  = {  0,  0,  0, 35, 35, 37, 30, 30,  0,  0,  0,  0 };
tint GAConfig::m_swbWindowSize_128[12]  = { 12, 12, 12, 14, 14, 14, 15, 15, 15, 15, 15, 15 };

//-------------------------------------------------------------------------------------------

tint *GAConfig::m_swbOffset_1024[12] =
{
    m_swbOffsetLongWindow_1024_96, // 96000 - 0
    m_swbOffsetLongWindow_1024_96, // 88200 - 1
    m_swbOffsetLongWindow_1024_64, // 64000 - 2
    m_swbOffsetLongWindow_1024_48, // 48000 - 3
    m_swbOffsetLongWindow_1024_48, // 44100 - 4
    m_swbOffsetLongWindow_1024_32, // 32000 - 5
    m_swbOffsetLongWindow_1024_24, // 24000 - 6
    m_swbOffsetLongWindow_1024_24, // 22050 - 7
    m_swbOffsetLongWindow_1024_16, // 16000 - 8
    m_swbOffsetLongWindow_1024_16, // 12000 - 9
    m_swbOffsetLongWindow_1024_16, // 11025 - 10
    m_swbOffsetLongWindow_1024_8   //  8000 - 11
};

//-------------------------------------------------------------------------------------------

tint *GAConfig::m_swbOffset_512[12] =
{
    0,                            // 96000
    0,                            // 88200
    0,                            // 64000
    m_swbOffsetLongWindow_512_48, // 48000
    m_swbOffsetLongWindow_512_48, // 44100
    m_swbOffsetLongWindow_512_32, // 32000
    m_swbOffsetLongWindow_512_24, // 24000
    m_swbOffsetLongWindow_512_24, // 22050
    0,                            // 16000
    0,                            // 12000
    0,                            // 11025
    0                             //  8000
};

//-------------------------------------------------------------------------------------------

tint *GAConfig::m_swbOffset_480[12] =
{
    0,                            // 96000
    0,                            // 88200
    0,                            // 64000
    m_swbOffsetLongWindow_480_48, // 48000
    m_swbOffsetLongWindow_480_48, // 44100
    m_swbOffsetLongWindow_480_32, // 32000
    m_swbOffsetLongWindow_480_24, // 24000
    m_swbOffsetLongWindow_480_24, // 22050
    0,                            // 16000
    0,                            // 12000
    0,                            // 11025
    0,                            //  8000
};

//-------------------------------------------------------------------------------------------

tint *GAConfig::m_swbOffset_128[12] =
{
    m_swbOffsetShortWindow_128_96, // 96000
    m_swbOffsetShortWindow_128_96, // 88200
    m_swbOffsetShortWindow_128_64, // 64000
    m_swbOffsetShortWindow_128_48, // 48000
    m_swbOffsetShortWindow_128_48, // 44100
    m_swbOffsetShortWindow_128_48, // 32000
    m_swbOffsetShortWindow_128_24, // 24000
    m_swbOffsetShortWindow_128_24, // 22050
    m_swbOffsetShortWindow_128_16, // 16000
    m_swbOffsetShortWindow_128_16, // 12000
    m_swbOffsetShortWindow_128_16, // 11025
    m_swbOffsetShortWindow_128_8   //  8000
};

//-------------------------------------------------------------------------------------------
// 1024
//-------------------------------------------------------------------------------------------
// Table 4.121 - 88.2, 96 kHz - 1024
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_1024_96[41] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  48,  52,  56,  64,
     72,  80,  88,  96, 108, 120, 132, 144,
    156, 172, 188, 212, 240, 276, 320, 384,
    448, 512, 576, 640, 704, 768, 832, 896,
    960
};

//-------------------------------------------------------------------------------------------
// Table 4.119 - 64 kHz - 1024
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_1024_64[47] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  48,  52,  56,  64,
     72,  80,  88, 100, 112, 124, 140, 156,
    172, 192, 216, 240, 268, 304, 344, 384,
    424, 464, 504, 544, 584, 624, 664, 704,
    744, 784, 824, 864, 904, 944, 984
};

//-------------------------------------------------------------------------------------------
// Table 4.110 - 44.1, 48 kHz - 1024
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_1024_48[49] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  48,  56,  64,  72,  80,
     88,  96, 108, 120, 132, 144, 160, 176,
    196, 216, 240, 264, 292, 320, 352, 384,
    416, 448, 480, 512, 544, 576, 608, 640,
    672, 704, 736, 768, 800, 832, 864, 896,
    928
};

//-------------------------------------------------------------------------------------------
// Table 4.112 - 32 kHz - 1024
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_1024_32[51] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  48,  56,  64,  72,  80,
     88,  96, 108, 120, 132, 144, 160, 176,
    196, 216, 240, 264, 292, 320, 352, 384,
    416, 448, 480, 512, 544, 576, 608, 640,
    672, 704, 736, 768, 800, 832, 864, 896,
    928, 960, 992
};

//-------------------------------------------------------------------------------------------
// Table 4.117 - 22.05, 24 kHz - 1024
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_1024_24[47] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  52,  60,  68,  76,
     84,  92, 100, 108, 116, 124, 136, 148,
    160, 172, 188, 204, 220, 240, 260, 284,
    308, 336, 364, 396, 432, 468, 508, 552,
    600, 652, 704, 768, 832, 896, 960
};

//-------------------------------------------------------------------------------------------
// Table 4.115 - 11.025, 12, 16 kHz - 1024
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_1024_16[43] =
{
      0,   8,  16,  24,  32,  40,  48,  56,
     64,  72,  80,  88, 100, 112, 124, 136,
    148, 160, 172, 184, 196, 212, 228, 244,
    260, 280, 300, 320, 344, 368, 396, 424,
    456, 492, 532, 572, 616, 664, 716, 772,
    832, 896, 960
};

//-------------------------------------------------------------------------------------------
// Table 4.113 - 8 kHz - 1024
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_1024_8[40] =
{
      0,  12,  24,  36,  48,  60,  72,  84,
     96, 108, 120, 132, 144, 156, 172, 188,
    204, 220, 236, 252, 268, 288, 308, 328,
    348, 372, 396, 420, 448, 476, 508, 544,
    580, 620, 664, 712, 764, 820, 880, 944
};

//-------------------------------------------------------------------------------------------
// 512
//-------------------------------------------------------------------------------------------
// Table 4.124 - 44.1, 48 kHz - 512
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_512_48[36] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  48,  52,  56,  60,
     68,  76,  84,  92, 100, 112, 124, 136,
    148, 164, 184, 208, 236, 268, 300, 332,
    364, 396, 428, 460
};

//-------------------------------------------------------------------------------------------
// Table 4.126 - 32 kHz - 512
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_512_32[37] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  48,  52,  56,  64,
     72,  80,  88,  96, 108, 120, 132, 144,
    160, 176, 192, 212, 236, 260, 288, 320,
    352, 384, 416, 448, 480
};

//-------------------------------------------------------------------------------------------
// Table 4.128 - 22.05, 24 kHz - 512
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_512_24[31] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  52,  60,  68,  80,
     92, 104, 120, 140, 164, 192, 224, 256,
    288, 320, 352, 384, 416, 448, 480
};

//-------------------------------------------------------------------------------------------
// 480
//-------------------------------------------------------------------------------------------
// Table 4.123 - 44.1, 48 kHz - 480
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_480_48[35] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  48,  52,  56,  64,
     72,  80,  88,  96, 108, 120, 132, 144,
    156, 172, 188, 212, 240, 272, 304, 336,
    368, 400, 432
};

//-------------------------------------------------------------------------------------------
// Table 4.125 - 32 kHz - 480
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_480_32[37] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  48,  52,  56,  60,
     64,  72,  80,  88,  96, 104, 112, 124,
    136, 148, 164, 180, 200, 224, 256, 288,
    320, 352, 384, 416, 448
};

//-------------------------------------------------------------------------------------------
// Table 4.127 - 22.05, 24 kHz - 480
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetLongWindow_480_24[30] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  52,  60,  68,  80,
     92, 104, 120, 140, 164, 192, 224, 256,
    288, 320, 352, 384, 416, 448
};

//-------------------------------------------------------------------------------------------
// 128
//-------------------------------------------------------------------------------------------
// Table 4.122 - 88.2, 96 kHz - 128
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetShortWindow_128_96[12] =
{
      0,   4,   8,  12,  16,  20,  24,  32,
     40,  48,  64,  92
};

//-------------------------------------------------------------------------------------------
// Table 4.120 - 64 kHz - 128
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetShortWindow_128_64[12] =
{
      0,   4,   8,  12,  16,  20,  24,  32,
     40,  48,  64,  92
};

//-------------------------------------------------------------------------------------------
// Table 4.111 - 32, 44.1, 48 kHz - 128
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetShortWindow_128_48[14] =
{
      0,   4,   8,  12,  16,  20,  28,  36,
     44,  56,  68,  80,  96, 112
};

//-------------------------------------------------------------------------------------------
// Table 4.118 - 22.05, 24 kHz - 128
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetShortWindow_128_24[15] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     36,  44,  52,  64,  76,  92, 108
};

//-------------------------------------------------------------------------------------------
// Table 4.116 - 11.025, 12, 16 kHz - 128
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetShortWindow_128_16[15] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  40,  48,  60,  72,  88, 108
};

//-------------------------------------------------------------------------------------------
// Table 4.114 - 8 kHz - 128
//-------------------------------------------------------------------------------------------

tint GAConfig::m_swbOffsetShortWindow_128_8[15] =
{
      0,   4,   8,  12,  16,  20,  24,  28,
     36,  44,  52,  60,  72,  88, 108
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
