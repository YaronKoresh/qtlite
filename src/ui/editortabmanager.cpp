#include "editortabmanager.h"

EditorTabManager::EditorTabManager(QTabWidget* tabs) : editorTabs(tabs), currentLanguage("C++ (Qt)") {
    connect(editorTabs, &QTabWidget::tabCloseRequested, this, &EditorTabManager::closeTab);
}

CodeEditor* EditorTabManager::getCurrentEditor() {
    return qobject_cast<CodeEditor*>(editorTabs->currentWidget());
}

CodeEditor* EditorTabManager::addNewTab(const QString& title) {
    CodeEditor* editor = new CodeEditor(editorTabs);
    updateEditorHighlighter(editor);
    editorTabs->addTab(editor, title);
    editorTabs->setCurrentWidget(editor);
    return editor;
}

void EditorTabManager::openFile(const QString& filePath) {
    for (int i = 0; i < editorTabs->count(); ++i) {
        CodeEditor* editor = qobject_cast<CodeEditor*>(editorTabs->widget(i));
        if (editor && editor->filePath() == filePath) {
            editorTabs->setCurrentIndex(i);
            return;
        }
    }

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        CodeEditor* editor = addNewTab(QFileInfo(filePath).fileName());
        editor->setPlainText(file.readAll());
        editor->setFilePath(filePath);
        file.close();
    }
}

void EditorTabManager::setLanguage(const QString& lang) {
    currentLanguage = lang;
    updateAllHighlighters();
}

void EditorTabManager::insertSnippet(const QString& snippet) {
    CodeEditor* editor = getCurrentEditor();
    if (editor) {
        editor->textCursor().insertText(snippet);
    }
}

QString EditorTabManager::saveCurrentEditor(const QString& buildDir) {
    CodeEditor* editor = getCurrentEditor();
    if (!editor) return QString();

    QString path = editor->filePath();
    if (path.isEmpty()) {
        const bool python = currentLanguage == "Python";
        const QString title = editorTabs->tabText(editorTabs->indexOf(editor));
        const QString suffix = QFileInfo(title).suffix().toLower();
        const bool titleFitsLanguage = python ? suffix == "py" : (suffix == "cpp" || suffix == "c");
        const QString fileName = titleFitsLanguage ? QFileInfo(title).fileName()
                                                   : QString(python ? "main.py" : "main.cpp");
        if (buildDir.isEmpty() || !QDir().mkpath(buildDir)) return QString();
        path = QDir(buildDir).absoluteFilePath(fileName);
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return QString();
    file.write(editor->toPlainText().toUtf8());
    if (!file.commit()) return QString();

    editor->setFilePath(path);
    return path;
}

void EditorTabManager::closeTab(int index) {
    CodeEditor* editor = qobject_cast<CodeEditor*>(editorTabs->widget(index));
    editorTabs->removeTab(index);
    delete editor;
    if (editorTabs->count() == 0) addNewTab("main.cpp");
}

void EditorTabManager::updateEditorHighlighter(CodeEditor* editor) {
    if (!editor) return;
    if (currentLanguage == "Python") {
        editor->setHighlighter(new PythonHighlighter());
    } else {
        editor->setHighlighter(new CppHighlighter());
    }
}

void EditorTabManager::updateAllHighlighters() {
    for (int i = 0; i < editorTabs->count(); ++i) {
        CodeEditor* editor = qobject_cast<CodeEditor*>(editorTabs->widget(i));
        updateEditorHighlighter(editor);
    }
}
