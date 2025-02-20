set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

message ("Qt path")

set(QTVER "Qt5")

if (OMEGA_WIN32)
	set(QT_HOME "" CACHE PATH "Qt install directory")
	set(CMAKE_PREFIX_PATH "${QT_HOME}")
else (OMEGA_WIN32)
	string(STRIP "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH)
	if (CMAKE_PREFIX_PATH STREQUAL "")
		message("Qt - 1")
		set(QT_HOME "" CACHE PATH "Qt install directory")
		set(CMAKE_PREFIX_PATH "${QT_HOME}")
	else (CMAKE_PREFIX_PATH STREQUAL "")
		message("Qt - 2")
		set(QT_HOME "${CMAKE_PREFIX_PATH}")
	endif (CMAKE_PREFIX_PATH STREQUAL "")
	message("${QT_HOME}")
	message("${CMAKE_PREFIX_PATH}")
endif (OMEGA_WIN32)

set(QT_HOME2 "${QT_HOME}")

find_package(Qt5Core)
find_package(Qt5Gui)
find_package(Qt5Xml)
find_package(Qt5Widgets)
find_package(Qt5Test)
find_package(Qt5Network)
if (NOT OMEGA_MSVC12)
	find_package(Qt5Qml)
	find_package(Qt5Quick)
	find_package(Qt5DBus)
endif (NOT OMEGA_MSVC12)

if (${TIGER_LINUX_DISTRO})
	set(QT_HOME "${BLACKOMEGA_UTILS}/usr")
endif (${TIGER_LINUX_DISTRO})

include_directories(AFTER "${QT_HOME}/include" )
include_directories(AFTER "${QT_HOME}/include/QtCore" )
include_directories(AFTER "${QT_HOME}/include/QtGui" )
include_directories(AFTER "${QT_HOME}/include/QtXml" )
include_directories(AFTER "${QT_HOME}/include/QtWidgets" )
include_directories(AFTER "${QT_HOME}/include/QtTest" )
include_directories(AFTER "${QT_HOME}/include/QtQml" )
include_directories(AFTER "${QT_HOME}/include/QtQuick" )
include_directories(AFTER "${QT_HOME}/include/QtDBus" )
include_directories(AFTER "${QT_HOME}/include/QtNetwork" )

if (OMEGA_WIN32)

	if (TIGER_DEBUG_BUILD)
		message("Qt Set Debug")
		set(QT_CORE_LIBNAME "Qt5Cored")
		set(QT_GUI_LIBNAME "Qt5Guid")
		set(QT_XML_LIBNAME "Qt5Xmld")
		set(QT_WIDGETS_LIBNAME "Qt5Widgetsd")
		set(QT_NETWORK_LIBNAME "Qt5Networkd")
		set(QT_TEST_LIBNAME "Qt5Testd")
		
		if (NOT OMEGA_MSVC12)
			set(QT_QML_LIBNAME "Qt5Qmld")
			set(QT_QUICK_LIBNAME "Qt5Quickd")
			set(QT_DBUS_LIBNAME "Qt5DBusd")
		endif (NOT OMEGA_MSVC12)
		
	else (TIGER_DEBUG_BUILD)
		set(QT_CORE_LIBNAME "Qt5Core")
		set(QT_GUI_LIBNAME "Qt5Gui")
		set(QT_XML_LIBNAME "Qt5Xml")
		set(QT_WIDGETS_LIBNAME "Qt5Widgets")
		set(QT_NETWORK_LIBNAME "Qt5Network")
		set(QT_TEST_LIBNAME "Qt5Test")
		if (NOT OMEGA_MSVC12)
			set(QT_QML_LIBNAME "Qt5Qml")
			set(QT_QUICK_LIBNAME "Qt5Quick")
			set(QT_DBUS_LIBNAME "Qt5DBus")
		endif (NOT OMEGA_MSVC12)
		
	endif (TIGER_DEBUG_BUILD)

	if (OMEGA_MSVC12 OR OMEGA_MSVC16)
		if (TIGER_DEBUG_BUILD)
			set(LIB_EGL_LIBNAME "libEGLd")
			set(LIB_GLES_LIBNAME "libGLESv2d")
		else (TIGER_DEBUG_BUILD)
			set(LIB_EGL_LIBNAME "libEGL")
			set(LIB_GLES_LIBNAME "libGLESv2")
		endif (TIGER_DEBUG_BUILD)
	endif (OMEGA_MSVC12 OR OMEGA_MSVC16)
	
elseif (OMEGA_MACOSX)

	if (${TIGER_DEBUG_BUILD})
		set(QT_CORE_LIBNAME "libQt5Core")
		set(QT_GUI_LIBNAME "libQt5Gui")
		set(QT_XML_LIBNAME "libQt5Xml")
		set(QT_WIDGETS_LIBNAME "libQt5Widgets")
		set(QT_TEST_LIBNAME "libQt5Test")
		set(QT_PRINTSUPPORT_LIBNAME "libQt5PrintSupport")
		set(QT_QML_LIBNAME "libQt5Qml")
		set(QT_QUICK_LIBNAME "libQt5Quick")
		set(QT_NETWORK_LIBNAME "libQt5Network")
		set(QT_DBUS_LIBNAME "libQt5DBus")

#		set(QT_CORE_LIBNAME "libQt5Core_debug")
#		set(QT_GUI_LIBNAME "libQt5Gui_debug")
#		set(QT_XML_LIBNAME "libQt5Xml_debug")
#		set(QT_WIDGETS_LIBNAME "libQt5Widgets_debug")
#		set(QT_TEST_LIBNAME "libQt5Test_debug")
#		set(QT_PRINTSUPPORT_LIBNAME "libQt5PrintSupport_debug")
#		set(QT_QML_LIBNAME "libQt5Qml_debug")
#		set(QT_QUICK_LIBNAME "libQt5Quick_debug")
#		set(QT_NETWORK_LIBNAME "libQt5Network_debug")
#		set(QT_DBUS_LIBNAME "libQt5DBus_debug")
		
	else (${TIGER_DEBUG_BUILD})
		set(QT_CORE_LIBNAME "libQt5Core")
		set(QT_GUI_LIBNAME "libQt5Gui")
		set(QT_XML_LIBNAME "libQt5Xml")
		set(QT_WIDGETS_LIBNAME "libQt5Widgets")
		set(QT_TEST_LIBNAME "libQt5Test")
		set(QT_PRINTSUPPORT_LIBNAME "libQt5PrintSupport")
		set(QT_QML_LIBNAME "libQt5Qml")
		set(QT_QUICK_LIBNAME "libQt5Quick")
		set(QT_NETWORK_LIBNAME "libQt5Network")
		set(QT_DBUS_LIBNAME "libQt5DBus")
		
	endif (${TIGER_DEBUG_BUILD})

elseif (OMEGA_LINUX)

	set(QT_CORE_LIBNAME "libQt5Core")
	set(QT_GUI_LIBNAME "libQt5Gui")
	set(QT_XML_LIBNAME "libQt5Xml")
	set(QT_WIDGETS_LIBNAME "libQt5Widgets")
	set(QT_TEST_LIBNAME "libQt5Test")
	set(QT_QML_LIBNAME "libQt5Qml")
	set(QT_QUICK_LIBNAME "libQt5Quick")
	set(QT_NETWORK_LIBNAME "libQt5Network")
	set(QT_DBUS_LIBNAME "libQt5DBus")

endif (OMEGA_WIN32)
