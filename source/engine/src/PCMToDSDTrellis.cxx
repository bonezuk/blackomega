/*
//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_PCMTODSDTRELLIS_H
#define __OMEGA_ENGINE_PCMTODSDTRELLIS_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/PCMToDSD.h"
#include "engine/inc/SDMTrellisBlock.h"
#if defined(OMEGA_CUDA)
#include "engine/inc/SDMTrellisCUDA.h"
#endif

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT PCMToDSDTrellis : public PCMToDSD
{
    public:
        PCMToDSDTrellis();
        PCMToDSDTrellis(PCMToDSD::ComputeMethod computeMethod);
        virtual ~PCMToDSDTrellis();
};

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

#include "engine/inc/PCMToDSDTrellis.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

PCMToDSDTrellis::PCMToDSDTrellis()
{}

//-------------------------------------------------------------------------------------------

PCMToDSDTrellis::PCMToDSDTrellis(PCMToDSD::ComputeMethod computeMethod)
{}

//-------------------------------------------------------------------------------------------

PCMToDSDTrellis::~PCMToDSDTrellis()
{}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
*/