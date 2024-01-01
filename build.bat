@echo off
set version=1.0.0
set scriptFolder=%~dp0

set buildEntry=src\window.c
set buildOutput=build\simulation.exe

echo Starting build

echo.
echo Removing Existing Build
echo.

del %scriptFolder%\%buildOutput%

echo.
echo Starting Build
echo.

gcc %scriptFolder%\%buildEntry% -lcomctl32 -lgdi32 -o%scriptFolder%\%buildOutput%

echo.
echo Finished Build
echo.

exit /b 0