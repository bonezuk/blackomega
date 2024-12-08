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
target_link_libraries(versioner ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml)
