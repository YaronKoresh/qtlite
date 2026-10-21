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
    void testModuleDetectionFromClassAndUmbrellaIncludes();
    void testModuleDetectionIgnoresCoreHeaders();
    void testModuleDetectionKeepsSlashForm();
    void testBuildOutputWithPlainTextErrorCIsNotAFailure();
    void testBuildOutputWithCompilerErrorCodeIsAFailure();
    void testSecondRunWhileRunningIsRejectedWithoutCrash();
    void testFailedProcessStartIsReportedAndEngineIsFreeAgain();
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

static QString proQtLine(const QByteArray& code)
{
    QTemporaryDir dir;
    if (!dir.isValid())
        return QString();
    const QString source = dir.path() + "/src.cpp";
    QFile f(source);
    if (!f.open(QIODevice::WriteOnly))
        return QString();
    f.write(code);
    f.close();

    QTextEdit log;
    CompilationEngine engine(&log);
    ProjectSettings settings;
    settings.buildDir = dir.path() + "/build";
    settings.buildConfig = "Release";
    settings.bundleToExe = false;
    engine.setProjectSettings(settings);
    engine.compileQt(source);

    QFile pro(dir.path() + "/build/temp_app.pro");
    if (!pro.open(QIODevice::ReadOnly))
        return QString();
    const QStringList lines = QString::fromUtf8(pro.readAll()).split('\n');
    for (const QString& line : lines) {
        if (line.startsWith("QT +="))
            return line;
    }
    return QString();
}

void TestCompilationEngine::testModuleDetectionFromClassAndUmbrellaIncludes()
{
    QCOMPARE(proQtLine("#include <QNetworkAccessManager>\n#include <QtSql>\nint main(){}\n"),
             QString("QT += network sql widgets"));
    QCOMPARE(proQtLine("#include <QTcpSocket>\n#include <QtTest>\nint main(){}\n"),
             QString("QT += network testlib widgets"));
    QCOMPARE(proQtLine("#include <QWebEngineView>\nint main(){}\n"),
             QString("QT += webenginewidgets widgets"));
}

void TestCompilationEngine::testModuleDetectionIgnoresCoreHeaders()
{
    QCOMPARE(proQtLine("#include <QString>\n#include <QTimer>\n#include <QtGlobal>\n#include <QApplication>\n"
                       "#include <QTextEdit>\nint main(){}\n"),
             QString("QT += widgets"));
}

void TestCompilationEngine::testModuleDetectionKeepsSlashForm()
{
    QCOMPARE(proQtLine("#include <QtNetwork/QTcpSocket>\n#include <QtWidgets/QApplication>\nint main(){}\n"),
             QString("QT += network widgets"));
}

#ifndef Q_OS_WIN
static QString runConsoleBuildWithFakeShell(const QByteArray& shellOutput)
{
    QTemporaryDir dir;
    if (!dir.isValid())
        return QString();
    const QString fake = dir.path() + "/powershell.exe";
    QFile script(fake);
    if (!script.open(QIODevice::WriteOnly))
        return QString();
    script.write("#!/bin/sh\necho '" + shellOutput + "'\nexit 0\n");
    script.close();
    QFile::setPermissions(fake, QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);

    const QByteArray oldPath = qgetenv("PATH");
    qputenv("PATH", dir.path().toUtf8() + ":" + oldPath);

    QTextEdit log;
    CompilationEngine engine(&log);
    ProjectSettings settings;
    settings.buildDir = dir.path() + "/build";
    settings.buildConfig = "Release";
    settings.bundleToExe = false;
    CompilerPaths paths;
    paths.vcvarsPath = "vcvarsall.bat";
    engine.setProjectSettings(settings);
    engine.setCompilerPaths(paths);
    engine.compileConsole(dir.path() + "/main.cpp");

    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < 10000 && !log.toPlainText().contains("Build")) {
        QTest::qWait(20);
    }
    while (timer.elapsed() < 10000 && !log.toPlainText().contains("Build FAILED") &&
           !log.toPlainText().contains("Build Succeeded")) {
        QTest::qWait(20);
    }
    qputenv("PATH", oldPath);
    return log.toPlainText();
}
#endif

