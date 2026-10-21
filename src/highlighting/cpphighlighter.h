#ifndef CPPHIGHLIGHTER_H
#define CPPHIGHLIGHTER_H

#include "syntaxhighlighterbase.h"
#include <QRegularExpression>
#include <QTextCharFormat>
#include <QHash>

class CppHighlighter : public SyntaxHighlighterBase {
    Q_OBJECT
public:
    CppHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QRegularExpression commentStartExpression;
    QRegularExpression commentEndExpression;
    QTextCharFormat multiLineCommentFormat;
    QHash<int, QString> rawDelimiters;

    void initializeCppKeywords();
    void initializeQtClasses();
    void initializeQtMacros();
    void initializePreprocessor();
    void initializeComments();
    void initializeStrings();
    void highlightMultilineComments(const QString& text);
    void paintLiteral(int start, int end, const QTextCharFormat& format);
    static bool rawStringStart(const QString& text, int quoteIndex);
    static bool isDigitSeparator(const QString& text, int quoteIndex);
};

#endif // CPPHIGHLIGHTER_H
