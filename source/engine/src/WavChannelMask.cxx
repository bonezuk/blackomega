#include "engine/inc/WavChannelMask.h"

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace engine
{
//-------------------------------------------------------------------------------------------

WavChannelMask::WavChannelMask() : m_noOutputChannels(2)
{
	m_channelMap = &m_chMap[1];
}

//-------------------------------------------------------------------------------------------

WavChannelMask::WavChannelMask(const WavChannelMask& rhs)  : m_noOutputChannels(2)
{
	m_channelMap = &m_chMap[1];
	copy(rhs);
}

//-------------------------------------------------------------------------------------------

WavChannelMask::~WavChannelMask()
{}

//-------------------------------------------------------------------------------------------

const WavChannelMask& WavChannelMask::operator = (const WavChannelMask& rhs)
{
	if(this!=&rhs)
	{
		copy(rhs);
	}
	return *this;
}

//-------------------------------------------------------------------------------------------

void WavChannelMask::copy(const WavChannelMask& rhs)
{
	m_noOutputChannels = rhs.m_noOutputChannels;
	::memcpy(m_channelMap,rhs.m_channelMap,18 * sizeof(tint));
}

//-------------------------------------------------------------------------------------------

void WavChannelMask::setup(tint noInChannels, tint noOutChannels, tint chMask)
{
	if(noInChannels == 2 && chMask == (e_speakerFrontLeft | e_speakerFrontRight))
	{
		m_noOutputChannels = 2;
		m_channelMap[0] = 0;
		m_channelMap[1] = 1;
		for(int i = 2; i < 18; i++)
		{
			m_channelMap[i] = -1;
		}
	}
	else
	{
		setupMultiChannel(noOutChannels, chMask);
	}
}

//-------------------------------------------------------------------------------------------

void WavChannelMask::setupMultiChannel(tint noOutChannels, tint chMask)
{
	tint i,j;
	tint map[8],avMap[8];
	bool downMixFlag;
	
	m_noOutputChannels = noOutChannels;
	
	for(i=0;i<8;i++)
	{
		map[i] = -1;
		avMap[i] = 0;
	}
	for(i=0;i<18;i++)
	{
		m_channelMap[i] = -1;
	}
	
	for(i=0,j=0;i<18;i++)
	{
		switch((1 << i) & chMask)
		{
			case e_speakerFrontLeft:
				if(map[e_channelFrontLeft]<0)
				{
					map[e_channelFrontLeft] = j;
				}
				break;
				
			case e_speakerFrontRight:
				if(map[e_channelFrontRight]<0)
				{
					map[e_channelFrontRight] = j;
				}
				break;
				
			case e_speakerFrontCenter:
				if(map[e_channelCenter]<0)
				{
					map[e_channelCenter] = j;
				}
				break;
				
			case e_speakerLowFrequency:
				if(map[e_channelLFE]<0)
				{
					map[e_channelLFE] = j;
				}
				break;
				
			case e_speakerBackLeft:
				if(map[e_channelRearLeft]<0)
				{
					map[e_channelRearLeft] = j;
				}
				break;
				
			case e_speakerBackRight:
				if(map[e_channelRearRight]<0)
				{
					map[e_channelRearRight] = j;
				}
				break;
				
			case e_speakerFrontLeftCenter:
				if(map[e_channelFrontLeft]<0)
				{
					map[e_channelFrontLeft] = j;
				}
				break;
				
			case e_speakerFrontRightCenter:
				if(map[e_channelFrontRight]<0)
				{
					map[e_channelFrontRight] = j;
				}
				break;
				
			case e_speakerBackCenter:
				break;
				
			case e_speakerSideLeft:
				if(map[e_channelSurroundLeft]<0)
				{
					map[e_channelSurroundLeft] = j;
				}
				break;
				
			case e_speakerSideRight:
				if(map[e_channelSurroundRight]<0)
				{
					map[e_channelSurroundRight] = j;
				}
				break;
				
			case e_speakerTopCenter:
				if(map[e_channelCenter]<0)
				{
					map[e_channelCenter] = j;
				}
				break;

			case e_speakerTopFrontLeft:
				if(map[e_channelFrontLeft]<0)
				{
					map[e_channelFrontLeft] = j;
				}
				break;

			case e_speakerTopFrontCenter:
				if(map[e_channelCenter]<0)
				{
					map[e_channelCenter] = j;
				}
				break;

			case e_speakerTopFrontRight:
				if(map[e_channelFrontRight]<0)
				{
					map[e_channelFrontRight] = j;
				}
				break;

			case e_speakerTopBackLeft:
				if(map[e_channelRearLeft]<0)
				{
					map[e_channelRearLeft] = j;
				}
				break;

			case e_speakerTopBackCenter:
				break;

			case e_speakerTopBackRight:
				if(map[e_channelRearRight]<0)
				{
					map[e_channelRearRight] = j;
				}
				break;

			default:
				break;
		}

		if((1 << i) & chMask)
		{
			j++;
		}
	}
	
	switch(m_noOutputChannels)
	{
		case 1:
			// Type A = 0.Center
			avMap[e_channelCenter] = 1;
			break;
			
		case 2:
			// Type B = 0.F-Left, 1.F-Right
			avMap[e_channelFrontLeft] = 1;
			avMap[e_channelFrontRight] = 1;
			break;
			
		case 3:
			// Type C = 0.F-Left, 1.F-Right, 2.Center
			avMap[e_channelCenter] = 1;
			avMap[e_channelFrontLeft] = 1;
			avMap[e_channelFrontRight] = 1;			
			break;
			
		case 4:
			// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
			avMap[e_channelFrontLeft] = 1;
			avMap[e_channelFrontRight] = 1;
			avMap[e_channelSurroundLeft] = 1;
			avMap[e_channelSurroundRight] = 1;
			break;
			
		case 5:
			// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
			avMap[e_channelCenter] = 1;
			avMap[e_channelFrontLeft] = 1;
			avMap[e_channelFrontRight] = 1;
			avMap[e_channelSurroundLeft] = 1;
			avMap[e_channelSurroundRight] = 1;
			break;
			
		case 6:
			// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
			avMap[e_channelCenter] = 1;
			avMap[e_channelFrontLeft] = 1;
			avMap[e_channelFrontRight] = 1;
			avMap[e_channelLFE] = 1;
			avMap[e_channelSurroundLeft] = 1;
			avMap[e_channelSurroundRight] = 1;
			break;
			
		case 7:
			// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
			avMap[e_channelCenter] = 1;
			avMap[e_channelFrontLeft] = 1;
			avMap[e_channelFrontRight] = 1;
			avMap[e_channelSurroundLeft] = 1;
			avMap[e_channelSurroundRight] = 1;
			avMap[e_channelRearLeft] = 1;
			avMap[e_channelRearRight] = 1;
			break;
			
		case 8:
			// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right
			avMap[e_channelCenter] = 1;
			avMap[e_channelFrontLeft] = 1;
			avMap[e_channelFrontRight] = 1;
			avMap[e_channelLFE] = 1;
			avMap[e_channelSurroundLeft] = 1;
			avMap[e_channelSurroundRight] = 1;
			avMap[e_channelRearLeft] = 1;
			avMap[e_channelRearRight] = 1;
			break;

		default:
			break;
	}
	
	downMixFlag = false;
	for(i=0;i<8 && !downMixFlag;i++)
	{
		if(map[i]>=0 && !avMap[i])
		{
			downMixFlag = true;
		}
	}
	
	if(!downMixFlag)
	{
		switch(m_noOutputChannels)
		{
			case 1:
				// Type A = 0.Center
				m_channelMap[map[e_channelCenter]] = 0;
				break;
				
			case 2:
				// Type B = 0.F-Left, 1.F-Right	
				m_channelMap[map[e_channelFrontLeft]] = 0;
				m_channelMap[map[e_channelFrontRight]] = 1;
				break;
				
			case 3:
				// Type C = 0.F-Left, 1.F-Right, 2.Center
				m_channelMap[map[e_channelFrontLeft]] = 0;
				m_channelMap[map[e_channelFrontRight]] = 1;
				m_channelMap[map[e_channelCenter]] = 2;
				break;
				
			case 4:
				// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
				m_channelMap[map[e_channelFrontLeft]] = 0;
				m_channelMap[map[e_channelFrontRight]] = 1;
				m_channelMap[map[e_channelSurroundLeft]] = 2;
				m_channelMap[map[e_channelSurroundRight]] = 3;
				break;
				
			case 5:
				// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
				m_channelMap[map[e_channelFrontLeft]] = 0;
				m_channelMap[map[e_channelFrontRight]] = 1;
				m_channelMap[map[e_channelCenter]] = 2;
				m_channelMap[map[e_channelSurroundLeft]] = 3;
				m_channelMap[map[e_channelSurroundRight]] = 4;
				break;
				
			case 6:
				// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
				m_channelMap[map[e_channelFrontLeft]] = 0;
				m_channelMap[map[e_channelFrontRight]] = 1;
				m_channelMap[map[e_channelCenter]] = 2;
				m_channelMap[map[e_channelLFE]] = 3;
				m_channelMap[map[e_channelSurroundLeft]] = 4;
				m_channelMap[map[e_channelSurroundRight]] = 5;
				break;
				
			case 7:
				// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
				m_channelMap[map[e_channelFrontLeft]] = 0;
				m_channelMap[map[e_channelFrontRight]] = 1;
				m_channelMap[map[e_channelCenter]] = 2;
				m_channelMap[map[e_channelSurroundLeft]] = 3;
				m_channelMap[map[e_channelSurroundRight]] = 4;
				m_channelMap[map[e_channelRearLeft]] = 5;
				m_channelMap[map[e_channelRearRight]] = 6;
				break;
				
			case 8:
				// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right			
				m_channelMap[map[e_channelFrontLeft]] = 0;
				m_channelMap[map[e_channelFrontRight]] = 1;
				m_channelMap[map[e_channelCenter]] = 2;
				m_channelMap[map[e_channelLFE]] = 3;
				m_channelMap[map[e_channelSurroundLeft]] = 4;
				m_channelMap[map[e_channelSurroundRight]] = 5;
				m_channelMap[map[e_channelRearLeft]] = 6;
				m_channelMap[map[e_channelRearRight]] = 7;
				break;

			default:
				break;
		}
	}
	else
	{
		if(map[e_channelSurroundLeft]>=0 || map[e_channelSurroundRight]>=0)
		{
			if(map[e_channelRearLeft]>=0 || map[e_channelRearRight]>=0)
			{
				if(map[e_channelLFE]>=0)
				{
					// Type H = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right, 6.R-Left, 7.R-Right			
					m_channelMap[map[e_channelFrontLeft]] = 0;
					m_channelMap[map[e_channelFrontRight]] = 1;
					m_channelMap[map[e_channelCenter]] = 2;
					m_channelMap[map[e_channelLFE]] = 3;
					m_channelMap[map[e_channelSurroundLeft]] = 4;
					m_channelMap[map[e_channelSurroundRight]] = 5;
					m_channelMap[map[e_channelRearLeft]] = 6;
					m_channelMap[map[e_channelRearRight]] = 7;
					m_noOutputChannels = 8;
				}
				else
				{
					// Type G = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right, 5.R-Left, 6.R-Right
					m_channelMap[map[e_channelFrontLeft]] = 0;
					m_channelMap[map[e_channelFrontRight]] = 1;
					m_channelMap[map[e_channelCenter]] = 2;
					m_channelMap[map[e_channelSurroundLeft]] = 3;
					m_channelMap[map[e_channelSurroundRight]] = 4;
					m_channelMap[map[e_channelRearLeft]] = 5;
					m_channelMap[map[e_channelRearRight]] = 6;
					m_noOutputChannels = 7;
				}
			}
			else
			{
				if(map[e_channelCenter]>=0 || map[e_channelLFE]>=0)
				{
					if(map[e_channelLFE]>=0)
					{
						// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
						m_channelMap[map[e_channelFrontLeft]] = 0;
						m_channelMap[map[e_channelFrontRight]] = 1;
						m_channelMap[map[e_channelCenter]] = 2;
						m_channelMap[map[e_channelLFE]] = 3;
						m_channelMap[map[e_channelSurroundLeft]] = 4;
						m_channelMap[map[e_channelSurroundRight]] = 5;
						m_noOutputChannels = 6;
					}
					else
					{
						// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
						m_channelMap[map[e_channelFrontLeft]] = 0;
						m_channelMap[map[e_channelFrontRight]] = 1;
						m_channelMap[map[e_channelCenter]] = 2;
						m_channelMap[map[e_channelSurroundLeft]] = 3;
						m_channelMap[map[e_channelSurroundRight]] = 4;
						m_noOutputChannels = 5;
					}
				}
				else
				{
					// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
					m_channelMap[map[e_channelFrontLeft]] = 0;
					m_channelMap[map[e_channelFrontRight]] = 1;
					m_channelMap[map[e_channelSurroundLeft]] = 2;
					m_channelMap[map[e_channelSurroundRight]] = 3;
					m_noOutputChannels = 4;
				}		
			}	
		}
		else
		{
			if(map[e_channelRearLeft]>=0 || map[e_channelRearRight]>=0)
			{
				if(map[e_channelCenter]>=0 || map[e_channelLFE]>=0)
				{
					if(map[e_channelLFE]>=0)
					{
						// Type F = 0.F-Left, 1.F-Right, 2.Center, 3.LFE, 4.S-Left, 5.S-Right
						m_channelMap[map[e_channelFrontLeft]] = 0;
						m_channelMap[map[e_channelFrontRight]] = 1;
						m_channelMap[map[e_channelCenter]] = 2;
						m_channelMap[map[e_channelLFE]] = 3;
						m_channelMap[map[e_channelRearLeft]] = 4;
						m_channelMap[map[e_channelRearRight]] = 5;
						m_noOutputChannels = 6;
					}
					else
					{
						// Type E = 0.F-Left, 1.F-Right, 2.Center, 3.S-Left, 4.S-Right
						m_channelMap[map[e_channelFrontLeft]] = 0;
						m_channelMap[map[e_channelFrontRight]] = 1;
						m_channelMap[map[e_channelCenter]] = 2;
						m_channelMap[map[e_channelRearLeft]] = 3;
						m_channelMap[map[e_channelRearRight]] = 4;
						m_noOutputChannels = 5;
					}
				}
				else
				{
						// Type D = 0.F-Left, 1.F-Right, 2.S-Left, 3.S-Right
						m_channelMap[map[e_channelFrontLeft]] = 0;
						m_channelMap[map[e_channelFrontRight]] = 1;
						m_channelMap[map[e_channelRearLeft]] = 2;
						m_channelMap[map[e_channelRearRight]] = 3;
						m_noOutputChannels = 4;
				}
			}
			else
			{
				if(map[e_channelCenter]>=0)
				{
					if(map[e_channelFrontLeft]>=0 || map[e_channelFrontRight]>=0)
					{
						// Type C = 0.F-Left, 1.F-Right, 2.Center
						m_channelMap[map[e_channelFrontLeft]] = 0;
						m_channelMap[map[e_channelFrontRight]] = 1;
						m_channelMap[map[e_channelCenter]] = 2;
						m_noOutputChannels = 3;
					}
					else
					{
						// Type A = 0.Center
						m_channelMap[map[e_channelCenter]] = 0;
						m_noOutputChannels = 1;
					}
				}
				else
				{
					// Type B = 0.F-Left, 1.F-Right	
					m_channelMap[map[e_channelFrontLeft]] = 0;
					m_channelMap[map[e_channelFrontRight]] = 1;
					m_noOutputChannels = 2;
				}
			}
		}
	}
}

//-------------------------------------------------------------------------------------------
} // namespace engine
} // namespace omega
//-------------------------------------------------------------------------------------------
