@echo off
for %%f in (*.wav) do (
  ffmpeg -i "%%f" -c:a libvorbis -q:a 5 "%%~nf.ogg"
)
