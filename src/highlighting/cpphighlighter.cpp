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
        "\\bconstexpr\\b", "\\bnoexcept\\b", "\\boverride\\b", "\\bfinal\\b", "\\bfloat\\b", "\\bwchar_t\\b", "\\bmutable\\b",
        "\\bstatic_assert\\b", "\\bdecltype\\b"
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

    multiLineCommentFormat.setForeground(Qt::gray);
    commentStartExpression = QRegularExpression("/\\*");
    commentEndExpression = QRegularExpression("\\*/");
}

void CppHighlighter::initializeStrings() {
    setQuotationFormat(QColor(163, 21, 21));
}

void CppHighlighter::highlightMultilineComments(const QString& text) {
    setCurrentBlockState(0);

    const int length = text.length();
    bool inBlock = (previousBlockState() == 1);
    int blockStart = 0;
    int i = 0;

    if (previousBlockState() == 2) {
        const QString delimiter = rawDelimiters.value(currentBlock().blockNumber() - 1);
        const QString closing = ")" + delimiter + "\"";
        const int end = text.indexOf(closing);
        if (end == -1) {
            paintLiteral(0, length, quotationFormat);
            rawDelimiters[currentBlock().blockNumber()] = delimiter;
            setCurrentBlockState(2);
            return;
        }
        paintLiteral(0, end + closing.length(), quotationFormat);
        i = end + closing.length();
    }

    while (i < length) {
        if (inBlock) {
            const int end = text.indexOf(commentEndExpression, i);
            if (end == -1) break;
            paintLiteral(blockStart, end + 2, multiLineCommentFormat);
            inBlock = false;
            i = end + 2;
            continue;
        }

        const QChar c = text.at(i);
        if (c == QLatin1Char('\'') && isDigitSeparator(text, i)) {
            ++i;
            continue;
        }
        if (c == QLatin1Char('"') && rawStringStart(text, i)) {
            int open = i + 1;
            while (open < length && text.at(open) != QLatin1Char('(')) ++open;
            const QString delimiter = text.mid(i + 1, open - i - 1);
            const QString closing = ")" + delimiter + "\"";
            const int end = text.indexOf(closing, open + 1);
            if (end == -1) {
                paintLiteral(i - 1, length, quotationFormat);
                rawDelimiters[currentBlock().blockNumber()] = delimiter;
                setCurrentBlockState(2);
                return;
            }
            paintLiteral(i - 1, end + closing.length(), quotationFormat);
            i = end + closing.length();
            continue;
        }

        if (c == QLatin1Char('"') || c == QLatin1Char('\'')) {
            int j = i + 1;
            while (j < length && text.at(j) != c) {
                if (text.at(j) == QLatin1Char('\\')) ++j;
                ++j;
            }
            if (c == QLatin1Char('"')) paintLiteral(i, qMin(j + 1, length), quotationFormat);
            i = j + 1;
            continue;
        }

        if (c == QLatin1Char('/') && i + 1 < length) {
            if (text.at(i + 1) == QLatin1Char('/')) {
                paintLiteral(i, length, commentFormat);
                return;
            }
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
        paintLiteral(blockStart, length, multiLineCommentFormat);
        setCurrentBlockState(1);
    }
}

bool CppHighlighter::rawStringStart(const QString& text, int quoteIndex) {
    if (quoteIndex < 1 || text.at(quoteIndex - 1) != QLatin1Char('R')) return false;

    int start = quoteIndex - 1;
    while (start > 0 && (text.at(start - 1).isLetterOrNumber() || text.at(start - 1) == QLatin1Char('_'))) {
        --start;
    }
    const QString prefix = text.mid(start, quoteIndex - 1 - start);
    if (!prefix.isEmpty() && prefix != "u8" && prefix != "u" && prefix != "U" && prefix != "L") return false;

    int j = quoteIndex + 1;
    while (j < text.length() && j - quoteIndex <= 17) {
        const QChar d = text.at(j);
        if (d == QLatin1Char('(')) return true;
        if (d.isSpace() || d == QLatin1Char(')') || d == QLatin1Char('\\') || d == QLatin1Char('"')) return false;
        ++j;
    }
    return false;
}

bool CppHighlighter::isDigitSeparator(const QString& text, int quoteIndex) {
    int start = quoteIndex;
    while (start > 0 && (text.at(start - 1).isLetterOrNumber() || text.at(start - 1) == QLatin1Char('_')
                         || text.at(start - 1) == QLatin1Char('\''))) {
        --start;
    }
    return start < quoteIndex && text.at(start).isDigit() &&
           text.at(quoteIndex - 1) != QLatin1Char('\'');
}

void CppHighlighter::paintLiteral(int start, int end, const QTextCharFormat& format) {
    setFormat(start, end - start, QTextCharFormat());
    setFormat(start, end - start, format);
}
