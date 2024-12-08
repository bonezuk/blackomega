set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

message ("Qt path")

set(QTVER "Qt6")

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

message("Ver-A Qt6")

find_package(Qt6Core)
find_package(Qt6Gui)
find_package(Qt6Xml)

if (${TIGER_LINUX_DISTRO})
	set(QT_HOME "${BLACKOMEGA_UTILS}/usr")
endif (${TIGER_LINUX_DISTRO})

include_directories(AFTER "${QT_HOME}/include" )
include_directories(AFTER "${QT_HOME}/include/QtCore" )
include_directories(AFTER "${QT_HOME}/include/QtGui" )
include_directories(AFTER "${QT_HOME}/include/QtXml" )

if (OMEGA_WIN32)

	if (TIGER_DEBUG_BUILD)
		message("Qt Set Debug")
		set(QT_CORE_LIBNAME "Qt6Cored")
		set(QT_GUI_LIBNAME "Qt6Guid")
		set(QT_XML_LIBNAME "Qt6Xmld")
		
	else (TIGER_DEBUG_BUILD)
		set(QT_CORE_LIBNAME "Qt6Core")
		set(QT_GUI_LIBNAME "Qt6Gui")
		set(QT_XML_LIBNAME "Qt6Xml")
		
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

	set(QT_CORE_LIBNAME "libQt6Core")
	set(QT_GUI_LIBNAME "libQt6Gui")
	set(QT_XML_LIBNAME "libQt6Xml")

elseif (OMEGA_LINUX)

	set(QT_CORE_LIBNAME "libQt6Core")
	set(QT_GUI_LIBNAME "libQt6Gui")
	set(QT_XML_LIBNAME "libQt6Xml")

endif (OMEGA_WIN32)
