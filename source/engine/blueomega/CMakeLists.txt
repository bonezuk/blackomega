add_definitions(-DOMEGA_ENGINE_BLUEOMEGA_DLL)

file(GLOB BLUEOMEGA_ENGINE_HEADER inc/*.h)
file(GLOB BLUEOMEGA_ENGINE_SOURCE src/*.cxx)

add_library(blueomega SHARED ${BLUEOMEGA_ENGINE_HEADER} ${BLUEOMEGA_ENGINE_SOURCE})

if(MSVC)
	target_compile_options(blueomega PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(blueomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
	target_link_libraries(blueomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS blueomega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
