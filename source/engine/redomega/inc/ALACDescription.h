//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_ALACDESCRIPTION_H
#define __OMEGA_ENGINE_REDOMEGA_ALACDESCRIPTION_H
//-------------------------------------------------------------------------------------------

#include "engine/redomega/inc/ALACSequence.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace redomega
{
//-------------------------------------------------------------------------------------------

class REDOMEGA_EXPORT ALACDescription
{
    public:
        ALACDescription();
        virtual ~ALACDescription();

        bool read(ALACSequence *seq);

        tfloat64& sampleRate();
        tuint32& formatID();
        tint& formatFlags();
        tint& bytesPerPacket();
        tint& framesPerPacket();
        tint& bytesPerFrame();
        tint& channelsPerFrame();
        tint& bitsPerChannel();

        const tfloat64& sampleRate() const;
        const tuint32& formatID() const;
        const tint& formatFlags() const;
        const tint& bytesPerPacket() const;
        const tint& framesPerPacket() const;
        const tint& bytesPerFrame() const;
        const tint& channelsPerFrame() const;
        const tint& bitsPerChannel() const;

    protected:

        void printError(const tchar *strR,const tchar *strE) const;

        tfloat64 m_sampleRate;
        tuint32 m_formatID;
        tint m_formatFlags;
        tint m_bytesPerPacket;
        tint m_framesPerPacket;
        tint m_bytesPerFrame;
        tint m_channelsPerFrame;
        tint m_bitsPerChannel;

};

//-------------------------------------------------------------------------------------------

inline tfloat64& ALACDescription::sampleRate()
{
    return m_sampleRate;
}

//-------------------------------------------------------------------------------------------

inline tuint32& ALACDescription::formatID()
{
    return m_formatID;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACDescription::formatFlags()
{
    return m_formatFlags;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACDescription::bytesPerPacket()
{
    return m_bytesPerPacket;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACDescription::framesPerPacket()
{
    return m_framesPerPacket;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACDescription::bytesPerFrame()
{
    return m_bytesPerFrame;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACDescription::channelsPerFrame()
{
    return m_channelsPerFrame;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACDescription::bitsPerChannel()
{
    return m_bitsPerChannel;
}

//-------------------------------------------------------------------------------------------

inline const tfloat64& ALACDescription::sampleRate() const
{
    return m_sampleRate;
}

//-------------------------------------------------------------------------------------------

inline const tuint32& ALACDescription::formatID() const
{
    return m_formatID;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACDescription::formatFlags() const
{
    return m_formatFlags;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACDescription::bytesPerPacket() const
{
    return m_bytesPerPacket;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACDescription::framesPerPacket() const
{
    return m_framesPerPacket;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACDescription::bytesPerFrame() const
{
    return m_bytesPerFrame;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACDescription::channelsPerFrame() const
{
    return m_channelsPerFrame;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACDescription::bitsPerChannel() const
{
    return m_bitsPerChannel;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

