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

QTEST_MAIN(TestCompilationEngine)
#include "test_compilationengine.moc"
