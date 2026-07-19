@echo off
echo Compiling Donation Transparency System...

echo Building Resource File...
windres resource.rc -O coff -o resource.res

echo Building CLI Version...
g++ src/cli/main_cli.cpp src/core/*.cpp src/ds/*.cpp -o donation_system_cli.exe -O2 -Wall -Wno-missing-braces

echo Building GUI Version...
g++ src/gui/main_gui.cpp src/core/*.cpp src/ds/*.cpp src/gui/*.cpp resource.res -o donation_system_gui.exe -O2 -Wall -Wno-missing-braces -lraylib -lopengl32 -lgdi32 -lwinmm -mwindows

if %ERRORLEVEL% == 0 (
    echo Build successful.
    echo Run donation_system_cli.exe for Command Line Interface
    echo Run donation_system_gui.exe for Raylib GUI
) else (
    echo Build failed.
)
