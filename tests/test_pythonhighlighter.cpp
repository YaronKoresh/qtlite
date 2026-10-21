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
    void testAssertDelAsyncAwaitAreKeywords();
    void testCommonBuiltinsAreHighlighted();
    void testMoreBuiltinsAreHighlighted();
    void testDottedDecoratorIsColoredAsAWhole();
    void testCodeAfterStringWithHashIsNotComment();
    void testQuotesInsideCommentAreNotStrings();
    void testTripleQuoteInsideCommentDoesNotOpenString();
    void testTripleQuoteInsideSingleLineStringDoesNotOpenString();
    void testEscapedQuoteBeforeTripleQuoteDoesNotCloseTheString();
    void testEscapedQuoteBeforeClosingTripleQuoteOnLaterLine();
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

static QColor colorAt(const QString& code, int line, int column)
{
    QTextDocument doc;
    doc.setPlainText(code);
    PythonHighlighter highlighter(&doc);
    highlighter.rehighlight();

    QTextBlock block = doc.findBlockByNumber(line);
    const QVector<QTextLayout::FormatRange> ranges = block.layout()->formats();
    QColor color;
    for (const QTextLayout::FormatRange& range : ranges) {
        if (column >= range.start && column < range.start + range.length &&
            range.format.hasProperty(QTextFormat::ForegroundBrush))
            color = range.format.foreground().color();
    }
    return color;
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

void TestPythonHighlighter::testAssertDelAsyncAwaitAreKeywords()
{
    const QStringList words = {"assert", "del", "async", "await"};
    for (const QString& word : words) {
        QTextDocument doc;
        doc.setPlainText(word + " x");
        PythonHighlighter highlighter(&doc);
        highlighter.rehighlight();

        bool colored = false;
        for (const QTextLayout::FormatRange& range : doc.firstBlock().layout()->formats()) {
            if (range.start == 0 && range.length == word.length() &&
                range.format.foreground().color() == QColor(Qt::darkBlue))
                colored = true;
        }
        QVERIFY2(colored, qPrintable(word));
    }
}

void TestPythonHighlighter::testCommonBuiltinsAreHighlighted()
{
    const QStringList words = {"bool", "range", "open", "input", "super", "cls", "enumerate", "zip",
                               "sorted", "isinstance", "sum", "min", "max", "abs"};
    for (const QString& word : words) {
        QTextDocument doc;
        doc.setPlainText(word + "(x)");
        PythonHighlighter highlighter(&doc);
        highlighter.rehighlight();

        bool colored = false;
        for (const QTextLayout::FormatRange& range : doc.firstBlock().layout()->formats()) {
            if (range.start == 0 && range.length == word.length() &&
                range.format.foreground().color() == QColor(0, 100, 100))
                colored = true;
        }
        QVERIFY2(colored, qPrintable(word));
    }
}

void TestPythonHighlighter::testCodeAfterStringWithHashIsNotComment()
{
    const QString code = "x = \"a#b\" + str(1)";

    QCOMPARE(colorAt(code, 0, 6), QColor(163, 21, 21));
    QCOMPARE(colorAt(code, 0, 12), QColor(0, 100, 100));
}

void TestPythonHighlighter::testQuotesInsideCommentAreNotStrings()
{
    const QString code = "# it's fine\ny = 1";

    QCOMPARE(colorAt(code, 0, 4), QColor(Qt::gray));
    QCOMPARE(colorAt(code, 0, 5), QColor(Qt::gray));
}

void TestPythonHighlighter::testTripleQuoteInsideCommentDoesNotOpenString()
{
    const QList<int> states = blockStates("# do not use \"\"\" here\nx = 1\ny = 2\n");

    QCOMPARE(states.at(0), 0);
    QCOMPARE(states.at(1), 0);
    QCOMPARE(states.at(2), 0);
}

void TestPythonHighlighter::testTripleQuoteInsideSingleLineStringDoesNotOpenString()
{
    const QList<int> states = blockStates("x = '\"\"\"'\ny = 1\n");

    QCOMPARE(states.at(0), 0);
    QCOMPARE(states.at(1), 0);
}

void TestPythonHighlighter::testEscapedQuoteBeforeTripleQuoteDoesNotCloseTheString()
{
    const QString code = "s = \"\"\"say \\\"\"\" hi\"\"\"\nx = 1\n";
    const QList<int> states = blockStates(code);

    QCOMPARE(states.at(0), 0);
    QCOMPARE(colorAt(code, 0, 14), QColor(163, 21, 21));
}

void TestPythonHighlighter::testEscapedQuoteBeforeClosingTripleQuoteOnLaterLine()
{
    const QString code = "s = \"\"\"first\nsay \\\"\"\" hi\"\"\"\nx = 1\n";
    const QList<int> states = blockStates(code);

    QCOMPARE(states.at(0), 1);
    QCOMPARE(states.at(1), 0);
    QCOMPARE(states.at(2), 0);
}

void TestPythonHighlighter::testMoreBuiltinsAreHighlighted()
{
    const QStringList words = {"map", "filter", "type", "any", "all", "reversed", "round", "repr",
                               "bytes", "id", "ord", "chr", "getattr", "setattr", "hasattr", "callable",
                               "iter", "next", "format", "pow", "divmod", "object", "hex", "bin",
                               "frozenset", "Exception", "ValueError", "__name__"};
    for (const QString& word : words) {
        QTextDocument doc;
        doc.setPlainText(word + "(x)");
        PythonHighlighter highlighter(&doc);
        highlighter.rehighlight();

        bool colored = false;
        for (const QTextLayout::FormatRange& range : doc.firstBlock().layout()->formats()) {
            if (range.start == 0 && range.length == word.length() &&
                range.format.foreground().color() == QColor(0, 100, 100))
                colored = true;
        }
        QVERIFY2(colored, qPrintable(word));
    }
}

void TestPythonHighlighter::testDottedDecoratorIsColoredAsAWhole()
{
    const QString code = "@app.route(\"/\")\ndef home(): pass\n";

    QCOMPARE(colorAt(code, 0, 1), QColor(Qt::darkMagenta));
    QCOMPARE(colorAt(code, 0, 5), QColor(Qt::darkMagenta));
    QCOMPARE(colorAt(code, 0, 9), QColor(Qt::darkMagenta));
}

QTEST_MAIN(TestPythonHighlighter)
#include "test_pythonhighlighter.moc"
