#include "audioio/inc/AbstractAudioHardwareBuffer.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AbstractAudioHardwareBuffer::AbstractAudioHardwareBuffer()
{}

//-------------------------------------------------------------------------------------------

AbstractAudioHardwareBuffer::~AbstractAudioHardwareBuffer()
{}

//-------------------------------------------------------------------------------------------

tint AbstractAudioHardwareBuffer::numberOfOutputForEveryOneInputSamples()
{
    return 1;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
