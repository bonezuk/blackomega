# Monkey Audio configuration

set(MONKEYAUDIO_VERSION "4.12b")
set(MONKEYAUDIO_HOME "${TIGER_UTILS}/monkeyaudio/monkeyaudio-${MONKEYAUDIO_VERSION}-${BUILD_SUFFIX}")

include_directories(AFTER "${MONKEYAUDIO_HOME}/include" )

if (OMEGA_WIN32)
    set(MONKEYAUDIO_LIBNAME "monkey")
    add_definitions(-DPLATFORM_WINDOWS)
else (OMEGA_WIN32)
    set(MONKEYAUDIO_LIBNAME "libMACLib")
    add_definitions(-DPLATFORM_APPLE)
endif (OMEGA_WIN32)

add_library(monkey SHARED IMPORTED)
set_property(TARGET monkey PROPERTY IMPORTED_LOCATION "${MONKEYAUDIO_HOME}/lib/${MONKEYAUDIO_LIBNAME}.${LIBEXT}" )
if (OMEGA_WIN32)
    set_property(TARGET monkey PROPERTY IMPORTED_IMPLIB "${MONKEYAUDIO_HOME}/lib/${MONKEYAUDIO_LIBNAME}.lib" )
endif (OMEGA_WIN32)
