#ifndef PYTHONHIGHLIGHTER_H
#define PYTHONHIGHLIGHTER_H

#include "syntaxhighlighterbase.h"
#include <QRegularExpression>
#include <QTextCharFormat>

class PythonHighlighter : public SyntaxHighlighterBase {
    Q_OBJECT
public:
    PythonHighlighter(QTextDocument* parent = nullptr);

protected:
    void highlightBlock(const QString& text) override;

private:
    QTextCharFormat multiLineStringFormat;

    void initializePythonKeywords();
    void initializeBuiltins();
    void initializeDecorators();
    void initializeComments();
    void initializeStrings();
    void initializeMultilineStrings();
    void highlightMultilineStrings(const QString& text);
    void paintLiteral(int start, int end, const QTextCharFormat& format);
};

#endif // PYTHONHIGHLIGHTER_H
