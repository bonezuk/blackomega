set ( VERSIONER_HEADERS
    ./inc/DictionaryXMLMap.h
    ./inc/ProductVersionInfo.h
)

set ( VERSIONER_SOURCES
    ./src/DictionaryXMLMap.cxx
    ./src/ProductVersionInfo.cxx
    ./src/versioner.cxx
)

add_executable(versioner ${VERSIONER_HEADERS} ${VERSIONER_SOURCES} )

if (OMEGA_QT4)
    target_link_libraries(versioner ${QTVER}::QtCore ${QTVER}::QtGui ${QTVER}::QtXml)
else (OMEGA_QT4)
    target_link_libraries(versioner ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
endif (OMEGA_QT4)
