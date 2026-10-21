#include "editortabmanager.h"
#include <QMessageBox>

EditorTabManager::EditorTabManager(QTabWidget* tabs) : editorTabs(tabs), currentLanguage("C++ (Qt)") {
    connect(editorTabs, &QTabWidget::tabCloseRequested, this, &EditorTabManager::closeTab);

    QShortcut* saveShortcut = new QShortcut(QKeySequence::Save, editorTabs);
    saveShortcut->setContext(Qt::WindowShortcut);
    connect(saveShortcut, &QShortcut::activated, this, &EditorTabManager::saveRequested);

    discardConfirmer = [this](const QString& title) {
        return QMessageBox::question(editorTabs, "Unsaved changes",
                                     "\"" + title + "\" has unsaved changes. Close it anyway?",
                                     QMessageBox::Yes | QMessageBox::No,
                                     QMessageBox::No) == QMessageBox::Yes;
    };
}

void EditorTabManager::setDiscardConfirmer(std::function<bool(const QString&)> confirmer) {
    discardConfirmer = std::move(confirmer);
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
        const bool titleFitsLanguage = python ? suffix == "py"
                                              : (suffix == "cpp" || suffix == "cc" || suffix == "cxx" || suffix == "c");
        const QString fileName = titleFitsLanguage ? QFileInfo(title).fileName()
                                                   : QString(python ? "main.py" : "main.cpp");
        if (buildDir.isEmpty() || !QDir().mkpath(buildDir)) return QString();
        path = uniquePathAmongOpenFiles(QDir(buildDir).absoluteFilePath(fileName), editor);
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) return QString();
    file.write(editor->toPlainText().toUtf8());
    if (!file.commit()) return QString();

    editor->document()->setModified(false);
    editor->setFilePath(path);
    return path;
}

QString EditorTabManager::normalizeNewFileName(const QString& typed) {
    const QString name = QFileInfo(typed.trimmed()).fileName();
    if (name.isEmpty()) return QString();

    static const QStringList known = {"cpp", "cc", "cxx", "c", "h", "hpp", "hxx", "py"};
    if (known.contains(QFileInfo(name).suffix().toLower())) return name;
    return name + ".cpp";
}

QString EditorTabManager::uniquePathAmongOpenFiles(const QString& wanted, const CodeEditor* self) const {
    auto taken = [&](const QString& candidate) {
        const QString clean = QDir::cleanPath(candidate);
        for (int i = 0; i < editorTabs->count(); ++i) {
            CodeEditor* other = qobject_cast<CodeEditor*>(editorTabs->widget(i));
            if (other && other != self && !other->filePath().isEmpty() &&
                QDir::cleanPath(other->filePath()) == clean) {
                return true;
            }
        }
        return false;
    };

    if (!taken(wanted)) return wanted;

    const QFileInfo info(wanted);
    const QString suffix = info.suffix().isEmpty() ? QString() : "." + info.suffix();
    for (int n = 2;; ++n) {
        const QString candidate = info.absolutePath() + "/" + info.completeBaseName() +
                                  "_" + QString::number(n) + suffix;
        if (!taken(candidate)) return candidate;
    }
}

void EditorTabManager::closeTab(int index) {
    CodeEditor* editor = qobject_cast<CodeEditor*>(editorTabs->widget(index));
    if (editor && editor->document()->isModified() && discardConfirmer &&
        !discardConfirmer(editorTabs->tabText(index))) {
        return;
    }
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
