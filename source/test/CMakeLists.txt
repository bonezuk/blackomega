set ( TEST_HEADERS
	../audioio/test/AOChannelMapMock.h
	../audioio/test/AudioHardwareBufferTester.h
	../audioio/test/AudioItemMock.h
	../audioio/test/AOBaseTest.h
	../audioio/test/AOQueryDeviceMock.h
	../common/test/AsynchronousFileReaderMock.h
	../common/test/AsynchronousFileReaderTest.h
	../common/test/BIOStreamMock.h
	../common/test/BIOTimeCachedStreamTest.h
	../common/test/CachedFileStreamTest.h
	../common/test/CachedFileStreamMock.h
	../common/test/ServiceEventTest.h
	../common/test/DiskMockIF.h
	../common/test/XMLLibMockIF.h
	../common/test/UPnPProviderTestEnviroment.h
	../engine/test/ADataMock.h
	../engine/test/CodecMock.h
	../engine/test/CodecTest.h
	../engine/test/RDataMock.h
	../remote/test/KeyAssignmentMock.h
	../remote/test/RemoteIFMock.h
	../remote/test/WinLIRCSettingsMock.h
	../track/db/test/SQLiteMock.h
	../track/info/test/AIFFInfoTest.h
	../track/info/test/WavInfoTest.h
	../track/model/test/AbstractTrackModelTest.h
	../track/model/test/ImageRepositaryMock.h
	../track/model/test/TrackDBTestEnviroment.h
	QUnitTestTimer.h
)

set ( TEST_SOURCE
	../audioio/test/AOBaseTest.cxx
	../audioio/test/AOChannelMapMock.cxx
	../audioio/test/AOChannelMapTest.cxx
	../audioio/test/AudioHardwareBufferTester.cxx
	../audioio/test/AudioItemMock.cxx
	../audioio/test/AudioSettingsTest.cxx
	../audioio/test/FormatDescriptionTest.cxx
	../audioio/test/FormatDescriptionUtilsTest.cxx
	../audioio/test/FormatsSupportedTest.cxx
	../audioio/test/IOTimeStampTest.cxx
	../audioio/test/SampleConverterTest.cxx

	../common/test/AsynchronousFileReaderMock.cxx
	../common/test/AsynchronousFileReaderTest.cxx
	../common/test/BIOStreamTest.cxx
	../common/test/BIOStreamFunctionalTest.cxx
	../common/test/BIOBufferedStreamTest.cxx
	../common/test/BIOMemoryTest.cxx
	../common/test/BIOMemoryStreamTest.cxx
	../common/test/BIOStreamMock.cxx
	../common/test/BIOTimeCachedStreamTest.cxx
	../common/test/CachedFileStreamTest.cxx
	../common/test/CachedFileStreamMock.cxx
	../common/test/CommonTypesTest.cxx
	../common/test/CommonDirectoriesForFilesTest.cxx
	../common/test/DictionaryXMLMapTest.cxx
	../common/test/ProductVersionInfoTest.cxx
	../common/test/JaroWinklerDistanceTest.cxx
	../common/test/RandomTest.cxx
	../common/test/ServiceEventTest.cxx
	../common/test/DiskLayerTestIF.cxx
	../common/test/DiskOpsTest.cxx
	../common/test/DiskMockIF.cxx
	../common/test/UPnPProviderTestEnviroment.cxx
	../common/test/XMLLibMockIF.cxx
	../common/test/XMLNodeParserTest.cxx
	../common/test/XMLReadOpsTest.cxx

	../engine/test/ADataMock.cxx
	../engine/test/BiQuadFilterTest.cxx
	../engine/test/CodecMock.cxx
	../engine/test/CodecTest.cxx
	../engine/test/FIRFilterTest.cxx
	../engine/test/FormatTypeTest.cxx
	../engine/test/FormatType8BitTest.cxx
	../engine/test/FormatTypeFromFloatTest.cxx
	../engine/test/InterleavedCodecTest.cxx
	../engine/test/OmegaDCT64Test.cxx
	../engine/test/RDataMock.cxx
	../engine/test/RDataTest.cxx
	../engine/test/RemezFIRTest.cxx

	../engine/violetomega/test/IFFChunkTest.cxx
	../engine/violetomega/test/IFFCommonChunkTest.cxx
	../engine/violetomega/test/IFFFileTest.cxx
	../engine/violetomega/test/IFFFormChunkTest.cxx
	../engine/violetomega/test/IFFSoundChunkTest.cxx
	../engine/violetomega/test/IFFTextChunkTest.cxx
	
	../engine/wavpackomega/test/WavPackCodecTest.cxx
	../engine/wavpackomega/test/WavPackBIOReaderTest.cxx

	../remote/test/KeyAssignmentMock.cxx
	../remote/test/KeyAssignmentTest.cxx
	../remote/test/KeyCodeExclusionTest.cxx
	../remote/test/KeyCodesContainerTest.cxx
	../remote/test/KeyCodeStateEngineTest.cxx
	../remote/test/KeyControlServiceTest.cxx
	../remote/test/KeyStateParserTest.cxx
	../remote/test/WinLIRCClientTest.cxx
	../remote/test/WinLIRCRemoteTest.cxx
	../remote/test/WinLIRCSettingsMock.cxx

	../network/test/ResourceTest.cxx

	../track/db/test/ITunesLocationTest.cxx
	../track/db/test/SQLiteMock.cxx
	../track/db/test/TrackFileDependenciesTest.cxx
	
	../track/info/test/AIFFInfoTest.cxx
	../track/info/test/APEInfoTest.cxx
	../track/info/test/APETagTest.cxx
	../track/info/test/MPCChapterTagTest.cxx
	../track/info/test/CueInfoTest.cxx
	../track/info/test/InfoTest.cxx
	../track/info/test/WavInfoTest.cxx
	../track/info/test/XMCDInfoTest.cxx
	
	../track/model/test/TrackDBTestEnviroment.cxx
	../track/model/test/FileNameParserTest.cxx
	
	QUnitTestTimer.cxx
	TestMain.cxx
)

