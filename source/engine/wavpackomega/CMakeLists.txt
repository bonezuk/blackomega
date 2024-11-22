add_definitions(-DOMEGA_ENGINE_WAVPACKOMEGA_DLL)

file(GLOB WAVPACKOMEGA_ENGINE_HEADER inc/*.h)
file(GLOB WAVPACKOMEGA_ENGINE_SOURCE src/*.cxx)

add_library(wavpackomega SHARED ${WAVPACKOMEGA_ENGINE_HEADER} ${WAVPACKOMEGA_ENGINE_SOURCE})

if(MSVC)
	target_compile_options(wavpackomega PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(wavpackomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm wavpack)
else (OMEGA_WIN32)
	target_link_libraries(wavpackomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml wavpack)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS wavpackomega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
