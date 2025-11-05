#ifndef SYNTAXHIGHLIGHTERBASE_H
#define SYNTAXHIGHLIGHTERBASE_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QVector>
#include <QRegularExpression>
#include "../core/projectstructs.h"

class SyntaxHighlighterBase : public QSyntaxHighlighter {
    Q_OBJECT
protected:
    QVector<HighlightingRule> highlightingRules;
    QTextCharFormat keywordFormat;
    QTextCharFormat commentFormat;
    QTextCharFormat quotationFormat;

    SyntaxHighlighterBase(QTextDocument* parent = nullptr) : QSyntaxHighlighter(parent) {}

    void setKeywordFormat(const QColor& color, bool bold = true) {
        keywordFormat.setForeground(color);
        if (bold) keywordFormat.setFontWeight(QFont::Bold);
    }

    void setCommentFormat(const QColor& color) {
        commentFormat.setForeground(color);
    }

    void setQuotationFormat(const QColor& color) {
        quotationFormat.setForeground(color);
    }

    void addHighlightingRule(const QString& pattern, const QTextCharFormat& format) {
        HighlightingRule rule;
        rule.pattern = QRegularExpression(pattern);
        rule.format = format;
        highlightingRules.append(rule);
    }

    void highlightSingleLinePatterns(const QString& text) {
        foreach (const HighlightingRule& rule, highlightingRules) {
            QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
            while (matchIterator.hasNext()) {
                QRegularExpressionMatch match = matchIterator.next();
                setFormat(match.capturedStart(), match.capturedLength(), rule.format);
            }
        }
    }
};

#endif // SYNTAXHIGHLIGHTERBASE_H
