# QtLite

A lightweight GUI wrapper for compiling and running C++, Qt, and Python projects on Windows without needing a full IDE like Qt Creator or Visual Studio.

## The Problem
Sometimes you just want to test a simple code snippet (like a single `main.cpp` or `script.py` file) without creating a complex CMake or qmake project, opening an IDE, and managing build configurations.

## The Solution
`QtLite` is a simple tool that does all the work for you. You write your code in the editor, click "Build & Run," and it handles the entire build process behind the scenes.

### Key Features

- **Multi-Language Support**: C++ (Qt), C/C++ (Console), and Python
- **Automatic Qt Module Detection**: Scans your code for `#include` directives
- **Integrated Code Editor**: Syntax highlighting, line numbers, and tab management
- **Code Snippets**: Pre-built templates for common code patterns
- **Code Bundler**: Merge multiple files into a single file for easy sharing
- **File Browser**: Navigate and open project files
- **Settings Persistence**: Your configurations are saved automatically

### How It Works (for C++/Qt)

It automatically:
1.  Scans your code for `#include` directives to detect required Qt modules (e.g., `widgets`, `webenginewidgets`).
2.  Creates a temporary `.pro` file.
3.  Executes the MSVC build environment (`vcvarsall.bat`).
4.  Runs `qmake.exe` and `nmake.exe` (from your MSVC-based Qt install) to build your code.
5.  Runs the resulting `.exe` file.

## Prerequisites
To *use* this tool, you need the **MSVC** toolchain installed (for C++/Qt development):

* **Visual Studio 2022:** You must install the **"Desktop development with C++"** workload.
* **Qt 6 (MSVC Version):** From the Qt Installer, you must select the `MSVC 2022 64-bit` component.
* **Qt WebEngine:** (And any other modules you want to use) installed for the `MSVC 2022 64-bit` component.

For Python development:
* **Python 3.x:** Download from https://www.python.org/downloads/

## Documentation

For detailed usage instructions, see the [Usage Guide](docs/usage_guide.md).

## First-Time Setup
When you first run `QtLite`, you will need to configure the paths to your build tools. **You only need to do this once.**

1.  Click the **"Browse..."** button for **"Qt Version Directory"**.
2.  Navigate to your Qt *version* folder (the one containing `msvc2022_64`).
    * *Example: `C:\Qt\6.10.0`*
3.  Click the **"Browse..."** button for **"Visual Studio Directory"**.
4.  Navigate to your Visual Studio installation folder (e.g., `Community`, `Professional`).
    * *Example: `C:\Program Files\Microsoft Visual Studio\2022\Community`*

The paths will be saved automatically. Once both paths are set correctly (the text boxes are white), the "Build & Run" button will become enabled.

## Running Tests

To run the unit tests, each test is built as a separate executable:

1. Navigate to the `tests` directory
2. Run qmake for a specific test:
   ```cmd
   C:\Qt\6.10.0\mingw_64\bin\qmake.exe "TEST=test_pathvalidator" tests.pro
   ```
3. Build and run the test:
   ```cmd
   C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe
   release\test_pathvalidator.exe
   ```

See `tests/README.md` for more details on running all tests.

## How to Build `QtLite` (the Tool Itself)
To compile `QtLite.exe` from this repository:

1.  **Qt 6 (MinGW Version):** Make sure you also have the `MinGW` components installed for Qt.
2.  Clone this repository.
3.  Open a `cmd` prompt in the repository folder.
4.  Run the **MinGW** `qmake` (adjust path if needed):
    `C:\Qt\6.10.0\mingw_64\bin\qmake.exe`
5.  Run the **MinGW** `make` (adjust path if needed):
    `C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe`
6.  Your `QtLite.exe` will be in the `release` folder.

## Project Status

**Current Version:** 1.0.0

QtLite is now at its first stable release! All planned features have been implemented:

- ✅ Core functionality (modular architecture)
- ✅ Multi-language support (C++, Qt, Python)
- ✅ Code editor with syntax highlighting
- ✅ Code snippets library
- ✅ Code bundler utility
- ✅ Unit tests for core components
- ✅ Comprehensive documentation

For the development roadmap and changelog, see [CHANGELOG.md](CHANGELOG.md).

## Contributing

Contributions are welcome! Please see [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.