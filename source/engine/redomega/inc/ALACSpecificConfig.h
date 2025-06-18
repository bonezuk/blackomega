//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_REDOMEGA_ALACSPECIFICONFIG_H
#define __OMEGA_ENGINE_REDOMEGA_ALACSPECIFICONFIG_H
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

class REDOMEGA_EXPORT ALACSpecificConfig
{
    public:
        ALACSpecificConfig();
        virtual ~ALACSpecificConfig();
        
        bool read(ALACSequence *seq);

        tint& frameLength();
        tint& compatibleVersion();
        tint& bitDepth();
        tint& pb();
        tint& mb();
        tint& kb();
        tint& numChannels();
        tint& maxRun();
        tint& maxFrameBytes();
        tint& avgBitRate();
        tint& sampleRate();
        
        const tint& frameLength() const;
        const tint& compatibleVersion() const;
        const tint& bitDepth() const;
        const tint& pb() const;
        const tint& mb() const;
        const tint& kb() const;
        const tint& numChannels() const;
        const tint& maxRun() const;
        const tint& maxFrameBytes() const;
        const tint& avgBitRate() const;
        const tint& sampleRate() const;
        
    protected:
    
        tint m_frameLength;
        tint m_compatibleVersion;
        tint m_bitDepth;
        tint m_pb;
        tint m_mb;
        tint m_kb;
        tint m_numChannels;
        tint m_maxRun;
        tint m_maxFrameBytes;
        tint m_avgBitRate;
        tint m_sampleRate;
        
        void printError(const tchar *strR,const tchar *strE) const;
};

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::frameLength()
{
    return m_frameLength;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::compatibleVersion()
{
    return m_compatibleVersion;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::bitDepth()
{
    return m_bitDepth;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::pb()
{
    return m_pb;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::mb()
{
    return m_mb;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::kb()
{
    return m_kb;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::numChannels()
{
    return m_numChannels;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::maxRun()
{
    return m_maxRun;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::maxFrameBytes()
{
    return m_maxFrameBytes;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::avgBitRate()
{
    return m_avgBitRate;
}

//-------------------------------------------------------------------------------------------

inline tint& ALACSpecificConfig::sampleRate()
{
    return m_sampleRate;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::frameLength() const
{
    return m_frameLength;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::compatibleVersion() const
{
    return m_compatibleVersion;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::bitDepth() const
{
    return m_bitDepth;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::pb() const
{
    return m_pb;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::mb() const
{
    return m_mb;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::kb() const
{
    return m_kb;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::numChannels() const
{
    return m_numChannels;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::maxRun() const
{
    return m_maxRun;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::maxFrameBytes() const
{
    return m_maxFrameBytes;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::avgBitRate() const
{
    return m_avgBitRate;
}

//-------------------------------------------------------------------------------------------

inline const tint& ALACSpecificConfig::sampleRate() const
{
    return m_sampleRate;
}

//-------------------------------------------------------------------------------------------
} // namespace redomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

