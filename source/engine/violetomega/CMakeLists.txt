add_definitions(-DOMEGA_ENGINE_VIOLETOMEGA_DLL)

file(GLOB VIOLETOMEGA_ENGINE_HEADER inc/*.h)
file(GLOB VIOLETOMEGA_ENGINE_SOURCE src/*.cxx)

add_library(violetomega SHARED ${VIOLETOMEGA_ENGINE_HEADER} ${VIOLETOMEGA_ENGINE_SOURCE})

if(MSVC)
	target_compile_options(violetomega PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(violetomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
	target_link_libraries(violetomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS violetomega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
