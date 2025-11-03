//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_GREENOMEGA_FLACSUBFRAME_H
#define __OMEGA_ENGINE_GREENOMEGA_FLACSUBFRAME_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Allocation.h"
#include "engine/greenomega/inc/FLACHeader.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace greenomega
{
//-------------------------------------------------------------------------------------------

class GREENOMEGA_EXPORT FLACSubframe
{
    public:
        FLACSubframe(FLACMetaStreamInfo *info,FLACHeader *hdr,tint ch);
        virtual ~FLACSubframe();

        bool allocate();
        bool read(Sequence *seq,tint bps);

        tint *output();

    protected:

        common::Allocation m_alloc;
        FLACMetaStreamInfo *m_streamInfo;
        FLACHeader *m_header;
        tint m_channel;
        tint m_outputSize;
        tint *m_output;
        tint *m_residual;

        tuint m_resCapacityByOrder;
        tuint *m_resParameters;
        tuint *m_resRawBits;

        bool readConstant(Sequence *seq,tint bps);
        bool readVerbatim(Sequence *seq,tint bps);
        bool readFixed(Sequence *seq,tint order,tint bps);
        bool readLPC(Sequence *seq,tint order,tint bps);

        bool allocateResidualRice(Sequence *seq,tint maxOrder);
        bool readResidualRice(Sequence *seq,tint predOrder,tint partOrder,bool extended);
        void readRiceBlock(Sequence *seq,tint *sample,tint len,tint n);
        void restoreResidual(const tint *residual,tint len,tint order,tint *data);

        void restoreLPCSignal(const tint *residual,tint len,const tint *qCoeff,tint order,tint lpQuant,tint *data);
        void restoreLPCSignalWide(const tint *residual,tint len,const tint *qCoeff,tint order,tint lpQuant,tint *data);

        tint iLog2(tint v);
};

//-------------------------------------------------------------------------------------------

inline tint *FLACSubframe::output()
{
    return m_output;
}

//-------------------------------------------------------------------------------------------
} // namespace greenomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

