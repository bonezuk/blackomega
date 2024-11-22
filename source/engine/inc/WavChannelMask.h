//-------------------------------------------------------------------------------------------
#ifndef __OMEGA_ENGINE_WAVCHANNELMASK_H
#define __OMEGA_ENGINE_WAVCHANNELMASK_H
//-------------------------------------------------------------------------------------------

#include "engine/inc/EngineDLL.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

class ENGINE_EXPORT WavChannelMask
{
	public:

		typedef enum
		{
			e_channelFrontLeft     = 0,
			e_channelFrontRight    = 1,
			e_channelCenter        = 2,
			e_channelLFE           = 3,
			e_channelSurroundLeft  = 4,
			e_channelSurroundRight = 5,
			e_channelRearLeft      = 6,
			e_channelRearRight     = 7
		} ChannelType;

		typedef enum
		{
			e_speakerFrontLeft =        0x00000001,
			e_speakerFrontRight =       0x00000002,
			e_speakerFrontCenter =      0x00000004,
			e_speakerLowFrequency =     0x00000008,
			e_speakerBackLeft =         0x00000010,
			e_speakerBackRight =        0x00000020,
			e_speakerFrontLeftCenter =  0x00000040,
			e_speakerFrontRightCenter = 0x00000080,
			e_speakerBackCenter =       0x00000100,
			e_speakerSideLeft =         0x00000200,
			e_speakerSideRight =        0x00000400,
			e_speakerTopCenter =        0x00000800,
			e_speakerTopFrontLeft =     0x00001000,
			e_speakerTopFrontCenter =   0x00002000,
			e_speakerTopFrontRight =    0x00004000,
			e_speakerTopBackLeft =      0x00008000,
			e_speakerTopBackCenter =    0x00010000,
			e_speakerTopBackRight =     0x00020000
		} WAVEChannelMask;	

	public:
		WavChannelMask();
		WavChannelMask(const WavChannelMask& rhs);
		virtual ~WavChannelMask();
		
		const WavChannelMask& operator = (const WavChannelMask& rhs);
		
        virtual void setup(tint noInChannels, tint noOutChannels, tint chMask);

		const tint *channelMap() const;
		tint outChannelNo() const;
	
	protected:
		
		tint m_noOutputChannels;
		tint m_chMap[19];
		tint *m_channelMap;
		
		void copy(const WavChannelMask& rhs);
		virtual void setupMultiChannel(tint noOutChannels, tint chMask);
};

//-------------------------------------------------------------------------------------------

inline const tint *WavChannelMask::channelMap() const
{
	return m_channelMap;
}

//-------------------------------------------------------------------------------------------

inline tint WavChannelMask::outChannelNo() const
{
	return m_noOutputChannels;
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
#endif
//-------------------------------------------------------------------------------------------
