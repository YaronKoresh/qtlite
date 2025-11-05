#include <QtTest>
#include "../src/core/pathvalidator.h"
#include "../src/core/projectstructs.h"

class TestPathValidator : public QObject
{
    Q_OBJECT

private slots:
    void testValidateAndExtract();
    void testValidateQtPaths();
    void testValidateVsPaths();
    void testValidatePythonPath();
    void testGetValidationColor();
};

void TestPathValidator::testValidateAndExtract()
{
    PathValidator validator;
    ProjectSettings settings;
    
    settings.qtDirectory = "C:/Qt/6.10.0";
    settings.vsDirectory = "C:/Program Files/Microsoft Visual Studio/2022/Community";
    settings.pythonExecutable = "C:/Python311/python.exe";
    
    CompilerPaths paths = validator.validateAndExtract(settings);
    
    QVERIFY(paths.qmakePath.contains("qmake.exe"));
    QVERIFY(paths.vcvarsPath.contains("vcvarsall.bat"));
    QVERIFY(paths.windeployqtPath.contains("windeployqt.exe"));
    QCOMPARE(paths.pythonExePath, settings.pythonExecutable);
}

void TestPathValidator::testValidateQtPaths()
{
    PathValidator validator;
    CompilerPaths paths;
    
    // Test with non-existent paths
    paths.qmakePath = "/nonexistent/qmake.exe";
    paths.windeployqtPath = "/nonexistent/windeployqt.exe";
    
    QVERIFY(!validator.validateQtPaths(paths));
}

void TestPathValidator::testValidateVsPaths()
{
    PathValidator validator;
    CompilerPaths paths;
    
    // Test with non-existent path
    paths.vcvarsPath = "/nonexistent/vcvarsall.bat";
    
    QVERIFY(!validator.validateVsPaths(paths));
}

void TestPathValidator::testValidatePythonPath()
{
    PathValidator validator;
    CompilerPaths paths;
    
    // Test with non-existent path
    paths.pythonExePath = "/nonexistent/python.exe";
    
    QVERIFY(!validator.validatePythonPath(paths));
}

void TestPathValidator::testGetValidationColor()
{
    PathValidator validator;
    
    QColor validColor = validator.getValidationColor(true);
    QColor invalidColor = validator.getValidationColor(false);
    
    QCOMPARE(validColor, Qt::white);
    QCOMPARE(invalidColor, QColor(255, 220, 220));
}

QTEST_MAIN(TestPathValidator)
#include "test_pathvalidator.moc"
