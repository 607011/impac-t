!define VERSION "0.0.1"
!define APP "Breakout"
!define PUBLISHER "c't"
!define SFMLPATH "D:\Developer\SFML-2.1"

Name "${APP} ${VERSION}"
OutFile "${APP}-${VERSION}-setup.exe"
InstallDir $PROGRAMFILES\${APP}
InstallDirRegKey HKLM "Software\${PUBLISHER}\${APP}" "Install_Dir"
RequestExecutionLevel admin
SetCompressor lzma
ShowInstDetails show

Page license

  LicenseData ..\Breakout\LICENSE

Page directory

Page instfiles


Section "Gee"
  SetOutPath $INSTDIR
  CreateDirectory "$INSTDIR\resources"
  CreateDirectory "$INSTDIR\resources\backgrounds"
  CreateDirectory "$INSTDIR\resources\fonts"
  CreateDirectory "$INSTDIR\resources\images"
  CreateDirectory "$INSTDIR\resources\levels"
  CreateDirectory "$INSTDIR\resources\soundfx"
  File ..\Release\Breakout.exe
  File ..\zlib\zlib1.dll
  File glew32.dll
  File ${SFMLPATH}\bin\libsndfile-1.dll
  File ${SFMLPATH}\bin\openal32.dll
  File ${SFMLPATH}\bin\sfml-audio-2.dll
  File ${SFMLPATH}\bin\sfml-graphics-2.dll
  File ${SFMLPATH}\bin\sfml-system-2.dll
  File ${SFMLPATH}\bin\sfml-window-2.dll
  File ${SFMLPATH}\bin\sfml-network-2.dll
  File ..\Breakout\LICENSE
  WriteUninstaller $INSTDIR\uninstall.exe

  SetOutPath $INSTDIR\resources\backgrounds
  File /a /r "..\Breakout\resources\backgrounds\"

  SetOutPath $INSTDIR\resources\fonts
  File /a /r "..\Breakout\resources\fonts\"

  SetOutPath $INSTDIR\resources\images
  File /a /r "..\Breakout\resources\images\"

  SetOutPath $INSTDIR\resources\levels
  File /a /r "..\Breakout\resources\levels\"

  SetOutPath $INSTDIR\resources\soundfx
  File /a /r "..\Breakout\resources\soundfx\"

SectionEnd


Section "Start Menu Shortcuts"
  CreateDirectory "$SMPROGRAMS\${APP}"
  CreateShortCut "$SMPROGRAMS\${APP}\${APP} ${VERSION}.lnk" "$INSTDIR\${APP}.exe"
  CreateShortcut "$SMPROGRAMS\${APP}\Uninstall.lnk" "$INSTDIR\uninstall.exe" "" "$INSTDIR\uninstall.exe" 0
SectionEnd


Section "Uninstall"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\${APP}"
  DeleteRegKey HKLM "SOFTWARE\${APP}"

  Delete $INSTDIR\LICENSE
  Delete $INSTDIR\zlib1.dll
  Delete $INSTDIR\libsndfile-1.dll
  Delete $INSTDIR\openal32.dll
  Delete $INSTDIR\sfml-audio-2.dll
  Delete $INSTDIR\sfml-graphics-2.dll
  Delete $INSTDIR\sfml-system-2.dll
  Delete $INSTDIR\sfml-window-2.dll
  Delete $INSTDIR\sfml-network-2.dll
  Delete $INSTDIR\glew32.dll
  Delete $INSTDIR\Breakout.exe
  Delete $INSTDIR\uninstall.exe

  RMDir /r $INSTDIR\resources
  RMDir $INSTDIR

  Delete "$SMPROGRAMS\Breakout\*.*"
  RMDir "$SMPROGRAMS\Breakout"
  RMDir "$INSTDIR"
SectionEnd