if (OMEGA_WIN32)
	set ( TEST_WIN_HEADERS
		../audioio/test/ASIODataMock.h
		../audioio/test/ASIODriverMock.h
		../audioio/test/WasAPICOMInterfacesMock.h
		../audioio/test/WasAPIIFMock.h
	)

	set ( TEST_WIN_SOURCE
		../audioio/test/ASIODataMock.cxx
		../audioio/test/AOWin32Test.cxx
		../audioio/test/ASIODriverMock.cxx
		../audioio/test/ASIODataTest.cxx
		../audioio/test/AOQueryASIOTest.cxx
		../audioio/test/AOQueryWasAPITest.cxx
		../audioio/test/AudioHardwareBufferWasAPITest.cxx
		../audioio/test/WasAPICOMInterfacesMock.cxx		
		../audioio/test/WasAPIIFMock.cxx
		../audioio/test/WasAPILayerIFTest.cxx
	)
endif (OMEGA_WIN32)

if (OMEGA_MACOSX)
	set ( TEST_MAC_HEADERS
		../audioio/test/CoreAudioMockIF.h
	)
	set ( TEST_OBJECTIVE_SOURCE
		../audioio/test/AOCoreAudioTest.cxx
		../audioio/test/AOQueryCoreAudioTest.cxx
		../audioio/test/CoreAudioMockIF.cxx
	)
endif (OMEGA_MACOSX)

if (OMEGA_LINUX)
	set ( TEST_LINUX_HEADERS
		../audioio/test/AOQueryALSAMock.h
		../audioio/test/LinuxALSAMockIF.h
	)
	set ( TEST_LINUX_SOURCE
		../audioio/test/AOLinuxALSATest.cxx
		../audioio/test/AOQueryALSATest.cxx
		../audioio/test/AudioHardwareBufferALSATest.cxx
		../audioio/test/LinuxALSAMockIF.cxx
	)
endif (OMEGA_LINUX)

qt_add_resources(TEST_UI_RESOURCES test.qrc)

if (OMEGA_WIN32)
	add_executable(OmegaUnitTests ${TEST_HEADERS} ${TEST_SOURCE} ${TEST_UI_RESOURCES} ${TEST_WIN_HEADERS} ${TEST_WIN_SOURCE})
elseif (OMEGA_MACOSX)
	add_executable(OmegaUnitTests ${TEST_HEADERS} ${TEST_SOURCE} ${TEST_MAC_HEADERS} ${TEST_OBJECTIVE_SOURCE} ${TEST_UI_RESOURCES})
elseif(OMEGA_LINUX)
	add_executable(OmegaUnitTests ${TEST_HEADERS} ${TEST_SOURCE} ${TEST_UI_RESOURCES} ${TEST_LINUX_HEADERS} ${TEST_LINUX_SOURCE})
endif (OMEGA_WIN32)

if(MSVC)
	target_compile_options(OmegaUnitTests PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(OmegaUnitTests audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo trackmodel violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets ${QTVER}::Test winmm ws2_32 wavpack gtest gtest_main gmock gmock_main)
elseif (OMEGA_MACOSX)
	target_link_libraries(OmegaUnitTests audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo trackmodel violetomega whiteomega widget playercommon xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets ${QTVER}::Test wavpack gtest gtest_main gmock gmock_main ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT} ${CORESERVICES_FRAMEWORK_UNIT} ${COREAUDIO_FRAMEWORK_UNIT} ${AUDIOUNIT_FRAMEWORK_UNIT} ${AUDIOTOOLBOX_FRAMEWORK_UNIT})
elseif (OMEGA_LINUX)
	target_link_libraries(OmegaUnitTests audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo trackmodel violetomega whiteomega widget playercommon xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets ${QTVER}::Test wavpack mpcdec gtest gtest_main gmock gmock_main ${LIBUPNP_LIBRARIES} Threads::Threads)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS OmegaUnitTests DESTINATION bin)
endif (${TIGER_LINUX_DISTRO})
