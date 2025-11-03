//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WHITEOMEGA_PREDICTIONLTP_H
#define __OMEGA_ENGINE_WHITEOMEGA_PREDICTIONLTP_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/Common.h"
#include "engine/whiteomega/inc/GAConfig.h"
#include "engine/whiteomega/inc/AACDecode.h"
#include "engine/whiteomega/inc/Window.h"
#include "engine/whiteomega/inc/TNS.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT PredictionLTP
{
    public:
        PredictionLTP();
        ~PredictionLTP();

        void set(GAConfig *cfg,AACDecode *ch,Window *win,TNS *tns);

        void reset();

        bool isLTP() const;

        void update();

        void predict(LTPInfo *ltp);

    protected:

        common::Allocation m_alloc;
        GAConfig *m_gaConfig;
        AACDecode *m_channel;
        Window *m_window;
        TNS *m_tns;

        tint16 *m_state;
        tint m_stateLen;

        sample_t *m_xIn;

        tint16 realToShort(sample_t x);
};

//-------------------------------------------------------------------------------------------

inline tint16 PredictionLTP::realToShort(sample_t x)
{
#if defined(SINGLE_FLOAT_SAMPLE)
    if(x >= 0.0f)
    {
        x += 0.5f;
        if(x >= 32767.0f)
        {
            return 32767;
        }
    }
    else
    {
        x -= 0.5f;
        if(x <= -32768.0f)
        {
            return -32768;
        }
    }
#else
    if(x >= 0.0)
    {
        x += 0.5;
        if(x >= 32767.0)
        {
            return 32767;
        }
    }
    else
    {
        x -= 0.5;
        if(x <= -32768.0)
        {
            return -32768;
        }
    }
#endif
    return static_cast<tint16>(x);
}

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
