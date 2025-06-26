add_definitions(-DOMEGA_ENGINE_VIOLETOMEGA_DLL)

file(GLOB VIOLETOMEGA_ENGINE_HEADER inc/*.h)
file(GLOB VIOLETOMEGA_ENGINE_SOURCE src/*.cxx)

add_library(violetomega SHARED ${VIOLETOMEGA_ENGINE_HEADER} ${VIOLETOMEGA_ENGINE_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(violetomega PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(violetomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
    if (OMEGA_QT4)
        target_link_libraries(violetomega engine common xml2 ${QTVER}::QtCore ${QTVER}::QtGui ${QTVER}::QtXml)
    else (OMEGA_QT4)
        target_link_libraries(violetomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
    endif (OMEGA_QT4)
endif (OMEGA_WIN32)

if (${TIGER_SYSTEM_DEPS})
    install(TARGETS violetomega DESTINATION lib)
endif (${TIGER_SYSTEM_DEPS})
