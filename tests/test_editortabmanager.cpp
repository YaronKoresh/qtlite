#include <QtTest>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QFile>
#include "../src/ui/editortabmanager.h"

static QString readAll(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return QString();
    return QString::fromUtf8(f.readAll());
}

class TestEditorTabManager : public QObject
{
    Q_OBJECT

private slots:
    void testSaveUnsavedTabWritesDefaultCppFileToBuildDir();
    void testSaveUnsavedPythonTabWritesMainPy();
    void testSaveUnsavedTabUsesTabTitleWhenItFitsTheLanguage();
    void testSaveTabWithPathWritesToThatPath();
    void testSaveCreatesMissingBuildDir();
    void testSaveWithUnwritableTargetReturnsEmpty();
    void testCtrlSEmitsSaveRequestedOnce();
};

void TestEditorTabManager::testSaveUnsavedTabWritesDefaultCppFileToBuildDir()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    CodeEditor* editor = manager.addNewTab("notes.txt");
    editor->setPlainText("int main() { return 0; }\n");

    const QString path = manager.saveCurrentEditor(dir.path());

    QCOMPARE(path, dir.path() + "/main.cpp");
    QCOMPARE(readAll(path), QString("int main() { return 0; }\n"));
    QCOMPARE(editor->filePath(), path);
}

void TestEditorTabManager::testSaveUnsavedPythonTabWritesMainPy()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    manager.setLanguage("Python");
    CodeEditor* editor = manager.addNewTab("main.cpp");
    editor->setPlainText("print('hi')\n");

    const QString path = manager.saveCurrentEditor(dir.path());

    QCOMPARE(path, dir.path() + "/main.py");
    QCOMPARE(readAll(path), QString("print('hi')\n"));
}

void TestEditorTabManager::testSaveUnsavedTabUsesTabTitleWhenItFitsTheLanguage()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    CodeEditor* editor = manager.addNewTab("demo.cpp");
    editor->setPlainText("// demo\n");

    QCOMPARE(manager.saveCurrentEditor(dir.path()), dir.path() + "/demo.cpp");
}

void TestEditorTabManager::testSaveTabWithPathWritesToThatPath()
{
    QTemporaryDir dir;
    QTemporaryDir other;
    QVERIFY(dir.isValid() && other.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    CodeEditor* editor = manager.addNewTab("kept.cpp");
    const QString target = other.path() + "/kept.cpp";
    editor->setFilePath(target);
    editor->setPlainText("new text\n");

    const QString path = manager.saveCurrentEditor(dir.path());

    QCOMPARE(path, target);
    QCOMPARE(readAll(target), QString("new text\n"));
    QVERIFY(!QFile::exists(dir.path() + "/main.cpp"));
}

void TestEditorTabManager::testSaveCreatesMissingBuildDir()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    CodeEditor* editor = manager.addNewTab("main.cpp");
    editor->setPlainText("x\n");

    const QString buildDir = dir.path() + "/a/b";
    QCOMPARE(manager.saveCurrentEditor(buildDir), buildDir + "/main.cpp");
    QCOMPARE(readAll(buildDir + "/main.cpp"), QString("x\n"));
}

void TestEditorTabManager::testSaveWithUnwritableTargetReturnsEmpty()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    CodeEditor* editor = manager.addNewTab("main.cpp");
    editor->setPlainText("x\n");
    editor->setFilePath(dir.path() + "/missing-dir/file.cpp");

    QVERIFY(manager.saveCurrentEditor(dir.path()).isEmpty());
    QVERIFY(manager.saveCurrentEditor(QString()).isEmpty());
}

void TestEditorTabManager::testCtrlSEmitsSaveRequestedOnce()
{
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    CodeEditor* editor = manager.addNewTab("main.cpp");
    editor->setPlainText("x\n");
    QSignalSpy spy(&manager, &EditorTabManager::saveRequested);

    tabs.show();
    QVERIFY(QTest::qWaitForWindowActive(&tabs));
    editor->setFocus();
    QTest::keyClick(editor, Qt::Key_S, Qt::ControlModifier);

    QCOMPARE(spy.count(), 1);
}

QTEST_MAIN(TestEditorTabManager)
#include "test_editortabmanager.moc"
