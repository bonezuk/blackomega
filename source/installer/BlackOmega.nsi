# Script for Black Omega installer

!include "MUI2.nsh"

!define APPNAME "Black Omega"
!define COMPANYNAME "Stuart A. MacLean"

!include "Version.nsh"

!define HELPURL "http://www.blackomega.co.uk"
!define UPDATEURL "http://www.blackomega.co.uk"
!define ABOUTURL "http://www.blackomega.co.uk"

!define INSTALLSIZE 36046

!define OMEGAPATH "..\..\install\WIN32.OPT.VC9\omega"

InstallDir "$PROGRAMFILES\${APPNAME}"
InstallDirRegKey HKCU "Software\Black Omega 2\InstallDir" ""

LicenseData "license.rtf"

Name "${APPNAME}"
OutFile "${OMEGAPATH}\BlackOmega-${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}.${VERSION_BUILD}-Installer.exe"

ShowInstDetails "nevershow"
ShowUninstDetails "nevershow"

;--------------------------------
;Interface Settings

!define MUI_ABORTWARNING
!define MUI_ICON "..\player\omega.ico"
!define MUI_UNICON "..\player\omega.ico"

;--------------------------------
;Pages

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "license.rtf"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
!insertmacro MUI_LANGUAGE "English"

;--------------------------------

section  "Black Omega" SecDummy
    SetOverwrite on
    setOutPath "$INSTDIR\bin\Microsoft.VC90.CRT"
    file "${OMEGAPATH}\bin\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"
    file "${OMEGAPATH}\bin\Microsoft.VC90.CRT\msvcm90.dll"
    file "${OMEGAPATH}\bin\Microsoft.VC90.CRT\msvcp90.dll"
    file "${OMEGAPATH}\bin\Microsoft.VC90.CRT\msvcr90.dll"
    setOutPath "$INSTDIR\bin"
    file "${OMEGAPATH}\bin\assistant.exe"
    file "${OMEGAPATH}\bin\audioio.dll"
    file "${OMEGAPATH}\bin\Black Omega.exe"
    file "${OMEGAPATH}\bin\blackomega.dll"
    file "${OMEGAPATH}\bin\blueomega.dll"
    file "${OMEGAPATH}\bin\common.dll"
    file "${OMEGAPATH}\bin\dlna.dll"
    file "${OMEGAPATH}\bin\engine.dll"
    file "${OMEGAPATH}\bin\ftp.dll"
    file "${OMEGAPATH}\bin\greenomega.dll"
    file "${OMEGAPATH}\bin\violetomega.dll"
    file "${OMEGAPATH}\bin\http.dll"
    file "${OMEGAPATH}\bin\libupnp.dll"
    file "${OMEGAPATH}\bin\libxml2.dll"
    file "${OMEGAPATH}\bin\mime.dll"
    file "${OMEGAPATH}\bin\network.dll"
    file "${OMEGAPATH}\bin\PThreadVS2.dll"
    file "${OMEGAPATH}\bin\Qt5CLucene.dll"
    file "${OMEGAPATH}\bin\Qt5Core.dll"
    file "${OMEGAPATH}\bin\Qt5Gui.dll"
    file "${OMEGAPATH}\bin\Qt5Help.dll"
    file "${OMEGAPATH}\bin\Qt5Network.dll"
    file "${OMEGAPATH}\bin\Qt5PrintSupport.dll"
    file "${OMEGAPATH}\bin\Qt5Sql.dll"
    file "${OMEGAPATH}\bin\Qt5Widgets.dll"
    file "${OMEGAPATH}\bin\Qt5Xml.dll"
    file "${OMEGAPATH}\bin\redomega.dll"
    file "${OMEGAPATH}\bin\remote.dll"
    file "${OMEGAPATH}\bin\rtp.dll"
    file "${OMEGAPATH}\bin\rtp_silveromega.dll"
    file "${OMEGAPATH}\bin\silveromega.dll"
    file "${OMEGAPATH}\bin\track_db.dll"
    file "${OMEGAPATH}\bin\track_info.dll"
    file "${OMEGAPATH}\bin\track_model.dll"
    file "${OMEGAPATH}\bin\whiteomega.dll"
    file "${OMEGAPATH}\bin\omega.ico"
    file "${OMEGAPATH}\bin\WinSparkle.dll"
    file "${OMEGAPATH}\bin\widget.dll"
    file "${OMEGAPATH}\bin\cyanomega.dll"
    file "${OMEGAPATH}\bin\monkeyomega.dll"
    file "${OMEGAPATH}\bin\mpcdec.dll"
    file "${OMEGAPATH}\bin\monkey.dll"
    file "${OMEGAPATH}\bin\wavpackdll.dll"
    file "${OMEGAPATH}\bin\wavpackomega.dll"
    setOutPath "$INSTDIR\data\license"
    file "${OMEGAPATH}\data\license\license8.ogg"
    file "${OMEGAPATH}\data\license\license11.ogg"
    file "${OMEGAPATH}\data\license\license12.ogg"
    file "${OMEGAPATH}\data\license\license16.ogg"
    file "${OMEGAPATH}\data\license\license22.ogg"
    file "${OMEGAPATH}\data\license\license24.ogg"
    file "${OMEGAPATH}\data\license\license32.ogg"
    file "${OMEGAPATH}\data\license\license44.ogg"
    file "${OMEGAPATH}\data\license\license48.ogg"
    file "${OMEGAPATH}\data\license\license64.ogg"
    file "${OMEGAPATH}\data\license\license96.ogg"
    setOutPath "$INSTDIR\help"
    file "${OMEGAPATH}\help\BlackOmega.qch"
    file "${OMEGAPATH}\help\BlackOmega.qhc"
    setOutPath "$INSTDIR\plugins\platforms"
    file "${OMEGAPATH}\plugins\platforms\qwindows.dll"
    setOutPath "$INSTDIR\plugins\imageformats"
    file "${OMEGAPATH}\plugins\imageformats\qgif.dll"
    file "${OMEGAPATH}\plugins\imageformats\qjpeg.dll"
    file "${OMEGAPATH}\plugins\imageformats\qmng.dll"
    
    writeUninstaller "$INSTDIR\uninstall.exe"
    
    createDirectory "$SMPROGRAMS\${APPNAME}"
    createShortCut "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk" "$INSTDIR\bin\${APPNAME}.exe" "" "$INSTDIR\bin\omega.ico" 0

    WriteRegStr HKCU "Software\Black Omega 2\InstallDir" "" $INSTDIR
    
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayName" "${APPNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "UninstallString" "$\"$INSTDIR\uninstall.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "QuietUninstallString" "$\"$INSTDIR\uninstall.exe$\" /S"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "InstallLocation" "$\"$INSTDIR$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayIcon" "$\"$INSTDIR\bin\Black Omega.exe$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "Publisher" "${COMPANYNAME}"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "HelpLink" "$\"${HELPURL}$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLUpdateInfo" "$\"${UPDATEURL}$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "URLInfoAbout" "$\"${ABOUTURL}$\""
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "DisplayVersion" "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REVISION}"
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMajor" ${VERSION_MAJOR}
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionMinor" ${VERSION_MINOR}
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionRevision" ${VERSION_REVISION}
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "VersionBuild" ${VERSION_BUILD}
    # There is no option for modifying or repairing the install
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoModify" 1
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "NoRepair" 1
    # Set the INSTALLSIZE constant (!defined at the top of this script) so Add/Remove Programs can accurately report the size
    WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}" "EstimatedSize" ${INSTALLSIZE}
