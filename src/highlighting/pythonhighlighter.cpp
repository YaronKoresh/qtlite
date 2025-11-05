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
        "\\bnot\\b", "\\band\\b", "\\bor\\b"
    };
    foreach (const QString& keyword, keywords) {
        addHighlightingRule(keyword, keywordFormat);
    }
}

void PythonHighlighter::initializeBuiltins() {
    QTextCharFormat builtInFormat;
    builtInFormat.setForeground(QColor(0, 100, 100));
    addHighlightingRule("\\b(True|False|None|self|print|len|str|int|float|list|dict|set|tuple)\\b", builtInFormat);
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
    addHighlightingRule("\"[^\"]*\"", quotationFormat);
    addHighlightingRule("'[^']*'", quotationFormat);
}

void PythonHighlighter::initializeMultilineStrings() {
    multiLineStringFormat.setForeground(QColor(163, 21, 21));
    multiLineStringStartExpression = QRegularExpression("(\"\"\"|\'\'\')");
    multiLineStringEndExpression = QRegularExpression("(\"\"\"|\'\'\')");
}

void PythonHighlighter::highlightMultilineStrings(const QString& text) {
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1) startIndex = text.indexOf(multiLineStringStartExpression);
    else multiLineStringEndExpression = multiLineStringStartExpression;

    while (startIndex >= 0) {
        QRegularExpressionMatch match = multiLineStringEndExpression.match(text, startIndex + 3);
        int endIndex = match.capturedStart();
        int stringLength;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            stringLength = text.length() - startIndex;
        } else {
            stringLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, stringLength, multiLineStringFormat);
        startIndex = text.indexOf(multiLineStringStartExpression, startIndex + stringLength);
    }
}
