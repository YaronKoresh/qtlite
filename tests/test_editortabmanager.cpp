#include <QtTest>
#include <QTabWidget>
#include <QTemporaryDir>
#include <QFile>
#include <QTextBlock>
#include <QTextLayout>
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
    void testTwoUnsavedTabsWithTheSameTitleSaveToDifferentFiles();
    void testUnsavedTabDoesNotTakeThePathOfAnOpenFile();
    void testHighlightingFollowsTheLanguageNotTheFileExtension();
    void testClosingModifiedTabAsksAndCancelKeepsIt();
    void testClosingCleanOrSavedTabDoesNotAsk();
    void testNewFileNamesKeepKnownExtensionsAndDropFolders();
    void testSourceExtensionsCcAndCxxSaveUnderTheirOwnName();
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

void TestEditorTabManager::testTwoUnsavedTabsWithTheSameTitleSaveToDifferentFiles()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    CodeEditor* first = manager.addNewTab("untitled.cpp");
    first->setPlainText("// first\n");
    const QString firstPath = manager.saveCurrentEditor(dir.path());
    QCOMPARE(firstPath, dir.path() + "/untitled.cpp");

    CodeEditor* second = manager.addNewTab("untitled.cpp");
    second->setPlainText("// second\n");
    const QString secondPath = manager.saveCurrentEditor(dir.path());

    QVERIFY(!secondPath.isEmpty());
    QVERIFY2(secondPath != firstPath, qPrintable(secondPath));
    QCOMPARE(readAll(firstPath), QString("// first\n"));
    QCOMPARE(readAll(secondPath), QString("// second\n"));
    QCOMPARE(QFileInfo(secondPath).suffix(), QString("cpp"));
    QCOMPARE(QFileInfo(secondPath).absolutePath(), dir.path());
}

void TestEditorTabManager::testUnsavedTabDoesNotTakeThePathOfAnOpenFile()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QFile existing(dir.path() + "/main.cpp");
    QVERIFY(existing.open(QIODevice::WriteOnly));
    existing.write("// opened from disk\n");
    existing.close();

    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    manager.openFile(dir.path() + "/main.cpp");
    CodeEditor* fresh = manager.addNewTab("main.cpp");
    fresh->setPlainText("// new tab\n");

    const QString path = manager.saveCurrentEditor(dir.path());

    QVERIFY2(path != dir.path() + "/main.cpp", qPrintable(path));
    QCOMPARE(readAll(dir.path() + "/main.cpp"), QString("// opened from disk\n"));
    QCOMPARE(readAll(path), QString("// new tab\n"));
}

static bool hasFormattedRanges(CodeEditor* editor)
{
    QTest::qWait(50);
    const QTextBlock block = editor->document()->firstBlock();
    return block.layout() && !block.layout()->formats().isEmpty();
}

void TestEditorTabManager::testHighlightingFollowsTheLanguageNotTheFileExtension()
{
    QTabWidget tabs;
    EditorTabManager manager(&tabs);

    CodeEditor* pyNamedInCpp = manager.addNewTab("script.py");
    pyNamedInCpp->setPlainText("# a python comment\n");
    QVERIFY(!hasFormattedRanges(pyNamedInCpp));

    manager.setLanguage("Python");
    QVERIFY(hasFormattedRanges(pyNamedInCpp));

    CodeEditor* cppNamedInPython = manager.addNewTab("program.cpp");
    cppNamedInPython->setPlainText("# a python comment\n");
    QVERIFY(hasFormattedRanges(cppNamedInPython));
}

void TestEditorTabManager::testClosingModifiedTabAsksAndCancelKeepsIt()
{
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    QStringList asked;
    bool answer = false;
    manager.setDiscardConfirmer([&](const QString& title) {
        asked << title;
        return answer;
    });

    CodeEditor* editor = manager.addNewTab("work.cpp");
    editor->setPlainText("int x;\n");
    editor->document()->setModified(true);
    QCOMPARE(tabs.count(), 1);

    emit tabs.tabCloseRequested(0);
    QCOMPARE(asked, QStringList() << "work.cpp");
    QCOMPARE(tabs.count(), 1);
    QCOMPARE(manager.getCurrentEditor()->toPlainText(), QString("int x;\n"));

    answer = true;
    emit tabs.tabCloseRequested(0);
    QCOMPARE(asked.size(), 2);
    QCOMPARE(tabs.count(), 1);
    QVERIFY(manager.getCurrentEditor()->toPlainText().isEmpty());
}

void TestEditorTabManager::testClosingCleanOrSavedTabDoesNotAsk()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    int asked = 0;
    manager.setDiscardConfirmer([&](const QString&) {
        ++asked;
        return false;
    });

    CodeEditor* first = manager.addNewTab("a.cpp");
    first->insertPlainText("// a\n");
    QVERIFY(first->document()->isModified());
    QVERIFY(!manager.saveCurrentEditor(dir.path()).isEmpty());
    QVERIFY(!first->document()->isModified());

    manager.addNewTab("clean.cpp");
    QCOMPARE(tabs.count(), 2);

    emit tabs.tabCloseRequested(1);
    emit tabs.tabCloseRequested(0);

    QCOMPARE(asked, 0);
    QCOMPARE(tabs.count(), 1);
}

void TestEditorTabManager::testNewFileNamesKeepKnownExtensionsAndDropFolders()
{
    QCOMPARE(EditorTabManager::normalizeNewFileName("app.cpp"), QString("app.cpp"));
    QCOMPARE(EditorTabManager::normalizeNewFileName("util.hpp"), QString("util.hpp"));
    QCOMPARE(EditorTabManager::normalizeNewFileName("old.cc"), QString("old.cc"));
    QCOMPARE(EditorTabManager::normalizeNewFileName("tool.PY"), QString("tool.PY"));
    QCOMPARE(EditorTabManager::normalizeNewFileName("  spaced.py  "), QString("spaced.py"));
    QCOMPARE(EditorTabManager::normalizeNewFileName("notes"), QString("notes.cpp"));
    QCOMPARE(EditorTabManager::normalizeNewFileName("readme.txt"), QString("readme.txt.cpp"));
    QCOMPARE(EditorTabManager::normalizeNewFileName("../escape/up.cpp"), QString("up.cpp"));
    QVERIFY(EditorTabManager::normalizeNewFileName("   ").isEmpty());
}

void TestEditorTabManager::testSourceExtensionsCcAndCxxSaveUnderTheirOwnName()
{
    QTemporaryDir dir;
    QVERIFY(dir.isValid());
    QTabWidget tabs;
    EditorTabManager manager(&tabs);
    CodeEditor* editor = manager.addNewTab("legacy.cc");
    editor->setPlainText("int x;\n");

    QCOMPARE(manager.saveCurrentEditor(dir.path()), dir.path() + "/legacy.cc");
}

QTEST_MAIN(TestEditorTabManager)
#include "test_editortabmanager.moc"
