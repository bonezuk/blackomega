//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BQUADFILTER_H
#define __OMEGA_ENGINE_BQUADFILTER_H
//-------------------------------------------------------------------------------------------
/*
BiQuad IIR filter implementation based on:
https://www.w3.org/TR/audio-eq-cookbook/
https://webaudio.github.io/Audio-EQ-Cookbook/Audio-EQ-Cookbook.txt
https://octovoid.com/2017/11/04/coding-a-parametric-equalizer-for-audio-applications/
https://octovoid.com/wp-content/uploads/2017/11/orfanidis.pdf
https://github.com/dimtass/dsp-c-filters.git
https://github.com/dimtass/DSP-Cpp-filters.git
*/
//-------------------------------------------------------------------------------------------

#include "engine/inc/AData.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT BiQuadFilter
{
    public:
        typedef enum
        {
            e_AllPass_FirstOrder = 1,
            e_HighPass_FirstOrder,
            e_LowPass_FirstOrder,
            e_HighPassShelf_FirstOrder,
            e_LowPassShelf_FirstOrder,
            e_allPassSecondOrder,
            e_bandPassSecondOrder,
            e_bandStopSecondOrder,
            e_bandPassButterworthSecondOrder,
            e_bandStopButterworthSecondOrder,
            e_highPassButterworthSecondOrder,
            e_lowPassButterworthSecondOrder,
            e_highPassSecondOrder,
            e_lowPassSecondOrder,
            e_highPassLinkwitzRileySecondOrder,
            e_lowPassLinkwitzRileySecondOrder,
            e_parametricBoostCQ,
            e_parametricCutCQ,
            e_parametricNCQ,
            e_UnknownFilter = 0
        } Type;

    public:
        BiQuadFilter();
        BiQuadFilter(const BiQuadFilter& rhs);

        const BiQuadFilter& operator = (const BiQuadFilter& rhs);

        static BiQuadFilter filter(Type t, sample_t fC, sample_t fS);
        static BiQuadFilter filter(Type t, sample_t Q, sample_t fC, sample_t fS);
        static BiQuadFilter filter(Type t, sample_t gainDB, sample_t Q, sample_t fC, sample_t fS);

        sample_t process(const sample_t x0);

        Type type() const;
        sample_t a0() const;
        sample_t a1() const;
        sample_t a2() const;
        sample_t b1() const;
        sample_t b2() const;
        sample_t c0() const;
        sample_t d0() const;

    private:
        Type m_type;

        sample_t m_a0;
        sample_t m_a1;
        sample_t m_a2;
        sample_t m_b1;
        sample_t m_b2;
        sample_t m_c0;
        sample_t m_d0;

        sample_t m_xMinus1;
        sample_t m_xMinus2;
        sample_t m_yMinus1;
        sample_t m_yMinus2;

        void copy(const BiQuadFilter& rhs);

        sample_t processBiquad(const sample_t x0);
        sample_t processBiquadShelve(const sample_t x0);

        void coeffAllPassFirstOrder(sample_t fC, sample_t fS);
        void coeffHighPassFirstOrder(sample_t fC, sample_t fS);
        void coeffLowPassFirstOrder(sample_t fC, sample_t fS);
        void coeffHighPassShelfFirstOrder(sample_t gainDB, sample_t fC, sample_t fS);
        void coeffLowPassShelfFirstOrder(sample_t gainDB, sample_t fC, sample_t fS);
        void coeffAllPassSecondOrder(sample_t Q, sample_t fS);
        void coeffBandPassSecondOrder(sample_t Q, sample_t fC, sample_t fS);
        void coeffBandStopSecondOrder(sample_t Q, sample_t fC, sample_t fS);
        void coeffBandPassButterworthSecondOrder(sample_t bW, sample_t fC, sample_t fS);
        void coeffBandStopButterworthSecondOrder(sample_t bW, sample_t fC, sample_t fS);
        void coeffHighPassButterworthSecondOrder(sample_t fC, sample_t fS);
        void coeffLowPassButterworthSecondOrder(sample_t fC, sample_t fS);
        void coeffHighPassSecondOrder(sample_t Q, sample_t fC, sample_t fS);
        void coeffLowPassSecondOrder(sample_t Q, sample_t fC, sample_t fS);
        void coeffHighPassLinkwitzRileySecondOrder(sample_t fC, sample_t fS);
        void coeffLowPassLinkwitzRileySecondOrder(sample_t fC, sample_t fS);
        void coeffParametricBoostCQ(sample_t gainDB, sample_t Q, sample_t fC, sample_t fS);
        void coeffParametricCutCQ(sample_t gainDB, sample_t Q, sample_t fC, sample_t fS);
        void coeffParametricNCQ(sample_t gainDB, sample_t Q, sample_t fC, sample_t fS);
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

