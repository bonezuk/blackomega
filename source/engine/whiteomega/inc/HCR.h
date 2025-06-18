//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_WHITEOMEGA_HCR_H
#define __OMEGA_WHITEOMEGA_HCR_H
//-------------------------------------------------------------------------------------------

#include "engine/whiteomega/inc/WSequence.h"
#include "engine/whiteomega/inc/GAConfig.h"
#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/Common.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class HCR
{
    public:
        
        typedef struct
        {
            tint priority;
            tint codebook;
            tint position;
        } UInfo;
                
    public:
        HCR();
        
        bool read(WSequence *seq,ICSInfo *info,GAConfig *config,tint *spectralData);
        
    protected:

        static const tint m_codebookPriority[32];
        static const tint m_maxCWLen[32];

        tint m_lengthOfReorderedSpectralData;
        tint m_lengthOfLongestCodeword;
    
        UInfo m_units[257];
        
        tint m_noCW;
        WSequence::CWInfo m_codewords[512];
        
        tint m_noSegments;
        WSequence::Segment m_segments[512];
    
        void quickSort(UInfo *units,tint len);
        void quickSort(UInfo *units,tint lo,tint hi);
        void qSswap(UInfo *a,UInfo *b);
        
        void preSort(ICSInfo *info,GAConfig *gaConfig);
        void calcCodewords();
        void calcSegments(WSequence *seq);
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

