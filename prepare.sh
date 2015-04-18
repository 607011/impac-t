#!/bin/bash

DIR=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )

echo "creating directories in ~/.impact/"
if [ ! -d ~/.impact ]; then
   mkdir ~/.impact
fi
if [ ! -d ~/.impact/levels ]; then
   mkdir ~/.impact/levels
fi
if [ ! -d ~/.impact/soundfx ]; then
   mkdir ~/.impact/soundfx
fi
if [ ! -d ~/.impact/music ]; then
   mkdir ~/.impact/music
fi


echo "copying levels"
cp $DIR/Impact/resources/levels/*.zip ~/.impact/levels/

echo "copying sound fx ..."
cp $DIR/Impact/resources/soundfx/*.ogg ~/.impact/soundfx/

echo "copying music ..."
cp $DIR/Impact/resources/music/*.ogg ~/.impact/music/

echo "copying settings.xml ..."
cp $DIR/Impact/settings.xml ~/.impact/settings.xml
