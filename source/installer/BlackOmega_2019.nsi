# Script for Black Omega installer

!include "MUI2.nsh"
!include "x64.nsh"

!define APPNAME "Black Omega"
!define COMPANYNAME "Stuart A. MacLean"

!include "Version.nsh"

!define HELPURL "http://www.blackomega.co.uk"
!define UPDATEURL "http://www.blackomega.co.uk"
!define ABOUTURL "http://www.blackomega.co.uk"

!define INSTALLSIZE 36046

!define OMEGAPATH "..\..\Build\Omega"

InstallDir "$PROGRAMFILES64\${APPNAME}"
InstallDirRegKey HKCU "Software\Black Omega 2\InstallDir" ""

LicenseData "license.rtf"

Name "${APPNAME}"
OutFile "${OMEGAPATH}\BlackOmega-${VERSION_BUILD}-Installer.exe"

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
!define MUI_FINISHPAGE_NOAUTOCLOSE
!define MUI_FINISHPAGE_RUN
!define MUI_FINISHPAGE_RUN_CHECKED
!define MUI_FINISHPAGE_RUN_TEXT "Run Black Omega"
!define MUI_FINISHPAGE_RUN_FUNCTION "OmegaRun"
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
    setOutPath "$INSTDIR\bin"
    file "${OMEGAPATH}\bin\audioio.dll"
    file "${OMEGAPATH}\bin\blackomega.dll"
    file "${OMEGAPATH}\bin\blueomega.dll"
    file "${OMEGAPATH}\bin\common.dll"
    file "${OMEGAPATH}\bin\concrt140.dll"
    file "${OMEGAPATH}\bin\cyanomega.dll"
    file "${OMEGAPATH}\bin\engine.dll"
    file "${OMEGAPATH}\bin\ftp.dll"
    file "${OMEGAPATH}\bin\greenomega.dll"
    file "${OMEGAPATH}\bin\http.dll"
    file "${OMEGAPATH}\bin\libiconv.dll"
    file "${OMEGAPATH}\bin\libxml2.dll"
    file "${OMEGAPATH}\bin\mime.dll"
    file "${OMEGAPATH}\bin\msvcp140.dll"
    file "${OMEGAPATH}\bin\msvcp140_1.dll"
    file "${OMEGAPATH}\bin\msvcp140_2.dll"
    file "${OMEGAPATH}\bin\network_omega.dll"
    file "${OMEGAPATH}\bin\playercommon.dll"
    file "${OMEGAPATH}\bin\Qt6Core.dll"
    file "${OMEGAPATH}\bin\Qt6Core5Compat.dll"
    file "${OMEGAPATH}\bin\Qt6Gui.dll"
    file "${OMEGAPATH}\bin\Qt6Widgets.dll"
    file "${OMEGAPATH}\bin\Qt6Xml.dll"
    file "${OMEGAPATH}\bin\redomega.dll"
    file "${OMEGAPATH}\bin\remote.dll"
    file "${OMEGAPATH}\bin\rtp.dll"
    file "${OMEGAPATH}\bin\rtp_silveromega.dll"
    file "${OMEGAPATH}\bin\silveromega.dll"
    file "${OMEGAPATH}\bin\toneomega.dll"
    file "${OMEGAPATH}\bin\trackdb.dll"
    file "${OMEGAPATH}\bin\trackinfo.dll"
    file "${OMEGAPATH}\bin\trackmodel.dll"
    file "${OMEGAPATH}\bin\vccorlib140.dll"
    file "${OMEGAPATH}\bin\vcruntime140.dll"
    file "${OMEGAPATH}\bin\violetomega.dll"
    file "${OMEGAPATH}\bin\wavpackdll.dll"
    file "${OMEGAPATH}\bin\wavpackomega.dll"
    file "${OMEGAPATH}\bin\whiteomega.dll"
    file "${OMEGAPATH}\bin\widget.dll"
    file "${OMEGAPATH}\bin\Black Omega.exe"
    file "${OMEGAPATH}\bin\omega.ico"
    file "${OMEGAPATH}\bin\qt.conf"
    setOutPath "$INSTDIR\plugins\platforms"
    file "${OMEGAPATH}\plugins\platforms\qwindows.dll"
    setOutPath "$INSTDIR\plugins\imageformats"
    file "${OMEGAPATH}\plugins\imageformats\qgif.dll"
    file "${OMEGAPATH}\plugins\imageformats\qjpeg.dll"
    setOutPath "$INSTDIR\plugins\sqldrivers"
    file "${OMEGAPATH}\plugins\sqldrivers\qsqlite.dll"

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

    delete "$INSTDIR\bin\audioio.dll"
    delete "$INSTDIR\bin\blackomega.dll"
    delete "$INSTDIR\bin\blueomega.dll"
    delete "$INSTDIR\bin\common.dll"
    delete "$INSTDIR\bin\concrt140.dll"
    delete "$INSTDIR\bin\cyanomega.dll"
    delete "$INSTDIR\bin\engine.dll"
    delete "$INSTDIR\bin\ftp.dll"
    delete "$INSTDIR\bin\greenomega.dll"
    delete "$INSTDIR\bin\http.dll"
    delete "$INSTDIR\bin\libiconv.dll"
    delete "$INSTDIR\bin\libxml2.dll"
    delete "$INSTDIR\bin\mime.dll"
    delete "$INSTDIR\bin\msvcp140.dll"
    delete "$INSTDIR\bin\msvcp140_1.dll"
    delete "$INSTDIR\bin\msvcp140_2.dll"
    delete "$INSTDIR\bin\network_omega.dll"
    delete "$INSTDIR\bin\playercommon.dll"
    delete "$INSTDIR\bin\Qt6Core.dll"
    delete "$INSTDIR\bin\Qt6Core5Compat.dll"
    delete "$INSTDIR\bin\Qt6Gui.dll"
    delete "$INSTDIR\bin\Qt6Widgets.dll"
    delete "$INSTDIR\bin\Qt6Xml.dll"
    delete "$INSTDIR\bin\redomega.dll"
    delete "$INSTDIR\bin\remote.dll"
    delete "$INSTDIR\bin\rtp.dll"
    delete "$INSTDIR\bin\rtp_silveromega.dll"
    delete "$INSTDIR\bin\silveromega.dll"
    delete "$INSTDIR\bin\toneomega.dll"
    delete "$INSTDIR\bin\trackdb.dll"
    delete "$INSTDIR\bin\trackinfo.dll"
    delete "$INSTDIR\bin\trackmodel.dll"
    delete "$INSTDIR\bin\vccorlib140.dll"
    delete "$INSTDIR\bin\vcruntime140.dll"
    delete "$INSTDIR\bin\violetomega.dll"
    delete "$INSTDIR\bin\wavpackdll.dll"
    delete "$INSTDIR\bin\wavpackomega.dll"
    delete "$INSTDIR\bin\whiteomega.dll"
    delete "$INSTDIR\bin\widget.dll"
    delete "$INSTDIR\bin\Black Omega.exe"
    delete "$INSTDIR\bin\omega.ico"
    delete "$INSTDIR\bin\qt.conf"
    setOutPath "$INSTDIR\plugins\platforms"
    delete "$INSTDIR\plugins\platforms\qwindows.dll"
    setOutPath "$INSTDIR\plugins\imageformats"
    delete "$INSTDIR\plugins\imageformats\qgif.dll"
    delete "$INSTDIR\plugins\imageformats\qjpeg.dll"
    setOutPath "$INSTDIR\plugins\sqldrivers"
    delete "$INSTDIR\plugins\sqldrivers\qsqlite.dll"
    delete "$INSTDIR\uninstall.exe"
    setOutPath ""
    rmDir "$INSTDIR\bin\"
    rmDir "$INSTDIR\plugins\sqldrivers"
    rmDir "$INSTDIR\plugins\imageformats"
    rmDir "$INSTDIR\plugins\platforms"
    rmDir "$INSTDIR\plugins"
    rmDir "$INSTDIR"
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"
sectionEnd

Function OmegaRun
    SetOutPath $INSTDIR
    ShellExecAsUser::ShellExecAsUser "" "$INSTDIR\bin\Black Omega.exe" ""
FunctionEnd
