# **Changelog**

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),

and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## **1. Project Overview**

The "QtLite" project fills an underserved niche for developers, students, and hobbyists who need to compile and run lightweight C++, Qt, or Python projects without the overhead and complexity of a full IDE. The core problem is that compiling even simple GUI or console applications on Windows requires significant, complex setup of tools like Visual Studio and the Qt SDK.

QtLite solves this by providing a lightweight, portable GUI wrapper that automates the entire compilation chain. It provides a simple interface for defining tool paths (Qt SDK, MSVC, Python), a tabbed code editor with syntax highlighting, and a "Build and Run" button that handles all the complex underlying processes (setting environment variables, running `qmake`, `nmake`, `windeployqt`, `cl.exe`, or `python.exe`).

## **2. Technology Stack**

* **Language(s):** C++
* **Framework(s):** Qt 6 (Widgets)
* **Database:** N/A
* **Core Libraries:** `QProcess`, `QFileSystemModel`, `QSyntaxHighlighter`, `QSettings`
* **Other:** Windows build tools (`vcvarsall.bat`, `nmake.exe`, `windeployqt.exe`, `iexpress.exe`)

## **3. Hierarchical File Structure**

```
qtlite/
│
├── src/
│   │
│   ├── ui/
│   │   ├── qtliteapp.h
│   │   ├── qtliteapp.cpp
│   │   ├── uicontroller.h
│   │   ├── uicontroller.cpp
│   │   └── editortabmanager.h
│   │   └── editortabmanager.cpp
│   │
│   ├── editor/
│   │   ├── codeeditor.h
│   │   ├── codeeditor.cpp
│   │   ├── linenumberarea.h
│   │   └── linenumberarea.cpp
│   │
│   ├── highlighting/
│   │   ├── syntaxhighlighterbase.h
│   │   ├── cpphighlighter.h
│   │   ├── cpphighlighter.cpp
│   │   ├── pythonhighlighter.h
│   │   └── pythonhighlighter.cpp
│   │
│   ├── compilation/
│   │   ├── compilationengine.h
│   │   └── compilationengine.cpp
│   │
│   ├── core/
│   │   ├── pathvalidator.h
│   │   ├── pathvalidator.cpp
│   │   ├── settingspersistence.h
│   │   ├── settingspersistence.cpp
│   │   ├── snippetmanager.h
│   │   ├── snippetmanager.cpp
│   │   ├── codebundler.h
│   │   ├── codebundler.cpp
│   │   └── projectstructs.h
│   │
│   └── main.cpp
│
├── tests/
│   ├── test_main.cpp
│   ├── test_pathvalidator.cpp
│   ├── test_settingspersistence.cpp
│   ├── test_snippetmanager.cpp
│   ├── test_codebundler.cpp
│   ├── test_cpphighlighter.cpp
│   ├── test_pythonhighlighter.cpp
│   └── test_editortabmanager.cpp
│
├── docs/
│   └── usage_guide.md
│
├── .gitignore
├── qtlite.pro
├── README.md
├── CHANGELOG.md
├── CONTRIBUTING.md
└── LICENSE
```

## **4. Staged Development Roadmap**

### **Version 0.1.0 - Core Refactor & Modularization**

*Theme: "Refactor the monolithic `main.cpp` into the planned hierarchical file structure."*

* [X] **[Core Module Separation]**
  * [X] [Move data structures (`HighlightingRule`, `CompilerPaths`, `ProjectSettings`) to `src/core/projectstructs.h`]
  * [X] [Separate `PathValidator` class into `src/core/`]
  * [X] [Separate `SettingsPersistence` class into `src/core/`]
* [X] **[Editor Module Separation]**
  * [X] [Separate `CodeEditor` and `LineNumberArea` classes into `src/editor/`]
  * [X] [Separate `SyntaxHighlighterBase`, `CppHighlighter`, and `PythonHighlighter` into `src/highlighting/`]
* [X] **[UI & Compilation Separation]**
  * [X] [Separate `CompilationEngine` class into `src/compilation/`]
  * [X] [Separate `UIController` and `EditorTabManager` classes into `src/ui/`]
  * [X] [Refactor `QtLiteApp` into `src/ui/` and update `src/main.cpp` to be a minimal entry point]
* [X] **[Project File]**
  * [X] [Update `qtlite.pro` to include all new header (`.h`) and source (`.cpp`) files from the `src/` subdirectories]

### **Version 0.2.0 - Utility Integration & Stabilization**

*Theme: "Integrate and stabilize core utilities (Snippets, Bundler) and file system interactions."*

* [X] **[Utility Implementation]**
  * [X] [Separate `SnippetManager` class into `src/core/` and ensure it connects to the new UI]
  * [X] [Separate `CodeBundler` class into `src/core/`]
  * [X] [Implement the `bundleCode()` slot in `qtliteapp.cpp` (currently a placeholder) to use the `CodeBundler` service]
* [X] **[UI/UX Stabilization]**
  * [X] [Verify file browser (`QTreeView`) interactions (e.g., double-click to open) work correctly with the refactored `EditorTabManager`]
  * [X] [Verify persistence of all settings (paths, language, build config) across sessions using `SettingsPersistence`]

### **Version 1.0.0 - Stable Release**

*Theme: "Finalize all features, optimize, document, and prepare for a stable, public-ready launch."*

* [X] **[Feature Finalization]**
  * [X] [Review and finalize snippet menu logic (`updateSnippetMenu`, `insertSnippet`)]
  * [X] [Review and finalize "New Tab" (`createNewEditorTab`) functionality]
* [X] **[Unit Testing]**
  * [X] [Implement unit tests in `tests/` for core logic (Validator, Persistence, Snippets, Bundler)]
* [X] **[Polishing & Release]**
  * [X] [Conduct performance profiling and optimization, focusing on build process startup time]
  * [X] [Write comprehensive `README.md` and `docs/usage_guide.md` with setup instructions and usage]
  * [X] [Fix all known high-priority bugs, especially path validation edge cases and process handling]

For more information, see the [README](README.md) and [CONTRIBUTING](CONTRIBUTING.md) files.
