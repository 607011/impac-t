@echo off
for %%f in (*.ogg) do (
  ffmpeg -y -i "%%f" -acodec libmp3lame "%%~nf.mp3"
)