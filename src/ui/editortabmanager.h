#ifndef EDITORTABMANAGER_H
#define EDITORTABMANAGER_H

#include <QObject>
#include <QTabWidget>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QSaveFile>
#include <QShortcut>
#include <QKeySequence>
#include <functional>
#include "../editor/codeeditor.h"
#include "../highlighting/cpphighlighter.h"
#include "../highlighting/pythonhighlighter.h"

class EditorTabManager : public QObject {
    Q_OBJECT
private:
    QTabWidget* editorTabs;
    QString currentLanguage;
    std::function<bool(const QString&)> discardConfirmer;

public:
    EditorTabManager(QTabWidget* tabs);

    CodeEditor* getCurrentEditor();
    CodeEditor* addNewTab(const QString& title);
    void openFile(const QString& filePath);
    void setLanguage(const QString& lang);
    void insertSnippet(const QString& snippet);
    QString saveCurrentEditor(const QString& buildDir);
    void setDiscardConfirmer(std::function<bool(const QString&)> confirmer);

signals:
    void saveRequested();

private slots:
    void closeTab(int index);

private:
    QString uniquePathAmongOpenFiles(const QString& wanted, const CodeEditor* self) const;
    void updateEditorHighlighter(CodeEditor* editor);
    void updateAllHighlighters();
};

#endif // EDITORTABMANAGER_H
