# QtLite Unit Tests

This directory contains unit tests for QtLite core components.

## Running Tests

### Prerequisites

- Qt 6 with MinGW installed
- QtTest module available

### Building and Running

Each test is built as a separate executable. To build a specific test:

1. Navigate to the tests directory:
   ```cmd
   cd tests
   ```

2. Generate the Makefile for a specific test using qmake:
   ```cmd
   C:\Qt\6.10.0\mingw_64\bin\qmake.exe "TEST=test_pathvalidator" tests.pro
   ```

3. Build the test:
   ```cmd
   C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe
   ```

4. Run the generated executable:
   ```cmd
   release\test_pathvalidator.exe
   ```

To build and run all tests, repeat for each test:
- `TEST=test_pathvalidator`
- `TEST=test_settingspersistence`
- `TEST=test_snippetmanager`
- `TEST=test_codebundler`

Or use a batch script to run all tests:
```cmd
@echo off
for %%t in (test_pathvalidator test_settingspersistence test_snippetmanager test_codebundler) do (
    echo Building %%t...
    C:\Qt\6.10.0\mingw_64\bin\qmake.exe "TEST=%%t" tests.pro
    C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe clean
    C:\Qt\Tools\mingw1120_64\bin\mingw32-make.exe
    if exist release\%%t.exe (
        echo Running %%t...
        release\%%t.exe
    )
    echo.
)
```

## Test Files

- **test_pathvalidator.cpp** - Tests for PathValidator class
  - Path validation logic
  - Compiler path extraction
  - Validation color feedback

- **test_settingspersistence.cpp** - Tests for SettingsPersistence class
  - Loading settings
  - Saving settings
  - Default values

- **test_snippetmanager.cpp** - Tests for SnippetManager class
  - Snippet retrieval
  - Language-specific snippets
  - Snippet content validation

- **test_codebundler.cpp** - Tests for CodeBundler class
  - Simple file bundling
  - Local includes resolution
  - System includes handling
  - Circular dependency handling

## Adding New Tests

To add a new test file:

1. Create a new test file: `test_<component>.cpp`
2. Include QtTest and the component header
3. Create a test class inheriting from QObject
4. Add test methods as private slots
5. Use QTEST_MAIN macro at the end
6. Add the new file to `tests.pro`

Example:
```cpp
#include <QtTest>
#include "../src/core/mycomponent.h"

class TestMyComponent : public QObject
{
    Q_OBJECT

private slots:
    void testSomething();
};

void TestMyComponent::testSomething()
{
    MyComponent component;
    QVERIFY(component.isValid());
}

QTEST_MAIN(TestMyComponent)
#include "test_mycomponent.moc"
```

## Test Coverage

These tests cover the core logic components that don't require UI interaction:
- Configuration and settings management
- Path validation and extraction
- Code snippet management
- Code bundling functionality

UI components and compilation engine require integration testing with actual Windows tools (Qt, MSVC) and are tested manually.
