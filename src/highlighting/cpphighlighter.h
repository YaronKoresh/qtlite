#ifndef CPPHIGHLIGHTER_H
#define CPPHIGHLIGHTER_H

#include "syntaxhighlighterbase.h"
#include <QRegularExpression>
#include <QTextCharFormat>

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

    void initializeCppKeywords();
    void initializeQtClasses();
    void initializeQtMacros();
    void initializePreprocessor();
    void initializeComments();
    void initializeStrings();
    void highlightMultilineComments(const QString& text);
};

#endif // CPPHIGHLIGHTER_H
