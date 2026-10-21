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
        "\\bsizeof\\b", "\\bbreak\\b", "\\bcontinue\\b", "\\bswitch\\b",
        "\\bcase\\b", "\\bdefault\\b", "\\bdo\\b", "\\btry\\b",
        "\\bcatch\\b", "\\bthrow\\b", "\\busing\\b", "\\bnullptr\\b",
        "\\bconstexpr\\b", "\\bnoexcept\\b", "\\boverride\\b", "\\bfinal\\b"
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
    addHighlightingRule("\"(?:[^\"\\\\\n]|\\\\.)*\"", quotationFormat);
}

void CppHighlighter::highlightMultilineComments(const QString& text) {
    setCurrentBlockState(0);

    const int length = text.length();
    bool inBlock = (previousBlockState() == 1);
    int blockStart = 0;
    int i = 0;

    while (i < length) {
        if (inBlock) {
            const int end = text.indexOf(commentEndExpression, i);
            if (end == -1) break;
            setFormat(blockStart, end + 2 - blockStart, multiLineCommentFormat);
            inBlock = false;
            i = end + 2;
            continue;
        }

        const QChar c = text.at(i);
        if (c == QLatin1Char('"') || c == QLatin1Char('\'')) {
            int j = i + 1;
            while (j < length && text.at(j) != c) {
                if (text.at(j) == QLatin1Char('\\')) ++j;
                ++j;
            }
            i = j + 1;
            continue;
        }

        if (c == QLatin1Char('/') && i + 1 < length) {
            if (text.at(i + 1) == QLatin1Char('/')) return;
            if (text.at(i + 1) == QLatin1Char('*')) {
                inBlock = true;
                blockStart = i;
                i += 2;
                continue;
            }
        }
        ++i;
    }

    if (inBlock) {
        setFormat(blockStart, length - blockStart, multiLineCommentFormat);
        setCurrentBlockState(1);
    }
}
