//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WHITEOMEGA_PREDICTION_H
#define __OMEGA_ENGINE_WHITEOMEGA_PREDICTION_H
//-------------------------------------------------------------------------------------------

#include "engine/whiteomega/inc/ICS.h"
#include "engine/whiteomega/inc/AACDecode.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT Prediction
{
    public:

        typedef struct
        {
            tint16 r[2];
            tint16 cor[2];
            tint16 var[2];
        } State;

    public:
        Prediction();
        ~Prediction();

        void set(GAConfig *cfg,AACDecode *ch);
        void set(GAConfig *cfg,AACDecode *chL,AACDecode *chR);

        void icPredictionL();
        void icPredictionR();

        void pnsResetPredStateL();
        void pnsResetPredStateR();

    protected:

        static const sample_t m_mntTable[128];
        static const sample_t m_expTable[128];

        GAConfig *m_gaConfig;
        AACDecode *m_channelL;
        AACDecode *m_channelR;

        State m_stateL[1024];
        State m_stateR[1024];

        void icPrediction(AACDecode *ch,State *pState);

        void icPredict(State *state,sample_t input,sample_t *output,tint pred);

        void round(sample_t *x);

        tint16 quantPred(sample_t x);
        sample_t quadPredInv(tint16 q);

        void resetPredState(State *state);
        void resetAllPred(State *pState);
        void pnsResetPredState(AACDecode *ch,State *pState);

};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
