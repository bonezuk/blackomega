//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_BLUEOMEGA_WAVEINFORMATION_H
#define __OMEGA_ENGINE_BLUEOMEGA_WAVEINFORMATION_H
//-------------------------------------------------------------------------------------------

#include "common/inc/BIOStream.h"
#include "common/inc/Log.h"
#include "engine/inc/WavChannelMask.h"
#include "engine/blueomega/inc/BlueOmegaDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
namespace blueomega
{
//-------------------------------------------------------------------------------------------

#define BLUEOMEGA_ID(a,b,c,d) (((static_cast<tuint32>(static_cast<tint32>(d)) << 24) & 0xff000000) | \
                               ((static_cast<tuint32>(static_cast<tint32>(c)) << 16) & 0x00ff0000) | \
                               ((static_cast<tuint32>(static_cast<tint32>(b)) <<  8) & 0x0000ff00) | \
                               ((static_cast<tuint32>(static_cast<tint32>(a))      ) & 0x000000ff))

#define RIFF_ID BLUEOMEGA_ID('R','I','F','F')
#define WAVE_ID BLUEOMEGA_ID('W','A','V','E')
#define FMT_ID  BLUEOMEGA_ID('f','m','t',' ')

//-------------------------------------------------------------------------------------------

class BLUEOMEGA_EXPORT WaveInformation
{
    public:

        typedef enum
        {
            e_formatPCM = 1,
            e_formatFloat = 3,
            e_formatExtensible = 0xfffe
        } WAVEFormatType;
        
        typedef enum
        {
            e_8Bit = 0,
            e_16Bit_LittleEndian,
            e_24Bit_LittleEndian,
            e_32Bit_LittleEndian,
            e_Float_LittleEndian,
            e_16Bit_BigEndian,
            e_24Bit_BigEndian,
            e_32Bit_BigEndian,
            e_Float_BigEndian,
            e_WaveUnknown
        } WaveType;

    public:
        WaveInformation();
        WaveInformation(const WaveInformation& rhs);
        virtual ~WaveInformation();
        
        const WaveInformation& operator = (const WaveInformation& rhs);
        
        virtual bool read(common::BIOStream *in);
        
        tint format() const;
        tint audioChannels() const;
        tint sampleRate() const;
        tint bytesPerSecond() const;
        tint bitsPerSample() const;
        tint offset() const;
        
        WaveType type() const;

        const tint *channelMap() const;
        tint outChannelNo() const;
        
        void setupChannelMap(tint noOutChannels);
        
    protected:
        
        tint m_format;
        tint m_noChannels;
        tint m_sampleRate;
        tint m_bytesPerSecond;
        tint m_bytesPerSample;
        tint m_bitsPerSample;
        tint m_offset;
        
        WaveType m_type;
        WavChannelMask m_channelHandler;
        
        tint m_chMask;
        
        virtual void printError(const tchar *strR,const tchar *strE) const;
        
        virtual void copy(const WaveInformation& rhs);
        
        tint intFromMemory(tchar *mem) const;
        tint shortFromMemory(tchar *mem) const;
};

//-------------------------------------------------------------------------------------------

inline WaveInformation::WaveType WaveInformation::type() const
{
    return m_type;
}

//-------------------------------------------------------------------------------------------

inline tint WaveInformation::outChannelNo() const
{
    return m_channelHandler.outChannelNo();
}

//-------------------------------------------------------------------------------------------

inline const tint *WaveInformation::channelMap() const
{
    return m_channelHandler.channelMap();
}

//-------------------------------------------------------------------------------------------

inline tint WaveInformation::format() const
{
    return m_format;
}

//-------------------------------------------------------------------------------------------

inline tint WaveInformation::audioChannels() const
{
    return m_noChannels;
}

//-------------------------------------------------------------------------------------------

inline tint WaveInformation::sampleRate() const
{
    return m_sampleRate;
}

//-------------------------------------------------------------------------------------------

inline tint WaveInformation::bytesPerSecond() const
{
    return m_bytesPerSample;
}

//-------------------------------------------------------------------------------------------

inline tint WaveInformation::bitsPerSample() const
{
    return m_bitsPerSample;
}

//-------------------------------------------------------------------------------------------

inline tint WaveInformation::offset() const
{
    return m_offset;
}

//-------------------------------------------------------------------------------------------
} // namespace blueomega
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------

