add_definitions(-DOMEGA_TRACK_MODEL_DLL)

file(GLOB TRACK_MODEL_HEADER inc/*.h)
file(GLOB TRACK_MODEL_SOURCE src/*.cxx)

add_library(trackmodel SHARED ${TRACK_MODEL_HEADER} ${TRACK_MODEL_SOURCE})

if(MSVC)
	target_compile_options(trackmodel PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(trackmodel common engine trackdb trackinfo xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm ws2_32)
else (OMEGA_WIN32)
	target_link_libraries(trackmodel common engine trackdb trackinfo xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS trackmodel DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
