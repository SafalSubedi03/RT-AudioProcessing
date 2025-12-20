@echo off
REM ------------------------------
REM Build ANC Project (Windows/MSVC)
REM ------------------------------

REM Initialize Visual Studio environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

REM Create build folder if it doesn't exist
if not exist build mkdir build

REM Compile all .cpp files in src folder
for %%f in (src\*.cpp) do (
    echo Compiling %%f...
    cl.exe /c /EHsc "%%f" /I include /I ..\..\libs /Fo"build\%%~nf.obj"
)

REM Link all object files in build folder into executable
echo Linking...
cl.exe build\*.obj /Fe:build\ANCProject.exe ..\..\libs\portaudio_x64.lib

REM Copy DLL to build folder
echo Copying DLL...
copy ..\..\libs\portaudio_x64.dll build\

echo Build finished.
pause
