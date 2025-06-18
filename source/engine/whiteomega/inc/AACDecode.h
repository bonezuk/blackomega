//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_AACDECODE_H
#define __OMEGA_WHITEOMEGA_AACDECODE_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Sequence.h"
#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/GAConfig.h"
#include "engine/whiteomega/inc/WSequence.h"
#include "engine/whiteomega/inc/HCR.h"
#include "engine/whiteomega/inc/Common.h"

#include <math.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class AACDecode
{
    public:
        AACDecode();
        virtual ~AACDecode();
        
        static void start();
        static void stop();
        
        void reset();
        
        void setGAConfig(GAConfig *cfg);
        
        bool readICS(Sequence *s,bool commonWindow,bool scaleFlag);
        
        bool readICSInfo(bool commonWindow,WSequence *seq);
        void copyICSInfo(ICSInfo *dInfo);
        bool windowGroupingInfo();
        
        bool readLTPData(WSequence *seq,LTPInfo *ltp);
        
        void dequantize();
        
        ICSInfo m_info;
        tint m_spectralData[1024];
        sample_t m_spectralCoef[1024];

        static sample_t *m_dequantScalefactor;
        static sample_t *m_dequantSpectral;
        static sample_t *m_intensityScalefactor;

    protected:

#if defined(WHITEOMEGA_COMPARE)
        static tint m_ICSFrameNo;
#endif

        static sample_t *m_dequantSp;
        static sample_t *m_intensitySf;

        GAConfig *m_gaConfig;
        
        HCR m_hcr;

        void printError(const tchar *strR,const tchar *strE) const;

        bool readSectionData(WSequence *seq);
        bool readScalefactor(WSequence *seq);
        bool readScalefactorNormal(WSequence *seq);
        bool readScalefactorRLVC(WSequence *seq);
        bool readPulse(WSequence *seq);
        bool readTNS(WSequence *seq);
        bool readGainControlData(WSequence *seq);
        bool readSpectralData(WSequence *seq);
        void decodePulse();
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
