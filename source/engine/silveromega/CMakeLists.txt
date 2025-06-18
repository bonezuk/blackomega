add_definitions(-DOMEGA_ENGINE_SILVEROMEGA_DLL)

file(GLOB SILVEROMEGA_ENGINE_HEADER inc/*.h)
file(GLOB SILVEROMEGA_ENGINE_SOURCE src/*.cxx)

add_library(silveromega SHARED ${SILVEROMEGA_ENGINE_HEADER} ${SILVEROMEGA_ENGINE_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(silveromega PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(silveromega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
    target_link_libraries(silveromega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_WIN32)

if (${TIGER_SYSTEM_DEPS})
    install(TARGETS silveromega DESTINATION lib)
endif (${TIGER_SYSTEM_DEPS})
