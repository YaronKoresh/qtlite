#include <QtTest>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>
#include "../src/highlighting/pythonhighlighter.h"

class TestPythonHighlighter : public QObject
{
    Q_OBJECT

private slots:
    void testDocstringClosedOnItsOwnLineEndsTheString();
    void testSingleQuoteDocstringClosedOnItsOwnLineEndsTheString();
    void testOneLineTripleQuotedStringDoesNotContinue();
    void testMixedDelimitersDoNotCloseEachOther();
    void testStringStaysOpenUntilItsClosingDelimiter();
    void testStringsWithEscapedQuotesAreColoredAsOneString();
};

static QList<int> blockStates(const QString& code)
{
    QTextDocument doc;
    doc.setPlainText(code);
    PythonHighlighter highlighter(&doc);
    highlighter.rehighlight();

    QList<int> states;
    for (QTextBlock block = doc.begin(); block.isValid(); block = block.next())
        states << block.userState();
    return states;
}

void TestPythonHighlighter::testDocstringClosedOnItsOwnLineEndsTheString()
{
    const QList<int> states = blockStates("\"\"\"\nDescription\n\"\"\"\nx = 1\ny = 2\n");

    QCOMPARE(states.at(0), 1);
    QCOMPARE(states.at(1), 1);
    QCOMPARE(states.at(2), 0);
    QCOMPARE(states.at(3), 0);
    QCOMPARE(states.at(4), 0);
}

void TestPythonHighlighter::testSingleQuoteDocstringClosedOnItsOwnLineEndsTheString()
{
    const QList<int> states = blockStates("'''\nDescription\n'''\nx = 1\n");

    QCOMPARE(states.at(2), 0);
    QCOMPARE(states.at(3), 0);
}

void TestPythonHighlighter::testOneLineTripleQuotedStringDoesNotContinue()
{
    const QList<int> states = blockStates("\"\"\"doc\"\"\"\nx = 1\n");

    QCOMPARE(states.at(0), 0);
    QCOMPARE(states.at(1), 0);
}

void TestPythonHighlighter::testMixedDelimitersDoNotCloseEachOther()
{
    const QList<int> states = blockStates("\"\"\"\ntext '''\nmore\n\"\"\"\nx = 1\n");

    QCOMPARE(states.at(1), 1);
    QCOMPARE(states.at(2), 1);
    QCOMPARE(states.at(3), 0);
    QCOMPARE(states.at(4), 0);
}

void TestPythonHighlighter::testStringStaysOpenUntilItsClosingDelimiter()
{
    const QList<int> states = blockStates("x = \"\"\"start\nmiddle\nend\"\"\" + 'a'\ny = 1\n");

    QCOMPARE(states.at(0), 1);
    QCOMPARE(states.at(1), 1);
    QCOMPARE(states.at(2), 0);
    QCOMPARE(states.at(3), 0);
}

static int stringColoredLength(const QString& code)
{
    QTextDocument doc;
    doc.setPlainText(code);
    PythonHighlighter highlighter(&doc);
    highlighter.rehighlight();

    const QColor stringColor(163, 21, 21);
    int covered = 0;
    for (const QTextLayout::FormatRange& range : doc.firstBlock().layout()->formats()) {
        if (range.format.foreground().color() == stringColor)
            covered += range.length;
    }
    return covered;
}

void TestPythonHighlighter::testStringsWithEscapedQuotesAreColoredAsOneString()
{
    QCOMPARE(stringColoredLength("x = \"a\\\"b\" + y"), 6);
    QCOMPARE(stringColoredLength("x = 'it\\'s' + y"), 7);
}

QTEST_MAIN(TestPythonHighlighter)
#include "test_pythonhighlighter.moc"
