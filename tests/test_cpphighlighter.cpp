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

QTEST_MAIN(TestCppHighlighter)
#include "test_cpphighlighter.moc"
