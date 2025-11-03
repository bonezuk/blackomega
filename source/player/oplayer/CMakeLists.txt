set( OPLAYER_HEADER
    ./inc/OPlayerMain.h
)

set( OPLAYER_SOURCE
    ./src/OPlayerMain.cxx
)

add_executable(oplayer ${OPLAYER_HEADER} ${OPLAYER_SOURCE})

if(OMEGA_MSVC16)
    target_compile_options(oplayer PRIVATE "/MP")
endif (OMEGA_MSVC16)

if (OMEGA_WIN32)
    target_link_libraries(oplayer audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets winmm ws2_32 wavpack)
elseif (OMEGA_MACOSX)
    if (OMEGA_QT4)
        target_link_libraries(oplayer audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::QtCore ${QTVER}::QtGui ${QTVER}::QtXml wavpack ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT} ${SPARKLE_FRAMEWORK})
    else (OMEGA_QT4)
        target_link_libraries(oplayer audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets wavpack ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT} ${SPARKLE_FRAMEWORK})
    endif (OMEGA_QT4)
elseif (OMEGA_LINUX)
    target_link_libraries(oplayer audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets wavpack ${LIBUPNP_LIBRARIES} )
endif (OMEGA_WIN32)

if (${TIGER_SYSTEM_DEPS})
    install(TARGETS oplayer DESTINATION bin)
endif (${TIGER_SYSTEM_DEPS})
