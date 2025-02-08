add_definitions(-DOMEGA_AUDIOIO_DLL)

if (OMEGA_WIN32)

	set(AUDIOIO_HEADER 
		inc/AbstractAudioHardwareBuffer.h
		inc/AOBase.h
		inc/AOQueryASIO.h
		inc/AOQueryDevice.h
		inc/AOQuerySharedDevice.h
		inc/AOQueryWasAPI.h
		inc/AOQueryWin32.h
		inc/AOWin32.h
		inc/ASIO.h
		inc/ASIOData.h
		inc/ASIODriver.h
		inc/ASIOSys.h
		inc/AudioHardwareBufferASIO.h
		inc/AudioHardwareBufferWasAPI.h
		inc/AudioInfo.h
		inc/AudioIODLL.h
		inc/AudioSettings.h
		inc/FormatDescription.h
		inc/FormatDescriptionUtils.h
		inc/FormatsSupported.h
		inc/IOTimeStamp.h
		inc/SampleConverter.h
		inc/WasAPICOMInterfaces.h
		inc/WasAPIIF.h
		inc/WasAPILayerIF.h
		inc/WasAPIVolumeEvents.h
	)

	set(AUDIOIO_SOURCE 
		src/AbstractAudioHardwareBuffer.cxx
		src/AOChannelMap.cxx
		src/AOBase.cxx
		src/AOQueryASIO.cxx
		src/AOQueryDevice.cxx
		src/AOQuerySharedDevice.cxx
		src/AOQueryWasAPI.cxx
		src/AOQueryWin32.cxx
		src/AOWin32.cxx
		src/ASIOData.cxx
		src/ASIODriver.cxx
		src/AudioHardwareBufferASIO.cxx
		src/AudioHardwareBufferWasAPI.cxx
		src/AudioInfo.cxx
		src/AudioSettings.cxx
		src/FormatDescription.cxx
		src/FormatDescriptionUtils.cxx
		src/FormatsSupported.cxx
		src/IOTimeStamp.cxx
		src/SampleConverter.cxx
		src/WasAPICOMInterfaces.cxx
		src/WasAPIIF.cxx
		src/WasAPILayerIF.cxx
		src/WasAPIVolumeEvents.cxx
	)

elseif (OMEGA_MACOSX)

	set(AUDIOIO_HEADER 
		inc/AbstractAudioHardwareBuffer.h
		inc/AOBase.h
		inc/AOCoreAudio.h
		inc/AOCoreAudioMacOS.h
		inc/AudioInfo.h
		inc/AudioIODLL.h
		inc/AOQueryDevice.h
		inc/AOQueryCoreAudio.h
		inc/AOQuerySharedDevice.h
		inc/AudioHardwareBufferCoreAudio.h
		inc/AudioSettings.h
		inc/CoreAudioIF.h
		inc/CoreAudioLayerIF.h
		inc/FormatDescription.h
		inc/FormatDescriptionUtils.h
		inc/FormatsSupported.h
		inc/IOTimeStamp.h
		inc/SampleConverter.h
	)

	set(AUDIOIO_SOURCE
		src/AbstractAudioHardwareBuffer.cxx
		src/AOBase.cxx
		src/AOChannelMap.cxx
		src/AOCoreAudio.mm
		src/AOCoreAudioMacOS.mm
		src/AudioHardwareBufferCoreAudio.cxx
		src/AudioInfo.cxx
		src/AudioSettings.cxx
		src/AOQueryDevice.cxx
		src/AOQueryCoreAudio.mm
		src/AOQuerySharedDevice.cxx
		src/CoreAudioIF.cxx
		src/CoreAudioLayerIF.cxx
		src/FormatDescription.cxx
		src/FormatDescriptionUtils.cxx
		src/FormatsSupported.cxx
		src/IOTimeStamp.cxx
		src/SampleConverter.cxx
	)

elseif (OMEGA_LINUX)

	set(AUDIOIO_HEADER 
		inc/AbstractAudioHardwareBuffer.h
		inc/AOBase.h
		inc/AudioInfo.h
		inc/AudioIODLL.h
		inc/AOLinuxALSA.h
		inc/AOQueryALSA.h
		inc/AOQueryDevice.h
		inc/AudioHardwareBufferALSA.h
		inc/AudioSettings.h
		inc/FormatDescription.h
		inc/FormatDescriptionUtils.h
		inc/FormatsSupported.h
		inc/IOTimeStamp.h
		inc/LinuxALSAIF.h
		inc/LinuxALSALayerIF.h
		inc/SampleConverter.h
	)

	set(AUDIOIO_SOURCE
		src/AbstractAudioHardwareBuffer.cxx
		src/AOBase.cxx
		src/AOChannelMap.cxx
		src/AudioInfo.cxx
		src/AOLinuxALSA.cxx
		src/AOQueryDevice.cxx
		src/AOQueryALSA.cxx
		src/AudioHardwareBufferALSA.cxx
		src/AudioSettings.cxx
		src/FormatDescription.cxx
		src/FormatDescriptionUtils.cxx
		src/FormatsSupported.cxx
		src/IOTimeStamp.cxx
		src/LinuxALSAIF.cxx
		src/LinuxALSALayerIF.cxx
		src/SampleConverter.cxx
	)

endif (OMEGA_WIN32)

add_library(audioio SHARED ${AUDIOIO_HEADER} ${AUDIOIO_SOURCE})

if(OMEGA_MSVC16)
	target_compile_options(audioio PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
	target_link_libraries(audioio engine common network_omega mime dsdomega xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm avrt)
elseif (OMEGA_MACOSX)
	target_link_libraries(audioio engine common network_omega mime dsdomega xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml 
		${CORESERVICES_FRAMEWORK_UNIT}
		${COREAUDIO_FRAMEWORK_UNIT}
		${AUDIOUNIT_FRAMEWORK_UNIT}
		${AUDIOTOOLBOX_FRAMEWORK_UNIT}
	)
elseif (OMEGA_LINUX)
	target_link_libraries(audioio engine common network_omega mime dsdomega xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml asound)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS audioio DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
