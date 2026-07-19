@echo off
echo Compiling Donation Transparency System...
g++ src/main.cpp src/core/*.cpp src/ds/*.cpp -o donation_system.exe
if %ERRORLEVEL% == 0 (
    echo Build successful. Run donation_system.exe to start.
) else (
    echo Build failed.
)
