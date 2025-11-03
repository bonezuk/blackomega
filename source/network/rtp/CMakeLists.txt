add_definitions(-DOMEGA_NETWORKRTP_DLL)

file(GLOB NETWORK_RTP_HEADER inc/*.h)
file(GLOB NETWORK_RTP_SOURCE src/*.cxx)

add_library(rtp SHARED ${NETWORK_RTP_HEADER} ${NETWORK_RTP_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(rtp PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(rtp network_omega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm ws2_32)
else (OMEGA_WIN32)
    target_link_libraries(rtp network_omega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
    install(TARGETS rtp DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
