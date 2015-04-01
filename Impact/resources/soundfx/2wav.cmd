@echo off
for %%f in (*.ogg) do (
  ffmpeg -i "%%f" "%%~nf.wav"
)
  