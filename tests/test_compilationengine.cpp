#include <QtTest>
#include <QTextEdit>
#include <QTemporaryDir>
#include <QFile>
#include "../src/compilation/compilationengine.h"

static void writeFile(const QString& path, const QByteArray& data)
{
    QFile f(path);
    QVERIFY(f.open(QIODevice::WriteOnly));
    f.write(data);
}

class TestCompilationEngine : public QObject
{
    Q_OBJECT

private slots:
    void testCompileQtMissingFileIsReported();
    void testCompileQtEmptyFileIsReportedAndNothingIsWritten();
};

void TestCompilationEngine::testCompileQtMissingFileIsReported()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTextEdit log;
    CompilationEngine engine(&log);
    ProjectSettings settings;
    settings.buildDir = dir.path();
    settings.buildConfig = "Release";
    settings.bundleToExe = false;
    engine.setProjectSettings(settings);

    engine.compileQt(dir.path() + "/main.cpp");

    QVERIFY(log.toPlainText().contains("FAILED to read code"));
}

void TestCompilationEngine::testCompileQtEmptyFileIsReportedAndNothingIsWritten()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    writeFile(dir.path() + "/empty.cpp", QByteArray());

    QTextEdit log;
    CompilationEngine engine(&log);
    ProjectSettings settings;
    settings.buildDir = dir.path() + "/build";
    settings.buildConfig = "Release";
    settings.bundleToExe = false;
    engine.setProjectSettings(settings);

    engine.compileQt(dir.path() + "/empty.cpp");

    QVERIFY(log.toPlainText().contains("FAILED: the file is empty"));
    QVERIFY(!QFile::exists(dir.path() + "/build/temp_app.pro"));
    QVERIFY(!QFile::exists(dir.path() + "/build/main.cpp"));
}

QTEST_MAIN(TestCompilationEngine)
#include "test_compilationengine.moc"
