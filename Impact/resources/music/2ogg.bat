@ECHO OFF
FOR %%f in (*.wav) DO (
  ffmpeg -y -i %%f -c:a libvorbis -q:a 4 %%~nf.ogg
)
