#ifndef EDITORTABMANAGER_H
#define EDITORTABMANAGER_H

#include <QObject>
#include <QTabWidget>
#include <QFile>
#include <QFileInfo>
#include "../editor/codeeditor.h"
#include "../highlighting/cpphighlighter.h"
#include "../highlighting/pythonhighlighter.h"

class EditorTabManager : public QObject {
    Q_OBJECT
private:
    QTabWidget* editorTabs;
    QString currentLanguage;

public:
    EditorTabManager(QTabWidget* tabs);

    CodeEditor* getCurrentEditor();
    CodeEditor* addNewTab(const QString& title);
    void openFile(const QString& filePath);
    void setLanguage(const QString& lang);
    void insertSnippet(const QString& snippet);

private slots:
    void closeTab(int index);

private:
    void updateEditorHighlighter(CodeEditor* editor);
    void updateAllHighlighters();
};

#endif // EDITORTABMANAGER_H
