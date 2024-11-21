add_definitions(-DOMEGA_PLAYERCOMMON_DLL)

file(GLOB PLAYERCOMMON_HEADER inc/*.h)
file(GLOB PLAYERCOMMON_SOURCE src/*.cxx)

qt_add_resources(PLAYERCOMMON_RESOURCES playercommon.qrc)

add_library(playercommon SHARED ${PLAYERCOMMON_HEADER} ${PLAYERCOMMON_SOURCE} ${PLAYERCOMMON_RESOURCES})

if(MSVC)
	target_compile_options(playercommon PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(playercommon engine common blackomega blueomega cyanomega greenomega redomega silveromega violetomega wavpackomega whiteomega network_omega trackdb trackinfo trackmodel network_omega http xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Quick ${QTVER}::Qml winmm ws2_32 wavpack)
elseif (OMEGA_MACOSX)
	target_link_libraries(playercommon engine common blackomega blueomega cyanomega greenomega redomega silveromega violetomega wavpackomega whiteomega network_omega trackdb trackinfo trackmodel network_omega http xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Quick ${QTVER}::Qml wavpack ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT} ${SPARKLE_FRAMEWORK})
elseif (OMEGA_LINUX)
	target_link_libraries(playercommon engine common blackomega blueomega cyanomega greenomega redomega silveromega violetomega wavpackomega whiteomega network_omega trackdb trackinfo trackmodel network_omega http xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Quick ${QTVER}::Qml wavpack ${LIBUPNP_LIBRARIES} )
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS playercommon DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})

