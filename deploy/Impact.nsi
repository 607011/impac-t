!define VERSIONMAJOR "1"
!define VERSIONMINOR "0"
!define VERSION "${VERSIONMAJOR}.${VERSIONMINOR}-BETA14"
!define GUID "{95E41A25-7E41-45CA-A1F6-0FFAB66A1B2F}"
!define APP "Impact"
!define PUBLISHER "Heise Media GmbH - Redaktion c't"
!define SFMLPATH "D:\Developer\SFML-2.2"
!define GLEWPATH "D:\Developer\glew-1.12.0\bin\Release\Win32"
!define ZLIBPATH "..\zlib"

!include "LogicLib.nsh"
!include "FileFunc.nsh"

Name "${APP} ${VERSION}"
OutFile "${APP}-${VERSION}-setup.exe"
InstallDir $PROGRAMFILES\${APP}
InstallDirRegKey HKLM "Software\${PUBLISHER}\${APP}" "Install_Dir"
RequestExecutionLevel admin
SetCompressor lzma
ShowInstDetails show

Page license

  LicenseData "..\${APP}\LICENSE"

Page directory

Page instfiles


Section "vcredist"
  ClearErrors
  ReadRegDword $R0 HKLM "SOFTWARE\Wow6432Node\Microsoft\DevDiv\vc\Servicing\12.0\RuntimeMinimum" "Version"
  ${If} $R0 != "12.0.21005"
    SetOutPath "$INSTDIR"
    File "vcredist_x86.exe"
    ExecWait '"$INSTDIR\vcredist_x86.exe" /norestart /passive'
    Delete "$INSTDIR\vcredist_x86.exe"
  ${EndIf}
SectionEnd


Section "${APP}"
  SetOutPath "$INSTDIR"
  CreateDirectory "$INSTDIR\resources"
  CreateDirectory "$INSTDIR\resources\backgrounds"
  CreateDirectory "$INSTDIR\resources\fonts"
  CreateDirectory "$INSTDIR\resources\images"
  CreateDirectory "$INSTDIR\resources\levels"
  CreateDirectory "$INSTDIR\resources\music"
  CreateDirectory "$INSTDIR\resources\shaders"
  CreateDirectory "$INSTDIR\resources\soundfx"
  File "..\Release\${APP}.exe"
  File "..\${APP}\LICENSE"
  File "..\${APP}\app-icon.ico"
  File "..\README.md"
  File "${ZLIBPATH}\zlib1.dll"
  File "${SFMLPATH}\bin\libsndfile-1.dll"
  File "${SFMLPATH}\bin\openal32.dll"
  File "${SFMLPATH}\bin\sfml-audio-2.dll"
  File "${SFMLPATH}\bin\sfml-graphics-2.dll"
  File "${SFMLPATH}\bin\sfml-system-2.dll"
  File "${SFMLPATH}\bin\sfml-window-2.dll"
  File "${SFMLPATH}\bin\sfml-network-2.dll"
  File "${GLEWPATH}\glew32.dll"
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "DisplayName" "${APP}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "DisplayIcon" "$INSTDIR\app-icon.ico"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "Publisher" "${PUBLISHER}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "NoRepair" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "VersionMajor" "${VERSIONMAJOR}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "VersionMinor" "${VERSIONMINOR}"

  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "EstimatedSize" "$0"

  SetOutPath "$INSTDIR\resources\backgrounds"
  File /a /r "..\${APP}\resources\backgrounds\"

  SetOutPath "$INSTDIR\resources\fonts"
  File /a /r "..\${APP}\resources\fonts\"

  SetOutPath "$INSTDIR\resources\images"
  File /a /r "..\${APP}\resources\images\"

  SetOutPath "$INSTDIR\resources\levels"
  File /a /r "..\${APP}\resources\levels\"

;  SetOutPath "$INSTDIR\resources\music"
;  File /a /r "..\${APP}\resources\music\"

  SetOutPath "$INSTDIR\resources\shaders"
  File /a /r "..\${APP}\resources\shaders\"

  SetOutPath "$INSTDIR\resources\soundfx"
  File /a /r "..\${APP}\resources\soundfx\*.ogg"

  SetOutPath "$INSTDIR"

SectionEnd


Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\${APP}"
  CreateShortCut "$SMPROGRAMS\${APP}\${APP} ${VERSION}.lnk" "$INSTDIR\${APP}.exe"
  CreateShortcut "$SMPROGRAMS\${APP}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd


Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}"
  DeleteRegKey HKLM "SOFTWARE\${APP}"

  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\glew32.dll"
  Delete "$INSTDIR\zlib1.dll"
  Delete "$INSTDIR\libsndfile-1.dll"
  Delete "$INSTDIR\openal32.dll"
  Delete "$INSTDIR\sfml-audio-2.dll"
  Delete "$INSTDIR\sfml-graphics-2.dll"
  Delete "$INSTDIR\sfml-system-2.dll"
  Delete "$INSTDIR\sfml-window-2.dll"
  Delete "$INSTDIR\sfml-network-2.dll"
  Delete "$INSTDIR\${APP}.exe"
  Delete "$INSTDIR\uninstall.exe"

  RMDir /r "$INSTDIR\resources"
  
  Delete "$SMPROGRAMS\${APP}\*.*"
  RMDir "$SMPROGRAMS\${APP}"
  RMDir "$INSTDIR"
SectionEnd
