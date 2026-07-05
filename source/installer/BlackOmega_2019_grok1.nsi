!include "MUI2.nsh"
!include "x64.nsh"
!include "LogicLib.nsh"

!define APPNAME "Black Omega"
!define COMPANYNAME "Stuart A. MacLean"

!include "Version.nsh"

!define HELPURL "http://www.blackomega.co.uk"
!define UPDATEURL "http://www.blackomega.co.uk"
!define ABOUTURL "http://www.blackomega.co.uk"

!define INSTALLSIZE 36046

!define OMEGAPATH "..\..\Build\Omega"

; === Visual C++ Redistributable ===
!define VC_REDIST_URL      "https://aka.ms/vs/17/release/vc_redist.x64.exe"
!define VC_REDIST_FILENAME "vc_redist.x64.exe"

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
; Custom Function: Install VC++ Redist if needed
Function InstallVCRedistributable
    DetailPrint "Checking for Visual C++ 2015-2022 Redistributable..."

    ; Check registry (x64)
    ReadRegDWORD $0 HKLM "SOFTWARE\Microsoft\VisualStudio\14.0\VC\Runtimes\x64" "Installed"
    
    ${If} $0 != 1
        DetailPrint "Visual C++ Redistributable not found. Installing..."
        
        SetOutPath "$INSTDIR"
        NSISdl::download /TIMEOUT=30000 "${VC_REDIST_URL}" "$INSTDIR\${VC_REDIST_FILENAME}"
        
        Pop $0
        ${If} $0 != "success"
            MessageBox MB_ICONEXCLAMATION|MB_OK "Failed to download Visual C++ Redistributable.$\nPlease install it manually from Microsoft."
            Goto done
        ${EndIf}

        ExecWait '"$INSTDIR\${VC_REDIST_FILENAME}" /install /quiet /norestart' $1
        
        ${If} $1 != 0
            MessageBox MB_ICONEXCLAMATION|MB_OK "Visual C++ Redistributable installation failed (error $1)."
        ${Else}
            DetailPrint "Visual C++ Redistributable installed successfully."
        ${EndIf}
        
        Delete "$INSTDIR\${VC_REDIST_FILENAME}"
    ${Else}
        DetailPrint "Visual C++ 2015-2022 Redistributable is already installed."
    ${EndIf}
    
    done:
FunctionEnd

;--------------------------------
section "Black Omega" SecDummy
    Call InstallVCRedistributable

    SetOverwrite on
    setOutPath "$INSTDIR\bin"
    
    ; === REMOVE all the old MSVC DLL lines ===
    ; file "${OMEGAPATH}\bin\concrt140.dll"
    ; file "${OMEGAPATH}\bin\msvcp140.dll"
    ; file "${OMEGAPATH}\bin\msvcp140_1.dll"
    ; file "${OMEGAPATH}\bin\msvcp140_2.dll"
    ; file "${OMEGAPATH}\bin\vccorlib140.dll"
    ; file "${OMEGAPATH}\bin\vcruntime140.dll"

    file "${OMEGAPATH}\bin\audioio.dll"
    file "${OMEGAPATH}\bin\blackomega.dll"
    ; ... rest of your files ...
    file "${OMEGAPATH}\bin\Black Omega.exe"
    file "${OMEGAPATH}\bin\omega.ico"
    file "${OMEGAPATH}\bin\qt.conf"

    ; plugins ...
    
    writeUninstaller "$INSTDIR\uninstall.exe"
    ; ... rest of your section ...
sectionEnd