void TestCompilationEngine::testBuildOutputWithPlainTextErrorCIsNotAFailure()
{
#ifdef Q_OS_WIN
    QSKIP("Uses a fake POSIX shell script as powershell.exe");
#else
    const QString text = runConsoleBuildWithFakeShell("compiled, no error Code and no error Check");
    QVERIFY2(text.contains("Build Succeeded"), qPrintable(text));
    QVERIFY2(!text.contains("Build FAILED"), qPrintable(text));
#endif
}

void TestCompilationEngine::testBuildOutputWithCompilerErrorCodeIsAFailure()
{
#ifdef Q_OS_WIN
    QSKIP("Uses a fake POSIX shell script as powershell.exe");
#else
    const QString text = runConsoleBuildWithFakeShell("main.cpp(3): error C2065: x: undeclared identifier");
    QVERIFY2(text.contains("Build FAILED"), qPrintable(text));
#endif
}

#ifndef Q_OS_WIN
static bool waitForLog(const QTextEdit& log, const QString& text, int timeoutMs)
{
    QElapsedTimer timer;
    timer.start();
    while (timer.elapsed() < timeoutMs && !log.toPlainText().contains(text)) {
        QTest::qWait(20);
    }
    return log.toPlainText().contains(text);
}
#endif

void TestCompilationEngine::testSecondRunWhileRunningIsRejectedWithoutCrash()
{
#ifdef Q_OS_WIN
    QSKIP("Runs a POSIX python3 script");
#else
    if (QStandardPaths::findExecutable("python3").isEmpty())
        QSKIP("python3 is not installed");

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    const QString script = dir.path() + "/slow.py";
    writeFile(script, "import time\ntime.sleep(0.5)\nprint('script-ran')\n");

    QTextEdit log;
    CompilationEngine engine(&log);
    ProjectSettings settings;
    settings.buildDir = dir.path();
    CompilerPaths paths;
    paths.pythonExePath = QStandardPaths::findExecutable("python3");
    engine.setProjectSettings(settings);
    engine.setCompilerPaths(paths);

    engine.compilePython(script);
    engine.compilePython(script);

    QVERIFY2(log.toPlainText().contains("already in progress"), qPrintable(log.toPlainText()));
    QVERIFY2(waitForLog(log, "Script finished", 10000), qPrintable(log.toPlainText()));
    QTest::qWait(300);

    const QString text = log.toPlainText();
    QCOMPARE(text.count("script-ran"), 1);
    QCOMPARE(text.count("Script finished"), 1);

    log.clear();
    engine.compilePython(script);
    QVERIFY2(!log.toPlainText().contains("already in progress"), qPrintable(log.toPlainText()));
    QVERIFY2(waitForLog(log, "Script finished", 10000), qPrintable(log.toPlainText()));
#endif
}

void TestCompilationEngine::testFailedProcessStartIsReportedAndEngineIsFreeAgain()
{
#ifdef Q_OS_WIN
    QSKIP("Relies on powershell.exe being absent from PATH");
#else
    if (QStandardPaths::findExecutable("python3").isEmpty())
        QSKIP("python3 is not installed");

    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    writeFile(dir.path() + "/main.cpp", "int main() { return 0; }\n");
    writeFile(dir.path() + "/ok.py", "print('ok-ran')\n");

    const QString python = QStandardPaths::findExecutable("python3");
    const QByteArray oldPath = qgetenv("PATH");
    qputenv("PATH", dir.path().toUtf8());

    QTextEdit log;
    CompilationEngine engine(&log);
    ProjectSettings settings;
    settings.buildDir = dir.path() + "/build";
    settings.buildConfig = "Release";
    CompilerPaths paths;
    paths.vcvarsPath = "vcvarsall.bat";
    paths.pythonExePath = python;
    engine.setProjectSettings(settings);
    engine.setCompilerPaths(paths);

    engine.compileConsole(dir.path() + "/main.cpp");
    const bool reported = waitForLog(log, "FAILED to start process", 5000);
    qputenv("PATH", oldPath);
    QVERIFY2(reported, qPrintable(log.toPlainText()));

    log.clear();
    engine.compilePython(dir.path() + "/ok.py");
    QVERIFY2(!log.toPlainText().contains("already in progress"), qPrintable(log.toPlainText()));
    QVERIFY2(waitForLog(log, "Script finished", 10000), qPrintable(log.toPlainText()));
    QVERIFY(log.toPlainText().contains("ok-ran"));
#endif
}

QTEST_MAIN(TestCompilationEngine)
#include "test_compilationengine.moc"
