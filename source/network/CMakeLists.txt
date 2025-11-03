add_definitions(-DOMEGA_NETWORK_DLL)

if (OMEGA_MSVC12 OR OMEGA_MSVC16)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif (OMEGA_MSVC12 OR OMEGA_MSVC16)

file(GLOB NETWORK_HEADER inc/*.h)
file(GLOB NETWORK_SOURCE src/*.cxx)

add_library(network_omega SHARED ${NETWORK_HEADER} ${NETWORK_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(network_omega PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(network_omega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm ws2_32)
else (OMEGA_WIN32)
    target_link_libraries(network_omega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
    install(TARGETS network_omega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
