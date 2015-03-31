@ECHO OFF
FOR %%f in (*.mp3) DO (
  ffmpeg -y -i %%f -c:a libvorbis -q:a 4 %%~nf.ogg
)
