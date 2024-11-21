file(GLOB PLAYERAPP_HEADER inc/*.h)
file(GLOB PLAYERAPP_SOURCE src/*.cxx)

add_executable(OmegaPlayerApp ${PLAYERAPP_HEADER} ${PLAYERAPP_SOURCE} )

if(MSVC)
	target_compile_options(OmegaPlayerApp PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(OmegaPlayerApp audioio blackomega blueomega common cyanomega engine greenomega toneomega wavpackomega network_omega redomega mime silveromega trackdb trackinfo violetomega whiteomega playercommon remote widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Quick ${QTVER}::Qml winmm ws2_32 wavpack)
elseif (OMEGA_MACOSX)
	target_link_libraries(OmegaPlayerApp audioio blackomega blueomega common cyanomega engine greenomega toneomega wavpackomega network_omega redomega mime silveromega trackdb trackinfo violetomega whiteomega playercommon remote widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Quick ${QTVER}::Qml wavpack ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT} ${SPARKLE_FRAMEWORK})
elseif (OMEGA_LINUX)
	target_link_libraries(OmegaPlayerApp audioio blackomega blueomega common cyanomega engine greenomega toneomega wavpackomega network_omega redomega mime silveromega trackdb trackinfo violetomega whiteomega playercommon remote widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Quick ${QTVER}::Qml wavpack ${LIBUPNP_LIBRARIES} )
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS OmegaPlayerApp DESTINATION bin)
endif (${TIGER_LINUX_DISTRO})
