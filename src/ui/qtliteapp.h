#ifndef QTLITEAPP_H
#define QTLITEAPP_H

#include <QMainWindow>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QTreeView>
#include <QFileSystemModel>
#include <QTabWidget>
#include <QFormLayout>
#include <QAction>
#include <QModelIndex>
#include "uicontroller.h"
#include "editortabmanager.h"
#include "../core/pathvalidator.h"
#include "../compilation/compilationengine.h"
#include "../core/snippetmanager.h"
#include "../core/codebundler.h"

class QtLiteApp : public QMainWindow {
    Q_OBJECT

public:
    QtLiteApp(QWidget* parent = nullptr);

private:
    UIController uiController;
    EditorTabManager* editorTabManager;
    PathValidator pathValidator;
    CompilationEngine* compilationEngine;
    SnippetManager snippetManager;
    CodeBundler codeBundler;

    QLineEdit* qtDirEdit;
    QLineEdit* vsDirEdit;
    QLineEdit* pythonExeEdit;
    QLineEdit* includeDirsEdit;
    QLineEdit* libDirsEdit;
    QLineEdit* buildDirEdit;
    QComboBox* buildConfigCombo;
    QComboBox* languageCombo;
    QCheckBox* bundleCheckBox;
    QLabel* qtDirLabel;
    QLabel* vsDirLabel;
    QLabel* pythonExeLabel;
    QLabel* buildConfigLabel;
    QLabel* includeDirsLabel;
    QLabel* libDirsLabel;
    QTextEdit* outputLog;
    QPushButton* runButton;
    QPushButton* bundleCodeButton;
    QTreeView* fileBrowser;
    QFileSystemModel* fsModel;
    QTabWidget* editorTabs;

    void buildUserInterface();
    void buildSettingsPanel(QFormLayout* pathsLayout);
    void buildEditorArea(QHBoxLayout* mainRowLayout);
    void establishConnections();
    void loadAndValidateSettings();
    void validatePaths();
    void updateFileBrowserRootDirectory(const QString& path);

private slots:
    void savePathsAndValidate();
    void saveOtherPaths();
    void onLanguageChanged(const QString& lang);
    void browseForQtDir();
    void browseForVsDir();
    void browseForBuildDir();
    void browseForPythonExe();
    void openFileFromBrowser(const QModelIndex& index);
    void runCompilation();
    void bundleCode();
    void insertSnippet(QAction* action);
    void createNewEditorTab();
    void saveCurrentFile();
    void updateSnippetMenu();
};

#endif // QTLITEAPP_H
