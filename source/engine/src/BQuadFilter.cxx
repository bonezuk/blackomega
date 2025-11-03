#include "engine/inc/BiQuadFilter.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

BiQuadFilter::BiQuadFilter() : m_type(BiQuadFilter::e_UnknownFilter),
    m_a0(0.0),
    m_a1(0.0),
    m_a2(0.0),
    m_b1(0.0),
    m_b2(0.0),
    m_c0(0.0),
    m_d0(0.0),
    m_xMinus1(0.0),
    m_xMinus2(0.0),
    m_yMinus1(0.0),
    m_yMinus2(0.0)
{}

//-------------------------------------------------------------------------------------------

BiQuadFilter::BiQuadFilter(const BiQuadFilter& rhs)
{
    copy(rhs);
}

//-------------------------------------------------------------------------------------------

void BiQuadFilter::copy(const BiQuadFilter& rhs)
{
    m_type = rhs.m_type;        
    m_a0 = rhs.m_a0;
    m_a1 = rhs.m_a1;
    m_a2 = rhs.m_a2;
    m_b1 = rhs.m_b1;
    m_b2 = rhs.m_b2;
    m_c0 = rhs.m_c0;
    m_d0 = rhs.m_d0;
    m_xMinus1 = rhs.m_xMinus1;
    m_xMinus2 = rhs.m_xMinus2;
    m_yMinus1 = rhs.m_yMinus1;
    m_yMinus2 = rhs.m_yMinus2;
}

//-------------------------------------------------------------------------------------------

const BiQuadFilter& BiQuadFilter::operator = (const BiQuadFilter& rhs)
{
    if(this != &rhs)
    {
        copy(rhs);
    }
    return *this;
}

//-------------------------------------------------------------------------------------------

