@echo off
set version=1.0.0
set scriptFolder=%~dp0

set buildIndex=src\window.c
set buildName=build\simulation.exe

echo Starting build

echo.
echo Removing Existing Build
echo.

del %scriptFolder%\%buildName%

echo.
echo Starting Build
echo.

gcc %buildIndex% -lcomctl32

echo.
echo Finished Build
echo.

exit /b 0