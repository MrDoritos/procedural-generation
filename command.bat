@echo off
cd /D "H:\cpp-projects\procedural"
echo %CD%

cmd /c g++ random.cpp ..\console\console.windows.cpp -I..\console\ -w -ggdb -o procedural.exe %* -fpermissive
::-O2 -march=native -mtune=native -I..\..\console-gui\headers\ ..\..\console\lodepng.cpp ..\..\console\pixel.cpp
if "%errorlevel%" NEQ "0" (
pause
exit /b
)
procedural.exe
if "%errorlevel%" NEQ "0" (
echo %errorlevel%
pause
exit /b
)

::g++ generator3d.cpp random.cpp ..\..\console\console.windows.cpp procedural.cpp -I..\..\console-gui\headers\ -I..\..\console\ -w -ggdb %*


