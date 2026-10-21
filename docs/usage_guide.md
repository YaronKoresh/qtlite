# QtLite Usage Guide

## Table of Contents
1. [Introduction](#introduction)
2. [Installation](#installation)
3. [First-Time Setup](#first-time-setup)
4. [Basic Usage](#basic-usage)
5. [Language Support](#language-support)
6. [Code Snippets](#code-snippets)
7. [Code Bundler](#code-bundler)
8. [Advanced Features](#advanced-features)
9. [Troubleshooting](#troubleshooting)

---

## Introduction

QtLite is a lightweight GUI wrapper designed for compiling and running single-file or small-scale C++, Qt, and Python projects on Windows without the complexity of a full-featured IDE. It's perfect for:

- **Students** learning C++ or Qt programming
- **Developers** testing quick code snippets or prototypes
- **Hobbyists** working on small projects without the overhead of Visual Studio or Qt Creator

---

## Installation

### Prerequisites

Before using QtLite, ensure you have the following installed:

#### For C++ and Qt Development:
1. **Visual Studio 2022** with the "Desktop development with C++" workload
   - Download from: https://visualstudio.microsoft.com/downloads/
   - During installation, select "Desktop development with C++"

2. **Qt 6 (MSVC Version)**
   - Download from: https://www.qt.io/download
   - During installation, select the `MSVC 2022 64-bit` component
   - Also install any additional Qt modules you need (e.g., Qt WebEngine)

#### For Python Development:
1. **Python 3.x**
   - Download from: https://www.python.org/downloads/
   - Make sure to add Python to your PATH during installation

### Building QtLite

To build QtLite from source:

1. **Install Qt 6 with MinGW** (for building QtLite itself)
   - You need the MinGW version of Qt to build the QtLite application

2. **Clone the repository:**
   ```bash
   git clone https://github.com/YaronKoresh/qtlite-private-repo.git
   cd qtlite-private-repo
   ```

3. **Build using MinGW:**
   ```cmd
   C:\Qt\6.10.0\mingw_64\bin\qmake.exe
   C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe
   ```

4. The compiled `QtLite.exe` will be in the `release` folder.

---

## First-Time Setup

When you launch QtLite for the first time, you need to configure the paths to your development tools:

### 1. Qt Version Directory

1. Click the **"Browse..."** button next to "Qt Version Directory"
2. Navigate to your Qt **version** folder (the one containing `msvc2022_64`)
   - Example: `C:\Qt\6.10.0`
3. Select the folder

### 2. Visual Studio Directory

1. Click the **"Browse..."** button next to "Visual Studio Directory"
2. Navigate to your Visual Studio installation folder
   - Example: `C:\Program Files\Microsoft Visual Studio\2022\Community`
3. Select the folder

### 3. Python Executable (Optional)

If you plan to run Python scripts:

1. Click the **"Browse..."** button next to "Python Executable"
2. Navigate to your Python installation
   - Example: `C:\Python311\python.exe`
3. Select the `python.exe` file

### 4. Build Directory

1. Click the **"Browse..."** button next to "Build (Project) Directory"
2. Select a folder where you want to store your project files
   - Example: `C:\Users\YourName\Desktop\QtLiteProject`

All these settings are saved automatically and persist across sessions.

---

## Basic Usage

### Creating a New File

1. Click the **"Add Tab"** button
2. Enter a filename (e.g., `myapp.cpp`, `test.py`)
3. The selected **Language** determines the syntax highlighting (the file extension does not)
4. Start typing your code

### Opening Existing Files

1. Use the **file browser** on the left side
2. Navigate to your desired file
3. **Double-click** the file to open it in a new tab

### Writing Code

The editor supports:
- **Syntax highlighting** for C++, C, and Python
- **Line numbers** for easy navigation
- **Tab management** for working with multiple files
- **Auto-save** on compilation
- **Ctrl+S** saves the current tab (to its file, or to the build directory if it has none)

### Building and Running

1. Write your code in the editor
2. Click **"Build and Run"**
3. The output log at the bottom will show:
   - Compilation progress
   - Any errors or warnings
   - Program output when execution completes

---

## Language Support

QtLite supports three language modes:

### 1. C++ (Qt)

**Use this for:** Qt-based GUI applications or Qt library features

- Automatically detects required Qt modules from `#include` directives
- Generates a `.pro` file with the necessary Qt modules
- Uses `qmake` and `nmake` for building
- Runs `windeployqt` to bundle required DLLs

**Example:**
```cpp
#include <QApplication>
#include <QPushButton>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    QPushButton button("Hello, Qt!");
    button.show();
    return app.exec();
}
```

### 2. C/C++ (Console)

**Use this for:** Standard C++ console applications without Qt

- Compiles using MSVC (`cl.exe`)
- No Qt dependencies
- Faster compilation for simple programs

**Example:**
```cpp
#include <iostream>

int main() {
    std::cout << "Hello, World!" << std::endl;
    return 0;
}
```

### 3. Python

**Use this for:** Python scripts

- Runs scripts using your configured Python interpreter
- No compilation needed
- Output appears directly in the log

**Example:**
```python
def main():
    print("Hello, Python!")

if __name__ == "__main__":
    main()
```

---

## Code Snippets

QtLite includes a collection of code snippets to speed up your development:

### Using Snippets

1. Click the **"Insert Snippet"** button
2. Select a snippet from the dropdown menu
3. The snippet code will be inserted at the current cursor position

### Available Snippets

#### C++ (Qt) / C/C++ (Console)

- **C++ Main**: Basic C++ main function with iostream
- **C Main**: Basic C main function with stdio
- **For Loop**: Standard for loop structure
- **If/Else**: If-else conditional structure
- **C++ Class**: Basic C++ class template
- **C++ Struct**: Structure definition
- **C++ Template**: Template function example

#### Python

- **Python Main**: Main function pattern with `__name__ == "__main__"`
- **Python For Loop**: Python for loop
- **Python If/Else**: Python if-else structure
- **Python Function**: Function definition template
- **Python Class**: Class definition template
- **Python Try/Except**: Exception handling block
- **Python List Comprehension**: List comprehension example
- **Python Dictionary**: Dictionary definition

The available snippets automatically change based on your selected language.

---

## Code Bundler

The Code Bundler feature helps you combine multiple C++ files into a single file:

### What It Does

- Resolves local `#include "file.h"` directives
- Merges header and source files into one file
- Removes duplicate includes
- Preserves system includes (`#include <...>`)
- Useful for code submissions or portability

### How to Use

1. Open the main file you want to bundle
2. Make sure the file is **saved** (the bundler needs a file path)
3. Click **"Bundle Code"**
4. A new file will be created with `_bundled` appended to the name
   - Example: `main.cpp` → `main_bundled.cpp`

### Example

**Before bundling:**

`main.cpp`:
```cpp
#include <iostream>
#include "myheader.h"

int main() {
    myFunction();
    return 0;
}
```

`myheader.h`:
```cpp
#pragma once
void myFunction();
```

**After bundling (`main_bundled.cpp`):**
```cpp
#include <iostream>

// --- Start of bundled file: myheader.h ---
void myFunction();
// --- End of bundled file: myheader.h ---

int main() {
    myFunction();
    return 0;
}
```

---

## Advanced Features

### Build Configuration

Choose between **Release** and **Debug** build configurations:
- **Release**: Optimized for performance (default)
- **Debug**: Includes debugging symbols for troubleshooting

### Include Directories

Add custom include directories for external libraries:
- Enter paths separated by semicolons
- Example: `C:/libs/include;C:/boost/include`

### Library Paths/Files

Specify additional libraries to link:
- Can include both directories and specific `.lib` files
- Separate multiple entries with semicolons
- Example: `C:/libs/mylib.lib;C:/boost/lib`

### File Browser

The left panel shows your project directory:
- Navigate your project files
- Double-click to open files
- The root directory is your configured "Build Directory"

### Tab Management

- **Add Tab**: Create new files
- **Close Tab**: Click the `×` on any tab
- **Reorder Tabs**: Drag tabs to rearrange them
- **Switch Tabs**: Click to switch between open files

---

## Troubleshooting

### The "Build and Run" button is disabled

**Cause:** QtLite couldn't find the required tools.

**Solution:**
1. Check that the path text boxes are **white** (not pink/red)
2. Verify your Qt and Visual Studio paths are correct
3. Make sure the directories exist and contain the expected tools

### Compilation fails with "qmake not found"

**Cause:** Qt path is incorrect or MSVC version doesn't match.

**Solution:**
1. Verify your Qt directory points to the version folder (e.g., `C:\Qt\6.10.0`)
2. Make sure you have the `msvc2022_64` component installed
3. Check that `qmake.exe` exists in: `<Qt Dir>/msvc2022_64/bin/qmake.exe`

### Compilation fails with "vcvarsall.bat not found"

**Cause:** Visual Studio path is incorrect.

**Solution:**
1. Verify your VS directory points to the edition folder
   - Correct: `C:\Program Files\Microsoft Visual Studio\2022\Community`
   - Wrong: `C:\Program Files\Microsoft Visual Studio\2022`
2. Check that `vcvarsall.bat` exists in: `<VS Dir>/VC/Auxiliary/Build/vcvarsall.bat`

### Python script doesn't run

**Cause:** Python executable path is incorrect.

**Solution:**
1. Verify the path points to `python.exe` (not just the directory)
2. Test the path by running it in cmd: `"C:\Python311\python.exe" --version`

### Code Bundler fails

**Cause:** File hasn't been saved.

**Solution:**
1. Save your file first using Ctrl+S or by running a build
2. Make sure the file exists on disk
3. The bundler needs a file path to resolve relative includes

### Build is slow

**Tips for faster builds:**
1. Use **"C/C++ (Console)"** mode if you don't need Qt
2. Switch to **Release** configuration (Debug builds are slower)
3. Minimize the number of included Qt modules
4. Clear old build artifacts from your build directory

### Syntax highlighting doesn't work

**Solution:**
1. Check that the correct language is selected in the dropdown (highlighting follows the selected language for every tab, whatever the file extension)
2. Try closing and reopening the tab

---

## Tips and Best Practices

1. **Save frequently**: While QtLite auto-saves on build, it's good practice to save manually
2. **Use meaningful filenames**: Clear names help when managing multiple tabs
3. **Organize your project**: Keep related files in your build directory
4. **Test incrementally**: Build and test often to catch errors early
5. **Use snippets**: Speed up coding with the built-in snippet library
6. **Keep it simple**: QtLite is designed for single-file or small projects
7. **Check the output log**: Always review the log for warnings and errors

---

## Additional Resources

- **Qt Documentation**: https://doc.qt.io/
- **C++ Reference**: https://cppreference.com/
- **Python Documentation**: https://docs.python.org/

---

## Getting Help

If you encounter issues not covered in this guide:

1. Check the output log for detailed error messages
2. Review the CHANGELOG.md for known issues
3. Open an issue on the GitHub repository

---

**Happy Coding with QtLite!**
