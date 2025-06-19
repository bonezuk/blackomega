//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_FIRFILTER200HZLOWPASS_H
#define __OMEGA_ENGINE_FIRFILTER200HZLOWPASS_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/FIRFilter.h"

#include <QSharedPointer>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

ENGINE_EXPORT QSharedPointer<FIRFilter> createFIRFilter200HzLowPass(int frequency);

ENGINE_EXPORT tint minimumAudioItemLengthFor200HzLowPass(int frequency);

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
