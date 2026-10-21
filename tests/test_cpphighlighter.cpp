#include <QtTest>
#include <QTextDocument>
#include <QTextBlock>
#include <QTextLayout>
#include "../src/highlighting/cpphighlighter.h"

class TestCppHighlighter : public QObject
{
    Q_OBJECT

private slots:
    void testBlockCommentSpansLinesAndEnds();
    void testCommentStartInsideLineCommentDoesNotOpenBlock();
    void testCommentStartInsideStringDoesNotOpenBlock();
    void testCommentStartAfterCharLiteralQuoteStillOpensBlock();
    void testBlockCommentOnOneLineDoesNotContinue();
    void testSlashStarSlashDoesNotCloseItself();
    void testStringWithEscapedQuoteIsColoredAsOneString();
    void testControlFlowAndModernKeywordsAreHighlighted();
    void testCodeAfterStringWithLineCommentMarkerIsNotComment();
    void testQuotesInsideLineCommentAreNotStrings();
    void testQuotesInsideBlockCommentAreNotStrings();
    void testKeywordInsideStringKeepsStringColor();
    void testDigitSeparatorIsNotACharLiteralStart();
    void testHexDigitSeparatorIsNotACharLiteralStart();
};

static QList<int> blockStates(const QString& code)
{
    QTextDocument doc;
    doc.setPlainText(code);
    CppHighlighter highlighter(&doc);
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
    CppHighlighter highlighter(&doc);
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

void TestCppHighlighter::testBlockCommentSpansLinesAndEnds()
{
    const QList<int> states = blockStates("/* open\nstill\n*/ int z;\nint w;\n");

    QCOMPARE(states.at(0), 1);
    QCOMPARE(states.at(1), 1);
    QCOMPARE(states.at(2), 0);
    QCOMPARE(states.at(3), 0);
}

void TestCppHighlighter::testCommentStartInsideLineCommentDoesNotOpenBlock()
{
    const QList<int> states = blockStates("// see /* here\nint x;\nint y;\n");

    QCOMPARE(states.at(0), 0);
    QCOMPARE(states.at(1), 0);
    QCOMPARE(states.at(2), 0);
}

void TestCppHighlighter::testCommentStartInsideStringDoesNotOpenBlock()
{
    const QList<int> states = blockStates("const char* s = \"/*\";\nint y;\n");

    QCOMPARE(states.at(0), 0);
    QCOMPARE(states.at(1), 0);
}

void TestCppHighlighter::testCommentStartAfterCharLiteralQuoteStillOpensBlock()
{
    const QList<int> states = blockStates("char q = '\"'; /* open\nstill\n*/\nint y;\n");

    QCOMPARE(states.at(0), 1);
    QCOMPARE(states.at(1), 1);
    QCOMPARE(states.at(2), 0);
    QCOMPARE(states.at(3), 0);
}

void TestCppHighlighter::testBlockCommentOnOneLineDoesNotContinue()
{
    const QList<int> states = blockStates("int a; /* c */ int b;\nint y;\n");

    QCOMPARE(states.at(0), 0);
    QCOMPARE(states.at(1), 0);
}

void TestCppHighlighter::testSlashStarSlashDoesNotCloseItself()
{
    const QList<int> states = blockStates("/*/ still comment\n*/\nint y;\n");

    QCOMPARE(states.at(0), 1);
    QCOMPARE(states.at(1), 0);
    QCOMPARE(states.at(2), 0);
}

static QList<QTextLayout::FormatRange> formatsOfFirstLine(const QString& code)
{
    QTextDocument doc;
    doc.setPlainText(code);
    CppHighlighter highlighter(&doc);
    highlighter.rehighlight();
    return doc.firstBlock().layout()->formats();
}

void TestCppHighlighter::testStringWithEscapedQuoteIsColoredAsOneString()
{
    const QString code = "x = \"a\\\"b\" + y;";
    const QColor stringColor(163, 21, 21);

    int covered = 0;
    for (const QTextLayout::FormatRange& range : formatsOfFirstLine(code)) {
        if (range.format.foreground().color() == stringColor)
            covered += range.length;
    }

    QCOMPARE(covered, 6);
}

void TestCppHighlighter::testControlFlowAndModernKeywordsAreHighlighted()
{
    const QStringList words = {"break", "continue", "switch", "case", "default", "do", "try", "catch",
                               "throw", "using", "nullptr", "constexpr", "noexcept", "override", "final",
                               "float", "wchar_t", "mutable", "static_assert", "decltype"};
    for (const QString& word : words) {
        bool colored = false;
        for (const QTextLayout::FormatRange& range : formatsOfFirstLine(word + " x;")) {
            if (range.start == 0 && range.length == word.length() &&
                range.format.foreground().color() == QColor(Qt::darkBlue))
                colored = true;
        }
        QVERIFY2(colored, qPrintable(word));
    }
}

void TestCppHighlighter::testCodeAfterStringWithLineCommentMarkerIsNotComment()
{
    const QString code = "puts(\"a//b\"); int x; // note";

    QCOMPARE(colorAt(code, 0, 7), QColor(163, 21, 21));
    QCOMPARE(colorAt(code, 0, 14), QColor(Qt::darkBlue));
    QCOMPARE(colorAt(code, 0, 23), QColor(Qt::gray));
}

void TestCppHighlighter::testQuotesInsideLineCommentAreNotStrings()
{
    const QString code = "int a; // say \"hi\"";

    QCOMPARE(colorAt(code, 0, 16), QColor(Qt::gray));
    QCOMPARE(colorAt(code, 0, 17), QColor(Qt::gray));
}

void TestCppHighlighter::testQuotesInsideBlockCommentAreNotStrings()
{
    const QString code = "/* \"q\" */ int z;";

    QCOMPARE(colorAt(code, 0, 4), QColor(Qt::gray));
    QCOMPARE(colorAt(code, 0, 11), QColor(Qt::darkBlue));
}

void TestCppHighlighter::testKeywordInsideStringKeepsStringColor()
{
    const QString code = "auto s = \"for if\";";

    QCOMPARE(colorAt(code, 0, 11), QColor(163, 21, 21));
    QCOMPARE(colorAt(code, 0, 15), QColor(163, 21, 21));
}

void TestCppHighlighter::testDigitSeparatorIsNotACharLiteralStart()
{
    const QString code = "int n = 1'000; // it's";

    QCOMPARE(colorAt(code, 0, 15), QColor(Qt::gray));
    QCOMPARE(colorAt(code, 0, 21), QColor(Qt::gray));
}

void TestCppHighlighter::testHexDigitSeparatorIsNotACharLiteralStart()
{
    const QString code = "auto m = 0xFF'FF; /* a */ int z;";

    QCOMPARE(colorAt(code, 0, 18), QColor(Qt::gray));
    QCOMPARE(colorAt(code, 0, 26), QColor(Qt::darkBlue));
}

QTEST_MAIN(TestCppHighlighter)
#include "test_cpphighlighter.moc"
