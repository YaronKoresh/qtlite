#include <QtTest>
#include "../src/core/snippetmanager.h"

class TestSnippetManager : public QObject
{
    Q_OBJECT

private slots:
    void testGetSnippetNames();
    void testGetSnippet();
    void testGetAllLanguages();
    void testGetInvalidSnippet();
    void testPythonSnippetsArePython();
    void testTemplateSnippetDoesNotReuseStandardLibraryNames();
};

void TestSnippetManager::testGetSnippetNames()
{
    SnippetManager manager;
    
    QStringList cppQtSnippets = manager.getSnippetNames("C++ (Qt)");
    QVERIFY(cppQtSnippets.contains("C++ Main"));
    QVERIFY(cppQtSnippets.contains("For Loop"));
    QVERIFY(cppQtSnippets.contains("C++ Class"));
    
    QStringList pythonSnippets = manager.getSnippetNames("Python");
    QVERIFY(pythonSnippets.contains("Python Main"));
    QVERIFY(pythonSnippets.contains("Python Function"));
    QVERIFY(pythonSnippets.contains("Python Class"));
    
    QStringList cConsoleSnippets = manager.getSnippetNames("C/C++ (Console)");
    QVERIFY(cConsoleSnippets.contains("C++ Main"));
    QVERIFY(cConsoleSnippets.contains("C Main"));
}

void TestSnippetManager::testGetSnippet()
{
    SnippetManager manager;
    
    QString cppMain = manager.getSnippet("C++ Main");
    QVERIFY(!cppMain.isEmpty());
    QVERIFY(cppMain.contains("int main"));
    QVERIFY(cppMain.contains("iostream"));
    
    QString pythonMain = manager.getSnippet("Python Main");
    QVERIFY(!pythonMain.isEmpty());
    QVERIFY(pythonMain.contains("def main"));
    QVERIFY(pythonMain.contains("__name__"));
    
    QString forLoop = manager.getSnippet("For Loop");
    QVERIFY(!forLoop.isEmpty());
    QVERIFY(forLoop.contains("for"));
}

void TestSnippetManager::testGetAllLanguages()
{
    SnippetManager manager;
    
    QStringList languages = manager.getAllLanguages();
    QVERIFY(languages.contains("C++ (Qt)"));
    QVERIFY(languages.contains("Python"));
    QVERIFY(languages.contains("C/C++ (Console)"));
    QCOMPARE(languages.size(), 3);
}

void TestSnippetManager::testGetInvalidSnippet()
{
    SnippetManager manager;
    
    QString invalid = manager.getSnippet("NonexistentSnippet");
    QVERIFY(invalid.isEmpty());
}

void TestSnippetManager::testPythonSnippetsArePython()
{
    SnippetManager manager;

    const QStringList names = manager.getSnippetNames("Python");
    QVERIFY(names.contains("Python For Loop"));
    QVERIFY(names.contains("Python If/Else"));
    QVERIFY(!names.contains("For Loop"));
    QVERIFY(!names.contains("If/Else"));

    for (const QString& name : names) {
        const QString code = manager.getSnippet(name);
        QVERIFY2(!code.isEmpty(), qPrintable(name));
        QVERIFY2(!code.contains("for("), qPrintable(name));
        QVERIFY2(!code.contains("if ("), qPrintable(name));
        QVERIFY2(!code.contains("} else {"), qPrintable(name));
        QVERIFY2(!code.contains(';'), qPrintable(name));
        QVERIFY2(!code.contains("return result"), qPrintable(name));
    }

    const QString tryExcept = manager.getSnippet("Python Try/Except");
    QVERIFY(tryExcept.contains("try:\n    pass\nexcept"));
}

void TestSnippetManager::testTemplateSnippetDoesNotReuseStandardLibraryNames()
{
    SnippetManager manager;

    const QString code = manager.getSnippet("C++ Template");
    QVERIFY(code.contains("template<typename T>"));
    QVERIFY2(!code.contains(" max("), qPrintable(code));
    QVERIFY2(!code.contains(" min("), qPrintable(code));
}

QTEST_MAIN(TestSnippetManager)
#include "test_snippetmanager.moc"
