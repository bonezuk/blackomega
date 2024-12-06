add_definitions(-DOMEGA_TRACK_INFO_DLL)

file(GLOB TRACK_INFO_HEADER inc/*.h)
if (OMEGA_WIN32 OR OMEGA_LINUX)
	file(GLOB TRACK_INFO_SOURCE src/*.cxx src/*.c)
	list(REMOVE_ITEM TRACK_INFO_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/inc/SBBookmarkService.h" )
elseif (OMEGA_MACOSX)
	file(GLOB TRACK_INFO_SOURCE src/*.cxx src/*.c src/*.mm)
endif (OMEGA_WIN32 OR OMEGA_LINUX)


add_library(trackinfo SHARED ${TRACK_INFO_HEADER} ${TRACK_INFO_SOURCE})

target_include_directories(trackinfo PRIVATE "../../engine/cyanomega/musepack/inc/" )

if(OMEGA_MSVC16)
	target_compile_options(trackinfo PRIVATE "/MP")
endif (OMEGA_MSVC16)

set(QTLIBS_LIST ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
list(APPEND QTLIBS_LIST ${QTVER}::Core5Compat)

if (OMEGA_WIN32)
	target_link_libraries(trackinfo engine common greenomega silveromega violetomega whiteomega cyanomega xml2 ${QTLIBS_LIST} winmm ws2_32)
else (OMEGA_WIN32)
	target_link_libraries(trackinfo engine common greenomega silveromega violetomega whiteomega cyanomega xml2 ${QTLIBS_LIST})
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS trackinfo DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
