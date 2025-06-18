# Libxml2 configuration

if (${TIGER_SYSTEM_DEPS})
    set(LIBXML_HOME "${BLACKOMEGA_UTILS}/usr/lib")
    include_directories(AFTER "${BLACKOMEGA_UTILS}/usr/include/libxml2" )

    add_library(xml2 SHARED IMPORTED)
    set_property(TARGET xml2 PROPERTY IMPORTED_LOCATION "${LIBXML_HOME}/libxml2.${LIBEXT}" )
else (${TIGER_SYSTEM_DEPS})
    if (OMEGA_MACOSX)
        set(LIBXML_VERSION "2.13.5")
    else (OMEGA_MACOSX)
        set(LIBXML_VERSION "2.9.8")
    endif (OMEGA_MACOSX)
    set(LIBXML_LIBNAME "libxml2")

    if (OMEGA_MSVC16)
        if (TIGER_DEBUG_BUILD)
            set(LIBICONV_LIBNAME "libiconvD")
        else (TIGER_DEBUG_BUILD)
            set(LIBICONV_LIBNAME "libiconv")
        endif (TIGER_DEBUG_BUILD)
    endif (OMEGA_MSVC16)

    if (OMEGA_WIN32)
        set(LIBXML_HOME "${BLACKOMEGA_UTILS}/libxml2/libxml2-${LIBXML_VERSION}-${BUILD_SUFFIX}")
    else (OMEGA_WIN32)
        set(LIBXML_HOME "${BLACKOMEGA_UTILS}/libxml2/libxml2-${LIBXML_VERSION}-${TIGER_PLATFORM}")
    endif (OMEGA_WIN32)

    if (OMEGA_WIN32)
        include_directories(AFTER "${LIBXML_HOME}/include" )
    else (OMEGA_WIN32)
        include_directories(AFTER "${LIBXML_HOME}/include/libxml2" )
    endif (OMEGA_WIN32)

    add_library(xml2 SHARED IMPORTED)
    set_property(TARGET xml2 PROPERTY IMPORTED_LOCATION "${LIBXML_HOME}/lib/${LIBXML_LIBNAME}.${LIBEXT}" )
    if (OMEGA_WIN32)
        set_property(TARGET xml2 PROPERTY IMPORTED_IMPLIB "${LIBXML_HOME}/lib/${LIBXML_LIBNAME}.lib" )
    endif (OMEGA_WIN32)

    message("${LIBXML_HOME}/lib/${LIBXML_LIBNAME}.${LIBEXT}")
endif (${TIGER_SYSTEM_DEPS})
