# Musepack configuration

if (${TIGER_SYSTEM_DEPS})
    set(MUSEPACK_HOME "${BLACKOMEGA_UTILS}/usr/lib")

    add_library(mpcdec SHARED IMPORTED)
    set_property(TARGET mpcdec PROPERTY IMPORTED_LOCATION "${MUSEPACK_HOME}/libmpcdec.so" )

else (${TIGER_SYSTEM_DEPS})

    set(MUSEPACK_VERSION "r475")
    if (OMEGA_WIN32)
        set(MUSEPACK_HOME "${BLACKOMEGA_UTILS}/musepack/musepack-${MUSEPACK_VERSION}-${BUILD_SUFFIX}")
    else (OMEGA_WIN32)
        set(MUSEPACK_HOME "${BLACKOMEGA_UTILS}/musepack/musepack-${MUSEPACK_VERSION}-${TIGER_PLATFORM}")
    endif (OMEGA_WIN32)

    include_directories(AFTER "${MUSEPACK_HOME}/include" )

    if (OMEGA_WIN32)
        set(MUSEPACK_LIBNAME "mpcdec")
    else (OMEGA_WIN32)
        set(MUSEPACK_LIBNAME "libmpcdec")
    endif (OMEGA_WIN32)

    add_library(mpcdec SHARED IMPORTED)
    set_property(TARGET mpcdec PROPERTY IMPORTED_LOCATION "${MUSEPACK_HOME}/lib/${MUSEPACK_LIBNAME}.${LIBEXT}" )
    if (OMEGA_WIN32)
        set_property(TARGET mpcdec PROPERTY IMPORTED_IMPLIB "${MUSEPACK_HOME}/lib/${MUSEPACK_LIBNAME}.lib" )
    endif (OMEGA_WIN32)

    message("${MUSEPACK_HOME}/lib/${MUSEPACK_LIBNAME}.${LIBEXT}")
endif (${TIGER_SYSTEM_DEPS})
