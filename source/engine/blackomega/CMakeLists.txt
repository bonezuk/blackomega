add_definitions(-DOMEGA_ENGINE_BLACKOMEGA_DLL)

file(GLOB BLACKOMEGA_ENGINE_HEADER inc/*.h)
file(GLOB BLACKOMEGA_ENGINE_SOURCE src/*.cxx)

add_library(blackomega SHARED ${BLACKOMEGA_ENGINE_HEADER} ${BLACKOMEGA_ENGINE_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(blackomega PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(blackomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
    target_link_libraries(blackomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
    install(TARGETS blackomega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
