add_definitions(-DOMEGA_FTP_DLL)

file(GLOB NETWORK_FTP_HEADER inc/*.h)
file(GLOB NETWORK_FTP_SOURCE src/*.cxx)

add_library(ftp SHARED ${NETWORK_FTP_HEADER} ${NETWORK_FTP_SOURCE})

if(MSVC)
	target_compile_options(ftp PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(ftp network_omega common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm ws2_32)
else (OMEGA_WIN32)
	target_link_libraries(ftp network_omega common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS ftp DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
