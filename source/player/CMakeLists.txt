set( PLAYER_UI
	inc/Player.ui
	inc/ProgressMessageDialog.ui
	inc/SettingsAudio.ui
	inc/SettingsFile.ui
	inc/SettingsGeneral.ui
	inc/SettingsITunes.ui
	inc/SettingsKeyboard.ui
)

set( PLAYER_HEADER
	inc/CLIPipe.h
	inc/ITunesConfig.h
	inc/Player.h
	inc/PlayerController.h
	inc/ProgressMessageDialog.h
	inc/QPlayControls.h
	inc/QPlayerApplication.h
	inc/QPlaylistWidget.h
	inc/QPLItemAlbum.h
	inc/QPLItemATrack.h
	inc/QPLItemBase.h
	inc/QPLItemSingle.h
	inc/QPLProgress.h
	inc/QSpeakerButton.h
	inc/RegisterFileTypes.h
	inc/Settings.h
	inc/SettingsAudio.h
	inc/SettingsBase.h
	inc/SettingsFile.h
	inc/SettingsGeneral.h
	inc/SettingsITunes.h
	inc/SettingsKeyboard.h
	inc/SpeakerInfo.h
)

set( PLAYER_SOURCE
	src/CLIPipe.cxx
	src/ITunesConfig.cxx
	src/Main.cxx
	src/Player.cxx
	src/PlayerController.cxx
	src/ProgressMessageDialog.cxx
	src/QPlayControls.cxx
	src/QPlayerApplication.cxx
	src/QPlaylistWidget.cxx
	src/QPLItemAlbum.cxx
	src/QPLItemBase.cxx
	src/QPLItemATrack.cxx
	src/QPLItemSingle.cxx
	src/QPLProgress.cxx
	src/QSpeakerButton.cxx
	src/RegisterFileTypes.cxx
	src/Settings.cxx
	src/SettingsAudio.cxx
	src/SettingsBase.cxx
	src/SettingsFile.cxx
	src/SettingsGeneral.cxx
	src/SettingsITunes.cxx
	src/SettingsKeyboard.cxx
	src/SpeakerInfo.cxx
)

if (OMEGA_MACOSX)
	set( PLAYER_MAC_HEADER
		inc/CocoaInitializer.h
		inc/SettingsITunesMac.h
	)

	set( PLAYER_MAC_SOURCE
		src/ITunesConfigLocationMac.mm
		src/CocoaInitializer.mm
		src/SettingsITunesMac.mm
	)
endif (OMEGA_MACOSX)

if (OMEGA_WIN32)
	set( PLAYER_RESOURCE ./player.rc )
endif (OMEGA_WIN32)

qt_wrap_ui(PLAYER_UI_HEADERS ${PLAYER_UI})
if (OMEGA_WIN32)
	qt_add_resources(PLAYER_UI_RESOURCES player.qrc platform/windows/platform.qrc)
elseif (OMEGA_MACOSX OR OMEGA_LINUX)
	qt_add_resources(PLAYER_UI_RESOURCES player.qrc)
endif (OMEGA_WIN32)

if (OMEGA_WIN32)
	add_executable(Omega WIN32 ${PLAYER_HEADER} ${PLAYER_SOURCE} ${PLAYER_UI_RESOURCES} ${PLAYER_UI_HEADERS} ${PLAYER_RESOURCE})
elseif (OMEGA_MACOSX)
	add_executable(Omega ${PLAYER_HEADER} ${PLAYER_SOURCE} ${PLAYER_UI_RESOURCES} ${PLAYER_UI_HEADERS} ${PLAYER_MAC_HEADER} ${PLAYER_MAC_SOURCE})
elseif (OMEGA_LINUX)
	add_executable(Omega ${PLAYER_HEADER} ${PLAYER_SOURCE} ${PLAYER_UI_RESOURCES} ${PLAYER_UI_HEADERS} ${PLAYER_RESOURCE})
endif (OMEGA_WIN32)

if(MSVC)
	target_compile_options(Omega PRIVATE "/MP")
endif (MSVC)

if (OMEGA_WIN32)
	target_link_libraries(Omega audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets winmm ws2_32 wavpack)
elseif (OMEGA_MACOSX)
	target_link_libraries(Omega audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets wavpack ${URLTYPE_FRAMEWORK_UNIT} ${COCOA_FRAMEWORK_UNIT} ${APPKIT_FRAMEWORK_UNIT} ${IOKIT_FRAMEWORK_UNIT} ${SPARKLE_FRAMEWORK} )
elseif (OMEGA_LINUX)
	target_link_libraries(Omega audioio blackomega blueomega common cyanomega engine greenomega http mime toneomega wavpackomega network_omega redomega remote rtp rtp_silveromega silveromega trackdb trackinfo violetomega whiteomega widget xml2 ${QTVER}::Core ${QTVER}::Gui ${QTVER}::Xml ${QTVER}::Widgets wavpack mpcdec ${LIBUPNP_LIBRARIES})
endif (OMEGA_WIN32)

if (OMEGA_WIN32)
	set_target_properties(Omega PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
	set_target_properties(Omega PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
endif (OMEGA_WIN32)

if (${TIGER_LINUX_DISTRO})
	install(TARGETS cyanomega DESTINATION bin)
endif (${TIGER_LINUX_DISTRO})
