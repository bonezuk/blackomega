add_definitions(-DOMEGA_ENGINE_REDOMEGA_DLL)
#add_definitions(-DOMEGA_ALAC_COMPARE)

file(GLOB REDOMEGA_ENGINE_HEADER inc/*.h)
file(GLOB REDOMEGA_ENGINE_SOURCE src/*.cxx)

add_library(redomega SHARED ${REDOMEGA_ENGINE_HEADER} ${REDOMEGA_ENGINE_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(redomega PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(redomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml winmm)
else (OMEGA_WIN32)
    if (OMEGA_QT4)
        target_link_libraries(redomega engine common xml2 ${QTVER}::QtCore ${QTVER}::QtGui ${QTVER}::QtXml)
    else (OMEGA_QT4)
        target_link_libraries(redomega engine common xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
    endif (OMEGA_QT4)
endif (OMEGA_WIN32)

if (${TIGER_SYSTEM_DEPS})
    install(TARGETS redomega DESTINATION lib)
endif (${TIGER_SYSTEM_DEPS})
