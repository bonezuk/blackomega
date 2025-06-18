add_definitions(-DOMEGA_MIME_DLL)

file(GLOB NETWORK_MIME_HEADER inc/*.h)
file(GLOB NETWORK_MIME_SOURCE src/*.cxx)

add_library(mime SHARED ${NETWORK_MIME_HEADER} ${NETWORK_MIME_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(mime PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(mime network_omega http engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm ws2_32)
else (OMEGA_WIN32)
    target_link_libraries(mime network_omega http engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_SYSTEM_DEPS})
    install(TARGETS mime DESTINATION lib)
endif (${TIGER_SYSTEM_DEPS})
