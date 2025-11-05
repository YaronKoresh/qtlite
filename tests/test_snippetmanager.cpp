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

QTEST_MAIN(TestSnippetManager)
#include "test_snippetmanager.moc"
