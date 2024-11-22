add_definitions(-DOMEGA_ENGINE_DLL)

file(GLOB ENGINE_HEADER inc/*.h)
file(GLOB ENGINE_SOURCE src/*.cxx)

add_library(engine SHARED ${ENGINE_HEADER} ${ENGINE_SOURCE})

if(MSVC)
	target_compile_options(engine PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
	target_link_libraries(engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS engine DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
