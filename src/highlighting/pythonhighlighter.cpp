#include "pythonhighlighter.h"

PythonHighlighter::PythonHighlighter(QTextDocument* parent) : SyntaxHighlighterBase(parent) {
    initializePythonKeywords();
    initializeBuiltins();
    initializeDecorators();
    initializeComments();
    initializeStrings();
    initializeMultilineStrings();
}

void PythonHighlighter::highlightBlock(const QString& text) {
    highlightSingleLinePatterns(text);
    highlightMultilineStrings(text);
}

void PythonHighlighter::initializePythonKeywords() {
    setKeywordFormat(Qt::darkBlue, true);
    QStringList keywords = {
        "\\bif\\b", "\\belif\\b", "\\belse\\b", "\\bfor\\b", "\\bwhile\\b",
        "\\bpass\\b", "\\bbreak\\b", "\\bcontinue\\b", "\\breturn\\b",
        "\\byield\\b", "\\btry\\b", "\\bexcept\\b", "\\bfinally\\b",
        "\\braise\\b", "\\bwith\\b", "\\bas\\b", "\\bimport\\b",
        "\\bfrom\\b", "\\bclass\\b", "\\bdef\\b", "\\blambda\\b",
        "\\bglobal\\b", "\\bnonlocal\\b", "\\bin\\b", "\\bis\\b",
        "\\bnot\\b", "\\band\\b", "\\bor\\b", "\\bassert\\b",
        "\\bdel\\b", "\\basync\\b", "\\bawait\\b"
    };
    foreach (const QString& keyword, keywords) {
        addHighlightingRule(keyword, keywordFormat);
    }
}

void PythonHighlighter::initializeBuiltins() {
    QTextCharFormat builtInFormat;
    builtInFormat.setForeground(QColor(0, 100, 100));
    addHighlightingRule("\\b(True|False|None|self|print|len|str|int|float|list|dict|set|tuple|bool|range|open|input|super|cls|enumerate|zip|sorted|isinstance|sum|min|max|abs)\\b", builtInFormat);
}

void PythonHighlighter::initializeDecorators() {
    QTextCharFormat decoratorFormat;
    decoratorFormat.setForeground(Qt::darkMagenta);
    addHighlightingRule("@[A-Za-z0-9_]+", decoratorFormat);
}

void PythonHighlighter::initializeComments() {
    setCommentFormat(Qt::gray);
    addHighlightingRule("#[^\n]*", commentFormat);
}

void PythonHighlighter::initializeStrings() {
    setQuotationFormat(QColor(163, 21, 21));
    addHighlightingRule("\"(?:[^\"\\\\\n]|\\\\.)*\"", quotationFormat);
    addHighlightingRule("'(?:[^'\\\\\n]|\\\\.)*'", quotationFormat);
}

void PythonHighlighter::initializeMultilineStrings() {
    multiLineStringFormat.setForeground(QColor(163, 21, 21));
    multiLineStringStartExpression = QRegularExpression("(\"\"\"|\'\'\')");
}

void PythonHighlighter::highlightMultilineStrings(const QString& text) {
    setCurrentBlockState(0);

    const int previous = previousBlockState();
    const bool continuing = (previous == 1 || previous == 2);
    int startIndex = 0;
    QString delimiter;
    bool insideFromPreviousLine = continuing;

    if (continuing) {
        delimiter = (previous == 1) ? QStringLiteral("\"\"\"") : QStringLiteral("\'\'\'");
    } else {
        QRegularExpressionMatch start = multiLineStringStartExpression.match(text);
        startIndex = start.hasMatch() ? start.capturedStart() : -1;
        if (start.hasMatch()) delimiter = start.captured(1);
    }

    while (startIndex >= 0) {
        const int searchFrom = insideFromPreviousLine ? startIndex : startIndex + delimiter.length();
        const int endIndex = text.indexOf(delimiter, searchFrom);
        int stringLength;

        if (endIndex == -1) {
            setCurrentBlockState(delimiter == QLatin1String("\"\"\"") ? 1 : 2);
            stringLength = text.length() - startIndex;
        } else {
            stringLength = endIndex - startIndex + delimiter.length();
        }

        setFormat(startIndex, stringLength, multiLineStringFormat);
        insideFromPreviousLine = false;

        QRegularExpressionMatch next = multiLineStringStartExpression.match(text, startIndex + stringLength);
        if (!next.hasMatch()) break;
        startIndex = next.capturedStart();
        delimiter = next.captured(1);
    }
}
