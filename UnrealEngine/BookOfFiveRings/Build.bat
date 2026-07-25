@echo off
setlocal

:: Detect UE 5.8 install location
set UE_ROOT=C:\Program Files\Epic Games\UE_5.8
if not exist "%UE_ROOT%" set UE_ROOT=C:\Program Files\Epic Games\UE_5.8.0

if not exist "%UE_ROOT%" (
    echo ERROR: Could not find UE 5.8 at "%UE_ROOT%"
    echo Please edit this file and set UE_ROOT to your Unreal Engine 5.8 install path.
    pause
    exit /b 1
)

set PROJECT=%~dp0BookOfFiveRings.uproject
set BUILD="%UE_ROOT%\Engine\Build\BatchFiles\Build.bat"

echo Building BookOfFiveRings (Development Editor, Win64)...
call %BUILD% BookOfFiveRingsEditor Win64 Development "%PROJECT%" -WaitMutex -FromMsBuild
echo.
echo Build exited with code %ERRORLEVEL%
pause
