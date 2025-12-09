@echo off
setlocal

:: MSVC Environment
call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvarsall.bat" x64

set ROOTDIR=%~dp0
set OUTDIR=%ROOT%bin\

if not exist "%OUTDIR%" mkdir "%OUTDIR%"

set UNITY_FILE=%ROOT%src\build.c

set CFLAGS=/nologo /W3 /Zi /O2 /MD

:: Unity Build
cl %CFLAGS% %INCLUDES% "%UNITY_FILE%" ^
    /Fe:"%OUTDIR%out.exe"

endlocal
