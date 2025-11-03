set( REDOMEGACOMPARISON_HEADER
    ./ALACMacOSXForge/aglib.h
    ./ALACMacOSXForge/ALACAudioTypes.h
    ./ALACMacOSXForge/ALACDecoderRef.h
    ./ALACMacOSXForge/EndianPortable.h
    ./ALACMacOSXForge/ALACBitUtilities.h
    ./ALACMacOSXForge/ALACEncoder.h
    ./ALACMacOSXForge/matrixlib.h
    ./ALACMacOSXForge/dplib.h
    ./RedOmegaComparison.h
)

set( REDOMEGACOMPARISON_SOURCE
    ./ALACMacOSXForge/EndianPortable.c
    ./ALACMacOSXForge/ag_dec.c
    ./ALACMacOSXForge/dp_enc.c
    ./ALACMacOSXForge/matrix_enc.c
    ./ALACMacOSXForge/ALACBitUtilities.c
    ./ALACMacOSXForge/ALACEncoder.cpp
    ./ALACMacOSXForge/ag_enc.c
    ./ALACMacOSXForge/ALACDecoderRef.cpp
    ./ALACMacOSXForge/dp_dec.c
    ./ALACMacOSXForge/matrix_dec.c
    ./RedOmegaComparison.cxx
)

add_executable(redomegacomparison ${REDOMEGACOMPARISON_HEADER} ${REDOMEGACOMPARISON_SOURCE})

target_include_directories(redomegacomparison PRIVATE ./ALACMacOSXForge)

if(OMEGA_MSVC16)
    target_compile_options(redomegacomparison PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(redomegacomparison audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets winmm ws2_32 wavpack)
elseif (OMEGA_MACOSX)
    if (OMEGA_QT4)
        target_link_libraries(redomegacomparison audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::QtCore ${QTVER}::QtGui ${QTVER}::QtXml wavpack ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT} ${SPARKLE_FRAMEWORK})
    else (OMEGA_QT4)
        target_link_libraries(redomegacomparison audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets wavpack ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT} ${SPARKLE_FRAMEWORK})
    endif (OMEGA_QT4)
elseif (OMEGA_LINUX)
    target_link_libraries(redomegacomparison audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets wavpack ${LIBUPNP_LIBRARIES} )
endif (OMEGA_WIN32)

if (${TIGER_SYSTEM_DEPS})
    install(TARGETS redomegacomparison DESTINATION bin)
endif (${TIGER_SYSTEM_DEPS})
