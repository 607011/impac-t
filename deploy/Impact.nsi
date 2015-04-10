!define VERSIONMAJOR "1"
!define VERSIONMINOR "0"
!define VERSION "${VERSIONMAJOR}.${VERSIONMINOR}"
!define GUID "{95E41A25-7E41-45CA-A1F6-0FFAB66A1B2F}"
!define APP "Impact"
!define PUBLISHER "Heise Medien GmbH & Co. KG - Redaktion c't"

Name "${APP} ${VERSION}"
OutFile "${APP}-${VERSION}-setup.exe"
InstallDir $PROGRAMFILES\${APP}
InstallDirRegKey HKLM "Software\${PUBLISHER}\${APP}" "Install_Dir"
RequestExecutionLevel admin
SetCompressor lzma
ShowInstDetails show

# !include "MUI2.nsh"
!include "LogicLib.nsh"
!include "FileFunc.nsh"

# !define MUI_FINISHPAGE_RUN "$INSTDIR\${APP}.exe"
# !define MUI_FINISHPAGE_RUN_FUNCTION "LaunchLink"
# !define MUI_FINISHPAGE_RUN_TEXT "${APP} starten"

# Function LaunchLink
#   SetOutPath $INSTDIR
#   ExecShell "" '"$INSTDIR\${APP}.exe"'
# FunctionEnd



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
  CreateDirectory "$INSTDIR\resources\fonts"
  CreateDirectory "$INSTDIR\resources\images"
  CreateDirectory "$INSTDIR\resources\shaders"
  CreateDirectory "$APPDATA\${APP}"
  CreateDirectory "$APPDATA\${APP}\levels"
  CreateDirectory "$APPDATA\${APP}\soundfx"
  CreateDirectory "$APPDATA\${APP}\music"
  File "..\Release\${APP}.exe"
  File "..\${APP}\LICENSE"
  File "..\${APP}\LICENSE.md"
  File "..\${APP}\TODO.md"
  File "..\${APP}\exe-icon.ico"
  File "..\README.md"
  File "..\Release\glew32.dll"
  File "..\Release\zlib.dll"
  File "..\Release\libsndfile-1.dll"
  File "..\Release\openal32.dll"
  File "..\Release\sfml-audio-2.dll"
  File "..\Release\sfml-graphics-2.dll"
  File "..\Release\sfml-system-2.dll"
  File "..\Release\sfml-window-2.dll"
  File "..\ffmpeg\bin\avcodec-56.dll"
  File "..\ffmpeg\bin\avformat-56.dll"
  File "..\ffmpeg\bin\swscale-3.dll"
  File "..\ffmpeg\bin\swresample-1.dll"
  File "..\ffmpeg\bin\avutil-54.dll"
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "DisplayName" "${APP}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "DisplayIcon" "$INSTDIR\exe-icon.ico"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "Publisher" "${PUBLISHER}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "UninstallString" '"$INSTDIR\uninstall.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "NoRepair" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "VersionMajor" "${VERSIONMAJOR}"
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "VersionMinor" "${VERSIONMINOR}"

  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}" "EstimatedSize" "$0"

  SetOutPath "$INSTDIR\resources\fonts"
  File /a /r "..\${APP}\resources\fonts\"

  SetOutPath "$INSTDIR\resources\images"
  File /a /r "..\${APP}\resources\images\"

  SetOutPath "$INSTDIR\resources\shaders"
  File /a /r "..\${APP}\resources\shaders\"

  SetOutPath "$APPDATA\${APP}\levels"
  File /a /r "..\${APP}\resources\levels\"

  SetOutPath "$APPDATA\${APP}\soundfx"
  File /a /r "..\${APP}\resources\soundfx\*.ogg"

  SetOutPath "$APPDATA\${APP}\music"
  File /a /r "..\${APP}\resources\music\*.ogg"

  SetOutPath "$APPDATA\${APP}"
  File "..\${APP}\settings.xml"

  SetOutPath "$INSTDIR"

SectionEnd


Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\${APP}"
  CreateShortCut "$SMPROGRAMS\${APP}\${APP} ${VERSION}.lnk" "$INSTDIR\${APP}.exe"
  CreateShortcut "$SMPROGRAMS\${APP}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd


Section "Desktop Icon"
  CreateShortCut "$DESKTOP\${APP}-${VERSION}.lnk" "$INSTDIR\${APP}.exe" ""
SectionEnd


# !insertmacro MUI_PAGE_FINISH

Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${GUID}"
  DeleteRegKey HKLM "SOFTWARE\${APP}"

  Delete "$INSTDIR\README.md"
  Delete "$INSTDIR\LICENSE"
  Delete "$INSTDIR\glew32.dll"
  Delete "$INSTDIR\zlib.dll"
  Delete "$INSTDIR\libsndfile-1.dll"
  Delete "$INSTDIR\openal32.dll"
  Delete "$INSTDIR\sfml-audio-2.dll"
  Delete "$INSTDIR\sfml-graphics-2.dll"
  Delete "$INSTDIR\sfml-system-2.dll"
  Delete "$INSTDIR\sfml-window-2.dll"
  Delete "$INSTDIR\avcodec-56.dll"
  Delete "$INSTDIR\avformat-56.dll"
  Delete "$INSTDIR\avutil-54.dll"
  Delete "$INSTDIR\swscale-3.dll"
  Delete "$INSTDIR\swresample-1.dll"
  Delete "$INSTDIR\exe-icon.ico"
  Delete "$INSTDIR\${APP}.exe"
  Delete "$INSTDIR\uninstall.exe"

  Delete "$DESKTOP\${APP}-${VERSION}.lnk"
  Delete "$SMPROGRAMS\${APP}\*.*"
  RMDir "$SMPROGRAMS\${APP}"

  RMDir /r "$INSTDIR\resources"
  RMDir "$INSTDIR"
SectionEnd
