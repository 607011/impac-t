@ECHO OFF
SET CWD=%~dp0
SET MYPATH=%CWD:~0,-1%

ECHO Creating directories ...
IF NOT EXIST "%APPDATA%\Impact\" ( MD %APPDATA%\Impact )
IF NOT EXIST "%APPDATA%\Impact\levels\" ( MD "%APPDATA%\Impact\levels" )
IF NOT EXIST "%APPDATA%\Impact\soundfx\" ( MD "%APPDATA%\Impact\soundfx" )
IF NOT EXIST "%APPDATA%\Impact\music\" ( MD "%APPDATA%\Impact\music" )

ECHO Copying levels ...
@COPY "%MYPATH%\Impact\resources\levels\*.zip" "%APPDATA%\Impact\levels"

echo Copying sound fx ...
@COPY "%MYPATH%\Impact\resources\soundfx\*.ogg" "%APPDATA%\Impact\soundfx"

echo Copying music ...
@COPY "%MYPATH%\Impact\resources\music\*.ogg" "%APPDATA%\Impact\music"

echo Copying settings.xml ...
@COPY "%MYPATH%\Impact\settings.xml" "%APPDATA%\Impact\settings.xml"
