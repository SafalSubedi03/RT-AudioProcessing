@echo off
REM Initialize Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Create build folder if it doesn't exist
if not exist build mkdir build

REM Compile each cpp file to build folder
for %%f in (*.cpp) do cl.exe /c /EHsc "%%f" /I ..\..\libs /Fo"build\%%~nf.obj"

REM Link all object files in build folder into executable
cl.exe build\*.obj /Fe:build\LearnProject.exe ..\..\libs\portaudio_x64.lib

REM Copy DLL to build folder
copy ..\..\libs\portaudio_x64.dll build\

pause
