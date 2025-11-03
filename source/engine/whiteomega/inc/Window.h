//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WHITEOMEGA_WINDOW_H
#define __OMEGA_ENGINE_WHITEOMEGA_WINDOW_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#if defined(SINGLE_FLOAT_SAMPLE)
#include "engine/inc/OmegaDCT.h"
#else
#include "engine/inc/OmegaDCT64.h"
#endif
#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/GAConfig.h"
#include "engine/whiteomega/inc/AACDecode.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT Window
{
    public:
        Window();
        virtual ~Window();

        static void start();
        static void stop();

        void set(GAConfig *cfg,AACDecode *ch);

        void reset();

        sample_t *process(tint& len);
        sample_t *processLTP(sample_t *in,tint& len);

        sample_t *m_out;
        tint m_prevFrameLen;

    protected:

        static common::Allocation m_alloc;

        // Sine Window
        static const sample_t m_windowSine_2048[1024];
        static const sample_t m_windowSine_1920[960];
        static const sample_t m_windowSine_256[128];
        static const sample_t m_windowSine_240[120];

        // Kaiser-Bessel Window
        static const sample_t m_windowBessel_2048[1024];
        static const sample_t m_windowBessel_1920[960];
        static const sample_t m_windowBessel_256[128];
        static const sample_t m_windowBessel_240[120];

        static sample_t *m_winSine2048;
        static sample_t *m_winSine1920;
        static sample_t *m_winSine256;
        static sample_t *m_winSine240;

        static sample_t *m_winBessel2048;
        static sample_t *m_winBessel1920;
        static sample_t *m_winBessel256;
        static sample_t *m_winBessel240;

        GAConfig *m_gaConfig;
        AACDecode *m_channel;

        tint m_prevWindowShape;
        sample_t *m_X;

        sample_t *m_winInLTP;
        sample_t *m_outLTP;

        void doLong(sample_t *x,sample_t *z,sample_t *win0,sample_t *win1);
        void doShort(sample_t *x,sample_t *z,sample_t *win0,sample_t *win1);
        void doStart(sample_t *x,sample_t *z,sample_t *win0,sample_t *win1);
        void doStop(sample_t *x,sample_t *z,sample_t *win0,sample_t *win1);

        void doLongLTP(sample_t *x,sample_t *z,sample_t *win0,sample_t *win1);
        void doStartLTP(sample_t *x,sample_t *z,sample_t *win0,sample_t *win1);
        void doStopLTP(sample_t *x,sample_t *z,sample_t *win0,sample_t *win1);
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
