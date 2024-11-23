# Google Test configuration
if (${TIGER_LINUX_DISTRO})

	find_package(GTest REQUIRED)

else (${TIGER_LINUX_DISTRO})
	if (OMEGA_MACOSX)
		set(GTEST_VERSION "1.11.0")
		set(GMOCK_VERSION "1.11.0")	
	else (OMEGA_MACOSX)
		set(GTEST_VERSION "1.7.0")
		set(GMOCK_VERSION "1.7.0")
	endif (OMEGA_MACOSX)

	set(GTEST_LIBNAME "gtest")
	set(GTEST_MAIN_LIBNAME "gtest_main")

	set(GMOCK_LIBNAME "gmock")
	set(GMOCK_MAIN_LIBNAME "gmock_main")
	if (OMEGA_WIN32)
		set(GTEST_HOME "${ATHENA_UTILS}/gmock/gmock-${GMOCK_VERSION}-${BUILD_SUFFIX}")
		set(GMOCK_HOME "${ATHENA_UTILS}/gmock/gmock-${GMOCK_VERSION}-${BUILD_SUFFIX}")
	else (OMEGA_WIN32)
		set(GTEST_HOME "${ATHENA_UTILS}/gmock/gmock-${GMOCK_VERSION}-${TIGER_PLATFORM}")
		set(GMOCK_HOME "${ATHENA_UTILS}/gmock/gmock-${GMOCK_VERSION}-${TIGER_PLATFORM}")
	endif (OMEGA_WIN32)

	include_directories(AFTER "${GMOCK_HOME}/include" )

	add_library(gtest SHARED IMPORTED)
	if (OMEGA_WIN32)
		set_property(TARGET gtest PROPERTY IMPORTED_LOCATION "${GTEST_HOME}/lib/${GTEST_LIBNAME}.${LIBEXT}" )
		set_property(TARGET gtest PROPERTY IMPORTED_IMPLIB "${GTEST_HOME}/lib/${GTEST_LIBNAME}.lib" )
	else (OMEGA_WIN32)
		set_property(TARGET gtest PROPERTY IMPORTED_LOCATION "${GTEST_HOME}/lib/lib${GTEST_LIBNAME}.${LIBEXT}" )
	endif (OMEGA_WIN32)

	add_library(gtest_main SHARED IMPORTED)
	if (OMEGA_WIN32)
		set_property(TARGET gtest_main PROPERTY IMPORTED_LOCATION "${GTEST_HOME}/lib/${GTEST_MAIN_LIBNAME}.${LIBEXT}" )
		set_property(TARGET gtest_main PROPERTY IMPORTED_IMPLIB "${GTEST_HOME}/lib/${GTEST_MAIN_LIBNAME}.lib" )
	else (OMEGA_WIN32)
		set_property(TARGET gtest_main PROPERTY IMPORTED_LOCATION "${GTEST_HOME}/lib/lib${GTEST_MAIN_LIBNAME}.${LIBEXT}" )
	endif (OMEGA_WIN32)

	message("${GTEST_HOME}/lib/lib${GTEST_LIBNAME}.${LIBEXT}")

	add_library(gmock SHARED IMPORTED)
	if (OMEGA_WIN32)
		set_property(TARGET gmock PROPERTY IMPORTED_LOCATION "${GMOCK_HOME}/lib/${GMOCK_LIBNAME}.${LIBEXT}" )
		set_property(TARGET gmock PROPERTY IMPORTED_IMPLIB "${GMOCK_HOME}/lib/${GMOCK_LIBNAME}.lib" )
	else (OMEGA_WIN32)
		set_property(TARGET gmock PROPERTY IMPORTED_LOCATION "${GMOCK_HOME}/lib/lib${GMOCK_LIBNAME}.${LIBEXT}" )
	endif (OMEGA_WIN32)

	add_library(gmock_main SHARED IMPORTED)
	if (OMEGA_WIN32)
		set_property(TARGET gmock_main PROPERTY IMPORTED_LOCATION "${GMOCK_HOME}/lib/${GMOCK_MAIN_LIBNAME}.${LIBEXT}" )
		set_property(TARGET gmock_main PROPERTY IMPORTED_IMPLIB "${GMOCK_HOME}/lib/${GMOCK_MAIN_LIBNAME}.lib" )
	else (OMEGA_WIN32)
		set_property(TARGET gmock_main PROPERTY IMPORTED_LOCATION "${GMOCK_HOME}/lib/lib${GMOCK_MAIN_LIBNAME}.${LIBEXT}" )
	endif (OMEGA_WIN32)

	message("${GMOCK_HOME}/lib/lib${GMOCK_LIBNAME}.${LIBEXT}")

endif (${TIGER_LINUX_DISTRO})