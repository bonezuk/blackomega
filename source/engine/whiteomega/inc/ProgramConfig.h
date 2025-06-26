//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WHITEOMEGA_PROGRAMCONFIG_H
#define __OMEGA_ENGINE_WHITEOMEGA_PROGRAMCONFIG_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Sequence.h"
#include "engine/whiteomega/inc/Atom.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace whiteomega
{
//-------------------------------------------------------------------------------------------

class WHITEOMEGA_EXPORT ProgramConfig
{
    public:
        ProgramConfig();
        virtual ~ProgramConfig();

        virtual bool read(Sequence *seq);

        tint m_elementInstanceTag;
        tint m_objectType;
        tint m_samplingFrequencyIndex;
        tint m_numFrontChannelElements;
        tint m_numSideChannelElements;
        tint m_numBackChannelElements;
        tint m_numLfeChannelElements;
        tint m_numAssocDataElements;
        tint m_numValidCcElements;
        bool m_monoMixdownPresent;
        tint m_monoMixdownElementNumber;
        bool m_stereoMixdownPresent;
        tint m_stereoMixdownElementNumber;
        bool m_matrixMixdownIdxPresent;
        tint m_matrixMixdownIdx;
        bool m_pseudoSurroundEnable;
        bool m_frontElementIsCpe[16];
        tint m_frontElementTagSelect[16];
        bool m_sideElementIsCpe[16];
        tint m_sideElementTagSelect[16];
        bool m_backElementIsCpe[16];
        tint m_backElementTagSelect[16];
        tint m_lfeElementTagSelect[4];
        tint m_assocDataElementTagSelect[8];
        bool m_ccElementIsIndSw[16];
        tint m_validCcElementTagSelect[16];
        QString m_comment;
};

//-------------------------------------------------------------------------------------------
} // namespace whiteomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
