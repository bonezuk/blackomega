set ( INTEGRATION_HEADERS
	../../remote/test/integration/WinLIRCDummyServer.h
	../../remote/test/integration/QKeyLineEditTester.h
	../../engine/violetomega/test/integration/VioletPlayback.h
	../../engine/cyanomega/test/integration/CyanPlayback.h
	../../track/model/test/integration/DiscogsServiceTest.h
)

set ( INTEGRATION_SOURCE
	../../engine/test/integration/FFTRadix2Test.cxx
	../../remote/test/integration/WinLIRCDummyServer.cxx
	../../remote/test/integration/QKeyLineEditTester.cxx
	../../engine/violetomega/test/integration/VioletPlayback.cxx
	../../engine/cyanomega/test/integration/CyanPlayback.cxx
	../../engine/test/integration/WaveDSPTest.cxx
	../../track/model/test/integration/DiscogsServiceTest.cxx
	TestIntegrationMain.cxx
)

if (OMEGA_MACOSX)
	set ( INTEGRATION_MAC_HEADER
		../../audioio/test/integration/AOCoreHALInteger.h
		../../remote/test/integration/AppleRemoteServiceIntegration.h
	)
	set ( INTEGRATION_MAC_SOURCE
		../../audioio/test/integration/AOCoreHALInteger.mm
		../../remote/test/integration/AppleRemoteServiceIntegration.mm
	)
endif (OMEGA_MACOSX)

if (OMEGA_WIN32)
	set ( INTEGRATION_WIN32_SOURCE
		../../audioio/test/integration/WasAPITest.cxx
	)
endif (OMEGA_WIN32)

if (OMEGA_LINUX)
	set ( INTEGRATION_LINUX_HEADER
		../../audioio/test/integration/ALSATest.h
	)

	set ( INTEGRATION_LINUX_SOURCE
		../../audioio/test/integration/ALSATest.cxx
	)
endif (OMEGA_LINUX)

if (OMEGA_WIN32)
	add_executable(OmegaIntegrationTest ${INTEGRATION_HEADERS} ${INTEGRATION_SOURCE} ${INTEGRATION_WIN32_SOURCE} )
elseif (OMEGA_MACOSX)
	add_executable(OmegaIntegrationTest ${INTEGRATION_HEADERS} ${INTEGRATION_SOURCE} ${INTEGRATION_MAC_HEADER} ${INTEGRATION_MAC_SOURCE} )
elseif (OMEGA_LINUX)
	add_executable(OmegaIntegrationTest ${INTEGRATION_HEADERS} ${INTEGRATION_SOURCE} ${INTEGRATION_LINUX_HEADER} ${INTEGRATION_LINUX_SOURCE})
endif (OMEGA_WIN32)

if(OMEGA_MSVC16)
	target_compile_options(OmegaIntegrationTest PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
	target_link_libraries(OmegaIntegrationTest audioio blackomega blueomega common cyanomega engine greenomega http mime wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo trackmodel violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets ${QTVER}::Network ${QTVER}::Test winmm ws2_32 wavpack gtest gtest_main gmock gmock_main)
elseif (OMEGA_MACOSX)
	target_link_libraries(OmegaIntegrationTest audioio blackomega blueomega common cyanomega engine greenomega http mime wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo trackmodel violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets ${QTVER}::Network ${QTVER}::Test wavpack gtest gtest_main gmock gmock_main ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT})
elseif (OMEGA_LINUX)
	target_link_libraries(OmegaIntegrationTest audioio blackomega blueomega common cyanomega engine greenomega http mime wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo trackmodel violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets ${QTVER}::Network ${QTVER}::Test wavpack gtest gtest_main gmock gmock_main asound ${LIBUPNP_LIBRARIES} Threads::Threads )
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS OmegaIntegrationTest DESTINATION bin)
endif (${TIGER_LINUX_DISTRO})
