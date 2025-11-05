#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include <QPlainTextEdit>
#include <QFont>
#include <QSyntaxHighlighter>

class LineNumberArea;

class CodeEditor : public QPlainTextEdit {
    Q_OBJECT
public:
    CodeEditor(QWidget* parent = nullptr);

    int lineNumberAreaWidth();
    void lineNumberAreaPaintEvent(QPaintEvent* event);
    void setFilePath(const QString& path);
    QString filePath() const;
    void setHighlighter(QSyntaxHighlighter* highlighter);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void updateLineNumberAreaWidth(int);
    void updateLineNumberArea(const QRect& rect, int dy);
    void highlightCurrentLine();

private:
    LineNumberArea* lineNumberArea;
    QString m_filePath;
    QSyntaxHighlighter* m_highlighter;
};

#endif // CODEEDITOR_H
