@echo off
echo Compiling Donation Transparency System (Raylib UI)...
g++ src/main.cpp src/core/*.cpp src/ds/*.cpp src/gui/*.cpp -o donation_system.exe -O2 -Wall -Wno-missing-braces -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows
if %ERRORLEVEL% == 0 (
    echo Build successful. Run donation_system.exe to start.
) else (
    echo Build failed.
)
