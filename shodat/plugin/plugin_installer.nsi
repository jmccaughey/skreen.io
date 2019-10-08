; example1.nsi
;
; This script is perhaps one of the simplest NSIs you can make. All of the
; optional settings are left to their default settings. The installer simply 
; prompts the user asking them where to install, and drops a copy of example1.nsi
; there. 

;--------------------------------

; The name of the installer
Name "shodat"

; The file to write
OutFile "build/shodat.exe"

; The default installation directory
InstallDir $APPDATA\Mozilla\plugins

; Request application privileges for Windows Vista
RequestExecutionLevel user
SilentInstall silent
;--------------------------------

; Pages

;Page directory
;Page instfiles

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  
  ; Put file there
  File build/npshodat.dll  
  WriteUninstaller $INSTDIR\shodat_plugin_uninstaller.exe
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\shodat" "DisplayName" "shodat Plugin"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\shodat" "UninstallString" "$INSTDIR\shodat_plugin_uninstaller.exe"
  WriteRegStr HKCU "Software\Microsoft\Windows\CurrentVersion\Uninstall\shodat" "Publisher" "WebHuddle, Inc"
   MessageBox MB_OK|MB_ICONINFORMATION "shodat plugin install complete. Click 'Show My Desktop' to start sharing!"  
SectionEnd ; end the section

Section "Uninstall"
  Delete $INSTDIR\npshodat.dll
  Delete $INSTDIR\shodat_plugin_uninstaller.exe ; delete self (see explanation below why this works)
  DeleteRegKey HKCU "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\shodat"
SectionEnd
