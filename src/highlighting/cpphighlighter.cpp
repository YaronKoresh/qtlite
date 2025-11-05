#include "cpphighlighter.h"

CppHighlighter::CppHighlighter(QTextDocument* parent) : SyntaxHighlighterBase(parent) {
    initializeCppKeywords();
    initializeQtClasses();
    initializeQtMacros();
    initializePreprocessor();
    initializeComments();
    initializeStrings();
}

void CppHighlighter::highlightBlock(const QString& text) {
    highlightSingleLinePatterns(text);
    highlightMultilineComments(text);
}

void CppHighlighter::initializeCppKeywords() {
    setKeywordFormat(Qt::darkBlue, true);
    QStringList keywords = {
        "\\bchar\\b", "\\bclass\\b", "\\bconst\\b", "\\bdouble\\b",
        "\\benum\\b", "\\bexplicit\\b", "\\bfriend\\b", "\\binline\\b",
        "\\bint\\b", "\\blong\\b", "\\bnamespace\\b", "\\boperator\\b",
        "\\bprivate\\b", "\\bprotected\\b", "\\bpublic\\b", "\\bshort\\b",
        "\\bsignals\\b", "\\bsigned\\b", "\\bslots\\b", "\\bstatic\\b",
        "\\bstruct\\b", "\\btemplate\\b", "\\btypedef\\b", "\\btypename\\b",
        "\\bunion\\b", "\\bunsigned\\b", "\\bvirtual\\b", "\\bvoid\\b",
        "\\bvolatile\\b", "\\bbool\\b", "\\bfor\\b", "\\bif\\b",
        "\\belse\\b", "\\bwhile\\b", "\\breturn\\b", "\\bnew\\b",
        "\\bdelete\\b", "\\bthis\\b", "\\btrue\\b", "\\bfalse\\b",
        "\\bauto\\b", "\\bextern\\b", "\\bregister\\b", "\\bgoto\\b",
        "\\bsizeof\\b"
    };
    foreach (const QString& keyword, keywords) {
        addHighlightingRule(keyword, keywordFormat);
    }
}

void CppHighlighter::initializeQtClasses() {
    QTextCharFormat qtClassFormat;
    qtClassFormat.setForeground(QColor(0, 100, 100));
    qtClassFormat.setFontWeight(QFont::Bold);
    addHighlightingRule("\\bQ[A-Za-z]+\\b", qtClassFormat);
}

void CppHighlighter::initializeQtMacros() {
    QTextCharFormat qtMacroFormat;
    qtMacroFormat.setForeground(Qt::darkMagenta);
    addHighlightingRule("\\bQ_[A-Z_]+\\b", qtMacroFormat);
}

void CppHighlighter::initializePreprocessor() {
    QTextCharFormat preprocessorFormat;
    preprocessorFormat.setForeground(Qt::darkGreen);
    preprocessorFormat.setFontWeight(QFont::Bold);
    addHighlightingRule("^\\s*#[a-zA-Z_]+", preprocessorFormat);
}

void CppHighlighter::initializeComments() {
    setCommentFormat(Qt::gray);
    addHighlightingRule("//[^\n]*", commentFormat);

    multiLineCommentFormat.setForeground(Qt::gray);
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void CppHighlighter::initializeStrings() {
    setQuotationFormat(QColor(163, 21, 21));
    addHighlightingRule("\"[^\"]*\"", quotationFormat);
}

void CppHighlighter::highlightMultilineComments(const QString& text) {
    setCurrentBlockState(0);
    int startIndex = 0;
    if (previousBlockState() != 1) startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength;

        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex + match.capturedLength();
        }

        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
