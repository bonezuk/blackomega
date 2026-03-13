# FFTW configuration

if (${TIGER_LINUX_DISTRO})

	find_package(PkgConfig REQUIRED)
	pkg_search_module(FFTW REQUIRED fftw3 IMPORTED_TARGET)
	add_library(fftw ALIAS PkgConfig::FFTW)

else (${TIGER_LINUX_DISTRO})

	set(FFTW_VERSION "3.3.10")
	
	if (OMEGA_WIN32)
		set(FFTW_HOME "${BLACKOMEGA_UTILS}/fftw/fftw-${FFTW_VERSION}-${BUILD_SUFFIX}")
	else (OMEGA_WIN32)
		set(FFTW_HOME "${BLACKOMEGA_UTILS}/fftw/fftw-${FFTW_VERSION}-${TIGER_PLATFORM}")
	endif (OMEGA_WIN32)

	include_directories(AFTER "${FFTW_HOME}/include" )
	
	if (OMEGA_WIN32)
		set(FFTW_LIBNAME "libfftw3")
		set(FFTW_THREAD_LIBNAME "libfftw3_threads")
	else (OMEGA_WIN32)
		set(FFTW_LIBNAME "libfftw3")
		set(FFTW_THREAD_LIBNAME "libfftw3_threads")
	endif (OMEGA_WIN32)

	add_library(fftw SHARED IMPORTED)
	set_property(TARGET fftw PROPERTY IMPORTED_LOCATION "${FFTW_HOME}/lib/${FFTW_LIBNAME}.${LIBEXT}" )
	if (OMEGA_WIN32)
		set_property(TARGET fftw PROPERTY IMPORTED_IMPLIB "${FFTW_HOME}/lib/${FFTW_LIBNAME}.lib" )
	endif (OMEGA_WIN32)

	add_library(fftw_threads SHARED IMPORTED)
	set_property(TARGET fftw_threads PROPERTY IMPORTED_LOCATION "${FFTW_HOME}/lib/${FFTW_THREAD_LIBNAME}.${LIBEXT}" )
	if (OMEGA_WIN32)
		set_property(TARGET fftw_threads PROPERTY IMPORTED_IMPLIB "${FFTW_HOME}/lib/${FFTW_THREAD_LIBNAME}.lib" )
	endif (OMEGA_WIN32)

	message("${FFTW_HOME}/lib/${FFTW_LIBNAME}.${LIBEXT}")
endif (${TIGER_LINUX_DISTRO})
