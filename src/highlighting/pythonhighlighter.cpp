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
    addHighlightingRule("\\b(True|False|None|self|print|len|str|int|float|list|dict|set|tuple|bool|range|open|input|super|cls|enumerate|zip|sorted|isinstance|sum|min|max|abs|map|filter|type|any|all|reversed|round|repr|bytes|id|ord|chr|getattr|setattr|hasattr|callable|iter|next|format|pow|divmod|object|hex|bin|frozenset|Exception|[A-Z][A-Za-z]*Error|__name__)\\b", builtInFormat);
}

void PythonHighlighter::initializeDecorators() {
    QTextCharFormat decoratorFormat;
    decoratorFormat.setForeground(Qt::darkMagenta);
    addHighlightingRule("@[A-Za-z0-9_]+", decoratorFormat);
}

void PythonHighlighter::initializeComments() {
    setCommentFormat(Qt::gray);
}

void PythonHighlighter::initializeStrings() {
    setQuotationFormat(QColor(163, 21, 21));
}

void PythonHighlighter::initializeMultilineStrings() {
    multiLineStringFormat.setForeground(QColor(163, 21, 21));
}


int PythonHighlighter::findClosingDelimiter(const QString& text, const QString& delimiter, int from) {
    int j = from;
    while (j < text.length()) {
        if (text.at(j) == QLatin1Char('\\')) {
            j += 2;
            continue;
        }
        if (text.mid(j, 3) == delimiter) return j;
        ++j;
    }
    return -1;
}

void PythonHighlighter::highlightMultilineStrings(const QString& text) {
    setCurrentBlockState(0);

    const int length = text.length();
    const int previous = previousBlockState();
    int i = 0;

    if (previous == 1 || previous == 2) {
        const QString delimiter = (previous == 1) ? QStringLiteral("\"\"\"") : QStringLiteral("\'\'\'");
        const int endIndex = findClosingDelimiter(text, delimiter, 0);
        const int stringEnd = (endIndex == -1) ? length : endIndex + 3;
        paintLiteral(0, stringEnd, multiLineStringFormat);
        if (endIndex == -1) {
            setCurrentBlockState(previous);
            return;
        }
        i = stringEnd;
    }

    while (i < length) {
        const QChar c = text.at(i);

        if (c == QLatin1Char('#')) {
            paintLiteral(i, length, commentFormat);
            return;
        }

        if (c == QLatin1Char('"') || c == QLatin1Char('\'')) {
            const QString triple = QString(3, c);
            if (text.mid(i, 3) == triple) {
                const int endIndex = findClosingDelimiter(text, triple, i + 3);
                const int stringEnd = (endIndex == -1) ? length : endIndex + 3;
                paintLiteral(i, stringEnd, multiLineStringFormat);
                if (endIndex == -1) {
                    setCurrentBlockState(c == QLatin1Char('"') ? 1 : 2);
                    return;
                }
                i = stringEnd;
                continue;
            }

            int j = i + 1;
            while (j < length && text.at(j) != c) {
                if (text.at(j) == QLatin1Char('\\')) ++j;
                ++j;
            }
            const int stringEnd = qMin(j + 1, length);
            paintLiteral(i, stringEnd, quotationFormat);
            i = stringEnd;
            continue;
        }

        ++i;
    }
}

void PythonHighlighter::paintLiteral(int start, int end, const QTextCharFormat& format) {
    setFormat(start, end - start, QTextCharFormat());
    setFormat(start, end - start, format);
}
