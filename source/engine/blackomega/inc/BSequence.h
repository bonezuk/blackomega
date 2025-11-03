//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLACKOMEGA_BSEQUENCE_H
#define __OMEGA_ENGINE_BLACKOMEGA_BSEQUENCE_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/Sequence.h"
#include "engine/blackomega/inc/BandIndex.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blackomega
{
//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT BBitstream : public engine::Bitstream
{
    public:
        BBitstream();
        virtual ~BBitstream();

    protected:

        virtual Sequence *newSequence();
};

//-------------------------------------------------------------------------------------------

class BLACKOMEGA_EXPORT BSequence : public engine::Sequence
{
    public:
        BSequence();
        virtual ~BSequence();

        virtual void reset();

        void startGrain(tint l);
        void countGrain();

        tint remain() const;

        void decode(tint h,tint& x,tint& y);
        void decode(tint h,tint& x,tint& y,tint& v,tint& w);

    protected:

        tint m_count;
        tint m_countOffset;
        tint m_grainLength;

        Band *m_band;

        virtual bool next();
};

//-------------------------------------------------------------------------------------------

inline tint BSequence::remain() const
{
    return m_grainLength - (m_count + (static_cast<tint>(m_bitOffset) - m_countOffset));
}

//-------------------------------------------------------------------------------------------
} // namespace blackomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

