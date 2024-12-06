add_definitions(-DOMEGA_TRACK_DB_DLL)

file(GLOB TRACK_DB_HEADER inc/*.h)
if (OMEGA_WIN32 OR OMEGA_LINUX)
	file(GLOB TRACK_DB_SOURCE src/*.cxx src/*.c)
	list(REMOVE_ITEM TRACK_DB_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/inc/ITunesLocationMac.h" "${CMAKE_CURRENT_SOURCE_DIR}/inc/SandboxBookmarkService.h" "${CMAKE_CURRENT_SOURCE_DIR}/inc/SBBookmarkTrackDB.h")
elseif (OMEGA_MACOSX)
	file(GLOB TRACK_DB_SOURCE src/*.cxx src/*.c src/*.mm)
endif (OMEGA_WIN32 OR OMEGA_LINUX)

add_library(trackdb SHARED ${TRACK_DB_HEADER} ${TRACK_DB_SOURCE})

if(OMEGA_MSVC16)
	target_compile_options(trackdb PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
	target_link_libraries(trackdb engine common network_omega trackinfo audioio xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm ws2_32)
elseif (OMEGA_MACOSX OR OMEGA_LINUX)
	if (${TIGER_MAC_STORE})
		target_link_libraries(trackdb engine common network_omega trackinfo widget audioio xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
	else (${TIGER_MAC_STORE})
		target_link_libraries(trackdb engine common network_omega trackinfo audioio xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
	endif (${TIGER_MAC_STORE})
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS trackdb DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
