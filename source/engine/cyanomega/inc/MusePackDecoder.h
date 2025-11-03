//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_CYANOMEGA_MUSEPACKDECODER_H
#define __OMEGA_ENGINE_CYANOMEGA_MUSEPACKDECODER_H
//-------------------------------------------------------------------------------------------

#include "common/inc/Factory.h"
#include "engine/inc/Codec.h"
#include "engine/cyanomega/inc/CyanOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace cyanomega
{
//-------------------------------------------------------------------------------------------

class CYANOMEGA_EXPORT MusePackDecoder
{
    public:
        MusePackDecoder();
        virtual ~MusePackDecoder();
        
        virtual bool open(const QString& fileName) = 0;
        virtual void close() = 0;
        
        virtual tint noChannels() const = 0;
        virtual tint frequency() const = 0;
        
        virtual bool read(QByteArray& mem) = 0;
        
        virtual tint bitrate() const = 0;
        virtual common::TimeStamp length() const = 0;
        
        virtual bool seek(const common::TimeStamp& v) = 0;
};

typedef QSharedPointer<MusePackDecoder> MusePackDecoderSPtr;

//-------------------------------------------------------------------------------------------

ABSTRACT_FACTORY_CLASS(CYANOMEGA_EXPORT,MusePackDecoderFactory,MusePackDecoder)

//-------------------------------------------------------------------------------------------
} // namespace cyanomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

