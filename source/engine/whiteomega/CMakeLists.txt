add_definitions(-DOMEGA_ENGINE_WHITEOMEGA_DLL)

file(GLOB WHITEOMEGA_ENGINE_HEADER inc/*.h)
file(GLOB WHITEOMEGA_ENGINE_SOURCE src/*.cxx)

add_library(whiteomega SHARED ${WHITEOMEGA_ENGINE_HEADER} ${WHITEOMEGA_ENGINE_SOURCE})

if(MSVC)
	target_compile_options(whiteomega PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(whiteomega engine common redomega xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
	target_link_libraries(whiteomega engine common redomega xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS whiteomega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
