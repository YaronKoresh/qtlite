# Contributing to QtLite

Thank you for your interest in contributing to QtLite! This document provides guidelines for contributing to the project.

## How to Contribute

### Reporting Issues

If you find a bug or have a suggestion for improvement:

1. Check if the issue already exists in the [GitHub Issues](https://github.com/YaronKoresh/qtlite-private-repo/issues)
2. If not, create a new issue with a clear description
3. Include steps to reproduce (for bugs)
4. Include your environment details (Windows version, Qt version, Visual Studio version)

### Submitting Pull Requests

1. **Fork the repository** and create a new branch from `main`
2. **Make your changes** following the code style guidelines
3. **Write or update tests** for your changes
4. **Update documentation** if needed
5. **Test your changes** thoroughly
6. **Submit a pull request** with a clear description of your changes

## Code Style Guidelines

### C++ Code

- Use 4 spaces for indentation (no tabs)
- Follow Qt naming conventions:
  - Classes: `PascalCase` (e.g., `CodeEditor`)
  - Methods/Functions: `camelCase` (e.g., `validatePaths`)
  - Member variables: `camelCase` (e.g., `qtDirEdit`)
  - Constants: `UPPER_SNAKE_CASE` (e.g., `MAX_BUFFER_SIZE`)
- Place opening braces on the same line for functions and classes
- Use meaningful variable and function names
- Add comments for complex logic

### File Organization

- Header files (`.h`) should have include guards
- Keep implementations in `.cpp` files
- Organize code into logical directories:
  - `src/core/` - Core utilities and data structures
  - `src/ui/` - User interface components
  - `src/editor/` - Text editor related code
  - `src/highlighting/` - Syntax highlighting
  - `src/compilation/` - Build and compilation logic

## Testing

### Running Tests

Tests are located in the `tests/` directory and use Qt Test framework.

To run tests:

```cmd
cd tests
qmake "TEST=test_example" tests.pro
make
```

### Writing Tests

- Create test files named `test_<component>.cpp`
- Use Qt Test macros: `QVERIFY`, `QCOMPARE`, `QTEST_MAIN`
- Test both successful and failure cases
- Test edge cases and boundary conditions
- Keep tests focused and independent

Example test structure:

```cpp
#include <QtTest>
#include "../src/core/mycomponent.h"

class TestMyComponent : public QObject
{
    Q_OBJECT

private slots:
    void testFeature();
    void testEdgeCase();
};

void TestMyComponent::testFeature()
{
    MyComponent component;
    QVERIFY(component.doSomething());
}

QTEST_MAIN(TestMyComponent)
#include "test_mycomponent.moc"
```

## Development Workflow

### Setting Up Your Development Environment

1. Install Qt 6 with MinGW (for building QtLite)
2. Install Visual Studio 2022 (for testing C++ compilation)
3. Clone the repository
4. Build the project using the instructions in README.md

### Making Changes

1. Create a feature branch: `git checkout -b feature/my-feature`
2. Make your changes in small, logical commits
3. Write descriptive commit messages
4. Test your changes thoroughly
5. Update documentation if needed

### Commit Message Guidelines

- Use clear, descriptive commit messages
- Start with a verb in present tense (e.g., "Add", "Fix", "Update")
- Keep the first line under 72 characters
- Add details in the body if needed

Examples:
```
Add code bundler feature for merging multiple files

Fix path validation for Windows network paths

Update README with Python setup instructions
```

## Project Structure

```
qtlite/
├── src/
│   ├── core/           # Core utilities
│   ├── ui/             # UI components
│   ├── editor/         # Code editor
│   ├── highlighting/   # Syntax highlighting
│   ├── compilation/    # Build engine
│   └── main.cpp        # Entry point
├── tests/              # Unit tests
├── docs/               # Documentation
├── qtlite.pro          # Main project file
└── README.md
```

## Areas for Contribution

We welcome contributions in these areas:

### Features
- Additional language support (Java, JavaScript, etc.)
- More code snippets
- Improved syntax highlighting
- Theme support (dark mode, etc.)
- Code formatting tools
- Debugger integration

### Bug Fixes
- Path validation edge cases
- Process handling improvements
- UI/UX improvements
- Performance optimizations

### Documentation
- Tutorial videos
- Example projects
- Translations
- API documentation

### Testing
- More comprehensive unit tests
- Integration tests
- Performance tests

## Code Review Process

All submissions require review before being merged:

1. A maintainer will review your pull request
2. Address any feedback or requested changes
3. Once approved, your PR will be merged

## Getting Help

If you need help or have questions:

- Open a discussion in GitHub Discussions
- Ask in the pull request comments
- Review existing documentation and issues

## License

By contributing to QtLite, you agree that your contributions will be licensed under the MIT License.

---

Thank you for contributing to QtLite! 🎉
