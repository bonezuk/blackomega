add_definitions(-DOMEGA_WIDGET_DLL)

file(GLOB WIDGET_HEADER inc/*.h)
if (OMEGA_WIN32 OR OMEGA_LINUX)
	file(GLOB WIDGET_SOURCE src/*.cxx)
	list(REMOVE_ITEM WIDGET_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/inc/ImportPlaylistDialog.h" "${CMAKE_CURRENT_SOURCE_DIR}/inc/SandBoxMac.h")
	qt_wrap_ui(WIDGET_UI_HEADERS inc/AboutDialog.ui)
elseif (OMEGA_MACOSX)
	file(GLOB WIDGET_SOURCE src/*.cxx src/*.mm)
	list(REMOVE_ITEM WIDGET_HEADER "${CMAKE_CURRENT_SOURCE_DIR}/inc/SandBoxWindows.h")
	list(REMOVE_ITEM WIDGET_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/src/SandBoxWindows.cxx")
	qt_wrap_ui(WIDGET_UI_HEADERS inc/AboutDialog.ui inc/ImportPlaylistDialog.ui)
endif (OMEGA_WIN32 OR OMEGA_LINUX)

qt_add_resources(WIDGET_UI_RESOURCES widget.qrc)

add_library(widget SHARED ${WIDGET_HEADER} ${WIDGET_SOURCE} ${WIDGET_UI_HEADERS} ${WIDGET_UI_RESOURCES})

if(MSVC)
	target_compile_options(widget PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(widget common http mime network_omega trackinfo xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets winmm ws2_32)
elseif (OMEGA_MACOSX)
	target_link_libraries(widget common http mime network_omega trackinfo xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets ${URLTYPE_FRAMEWORK_UNIT})
elseif (OMEGA_LINUX)
	target_link_libraries(widget common http mime network_omega trackinfo xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets)
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS widget DESTINATION lib)
endif (${TIGER_LINUX_DISTRO})
