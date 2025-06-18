//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FIRFILTER_H
#define __OMEGA_ENGINE_FIRFILTER_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/RData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT FIRFilter
{
    public:
        FIRFilter(const sample_t *coefficients, tint noCoeff);
        virtual ~FIRFilter();
        
        tint offset() const;
        void setOffset(tint of);
        
        void process(RData *pData, tint channelIdx, bool isLast, bool clip = true);
        void process(RData *pData, tint channelIdx, tint filterIdx, bool isLast, bool clip = true);
        
        void reset();

    private:
    
        sample_t *m_coefficients;
        tint m_filterLength;
        tint m_offset;
        
        RData *m_pPrevious;
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
