#!/bin/bash
#@echo off
#cd /D "F:\file-catalog\other\procedural"
#echo %CD%

g++ random.cpp ../console/console.linux.cpp -I../console/ -w -ggdb -o procedural $@ -fpermissive
#-O2 -march=native -mtune=native -I..\..\console-gui\headers\ ..\..\console\lodepng.cpp ..\..\console\pixel.cpp
#g++ generator3d.cpp random.cpp ..\..\console\console.windows.cpp procedural.cpp -I..\..\console-gui\headers\ -I..\..\console\ -w -ggdb %*


