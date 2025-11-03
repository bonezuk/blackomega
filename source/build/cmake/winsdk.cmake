# Windows 10 SDK configuration

if ((${CMAKE_GENERATOR} MATCHES "NMake Makefiles") OR (${CMAKE_GENERATOR} MATCHES "Ninja"))
    set(WIN_SDK_HOME "C:\\Program Files (x86)\\Windows Kits\\10\\Lib\\10.0.18362.0\\um\\x64" CACHE PATH "Windows SDK 10.0.18362")
    message("Set ${WIN_SDK_HOME}")

    set_property(TARGET wbemuuid PROPERTY IMPORTED_IMPLIB "${WIN_SDK_HOME}/WbemUuid.Lib" )
    set_property(TARGET iphlpapi PROPERTY IMPORTED_IMPLIB "${WIN_SDK_HOME}/IPHlpApi.Lib" )
    set_property(TARGET winmm PROPERTY IMPORTED_IMPLIB "${WIN_SDK_HOME}/WinMM.Lib" )
endif ((${CMAKE_GENERATOR} MATCHES "NMake Makefiles") OR (${CMAKE_GENERATOR} MATCHES "Ninja"))