sectionEnd

 ;Language strings
 LangString DESC_SecDummy ${LANG_ENGLISH} "A test section."

 ;Assign language strings to sections
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecDummy} $(DESC_SecDummy)
!insertmacro MUI_FUNCTION_DESCRIPTION_END


section "Uninstall"
    delete "$SMPROGRAMS\${APPNAME}\${APPNAME}.lnk"
    rmDir "$SMPROGRAMS\${APPNAME}"

    delete "$INSTDIR\bin\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"
    delete "$INSTDIR\bin\Microsoft.VC90.CRT\msvcm90.dll"
    delete "$INSTDIR\bin\Microsoft.VC90.CRT\msvcp90.dll"
    delete "$INSTDIR\bin\Microsoft.VC90.CRT\msvcr90.dll"
    delete "$INSTDIR\bin\assistant.exe"
    delete "$INSTDIR\bin\audioio.dll"
    delete "$INSTDIR\bin\Black Omega.exe"
    delete "$INSTDIR\bin\blackomega.dll"
    delete "$INSTDIR\bin\blueomega.dll"
    delete "$INSTDIR\bin\common.dll"
    delete "$INSTDIR\bin\dlna.dll"
    delete "$INSTDIR\bin\engine.dll"
    delete "$INSTDIR\bin\ftp.dll"
    delete "$INSTDIR\bin\greenomega.dll"
    delete "$INSTDIR\bin\violetomega.dll"
    delete "$INSTDIR\bin\http.dll"
    delete "$INSTDIR\bin\libxml2.dll"
    delete "$INSTDIR\bin\libupnp.dll"
    delete "$INSTDIR\bin\mime.dll"
    delete "$INSTDIR\bin\network.dll"
    delete "$INSTDIR\bin\PThreadVS2.dll"
    delete "$INSTDIR\bin\QtCLucene4.dll"
    delete "$INSTDIR\bin\QtCore4.dll"
    delete "$INSTDIR\bin\QtGui4.dll"
    delete "$INSTDIR\bin\QtHelp4.dll"
    delete "$INSTDIR\bin\QtNetwork4.dll"
    delete "$INSTDIR\bin\QtSql4.dll"
    delete "$INSTDIR\bin\QtWebKit4.dll"
    delete "$INSTDIR\bin\QtXml4.dll"
    delete "$INSTDIR\bin\Qt5CLucene.dll"
    delete "$INSTDIR\bin\Qt5Core.dll"
    delete "$INSTDIR\bin\Qt5Gui.dll"
    delete "$INSTDIR\bin\Qt5Help.dll"
    delete "$INSTDIR\bin\Qt5Network.dll"
    delete "$INSTDIR\bin\Qt5PrintSupport.dll"
    delete "$INSTDIR\bin\Qt5Sql.dll"
    delete "$INSTDIR\bin\Qt5Widgets.dll"
    delete "$INSTDIR\bin\Qt5Xml.dll"
    delete "$INSTDIR\bin\remote.dll"
    delete "$INSTDIR\bin\redomega.dll"
    delete "$INSTDIR\bin\rtp.dll"
    delete "$INSTDIR\bin\rtp_silveromega.dll"
    delete "$INSTDIR\bin\silveromega.dll"
    delete "$INSTDIR\bin\track_db.dll"
    delete "$INSTDIR\bin\track_info.dll"
    delete "$INSTDIR\bin\track_model.dll"
    delete "$INSTDIR\bin\whiteomega.dll"
    delete "$INSTDIR\bin\omega.ico"
    delete "$INSTDIR\bin\WinSparkle.dll"
    delete "$INSTDIR\bin\widget.dll"
    delete "$INSTDIR\bin\cyanomega.dll"
    delete "$INSTDIR\bin\monkeyomega.dll"
    delete "$INSTDIR\bin\mpcdec.dll"
    delete "$INSTDIR\bin\monkey.dll"
    delete "$INSTDIR\bin\wavpackdll.dll"
    delete "$INSTDIR\bin\wavpackomega.dll"
    delete "$INSTDIR\data\license\license8.ogg"
    delete "$INSTDIR\data\license\license11.ogg"
    delete "$INSTDIR\data\license\license12.ogg"
    delete "$INSTDIR\data\license\license16.ogg"
    delete "$INSTDIR\data\license\license22.ogg"
    delete "$INSTDIR\data\license\license24.ogg"
    delete "$INSTDIR\data\license\license32.ogg"
    delete "$INSTDIR\data\license\license44.ogg"
    delete "$INSTDIR\data\license\license48.ogg"
    delete "$INSTDIR\data\license\license64.ogg"
    delete "$INSTDIR\data\license\license96.ogg"
    delete "$INSTDIR\data\license\(01)-center.wav"
    delete "$INSTDIR\data\license\(02)-left_front.wav"
    delete "$INSTDIR\data\license\(03)-right_front.wav"
    delete "$INSTDIR\data\license\(04)-left_surround.wav"
    delete "$INSTDIR\data\license\(05)-right_surround.wav"
    delete "$INSTDIR\data\license\(06)-left_rear.wav"
    delete "$INSTDIR\data\license\(07)-right_rear.wav"
    delete "$INSTDIR\data\license\(08)-subwoofer.wav"
    delete "$INSTDIR\data\surround\(01)-center.wav"
    delete "$INSTDIR\data\surround\(02)-left_front.wav"
    delete "$INSTDIR\data\surround\(03)-right_front.wav"
    delete "$INSTDIR\data\surround\(04)-left_surround.wav"
    delete "$INSTDIR\data\surround\(05)-right_surround.wav"
    delete "$INSTDIR\data\surround\(06)-left_rear.wav"
    delete "$INSTDIR\data\surround\(07)-right_rear.wav"
    delete "$INSTDIR\data\surround\(08)-subwoofer.wav"
    delete "$INSTDIR\help\BlackOmega.qch"
    delete "$INSTDIR\help\BlackOmega.qhc"
    delete "$INSTDIR\plugins\imageformats\qgif4.dll"
    delete "$INSTDIR\plugins\imageformats\qjpeg4.dll"
    delete "$INSTDIR\plugins\imageformats\qmng4.dll"
    delete "$INSTDIR\plugins\imageformats\qgif.dll"
    delete "$INSTDIR\plugins\imageformats\qjpeg.dll"
    delete "$INSTDIR\plugins\imageformats\qmng.dll"
    delete "$INSTDIR\plugins\platforms\qwindows.dll"
    delete "$INSTDIR\uninstall.exe"
    
    rmDir "$INSTDIR\bin\Microsoft.VC90.CRT"
    rmDir "$INSTDIR\bin\"
    rmDir "$INSTDIR\data\license"
    rmDir "$INSTDIR\data\"
    rmDir "$INSTDIR\help\"
    rmDir "$INSTDIR\plugins\imageformats"
    rmDir "$INSTDIR\plugins\platforms"
    rmDir "$INSTDIR\plugins"
    rmDir "$INSTDIR"
    
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
sectionEnd
