<div align="center">
  <img src="https://img.shields.io/badge/C++-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++" />
  <img src="https://img.shields.io/badge/Raylib-000000?style=for-the-badge&logo=raylib&logoColor=white" alt="Raylib" />
  <img src="https://img.shields.io/badge/Windows-0078D6?style=for-the-badge&logo=windows&logoColor=white" alt="Windows" />
  <img src="https://img.shields.io/badge/Architecture-Procedural-success?style=for-the-badge" alt="Procedural" />

  <h1>Donation Transparency System</h1>
  <p><em>A robust, strictly procedural C++ application for managing, allocating, and chronologically tracking welfare donations using custom Data Structures.</em></p>
</div>

---

## 📖 Overview

The **Donation Transparency System** acts as a high-fidelity digital ledger, ensuring 100% data transparency for non-profit organizations and welfare projects. It allows donors to chronologically track exactly where their funds are assigned and utilized.

Built entirely **without Object-Oriented Programming (OOP) or the Standard Template Library (STL)**, this engine relies on raw C-style procedural architecture. Every internal system is powered by custom-built, optimized Data Structures and Algorithms (DSA) developed from scratch.

## ✨ Features

- **End-to-End Fund Tracking**: Track every penny from the moment a donation is received, assigned to a project, and eventually spent.
- **Dual Interfaces**: 
  - **Raylib GUI**: A visually modern, click-driven graphical interface with dynamic accordion animations, robust layout management, and an interactive tracking dashboard.
  - **CLI Mode**: A lightning-fast, lightweight menu-driven terminal experience.
- **Zero-Setup Cloning**: All required dependencies (Raylib 5.0) are bundled directly into the repository. Just clone and build!

## 🧠 Data Structures & Architecture

The core logic operates entirely on custom implementations to guarantee extreme performance and strict memory control:

| Data Structure | Implementation Purpose |
|----------------|------------------------|
| **Hash Tables** | Provides lightning-fast, `O(1)` retrieval and storage of Donor profiles. |
| **AVL Trees** | Ensures balanced, sorted, and highly efficient management of Welfare Projects. |
| **Doubly Linked Lists** | Chronologically records and easily navigates global and local donation ledgers. |
| **Queues** | Manages automated, FIFO-based fund allocation systems. |
| **Stacks** | Embedded within individual donations to track strict chronological state transitions (`Received` ➔ `Assigned` ➔ `Spent`). |

## 🚀 Quick Start (Windows)

Because all necessary libraries are bundled, getting the project running on Windows takes only seconds.

### Prerequisites
- **MinGW** with `g++` and `windres` installed and added to your system `PATH`.

### Installation & Build

1. Clone the repository:
   ```bash
   git clone https://github.com/Asad101001/donation-transparency-system.git
   cd donation-transparency-system
   ```

2. Run the automated build script:
   ```bat
   .\build.bat
   ```

3. Launch your preferred interface:
   - **Graphical Version**: `donation_system_gui.exe`
   - **Terminal Version**: `donation_system_cli.exe`

## 📂 Project Structure

```text
📦 donation-transparency-system
 ┣ 📂 assets/        # Application icons and compiled resource files
 ┣ 📂 include/       # Bundled Raylib 5.0 Headers (Zero setup!)
 ┣ 📂 lib/           # Bundled Raylib 5.0 Static Libraries
 ┣ 📂 src/
 ┃ ┣ 📂 cli/         # Command Line Interface logic
 ┃ ┣ 📂 core/        # Core system engines and managers
 ┃ ┣ 📂 ds/          # Custom Data Structure implementations
 ┃ ┗ 📂 gui/         # Raylib graphical interface logic
 ┣ 📜 build.bat      # Automated compilation script
 ┗ 📜 README.md      # Documentation
```

## 📝 License & Academic Honesty

This project was developed with strict adherence to procedural C-style design patterns. It serves as a comprehensive demonstration of applied Data Structures & Algorithms.
