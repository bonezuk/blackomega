//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WHITEOMEGA_GACONFIG_H
#define __OMEGA_ENGINE_WHITEOMEGA_GACONFIG_H
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

class WHITEOMEGA_EXPORT GAConfig
{
    public:

        typedef enum
        {
            e_audioNULL = 0,
            e_audioAACMain = 1,
            e_audioAACLC = 2,
            e_audioAACSSR = 3,
            e_audioAACLTP = 4,
            e_audioSBR = 5,
            e_audioAACScalable = 6,
            e_audioTwinVQ = 7,
            e_audioCELP = 8,
            e_audioHVXC = 9,
            e_audioTTSI = 12,
            e_audioMainSynthetic = 13,
            e_audioWaveSynthesis = 14,
            e_audioGeneralMIDI = 15,
            e_audioAudioFX = 16,
            e_audioERAACLC = 17,
            e_audioERAACLTP = 19,
            e_audioERAACScalable = 20,
            e_audioERTwinVQ = 21,
            e_audioERBSAC = 22,
            e_audioERAACLD = 23,
            e_audioERCELP = 24,
            e_audioERHVXC = 25,
            e_audioERHILN = 26,
            e_audioERParametric = 27,
            e_audioSSC = 28,
            e_audioLayer1 = 32,
            e_audioLayer2 = 33,
            e_audioLayer3 = 34,
            e_audioDST = 35,
            e_audioUnknown
        } AudioType;

    public:
        GAConfig();
        virtual ~GAConfig();

        static tint getSampleRate(tint idx);
        static tint getSampleRateIndex(tint rate);

        virtual bool readAudioSpecificConfig(Sequence *seq);
        virtual bool readGAConfig(Sequence *seq);

        // AudioSpecificConfig - 1.6.2.1
        AudioType m_audioObjectType;
        tint m_samplingFrequencyIndex;
        tint m_samplingFrequency;
        tint m_channelConfiguration;
        bool m_sbrPresentFlag;
        AudioType m_extAudioObjectType;
        tint m_extSamplingFrequencyIndex;
        tint m_extSamplingFrequency;
        tint m_syncExtensionType;
        tint m_frameLength;

        // GASpecificConfig - Table 4.1
        bool m_frameLengthFlag;
        bool m_dependsOnCoreCoder;
        tint m_coreCoderDelay;
        bool m_extensionFlag;
        tint m_layerNr;
        tint m_numOfSubFrame;
        tint m_layerLength;
        bool m_aacSectionDataResilienceFlag;
        bool m_aacScalefactorDataResilienceFlag;
        bool m_aacSpectralDataResilienceFlag;
        bool m_extensionFlag3;

        ProgramConfig m_programConfig;

        static tint m_swbWindowSize_1024[12];
        static tint m_swbWindowSize_960[12];
        static tint m_swbWindowSize_512[12];
        static tint m_swbWindowSize_480[12];
        static tint m_swbWindowSize_128[12];

        static tint *m_swbOffset_1024[];
        static tint *m_swbOffset_512[];
        static tint *m_swbOffset_480[];
        static tint *m_swbOffset_128[];

        static tint m_swbOffsetLongWindow_1024_96[41];
        static tint m_swbOffsetLongWindow_1024_64[47];
        static tint m_swbOffsetLongWindow_1024_48[49];
        static tint m_swbOffsetLongWindow_1024_32[51];
        static tint m_swbOffsetLongWindow_1024_24[47];
        static tint m_swbOffsetLongWindow_1024_16[43];
        static tint m_swbOffsetLongWindow_1024_8[40];

        static tint m_swbOffsetLongWindow_512_48[36];
        static tint m_swbOffsetLongWindow_512_32[37];
        static tint m_swbOffsetLongWindow_512_24[31];

        static tint m_swbOffsetLongWindow_480_48[35];
        static tint m_swbOffsetLongWindow_480_32[37];
        static tint m_swbOffsetLongWindow_480_24[30];

        static tint m_swbOffsetShortWindow_128_96[12];
        static tint m_swbOffsetShortWindow_128_64[12];
        static tint m_swbOffsetShortWindow_128_48[14];
        static tint m_swbOffsetShortWindow_128_24[15];
        static tint m_swbOffsetShortWindow_128_16[15];
        static tint m_swbOffsetShortWindow_128_8[15];

    protected:

        virtual bool readObjectType(Sequence *seq,AudioType& type);
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
