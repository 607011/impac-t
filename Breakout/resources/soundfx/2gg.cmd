@echo off
ffmpeg -i "%1" -c:a libvorbis -q:a 4 "%~n1.ogg"