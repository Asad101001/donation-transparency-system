# Donation Transparency System

A robust, purely procedural C++ application designed to manage, allocate, and track welfare donations with 100% data transparency. This system acts as a digital ledger, providing donors with chronological tracking of exactly where their funds are assigned and utilized using custom, strictly-implemented Data Structures and Algorithms (DSA).

## Architecture & Data Structures
The core engine is built without any Object-Oriented Programming (OOP) or Standard Template Library (STL) containers, relying solely on C-style procedural architecture:
- **Hash Tables**: For lightning-fast, `O(1)` retrieval of Donor profiles.
- **AVL Trees**: For balanced, sorted, and efficient management of Welfare Projects.
- **Doubly Linked Lists**: To chronologically record and navigate global donation ledgers.
- **Queues**: For automated, FIFO-based fund allocation systems.
- **Stacks**: Embedded within donations to track their chronological state transitions (Received -> Assigned -> Spent).

## User Interfaces
This project provides two identical interfaces running on the same underlying procedural engine:
1. **Raylib GUI**: A modern, click-driven graphical interface with dynamic accordion animations, robust layout management, interactive submodules, and an interactive tracking dashboard.
2. **Command Line Interface (CLI)**: A lightweight, menu-driven terminal experience.

## Compilation & Execution
The system is built for Windows using the MinGW `g++` compiler, with Raylib bundled for the graphical interface.

1. Ensure you have `g++` and `windres` installed via MinGW.
2. Run `build.bat` in the root directory.
3. The script will automatically compile the icon resources and generate two executables:
   - `donation_system_gui.exe`
   - `donation_system_cli.exe`
