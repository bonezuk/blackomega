add_definitions(-DOMEGA_HTTP_DLL)

file(GLOB NETWORK_HTTP_HEADER inc/*.h)
file(GLOB NETWORK_HTTP_SOURCE src/*.cxx)

add_library(http SHARED ${NETWORK_HTTP_HEADER} ${NETWORK_HTTP_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(http PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(http network_omega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm ws2_32)
else (OMEGA_WIN32)
    if (OMEGA_QT4)
        target_link_libraries(http network_omega engine common xml2 ${QTVER}::QtCore ${QTVER}::QtGui ${QTVER}::QtXml)
    else (OMEGA_QT4)
        target_link_libraries(http network_omega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
    endif (OMEGA_QT4)
endif (OMEGA_WIN32)

if (${TIGER_SYSTEM_DEPS})
    install(TARGETS http DESTINATION lib)
endif (${TIGER_SYSTEM_DEPS})
