# WavPack configuration

if (${TIGER_LINUX_DISTRO})
	set(WAVPACK_HOME "${BLACKOMEGA_UTILS}/usr/lib")

	add_library(wavpack SHARED IMPORTED)
	set_property(TARGET wavpack PROPERTY IMPORTED_LOCATION "${WAVPACK_HOME}/libwavpack.so" )
	
else (${TIGER_LINUX_DISTRO})

	if (OMEGA_MACOSX)
		set(WAVPACK_VERSION "5.7.0")
	else (OMEGA_MACOSX)
		if (OMEGA_MSVC16)
			set(WAVPACK_VERSION "5.4.0")
		else (OMEGA_MSVC16)
			set(WAVPACK_VERSION "5.1.0")
		endif (OMEGA_MSVC16)
	endif (OMEGA_MACOSX)
	
	if (OMEGA_WIN32)
		set(WAVPACK_HOME "${BLACKOMEGA_UTILS}/wavpack/wavpack-${WAVPACK_VERSION}-${BUILD_SUFFIX}")
	else (OMEGA_WIN32)
		set(WAVPACK_HOME "${BLACKOMEGA_UTILS}/wavpack/wavpack-${WAVPACK_VERSION}-${TIGER_PLATFORM}")
	endif (OMEGA_WIN32)

	include_directories(AFTER "${WAVPACK_HOME}/include" )
	
	if (OMEGA_WIN32)
		set(WAVPACK_LIBNAME "wavpackdll")
	else (OMEGA_WIN32)
		set(WAVPACK_LIBNAME "libwavpack")
	endif (OMEGA_WIN32)

	add_library(wavpack SHARED IMPORTED)
	set_property(TARGET wavpack PROPERTY IMPORTED_LOCATION "${WAVPACK_HOME}/lib/${WAVPACK_LIBNAME}.${LIBEXT}" )
	if (OMEGA_WIN32)
		set_property(TARGET wavpack PROPERTY IMPORTED_IMPLIB "${WAVPACK_HOME}/lib/${WAVPACK_LIBNAME}.lib" )
	endif (OMEGA_WIN32)

	message("${WAVPACK_HOME}/lib/${WAVPACK_LIBNAME}.${LIBEXT}")
endif (${TIGER_LINUX_DISTRO})
