add_definitions(-DOMEGA_ENGINE_GREENOMEGA_DLL)

file(GLOB GREENOMEGA_ENGINE_HEADER inc/*.h)
file(GLOB GREENOMEGA_ENGINE_SOURCE src/*.cxx)

add_library(greenomega SHARED ${GREENOMEGA_ENGINE_HEADER} ${GREENOMEGA_ENGINE_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(greenomega PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(greenomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
    target_link_libraries(greenomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
    install(TARGETS greenomega DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