BiQuadFilter::Type BiQuadFilter::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::a0() const
{
    return m_a0;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::a1() const
{
    return m_a1;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::a2() const
{
    return m_a2;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::b1() const
{
    return m_b1;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::b2() const
{
    return m_b2;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::c0() const
{
    return m_c0;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::d0() const
{
    return m_d0;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::process(const sample_t x0)
{
    sample_t y0;
    
    switch(m_type)
    {
        case e_HighPassShelf_FirstOrder:
        case e_LowPassShelf_FirstOrder:
        case e_parametricBoostCQ:
        case e_parametricCutCQ:
        case e_parametricNCQ:
            y0 = processBiquadShelve(x0);
            break;
        default:
            y0 = processBiquad(x0);
            break;
    }
    return y0;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::processBiquad(const sample_t x0)
{
    sample_t y0 = (m_a0 * x0) + (m_a1 * m_xMinus1) + (m_a2 * m_xMinus2) - (m_b1 * m_yMinus1) - (m_b2 * m_yMinus2);
    m_xMinus2 = m_xMinus1;
    m_xMinus1 = x0;
    m_yMinus2 = m_yMinus1;
    m_yMinus1 = y0;
    return y0;
}

//-------------------------------------------------------------------------------------------

sample_t BiQuadFilter::processBiquadShelve(const sample_t x0)
{
    return (m_d0 * x0) + (m_c0 * processBiquad(x0));
}

//-------------------------------------------------------------------------------------------

BiQuadFilter BiQuadFilter::filter(Type t, sample_t fC, sample_t fS)
{
    BiQuadFilter f;

    f.m_type = t;
    switch(t)
    {
        case e_AllPass_FirstOrder:
            f.coeffAllPassFirstOrder(fC, fS);
            break;
        case e_HighPass_FirstOrder:
            f.coeffHighPassFirstOrder(fC, fS);
            break;
        case e_LowPass_FirstOrder:
            f.coeffLowPassFirstOrder(fC, fS);
            break;
        case e_allPassSecondOrder:
            f.coeffAllPassSecondOrder(fC, fS);
            break;
        case e_highPassButterworthSecondOrder:
            f.coeffHighPassButterworthSecondOrder(fC, fS);
            break;
        case e_lowPassButterworthSecondOrder:
            f.coeffLowPassButterworthSecondOrder(fC, fS);
            break;
        case e_highPassLinkwitzRileySecondOrder:
            f.coeffHighPassLinkwitzRileySecondOrder(fC, fS);
            break;
        case e_lowPassLinkwitzRileySecondOrder:
            f.coeffLowPassLinkwitzRileySecondOrder(fC, fS);
            break;
        default:
            f.m_type = e_UnknownFilter;
            break;
    }
    return f;
}

//-------------------------------------------------------------------------------------------

BiQuadFilter BiQuadFilter::filter(Type t, sample_t Q, sample_t fC, sample_t fS)
{
    BiQuadFilter f;

    f.m_type = t;
    switch(t)
    {
        case e_HighPassShelf_FirstOrder:
            f.coeffHighPassShelfFirstOrder(Q, fC, fS);
            break;
        case e_LowPassShelf_FirstOrder:
            f.coeffLowPassShelfFirstOrder(Q, fC, fS);
            break;
        case e_bandPassSecondOrder:
            f.coeffBandPassSecondOrder(Q, fC, fS);
            break;
        case e_bandStopSecondOrder:
            f.coeffBandStopSecondOrder(Q, fC, fS);
            break;
        case e_bandPassButterworthSecondOrder:
            f.coeffBandPassButterworthSecondOrder(Q, fC, fS);
            break;
        case e_bandStopButterworthSecondOrder:
            f.coeffBandStopButterworthSecondOrder(Q, fC, fS);
            break;
        case e_highPassSecondOrder:
            f.coeffHighPassSecondOrder(Q, fC, fS);
            break;
        case e_lowPassSecondOrder:
            f.coeffLowPassSecondOrder(Q, fC, fS);
            break;
        default:
            f.m_type = e_UnknownFilter;
            break;
    }
    return f;
}

//-------------------------------------------------------------------------------------------

BiQuadFilter BiQuadFilter::filter(Type t, sample_t gainDB, sample_t Q, sample_t fC, sample_t fS)
{
    BiQuadFilter f;

    f.m_type = t;
    switch(t)
    {
        case e_parametricBoostCQ:
            f.coeffParametricBoostCQ(gainDB, Q, fC, fS);
            break;
        case e_parametricCutCQ:
            f.coeffParametricCutCQ(gainDB, Q, fC, fS);
            break;
        case e_parametricNCQ:
            f.coeffParametricNCQ(gainDB, Q, fC, fS);
            break;
        default:
            f.m_type = e_UnknownFilter;
            break;
    }
    return f;
}

//-------------------------------------------------------------------------------------------
/* First order all-pass filter
    fC = corner frequency
    fS = sample frequency */
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffAllPassFirstOrder(sample_t fC, sample_t fS)
{
    sample_t a = (::tan((c_PI_D * fC) / fS) - 1.0) / (::tan((c_PI_D * fC) / fS) + 1.0);
    m_a0 = a;
    m_a1 = 1.0;
    m_a2 = 0.0;
    m_b1 = a;
    m_b2 = 0.0;
}

//-------------------------------------------------------------------------------------------
/* First order high pass filter
    fC = corner frequency
    fS = sample frequency */
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffHighPassFirstOrder(sample_t fC, sample_t fS)
{
    sample_t th = 2.0 * c_PI_D * fC / fS;
    sample_t g = ::cos(th) / (1.0 + ::sin(th));
    m_a0 = (1.0 + g) / 2.0;
    m_a1 = -((1.0 + g) / 2.0);
    m_a2 = 0.0;
    m_b1 = -g;
    m_b2 = 0.0;
}

//-------------------------------------------------------------------------------------------
/* First order low pass filter
    fC = corner frequency
    fS = sample frequency */
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffLowPassFirstOrder(sample_t fC, sample_t fS)
{
    sample_t th = 2.0 * c_PI_D * fC / fS;
    sample_t g = ::cos(th) / (1.0 + ::sin(th));
    m_a0 = (1.0 - g) / 2.0;
    m_a1 = (1.0 - g) / 2.0;
    m_a2 = 0.0;
    m_b1 = -g;
    m_b2 = 0.0;
}

//-------------------------------------------------------------------------------------------
/* First order high-shelving filter
 * fC: high shelf frequency
 * fS: sample frequency
 * gainDB: Low-frequency gain/attenuation in dB
 *
 * Shelving filters are used in many tone controls, especially when there are only two, bass
 * and treble, which are almost always implemented as shelf types. The filters have a corner
 * frequency and gain or attenuation value. */
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffHighPassShelfFirstOrder(sample_t gainDB, sample_t fC, sample_t fS)
{
    sample_t th = 2.0 * c_PI_D * fC / fS;
    sample_t m = ::pow(10.0, gainDB / 20.0);
    sample_t b = (1.0 + m) / 4.0;
    sample_t d = b * ::tan(th / 2.0);
    sample_t g = (1.0 - d) / (1.0 + d);
    m_a0 = (1.0 + g) / 2.0;
    m_a1 = -((1.0 + g) / 2.0);
    m_a2 = 0.0;
    m_b1 = -g;
    m_b2 = 0.0;
    m_c0 = m - 1.0;
    m_d0 = 1.0;
}

//-------------------------------------------------------------------------------------------
/*
* First order low-shelving filter
* fC: low shelf frequency
* fS: sample frequency
* gainDB: Low-frequency gain/attenuation in dB
*
* Shelving filters are used in many tone controls, especially when there are only two, bass
* and treble, which are almost always implemented as shelf types. The filters have a corner
* frequency and gain or attenuation value. */
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffLowPassShelfFirstOrder(sample_t gainDB, sample_t fC, sample_t fS)
{
    sample_t th = 2.0 * c_PI_D * fC / fS;
    sample_t m = ::pow(10.0, gainDB / 20.0);
    sample_t b = 4.0 / (1.0 + m);
    sample_t d = b * ::tan(th / 2.0);
    sample_t g = (1.0 - d) / (1.0 + d);
    m_a0 = (1.0 - g) / 2.0;
    m_a1 = (1.0 - g) / 2.0;
    m_a2 = 0.0;
    m_b1 = -g;
    m_b2 = 0.0;
    m_c0 = m - 1.0;
    m_d0 = 1.0;
}

//-------------------------------------------------------------------------------------------
/*
* Second order all-pass filter
* Q: steepness of phase shift at fc (second-order only)
* fS: sample frequency
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffAllPassSecondOrder(sample_t Q, sample_t fS)
{
    sample_t a = (::tan(c_PI_D*Q / fS) - 1.0) / (::tan(c_PI_D*Q / fS) + 1.0);
    sample_t b = -::cos(c_PI_D*Q / fS);
    m_a0 = -a;
    m_a1 = b*(1.0 - a);
    m_a2 = 1.0;
    m_b1 = m_a1;
    m_b2 = m_a0;
}

//-------------------------------------------------------------------------------------------
/*
* Second order band-pass filter
* Q: steepness of phase shift at fc (second-order only)
* fC: corner frequency
* fS: sample frequency
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffBandPassSecondOrder(sample_t Q, sample_t fC, sample_t fS)
{
    sample_t w = 2.0 * c_PI_D * fC / fS;
    sample_t b = 0.5*((1.0 - ::tan(w / (2.0*Q))) / (1.0 + ::tan(w / (2.0*Q))));
    sample_t g = (0.5 + b) * ::cos(w);
    m_a0 = 0.5 - b;
    m_a1 = 0.0;
    m_a2 = -(0.5 - b);
    m_b1 = -2.0 * g;
    m_b2 = 2.0 * b;
}

//-------------------------------------------------------------------------------------------
/*
* Second order band-stop filter
* bW: bandwidth of peak/notch = fc/Q
* fC: corner frequency
* fS: sample frequency
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffBandStopSecondOrder(sample_t Q, sample_t fC, sample_t fS)
{
    sample_t w = 2.0 * c_PI_D * fC / fS;
    sample_t b = 0.5*((1.0 - ::tan(w / (2.0*Q))) / (1.0 + ::tan(w / (2.0*Q))));
    sample_t g = (0.5 + b) * ::cos(w);
    m_a0 = 0.5 + b;
    m_a1 = -2.0 * g;
    m_a2 = 0.5 + b;
    m_b1 = -2.0 * g;
    m_b2 = 2.0 * b;
}

//-------------------------------------------------------------------------------------------
/*
* Second order Butterworth band-pass filter
* bW: bandwidth of peak/notch = fc/Q
* fC: corner frequency
* fS: sample frequency
*
* Butterworth BPF and BSF are made by cascading (BPF) or paralleling (BSF) a Butterworth
* LPF and Butterworth HPF.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffBandPassButterworthSecondOrder(sample_t bW, sample_t fC, sample_t fS)
{
    sample_t c = 1.0 / (::tan(c_PI_D * fC * bW / fS));
    sample_t d = 2.0 * ::cos(2.0 * c_PI_D * fC / fS);
    m_a0 = 1.0 / (1.0 + c);
    m_a1 = 0.0;
    m_a2 = -m_a0;
    m_b1 = -m_a0 * (c * d);
    m_b2 = m_a0 * (c - 1.0);
}

//-------------------------------------------------------------------------------------------
/*
* Second order Butterworth band-stop filter
* bW: bandwidth of peak/notch = fc/Q
* fC: corner frequency
* fS: sample frequency
*
* Butterworth BPF and BSF are made by cascading (BPF) or paralleling (BSF) a Butterworth
* LPF and Butterworth HPF.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffBandStopButterworthSecondOrder(sample_t bW, sample_t fC, sample_t fS)
{
    sample_t c = ::tan(c_PI_D*fC*bW / fS);
    sample_t d = 2.0 * ::cos(2.0 * c_PI_D * fC / fS);
    m_a0 = 1.0 / (1.0 + c);
    m_a1 = -m_a0 * d;
    m_a2 = m_a0;
    m_b1 = -m_a0 * d;
    m_b2 = m_a0 * (1.0 - c);
}

//-------------------------------------------------------------------------------------------
/*
* Second order Butterworth high-pass filter
* fC: corner frequency
* fS: sample frequency
*
* Butterworth low-pass and high-pass filters are specialized versions of the ordinary secondorder
* low-pass filter. Their Q values are fixed at 0.707, which is the largest value it can
* assume before peaking in the frequency response is observed.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffHighPassButterworthSecondOrder(sample_t fC, sample_t fS)
{
    sample_t c = ::tan(c_PI_D*fC / fS);
    m_a0 = 1.0 / (1.0 + c_SQR_TWO_D * c + ::pow(c, 2.0));
    m_a1 = -2.0 * m_a0;
    m_a2 = m_a0;
    m_b1 = 2.0 * m_a0*(::pow(c, 2.0) - 1.0);
    m_b2 = m_a0 * (1.0 - c_SQR_TWO_D*c + ::pow(c, 2.0));
}

//-------------------------------------------------------------------------------------------
/*
* Second order Butterworth low-pass filter
* fC: corner frequency
* fS: sample frequency
* 
* Butterworth low-pass and high-pass filters are specialized versions of the ordinary secondorder
* low-pass filter. Their Q values are fixed at 0.707, which is the largest value it can
* assume before peaking in the frequency response is observed.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffLowPassButterworthSecondOrder(sample_t fC, sample_t fS)
{
    sample_t c = 1.0 / (::tan(c_PI_D * fC / fS));
    m_a0 = 1.0 / (1.0 + c_SQR_TWO_D * c + ::pow(c, 2.0));
    m_a1 = 2.0 * m_a0;
    m_a2 = m_a0;
    m_b1 = 2.0 * m_a0*(1.0 - ::pow(c, 2.0));
    m_b2 = m_a0 * (1.0 - c_SQR_TWO_D * c + ::pow(c, 2.0));
}

//-------------------------------------------------------------------------------------------
/*
* Second order high-pass filter
* Q: quality factor controlling resonant peaking
* fC: corner frequency
* fS: sample frequency
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffHighPassSecondOrder(sample_t Q, sample_t fC, sample_t fS)
{
    sample_t w = 2.0 * c_PI_D * fC / fS;
    sample_t d = 1.0 / Q;
    sample_t b = 0.5 * (1.0 - (d / 2) * ::sin(w)) / (1.0 + (d / 2.0)* ::sin(w));
    sample_t g = (0.5 + b) * ::cos(w);
    m_a0 = (0.5 + b + g) / 2.0;
    m_a1 = -(0.5 + b + g);
    m_a2 = m_a0;
    m_b1 = -2.0 * g;
    m_b2 = 2.0 * b;
}

//-------------------------------------------------------------------------------------------
/*
* Second order Low-pass filter
* Q: quality factor controlling resonant peaking
* fC: corner frequency
* fS: sample frequency
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffLowPassSecondOrder(sample_t Q, sample_t fC, sample_t fS)
{
    sample_t w = 2.0 * c_PI_D * fC / fS;
    sample_t d = 1.0 / Q;
    sample_t b = 0.5 * (1.0 - (d / 2) * ::sin(w)) / (1.0 + (d / 2.0) * ::sin(w));
    sample_t g = (0.5 + b) * ::cos(w);
    m_a0 = (0.5 + b - g) / 2.0;
    m_a1 = 0.5 + b - g;
    m_a2 = m_a0;
    m_b1 = -2.0 * g;
    m_b2 = 2.0 * b;
}

//-------------------------------------------------------------------------------------------
/*
* Second order Linkwitz-Riley high-pass filter
* fC: corner frequency
* fS: sample frequency
*
* Second-order Linkwitz�Riley HPFs are designed to have an attenuation of -6 dB
* at the corner frequency rather than the standard -3 dB When these
* filters are placed in parallel with the same cutoff frequency, their outputs sum exactly and
* the resulting response is flat. They are often used in crossovers.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffHighPassLinkwitzRileySecondOrder(sample_t fC, sample_t fS)
{
    sample_t th = c_PI_D * fC / fS;
    sample_t Wc = c_PI_D * fC;
    sample_t k = Wc / ::tan(th);
    sample_t d = ::pow(k, 2.0) + ::pow(Wc, 2.0) + 2.0 * k * Wc;
    m_a0 = ::pow(k, 2.0) / d;
    m_a1 = -2.0 * ::pow(k, 2.0) / d;
    m_a2 = m_a0;
    m_b1 = (-2.0 * ::pow(k, 2.0) + 2.0 * ::pow(Wc, 2.0)) / d;
    m_b2 = (-2.0 * k * Wc + ::pow(k, 2.0) + ::pow(Wc, 2.0)) / d;
}

//-------------------------------------------------------------------------------------------
/*
* Second order Linkwitz-Riley low-pass filter
* fC: corner frequency
* fS: sample frequency
* 
* Second-order Linkwitz�Riley LPFs are designed to have an attenuation of -6 dB
* at the corner frequency rather than the standard -3 dB When these
* filters are placed in parallel with the same cutoff frequency, their outputs sum exactly and
* the resulting response is flat. They are often used in crossovers.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffLowPassLinkwitzRileySecondOrder(sample_t fC, sample_t fS)
{
    sample_t th = c_PI_D * fC / fS;
    sample_t Wc = c_PI_D * fC;
    sample_t k = Wc / ::tan(th);
    sample_t d = ::pow(k, 2.0) + ::pow(Wc, 2.0) + 2.0 * k * Wc;
    m_a0 = ::pow(Wc, 2.0) / d;
    m_a1 = 2.0 * ::pow(Wc, 2.0) / d;
    m_a2 = m_a0;
    m_b1 = (-2.0 * ::pow(k, 2.0) + 2.0 * ::pow(Wc, 2.0)) / d;
    m_b2 = (-2.0 * k * Wc + ::pow(k, 2.0) + ::pow(Wc, 2.0)) / d;
}

//-------------------------------------------------------------------------------------------
/*
* Second order parametric/peaking boost filter with constant-Q
* gainDB: Gain/attenuation in dB
* Q: quality factor
* fC: corner frequency
* fS: sample frequency
* 
* Parametric EQs allow you to adjust the center frequency, Q and boost or cut creating any
* arbitrary bumps or notches in the frequency response. The parametric EQ is a
* variation on the ordinary band-pass and band-stop fi lters that generates symmetrical boost/
* cut curves and mixes in the dry signal to create the fi nal response. A true digital parametric
* EQ not only has independent controls, but each control only varies one coeffi cient in the
* fi lter. The parametric EQs in this section afford the same frequency response but adjustments
* in any parameter require a recalculation of all the coeffi cients. These fi lters are also called
* peaking fi lters.
*
* This design creates an almost perfect constant-Q filter with only a small amount of error
* for low-boost (or cut) values.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffParametricBoostCQ(sample_t gainDB, sample_t Q, sample_t fC, sample_t fS)
{
    sample_t K = 2.0 * c_PI_D * fC / fS;
    sample_t V0 = ::pow(10.0, gainDB / 20.0);
    sample_t dd = 1.0 + K/Q + ::pow(K, 2.0);
    sample_t a = 1.0 + (V0*K)/Q + ::pow(K, 2.0);
    sample_t b = 2.0 * (::pow(K, 2.0) - 1.0);
    sample_t g = 1.0 - (V0*K)/Q + ::pow(K, 2.0);
    sample_t d = 1.0 - K/Q + ::pow(K, 2.0);
    m_a0 = a / dd;
    m_a1 = b / dd;
    m_a2 = g / dd;
    m_b1 = b / dd;
    m_b2 = d / dd;
    m_c0 = 1.0;
    m_d0 = 0.0;
}

//-------------------------------------------------------------------------------------------
/*
* Second order parametric/peaking cut filter with constant-Q
* gainDB: Gain/attenuation in dB
* Q: quality factor
* fC: corner frequency
* fS: sample frequency
* 
* Parametric EQs allow you to adjust the center frequency, Q and boost or cut creating any
* arbitrary bumps or notches in the frequency response. The parametric EQ is a
* variation on the ordinary band-pass and band-stop fi lters that generates symmetrical boost/
* cut curves and mixes in the dry signal to create the fi nal response. A true digital parametric
* EQ not only has independent controls, but each control only varies one coeffi cient in the
* fi lter. The parametric EQs in this section afford the same frequency response but adjustments
* in any parameter require a recalculation of all the coeffi cients. These fi lters are also called
* peaking fi lters.
*
* This design creates an almost perfect constant-Q filter with only a small amount of error
* for low-boost (or cut) values.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffParametricCutCQ(sample_t gainDB, sample_t Q, sample_t fC, sample_t fS)
{
    sample_t K = 2.0 * c_PI_D * fC / fS;
    sample_t V0 = ::pow(10.0, gainDB / 20.0);
    sample_t dd = 1.0 + K / Q + ::pow(K, 2.0);
    sample_t e = 1.0 + K / (V0*Q) + ::pow(K, 2.0);
    sample_t b = 2.0 * (::pow(K, 2.0) - 1.0);
    sample_t d = 1.0 - K / Q + ::pow(K, 2.0);
    sample_t h = 1.0 - K / (V0*Q) + ::pow(K, 2.0);
    m_a0 = dd / e;
    m_a1 = b / e;
    m_a2 = d / e;
    m_b1 = b / e;
    m_b2 = h / e;
    m_c0 = 1.0;
    m_d0 = 0.0;
}

//-------------------------------------------------------------------------------------------
/*
* Second order parametric/peaking filter with non-constant-Q
* gainDB: Gain/attenuation in dB
* Q: quality factor
* fC: corner frequency
* fS: sample frequency
* 
* Parametric EQs allow you to adjust the center frequency, Q and boost or cut creating any
* arbitrary bumps or notches in the frequency response. The parametric EQ is a
* variation on the ordinary band-pass and band-stop fi lters that generates symmetrical boost/
* cut curves and mixes in the dry signal to create the fi nal response. A true digital parametric
* EQ not only has independent controls, but each control only varies one coeffi cient in the
* fi lter. The parametric EQs in this section afford the same frequency response but adjustments
* in any parameter require a recalculation of all the coeffi cients. These filters are also called
* peaking fi lters.
* This parametric EQ is not constant-Q, which means the bandwidth varies depending on the
* boost/cut value. Some analog fi lters have the same issue, although there is occasional debate
* over whether or not this is desirable in an EQ design.
*/
//-------------------------------------------------------------------------------------------

void BiQuadFilter::coeffParametricNCQ(sample_t gainDB, sample_t Q, sample_t fC, sample_t fS)
{
    sample_t w = 2.0 * c_PI_D * fC / fS;
    sample_t m = ::pow(10.0, gainDB / 20.0);
    sample_t z = 4.0 / (1.0 + m);
    sample_t b = 0.5 * ((1.0 - z * ::tan(w / (2.0*Q)) / (1 + z * ::tan(w / (2.0*Q)))));
    sample_t g = (0.5 + b) * ::cos(w);
    m_a0 = 0.5 - b;
    m_a1 = 0.0;
    m_a2 = -(0.5 - b);
    m_b1 = -2.0*g;
    m_b2 = 2.0 * b;
    m_c0 = m - 1.0;
    m_d0 = 1.0;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
