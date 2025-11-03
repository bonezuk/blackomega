add_definitions(-DOMEGA_NETWORK_RTPSILVEROMEGA_DLL)

file(GLOB NETWORK_RTP_SILVEROMEGA_HEADER inc/*.h)
file(GLOB NETWORK_RTP_SILVEROMEGA_SOURCE src/*.cxx)

add_library(rtp_silveromega SHARED ${NETWORK_RTP_SILVEROMEGA_HEADER} ${NETWORK_RTP_SILVEROMEGA_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(rtp_silveromega PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(rtp_silveromega network_omega rtp silveromega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm ws2_32)
else (OMEGA_WIN32)
    target_link_libraries(rtp_silveromega network_omega rtp silveromega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
    install(TARGETS rtp_silveromega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
