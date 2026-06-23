@echo off

echo ===================================================
echo   University Database - Compile and Run Script
echo ===================================================

:: Add local w64devkit compiler to path
set "PATH=%~dp0w64devkit\bin;%PATH%"

:: Verify compiler
where g++ >nul 2>nul
if errorlevel 1 goto nocompiler

echo [1/2] Compiling C++ Backend...
g++ -std=c++17 -O3 backend/main.cpp -lws2_32 -lcrypt32 -o server.exe
if errorlevel 1 goto compfail

echo [2/2] Compilation successful! Starting localhost server...
echo.
echo Server is running! Open your browser and navigate to:
echo -------------------------------------
echo   http://localhost:8080
echo -------------------------------------
echo.
echo Press Ctrl+C in this terminal window to stop the server.
echo.

server.exe
goto end

:nocompiler
echo [ERROR] C++ Compiler g++ not found in local path.
echo Please run setup_toolchain.py first.
pause
exit /b 1

:compfail
echo.
echo [ERROR] Compilation failed.
pause
exit /b 1

:end
