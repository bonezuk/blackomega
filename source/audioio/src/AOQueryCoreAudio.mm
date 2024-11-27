#include "audioio/inc/AOQueryCoreAudio.h"
#include "common/inc/Log.h"

#import <CoreFoundation/CoreFoundation.h>
#import <Cocoa/Cocoa.h>

//-------------------------------------------------------------------------------------------
namespace omega
{
namespace common
{
//-------------------------------------------------------------------------------------------

QString qStringFromCFStringRef(CFStringRef cfStr)
{
	tint i;
	tint len = static_cast<tint>(CFStringGetLength(cfStr));
    char16_t *cName = new char16_t [len + 1];
	QString qStr;
	
	for(i=0;i<len;i++)
	{
        cName[i] = static_cast<char16_t>(CFStringGetCharacterAtIndex(cfStr,i));
	}
	cName[len] = 0;
    qStr = QString::fromUtf16(cName,len);
	delete [] cName;
	return qStr;
}

//-------------------------------------------------------------------------------------------
} // namespace common
} // namespace omega
//-------------------------------------------------------------------------------------------
namespace omega
{
namespace audioio
{
//-------------------------------------------------------------------------------------------

AOQueryCoreAudio::DeviceCoreAudio::DeviceCoreAudio() : AOQueryDevice::Device(),
	m_deviceID(kAudioDeviceUnknown),
	m_isIntegerMode(false)
{
	m_type = AOQueryDevice::Device::e_deviceCoreAudio;
}

//-------------------------------------------------------------------------------------------

AOQueryCoreAudio::DeviceCoreAudio::DeviceCoreAudio(const AOQueryDevice::Device& rhs) : AOQueryDevice::Device(),
	m_deviceID(kAudioDeviceUnknown),
	m_isIntegerMode(false)
{
	DeviceCoreAudio::copy(rhs);
}

//-------------------------------------------------------------------------------------------

AOQueryCoreAudio::DeviceCoreAudio::~DeviceCoreAudio()
{}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudio::printError(const tchar *strR,const tchar *strE,OSStatus err) const
{
	NSError *error = [NSError errorWithDomain:NSOSStatusErrorDomain code:err userInfo:nil];
	QString desc = QString::fromUtf8(error.localizedDescription.UTF8String);
	QString reason = QString::fromUtf8(error.localizedFailureReason.UTF8String);
	QString recovery = QString::fromUtf8(error.localizedRecoverySuggestion.UTF8String);
	QString eStr;
	eStr  = strE;
	eStr += common::c_endl;
	eStr += "\tDescription - " + desc;
	eStr += common::c_endl;
	eStr += "\tReason - " + reason;
	eStr += common::c_endl;
	eStr += "\tSuggestion - " + recovery + common::c_endl;
	common::Log::g_Log << "AOQueryCoreAudio::" << strR << " - " << eStr.toUtf8().constData();
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudio::DeviceCoreAudio::copy(const AOQueryDevice::Device& rhs)
{
	try
	{
		const DeviceCoreAudio& rA = dynamic_cast<const DeviceCoreAudio&>(rhs);
		m_deviceID = rA.m_deviceID;
		m_isIntegerMode = rA.m_isIntegerMode;
	}
	catch(...) {}
	AOQueryDevice::Device::copy(rhs);
}

//-------------------------------------------------------------------------------------------

AudioDeviceID AOQueryCoreAudio::DeviceCoreAudio::deviceID() const
{
	return m_deviceID;
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudio::DeviceCoreAudio::setDeviceID(AudioDeviceID devID)
{
	m_deviceID = devID;
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudio::DeviceCoreAudio::isIntegerMode() const
{
	return m_isIntegerMode;
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudio::DeviceCoreAudio::setIntegerMode(bool isInt)
{
	m_isIntegerMode = isInt;
}

//-------------------------------------------------------------------------------------------

void AOQueryCoreAudio::DeviceCoreAudio::print() const
{
	common::Log::g_Log.print("Device ID : %d\n",m_deviceID);
	AOQueryDevice::Device::print();
}

//-------------------------------------------------------------------------------------------
// AOQueryCoreAudio
//-------------------------------------------------------------------------------------------

AOQueryCoreAudio::AOQueryCoreAudio() : AOQueryDevice()
{}

//-------------------------------------------------------------------------------------------

AOQueryCoreAudio::~AOQueryCoreAudio()
{}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudio::queryNames()
{
	int i;
	UInt32 devIdSize;
	AudioObjectPropertyAddress propAddr;
	OSStatus err;
	bool res = false;
	
	propAddr.mSelector = kAudioHardwarePropertyDevices;
	propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = kAudioObjectPropertyElementMain;
	devIdSize = 0;
	
	err = AudioObjectGetPropertyDataSize(kAudioObjectSystemObject,&propAddr,0,0,&devIdSize);
	if(err==noErr)
	{
		devIdSize /= sizeof(AudioDeviceID);
		if(devIdSize>=1)
		{
			AudioDeviceID *devIDArray = new AudioDeviceID [devIdSize];
			
			devIdSize *= sizeof(AudioDeviceID);
			err = AudioObjectGetPropertyData(kAudioObjectSystemObject,&propAddr,0,0,&devIdSize,devIDArray);
			if(err==noErr)
			{
				devIdSize /= sizeof(AudioDeviceID);
				
				for(i=0;i<devIdSize;i++)
				{
					AudioDeviceID devID;
					AudioComponent comp;
					AudioComponentDescription desc;
					AudioComponentInstance outputUnit;
					UInt32 devNameSize = sizeof(CFStringRef);
					CFStringRef devNameRef;
					DeviceCoreAudio *dev = new DeviceCoreAudio;

					desc.componentType = kAudioUnitType_Output;
					desc.componentSubType = kAudioUnitSubType_HALOutput;
					desc.componentManufacturer = kAudioUnitManufacturer_Apple;
					desc.componentFlags = 0;
					desc.componentFlagsMask = 0;
					
					devID = devIDArray[i];
					
					comp = AudioComponentFindNext(0,&desc);
					if(comp!=0)
					{
						err = AudioComponentInstanceNew(comp,&outputUnit);
						if(err==noErr)
						{
							err = AudioUnitSetProperty(outputUnit,kAudioOutputUnitProperty_CurrentDevice,kAudioUnitScope_Global,0,&devID,sizeof(devID));
							if(err==noErr)
							{					
								propAddr.mSelector = kAudioObjectPropertyName;
								propAddr.mScope = kAudioDevicePropertyScopeOutput;
                                propAddr.mElement = kAudioObjectPropertyElementMain;

								err = AudioObjectGetPropertyData(devIDArray[i],&propAddr,0,0,&devNameSize,&devNameRef);
								if(err==noErr)
								{
									UInt32 devUIDSize = sizeof(CFStringRef);
									CFStringRef devUID;
						
									dev->name() = common::qStringFromCFStringRef(devNameRef);
									dev->setDeviceID(devIDArray[i]);
						
									propAddr.mSelector = kAudioDevicePropertyDeviceUID;
									propAddr.mScope = kAudioDevicePropertyScopeOutput;
                                    propAddr.mElement = kAudioObjectPropertyElementMain;
						
									err = AudioObjectGetPropertyData(devIDArray[i],&propAddr,0,0,&devUIDSize,&devUID);
									if(err==noErr)
									{
										dev->id() = common::qStringFromCFStringRef(devUID);
										CFRelease(devUID);
										
										fprintf(stdout,"Audio Device - %s\n",dev->id().toUtf8().constData());
										
										m_devices.append(dev);
										res = true;
									}
									else
									{
										printError("queryNames","Failed to get unique ID of audio device",err);
										delete dev;
									}
									CFRelease(devNameRef);
								}
								else
								{
									printError("queryNames","Failed to get name of audio device",err);
									delete dev;
								}
							}
							else
							{
								printError("queryNames","Failed to associate output unit instance with device",err);
								delete dev;
							}
							AudioComponentInstanceDispose(outputUnit);
						}
						else
						{
							printError("queryNames","Error creating instance of output unit",err);
							delete dev;
						}
					}
					else
					{
						delete dev;
					}
				}
			}
			else
			{
				printError("queryNames","Error getting audio hardware device list",err);
			}
			delete [] devIDArray;
		}
	}
	else
	{
		printError("queryNames","Error getting hardware device list size",err);
	}
	return res;
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudio::queryDevice(int idx)
{
	if(idx>=0 && idx<m_devices.size())
	{
		DeviceCoreAudio& dev = dynamic_cast<DeviceCoreAudio&>(*(m_devices[idx]));
		return queryDevice(dev.deviceID(),dev);
	}
	return false;
}

//-------------------------------------------------------------------------------------------

struct channelLabelToLetter
{
	AudioChannelLabel label;
	const char *letter;
} typedef ChannelLabelToLetter;

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudio::queryDevice(AudioDeviceID devID,Device& dev)
{
	static const double rates[18] = {
		768000.0, 705600.0, 384000.0, 352800.0,
		192000.0, 176400.0, 96000.0,
		 88200.0,  64000.0, 48000.0,
		 44100.0,  32000.0, 24000.0,
		 22050.0,  16000.0, 12000.0,
		 11025.0,   8000.0
	};
	
	int i,j;
	UInt32 sampleRateSize,propSize;
	bool rFlag = false,cFlag;
	AudioObjectPropertyAddress propAddr;
	OSStatus err;
	
	propAddr.mSelector = kAudioDevicePropertyAvailableNominalSampleRates;
	propAddr.mScope = kAudioDevicePropertyScopeOutput;
    propAddr.mElement = kAudioObjectPropertyElementMain;
	propSize = 0;

	err = AudioObjectGetPropertyDataSize(devID,&propAddr,0,0,&propSize);
	if(err==noErr)
	{
		sampleRateSize = propSize / static_cast<UInt32>(sizeof(struct AudioValueRange));
		if(sampleRateSize>=1)
		{
			struct AudioValueRange *sampleRateArray = reinterpret_cast<struct AudioValueRange *>(::malloc(sampleRateSize * sizeof(struct AudioValueRange)));
			if(sampleRateArray!=0)
			{
				sampleRateSize *= sizeof(struct AudioValueRange);
				
				err = AudioObjectGetPropertyData(devID,&propAddr,0,0,&sampleRateSize,sampleRateArray);
				if(err==noErr)
				{
					sampleRateSize /= sizeof(struct AudioValueRange);
					
					for(i=0;i<sampleRateSize;i++)
					{
						double min = sampleRateArray[i].mMinimum;
						double max = sampleRateArray[i].mMaximum;
						
						for(j=0;j<18;j++)
						{
							if(min>=rates[j] && rates[j]<=max)
							{
								dev.addFrequency(static_cast<int>(rates[j]));
								rFlag = true;
								break;
							}
						}
					}
					
				}
				else
				{
					printError("queryDevice","Failed to get list of available sample rates for device",err);
				}
				::free(sampleRateArray);
			}
		}
	}
	else
	{
		printError("queryDevice","Error getting size of available sample rate list",err);
	}
	
	cFlag = setupChannelLayout(devID,dev);
	
	if(rFlag && cFlag)
	{
		dev.setInitialized();
		return true;
	}
	else
	{
		return false;
	}
}

//-------------------------------------------------------------------------------------------

bool AOQueryCoreAudio::setupChannelLayout(AudioDeviceID devID,Device& dev)
{
	// Channel names based MS naming - http://msdn.microsoft.com/en-us/library/windows/hardware/dn653308(v=vs.85).aspx
	static const ChannelLabelToLetter channelLabelToLetterArray[] = {
		{ kAudioChannelLabel_Left, "FL"},
		{ kAudioChannelLabel_Right, "FR"},
		{ kAudioChannelLabel_Center, "FC"},
		{ kAudioChannelLabel_LFEScreen, "LF"},
		{ kAudioChannelLabel_LeftSurround, "BL"},
		{ kAudioChannelLabel_RightSurround, "BR"},
		{ kAudioChannelLabel_LeftCenter, "SL"},
		{ kAudioChannelLabel_RightCenter, "SR"},
		{ kAudioChannelLabel_CenterSurround, "Cs"},
		{ kAudioChannelLabel_Mono, "Mono"},
		{ kAudioChannelLabel_Unused, "-"},
		{ kAudioChannelLabel_Discrete_0, "D0"},           
		{ kAudioChannelLabel_Discrete_1, "D1"},             
		{ kAudioChannelLabel_Discrete_2, "D2"},            
		{ kAudioChannelLabel_Discrete_3, "D3"},          
		{ kAudioChannelLabel_Discrete_4, "D4"},         
		{ kAudioChannelLabel_Discrete_5, "D5"},      
		{ kAudioChannelLabel_Discrete_6, "D6"},    
		{ kAudioChannelLabel_Discrete_7, "D7"},   
		{ kAudioChannelLabel_Discrete_8, "D8"}, 
		{ kAudioChannelLabel_Discrete_9, "D9"},
		{ kAudioChannelLabel_Discrete_10, "D10" },
		{ kAudioChannelLabel_Discrete_11, "D11" },
		{ kAudioChannelLabel_Discrete_12, "D12" },  
		{ kAudioChannelLabel_Discrete_13, "D13" },
		{ kAudioChannelLabel_Discrete_14, "D14" },
		{ kAudioChannelLabel_Discrete_15, "D15"},
		{ kAudioChannelLabel_Discrete_0 | 16, "D16"},
		{ kAudioChannelLabel_Discrete_0 | 17, "D17"},
		{ kAudioChannelLabel_Discrete_0 | 18, "D18"},
		{ kAudioChannelLabel_Discrete_0 | 19, "D19"},
		{ kAudioChannelLabel_Discrete_0 | 20, "D20"},
		{ kAudioChannelLabel_Discrete_0 | 21, "D21"},
		{ kAudioChannelLabel_Discrete_0 | 22, "D22"},
		{ kAudioChannelLabel_Discrete_0 | 23, "D23"},
		{ kAudioChannelLabel_Discrete_0 | 24, "D24"},
		{ kAudioChannelLabel_Discrete_0 | 25, "D25"},
		{ kAudioChannelLabel_Discrete_0 | 26, "D26"},
		{ kAudioChannelLabel_Discrete_0 | 27, "D27"},
		{ kAudioChannelLabel_Discrete_0 | 28, "D28"},
		{ kAudioChannelLabel_Discrete_0 | 29, "D29"},
		{ kAudioChannelLabel_Discrete_0 | 30, "D30"},
		{ kAudioChannelLabel_Discrete_0 | 31, "D31"},
		{ kAudioChannelLabel_Discrete_0 | 32, "D32"},
	};
	
	int i,j,k;
	AudioComponent comp;
	AudioComponentDescription desc;
	OSStatus err;
	bool res = false;
	
	desc.componentType = kAudioUnitType_Output;
	desc.componentSubType = kAudioUnitSubType_HALOutput;
	desc.componentManufacturer = kAudioUnitManufacturer_Apple;
	desc.componentFlags = 0;
	desc.componentFlagsMask = 0;
	
	comp = AudioComponentFindNext(0,&desc);
	if(comp!=0)
	{
		UInt32 propSize;
		AudioChannelLayout *ioLayout;
		AudioComponentInstance outputUnit;
		
		err = AudioComponentInstanceNew(comp,&outputUnit);
		if(err==noErr)
		{
			err = AudioUnitSetProperty(outputUnit,kAudioOutputUnitProperty_CurrentDevice,kAudioUnitScope_Global,0,&devID,sizeof(devID));
			if(err==noErr)
			{
				AudioObjectPropertyAddress propAddr;

				propAddr.mSelector = kAudioDevicePropertyStreamConfiguration;
				propAddr.mScope = kAudioDevicePropertyScopeOutput;
                propAddr.mElement = kAudioObjectPropertyElementMain;
				propSize = 0;
				
				err = AudioObjectGetPropertyDataSize(devID,&propAddr,0,0,&propSize);
				if(err==noErr)
				{
					AudioBufferList *bufferList = reinterpret_cast<AudioBufferList *>(::malloc(propSize));
					if(bufferList!=0)
					{
						err = AudioObjectGetPropertyData(devID,&propAddr,0,0,&propSize,bufferList);
						if(err==noErr)
						{
							int noChs = 0;
	
							for(i=0;i<bufferList->mNumberBuffers;i++)
							{
								noChs += bufferList->mBuffers[i].mNumberChannels;
							}
							dev.setNoChannels(noChs);
							dev.loadChannelMap();
							res = true;

							propAddr.mSelector = kAudioDevicePropertyPreferredChannelLayout;
							propAddr.mScope = kAudioDevicePropertyScopeOutput;
                            propAddr.mElement = kAudioObjectPropertyElementMain;
							propSize = 0;
							
/* Decremented but leave code in place as may use later 
							err = AudioObjectGetPropertyDataSize(devID,&propAddr,0,0,&propSize);
							if(err==noErr)
							{
								ioLayout = reinterpret_cast<AudioChannelLayout *>(::malloc(propSize));
								if(ioLayout!=0)
								{
									err = AudioObjectGetPropertyData(devID,&propAddr,0,0,&propSize,ioLayout);
									if(err==noErr)
									{
										AudioChannelLayout *chLayout = expandChannelLayout(ioLayout,propSize);
										if(chLayout!=0)
										{
											for(i=0,k=0;i<bufferList->mNumberBuffers && k<chLayout->mNumberChannelDescriptions;i++)
											{
												for(j=0;j<bufferList->mBuffers[i].mNumberChannels && k<chLayout->mNumberChannelDescriptions;j++,k++)
												{
													AudioChannelLabel chLabel = chLayout->mChannelDescriptions[k].mChannelLabel;
													if(chLabel!=kAudioChannelLabel_Unknown)
													{
														for(int l=0;l<(sizeof(channelLabelToLetterArray) / sizeof(ChannelLabelToLetter));l++)
														{
															if(chLabel == channelLabelToLetterArray[l].label)
															{
																dev.channel(k).name() = channelLabelToLetterArray[l].letter;
															}
														}
													}
												}
											}
											::free(chLayout);
										}										
									}
									else
									{
										printError("setupChannelLayout","Error getting preferred channel layout",err);
									}
									::free(ioLayout);
								}
							}
							else
							{
								printError("setupChannelLayout","Error getting channel layout data size",err);
							}
*/
						}
						else
						{
							printError("setupChannelLayout","Failed to get stream configuration for device",err);
						}
						::free(bufferList);
					}
				}
				else
				{
					printError("setupChannelLayout","Error getting size of stream configuration for device",err);
				}
			}
			else
			{
				printError("setupChannelLayout","Failed to associate output unit instance with device",err);
			}
			AudioComponentInstanceDispose(outputUnit);
		}
		else
		{
			printError("setupChannelLayout","Error creating instance of output unit",err);
		}
	}
	return res;
}

//-------------------------------------------------------------------------------------------

/*
AudioChannelLayout *AOQueryCoreAudio::expandChannelLayout(AudioChannelLayout *ioLayout,UInt32 ioLayoutSize)
{
	OSStatus err;
	AudioChannelLayout *destLayout = 0;
	bool res = false;
	
	if(ioLayout!=0)
	{
		switch(ioLayout->mChannelLayoutTag)
		{
			case kAudioChannelLayoutTag_UseChannelDescriptions:
				{
					destLayout = reinterpret_cast<AudioChannelLayout *>(::malloc(ioLayoutSize));
					if(destLayout!=0)
					{
						::memcpy(destLayout,ioLayout,ioLayoutSize);
						res = true;
					}
				}
				break;
				
			case kAudioChannelLayoutTag_UseChannelBitmap:
				{
					UInt32 size;
					
					err = AudioFormatGetPropertyInfo(kAudioFormatProperty_ChannelLayoutForBitmap,sizeof(UInt32),&ioLayout->mChannelBitmap,&size);
					if(err==noErr)
					{
						destLayout = reinterpret_cast<AudioChannelLayout *>(::malloc(size));
						if(destLayout!=0)
						{
							err = AudioFormatGetProperty(kAudioFormatProperty_ChannelLayoutForBitmap,sizeof(UInt32),&ioLayout->mChannelBitmap,&size,destLayout);
							if(err==noErr)
							{
								res = true;
							}
							else
							{
								printError("expandChannelLayout","Error getting channel bitmap layout",err);
							}
						}
					}
				}
				break;
				
			case kAudioChannelLayoutTag_Mono:
				destLayout = reinterpret_cast<AudioChannelLayout *>(::malloc(sizeof(AudioChannelLayout)));
				if(destLayout!=0)
				{
					destLayout->mChannelLayoutTag = kAudioChannelLayoutTag_UseChannelDescriptions;
					destLayout->mNumberChannelDescriptions = 1;
					destLayout->mChannelDescriptions[0].mChannelLabel = kAudioChannelLabel_Mono;
				}
				break;
				
			default:
				{
					UInt32 size;
					
					err = AudioFormatGetPropertyInfo(kAudioFormatProperty_ChannelLayoutForTag,sizeof(AudioChannelLayoutTag),&ioLayout->mChannelLayoutTag,&size);
					if(err==noErr)
					{
						destLayout = reinterpret_cast<AudioChannelLayout *>(::malloc(size));
						if(destLayout!=0)
						{
							err = AudioFormatGetProperty(kAudioFormatProperty_ChannelLayoutForTag,sizeof(AudioChannelLayoutTag),&ioLayout->mChannelLayoutTag,&size,destLayout);
							if(err==noErr)
							{
								destLayout->mChannelLayoutTag = kAudioChannelLayoutTag_UseChannelDescriptions;
								res = true;
							}
							else
							{
								printError("expandChannelLayout","Error getting channel tagged information",err);
							}
						}
					}
				}
				break;
		}
	}
	if(!res && destLayout!=0)
	{
		::free(destLayout);
		destLayout = 0;
	}
	return destLayout;
}
*/

//-------------------------------------------------------------------------------------------

int AOQueryCoreAudio::defaultDeviceIndex()
{
	int i,defID = -1;
	UInt32 propSize;
	AudioObjectPropertyAddress propAddr;
	AudioDeviceID devId;
	OSStatus err;
	
	propAddr.mSelector = kAudioHardwarePropertyDefaultOutputDevice;
	propAddr.mScope = kAudioObjectPropertyScopeGlobal;
    propAddr.mElement = kAudioObjectPropertyElementMain;
	propSize = sizeof(AudioDeviceID);
	
	err = AudioObjectGetPropertyData(kAudioObjectSystemObject,&propAddr,0,0,&propSize,&devId);
	if(err==noErr)
	{
		for(i=0;i<m_devices.size() && defID<0;i++)
		{
			DeviceCoreAudio *dev = dynamic_cast<DeviceCoreAudio *>(m_devices[i]);
			if(dev!=0)
			{
				if(devId==dev->deviceID())
				{
					defID = i;
				}
			}
		}
	}
	else
	{
		printError("defaultDeviceIndex","Failed to get default audio device",err);
	}
	return defID;
}

//-------------------------------------------------------------------------------------------
} // namespace audioio
} // namespace omega
//-------------------------------------------------------------------------------------------
