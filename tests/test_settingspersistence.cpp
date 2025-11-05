#include <QtTest>
#include <QSettings>
#include "../src/core/settingspersistence.h"
#include "../src/core/projectstructs.h"

class TestSettingsPersistence : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testSaveAndLoadSettings();
    void testDefaultSettings();
};

void TestSettingsPersistence::initTestCase()
{
    // Clear any existing test settings
    QSettings settings("YaronKoresh", "QtLite");
    settings.clear();
}

void TestSettingsPersistence::cleanupTestCase()
{
    // Clean up test settings
    QSettings settings("YaronKoresh", "QtLite");
    settings.clear();
}

void TestSettingsPersistence::testSaveAndLoadSettings()
{
    ProjectSettings testSettings;
    testSettings.qtDirectory = "C:/Qt/6.10.0";
    testSettings.vsDirectory = "C:/Program Files/Microsoft Visual Studio/2022/Community";
    testSettings.pythonExecutable = "C:/Python311/python.exe";
    testSettings.includeDirs = "C:/includes";
    testSettings.libDirs = "C:/libs";
    testSettings.buildDir = "C:/build";
    testSettings.buildConfig = "Debug";
    testSettings.language = "Python";
    testSettings.bundleToExe = true;
    
    SettingsPersistence::saveSettings(testSettings);
    
    ProjectSettings loadedSettings = SettingsPersistence::loadSettings();
    
    QCOMPARE(loadedSettings.qtDirectory, testSettings.qtDirectory);
    QCOMPARE(loadedSettings.vsDirectory, testSettings.vsDirectory);
    QCOMPARE(loadedSettings.pythonExecutable, testSettings.pythonExecutable);
    QCOMPARE(loadedSettings.includeDirs, testSettings.includeDirs);
    QCOMPARE(loadedSettings.libDirs, testSettings.libDirs);
    QCOMPARE(loadedSettings.buildDir, testSettings.buildDir);
    QCOMPARE(loadedSettings.buildConfig, testSettings.buildConfig);
    QCOMPARE(loadedSettings.language, testSettings.language);
    QCOMPARE(loadedSettings.bundleToExe, testSettings.bundleToExe);
}

void TestSettingsPersistence::testDefaultSettings()
{
    // Clear settings to get defaults
    QSettings settings("YaronKoresh", "QtLite");
    settings.clear();
    
    ProjectSettings defaultSettings = SettingsPersistence::loadSettings();
    
    QCOMPARE(defaultSettings.qtDirectory, QString("C:/Qt"));
    QCOMPARE(defaultSettings.vsDirectory, QString("C:/Program Files/Microsoft Visual Studio/2022/Community"));
    QCOMPARE(defaultSettings.pythonExecutable, QString("C:/Python311/python.exe"));
    QCOMPARE(defaultSettings.includeDirs, QString(""));
    QCOMPARE(defaultSettings.libDirs, QString(""));
    QCOMPARE(defaultSettings.buildConfig, QString("Release"));
    QCOMPARE(defaultSettings.language, QString("C++ (Qt)"));
    QCOMPARE(defaultSettings.bundleToExe, false);
}

QTEST_MAIN(TestSettingsPersistence)
#include "test_settingspersistence.moc"
