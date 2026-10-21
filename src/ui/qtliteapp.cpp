#include "qtliteapp.h"
#include "../core/settingspersistence.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFileDialog>
#include <QMessageBox>
#include <QStandardPaths>
#include <QMenu>
#include <QToolButton>
#include <QInputDialog>
#include <QSplitter>
#include <memory>

QtLiteApp::QtLiteApp(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("QtLite IDE");
    resize(1024, 768);

    buildUserInterface();
    establishConnections();
    loadAndValidateSettings();
}

void QtLiteApp::buildUserInterface() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);

    QFormLayout* pathsLayout = new QFormLayout();
    buildSettingsPanel(pathsLayout);

    QHBoxLayout* mainRowLayout = new QHBoxLayout();
    buildEditorArea(mainRowLayout);

    mainLayout->addLayout(pathsLayout);
    mainLayout->addLayout(mainRowLayout, 1);
    setCentralWidget(centralWidget);
}

void QtLiteApp::buildSettingsPanel(QFormLayout* pathsLayout) {
    uiController.setSettingsFormLayout(pathsLayout);

    languageCombo = new QComboBox(this);
    languageCombo->addItems({"C++ (Qt)", "C/C++ (Console)", "Python"});
    pathsLayout->addRow("Language:", languageCombo);

    qtDirEdit = new QLineEdit(this);
    QPushButton* qtBrowseButton = new QPushButton("Browse...", this);
    QWidget* qtContainer = new QWidget(this);
    QHBoxLayout* qtLayout = new QHBoxLayout(qtContainer);
    qtLayout->setContentsMargins(0, 0, 0, 0);
    qtLayout->addWidget(qtDirEdit);
    qtLayout->addWidget(qtBrowseButton);
    qtDirLabel = new QLabel("Qt Version Directory:", this);
    pathsLayout->addRow(qtDirLabel, qtContainer);

    vsDirEdit = new QLineEdit(this);
    QPushButton* vsBrowseButton = new QPushButton("Browse...", this);
    QWidget* vsContainer = new QWidget(this);
    QHBoxLayout* vsLayout = new QHBoxLayout(vsContainer);
    vsLayout->setContentsMargins(0, 0, 0, 0);
    vsLayout->addWidget(vsDirEdit);
    vsLayout->addWidget(vsBrowseButton);
    vsDirLabel = new QLabel("Visual Studio Directory:", this);
    pathsLayout->addRow(vsDirLabel, vsContainer);

    pythonExeEdit = new QLineEdit(this);
    QPushButton* pyBrowseButton = new QPushButton("Browse...", this);
    QWidget* pyContainer = new QWidget(this);
    QHBoxLayout* pyLayout = new QHBoxLayout(pyContainer);
    pyLayout->setContentsMargins(0, 0, 0, 0);
    pyLayout->addWidget(pythonExeEdit);
    pyLayout->addWidget(pyBrowseButton);
    pythonExeLabel = new QLabel("Python Executable:", this);
    pathsLayout->addRow(pythonExeLabel, pyContainer);

    buildConfigCombo = new QComboBox(this);
    buildConfigCombo->addItems({"Release", "Debug"});
    buildConfigLabel = new QLabel("Build Config:", this);
    QWidget* buildConfigContainer = new QWidget(this);
    QHBoxLayout* buildConfigLayout = new QHBoxLayout(buildConfigContainer);
    buildConfigLayout->setContentsMargins(0, 0, 0, 0);
    buildConfigLayout->addWidget(buildConfigCombo);
    buildConfigLayout->addStretch();
    pathsLayout->addRow(buildConfigLabel, buildConfigContainer);

    includeDirsEdit = new QLineEdit(this);
    includeDirsEdit->setPlaceholderText("Optional: C:/path/one;C:/path/two");
    includeDirsLabel = new QLabel("Include Directories:", this);
    QWidget* includeDirsContainer = new QWidget(this);
    QHBoxLayout* includeDirsLayout = new QHBoxLayout(includeDirsContainer);
    includeDirsLayout->setContentsMargins(0, 0, 0, 0);
    includeDirsLayout->addWidget(includeDirsEdit);
    includeDirsLayout->addStretch();
    pathsLayout->addRow(includeDirsLabel, includeDirsContainer);

    libDirsEdit = new QLineEdit(this);
    libDirsEdit->setPlaceholderText("Optional: C:/path/to/libs;C:/path/mylib.lib");
    libDirsLabel = new QLabel("Library Paths/Files:", this);
    QWidget* libDirsContainer = new QWidget(this);
    QHBoxLayout* libDirsLayout = new QHBoxLayout(libDirsContainer);
    libDirsLayout->setContentsMargins(0, 0, 0, 0);
    libDirsLayout->addWidget(libDirsEdit);
    libDirsLayout->addStretch();
    pathsLayout->addRow(libDirsLabel, libDirsContainer);

    buildDirEdit = new QLineEdit(this);
    QPushButton* buildBrowseButton = new QPushButton("Browse...", this);
    QWidget* buildContainer = new QWidget(this);
    QHBoxLayout* buildLayout = new QHBoxLayout(buildContainer);
    buildLayout->setContentsMargins(0, 0, 0, 0);
    buildLayout->addWidget(buildDirEdit);
    buildLayout->addWidget(buildBrowseButton);
    pathsLayout->addRow("Build (Project) Directory:", buildContainer);

    connect(qtBrowseButton, &QPushButton::clicked, this, &QtLiteApp::browseForQtDir);
    connect(vsBrowseButton, &QPushButton::clicked, this, &QtLiteApp::browseForVsDir);
    connect(pyBrowseButton, &QPushButton::clicked, this, &QtLiteApp::browseForPythonExe);
    connect(buildBrowseButton, &QPushButton::clicked, this, &QtLiteApp::browseForBuildDir);

    uiController.setQtDirEdit(qtDirEdit);
    uiController.setVsDirEdit(vsDirEdit);
    uiController.setPythonExeEdit(pythonExeEdit);
    uiController.setIncludeDirsEdit(includeDirsEdit);
    uiController.setLibDirsEdit(libDirsEdit);
    uiController.setBuildDirEdit(buildDirEdit);
    uiController.setBuildConfigCombo(buildConfigCombo);
    uiController.setLanguageCombo(languageCombo);
    uiController.setQtDirLabel(qtDirLabel);
    uiController.setVsDirLabel(vsDirLabel);
    uiController.setPythonExeLabel(pythonExeLabel);
    uiController.setBuildConfigLabel(buildConfigLabel);
    uiController.setIncludeDirsLabel(includeDirsLabel);
    uiController.setLibDirsLabel(libDirsLabel);
    uiController.setQtDirContainer(qtContainer);
    uiController.setVsDirContainer(vsContainer);
    uiController.setPythonExeContainer(pyContainer);
    uiController.setBuildConfigContainer(buildConfigContainer);
    uiController.setIncludeDirsContainer(includeDirsContainer);
    uiController.setLibDirsContainer(libDirsContainer);
}

void QtLiteApp::buildEditorArea(QHBoxLayout* mainRowLayout) {
    QVBoxLayout* leftLayout = new QVBoxLayout();
    fileBrowser = new QTreeView();
    fsModel = new QFileSystemModel();
    fsModel->setFilter(QDir::AllDirs | QDir::Files | QDir::NoDotAndDotDot);
    fsModel->setRootPath(QStandardPaths::writableLocation(QStandardPaths::DesktopLocation));
    fileBrowser->setModel(fsModel);
    fileBrowser->setColumnWidth(0, 250);
    fileBrowser->setColumnHidden(1, true);
    fileBrowser->setColumnHidden(2, true);
    fileBrowser->setColumnHidden(3, true);
    leftLayout->addWidget(fileBrowser);

    QWidget* leftPanel = new QWidget();
    leftPanel->setLayout(leftLayout);

    QVBoxLayout* rightLayout = new QVBoxLayout();

    editorTabs = new QTabWidget(this);
    editorTabs->setTabsClosable(true);
    editorTabs->setMovable(true);

    editorTabManager = new EditorTabManager(editorTabs);
    editorTabManager->addNewTab("main.cpp");

    rightLayout->addWidget(editorTabs, 3);

    runButton = new QPushButton("Build and Run", this);
    bundleCodeButton = new QPushButton("Bundle Code", this);
    bundleCheckBox = new QCheckBox("Bundle into Single EXE (Installer)", this);
    QPushButton* addTabButton = new QPushButton("Add Tab", this);

    QHBoxLayout* buildControlsLayout = new QHBoxLayout();
    buildControlsLayout->addWidget(runButton);
    buildControlsLayout->addWidget(bundleCodeButton);
    buildControlsLayout->addWidget(bundleCheckBox);
    buildControlsLayout->addWidget(addTabButton);
    buildControlsLayout->addStretch();

    QToolButton* snippetButton = new QToolButton(this);
    snippetButton->setText("Insert Snippet");
    snippetButton->setPopupMode(QToolButton::InstantPopup);
    QMenu* snippetMenu = new QMenu(snippetButton);

    QString currentLanguage = languageCombo->currentText();
    QStringList currentSnippets = snippetManager.getSnippetNames(currentLanguage);

    for (const QString& name : std::as_const(currentSnippets)) {
        snippetMenu->addAction(name);
    }

    snippetButton->setMenu(snippetMenu);
    buildControlsLayout->addWidget(snippetButton);

    rightLayout->addLayout(buildControlsLayout);

    outputLog = new QTextEdit(this);
    outputLog->setReadOnly(true);
    rightLayout->addWidget(outputLog, 1);

    // Initialize compilationEngine after outputLog is created
    compilationEngine = new CompilationEngine(outputLog);

    QWidget* rightPanel = new QWidget();
    rightPanel->setLayout(rightLayout);

    mainRowLayout->addWidget(leftPanel, 1);
    mainRowLayout->addWidget(rightPanel, 3);

    uiController.setRunButton(runButton);
    uiController.setBundleCodeButton(bundleCodeButton);
    uiController.setBundleCheckBox(bundleCheckBox);
    uiController.setOutputLog(outputLog);
    uiController.setFileBrowser(fileBrowser);
    uiController.setFileSystemModel(fsModel);

    connect(runButton, &QPushButton::clicked, this, &QtLiteApp::runCompilation);
    connect(bundleCodeButton, &QPushButton::clicked, this, &QtLiteApp::bundleCode);
    connect(addTabButton, &QPushButton::clicked, this, &QtLiteApp::createNewEditorTab);
    connect(editorTabManager, &EditorTabManager::saveRequested, this, &QtLiteApp::saveCurrentFile);
    connect(fileBrowser, &QTreeView::doubleClicked, this, &QtLiteApp::openFileFromBrowser);
    connect(snippetMenu, &QMenu::triggered, this, &QtLiteApp::insertSnippet);
    connect(languageCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &QtLiteApp::updateSnippetMenu);
}

void QtLiteApp::updateSnippetMenu() {
    QToolButton* snippetButton = nullptr;
    for (QObject* child : findChildren<QToolButton*>()) {
        if (qobject_cast<QToolButton*>(child) &&
            qobject_cast<QToolButton*>(child)->text() == "Insert Snippet") {
            snippetButton = qobject_cast<QToolButton*>(child);
            break;
        }
    }

    if (!snippetButton) return;

    QMenu* oldMenu = snippetButton->menu();
    if (oldMenu) {
        delete oldMenu;
    }

    QMenu* newSnippetMenu = new QMenu(snippetButton);
    QString currentLanguage = languageCombo->currentText();
    QStringList currentSnippets = snippetManager.getSnippetNames(currentLanguage);

    for (const QString& name : std::as_const(currentSnippets)) {
        newSnippetMenu->addAction(name);
    }

    snippetButton->setMenu(newSnippetMenu);
    connect(newSnippetMenu, &QMenu::triggered, this, &QtLiteApp::insertSnippet);
}

void QtLiteApp::createNewEditorTab() {
    bool ok;
    QString fileName = QInputDialog::getText(this, "New Tab", "Enter file name:",
                                             QLineEdit::Normal, "untitled.cpp", &ok);

    if (ok) {
        fileName = EditorTabManager::normalizeNewFileName(fileName);
        if (!fileName.isEmpty()) editorTabManager->addNewTab(fileName);
    }
}

void QtLiteApp::establishConnections() {
    connect(qtDirEdit, &QLineEdit::textChanged, this, &QtLiteApp::savePathsAndValidate);
    connect(vsDirEdit, &QLineEdit::textChanged, this, &QtLiteApp::savePathsAndValidate);
    connect(pythonExeEdit, &QLineEdit::textChanged, this, &QtLiteApp::savePathsAndValidate);

    connect(includeDirsEdit, &QLineEdit::textChanged, this, &QtLiteApp::saveOtherPaths);
    connect(libDirsEdit, &QLineEdit::textChanged, this, &QtLiteApp::saveOtherPaths);
    connect(buildDirEdit, &QLineEdit::textChanged, this, &QtLiteApp::saveOtherPaths);
    connect(buildConfigCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &QtLiteApp::saveOtherPaths);
    connect(bundleCheckBox, &QCheckBox::checkStateChanged, this, &QtLiteApp::saveOtherPaths);

    connect(languageCombo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
            this, &QtLiteApp::onLanguageChanged);
    connect(buildDirEdit, &QLineEdit::textChanged, this, &QtLiteApp::updateFileBrowserRootDirectory);
}

void QtLiteApp::loadAndValidateSettings() {
    ProjectSettings settings = SettingsPersistence::loadSettings();
    uiController.updateProjectSettings(settings);

    PathValidator validator;
    CompilerPaths paths = validator.validateAndExtract(settings);

    uiController.updatePathValidationColors(paths, validator);
    uiController.updateLanguageVisibility(settings.language);
    uiController.updateFileBrowserRoot(settings.buildDir);

    validatePaths();
}

void QtLiteApp::validatePaths() {
    ProjectSettings settings = uiController.getProjectSettings();
    PathValidator validator;
    CompilerPaths paths = validator.validateAndExtract(settings);

    uiController.updatePathValidationColors(paths, validator);

    QString lang = languageCombo->currentText();
    bool runEnabled = false;
    QString errorMessage;

    if (lang == "C++ (Qt)") {
        if (validator.validateQtPaths(paths) && validator.validateVsPaths(paths)) {
            runEnabled = true;
        } else {
            errorMessage = "Qt or Visual Studio paths not found.";
        }
    } else if (lang == "C/C++ (Console)") {
        if (validator.validateVsPaths(paths)) {
            runEnabled = true;
        } else {
            errorMessage = "Visual Studio path not found.";
        }
    } else if (lang == "Python") {
        if (validator.validatePythonPath(paths)) {
            runEnabled = true;
        } else {
            errorMessage = "Python executable not found.";
        }
    }

    uiController.setRunButtonEnabled(runEnabled);
    uiController.setOutputPlaceholder(errorMessage.isEmpty() ? "Ready to build." : errorMessage);
}

void QtLiteApp::savePathsAndValidate() {
    ProjectSettings settings = uiController.getProjectSettings();
    SettingsPersistence::saveSettings(settings);
    validatePaths();
}

void QtLiteApp::saveOtherPaths() {
    ProjectSettings settings = uiController.getProjectSettings();
    SettingsPersistence::saveSettings(settings);
}

void QtLiteApp::onLanguageChanged(const QString& lang) {
    editorTabManager->setLanguage(lang);
    uiController.updateLanguageVisibility(lang);
    validatePaths();
    saveOtherPaths();
}

void QtLiteApp::browseForQtDir() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Qt Version Directory", qtDirEdit->text());
    if (!dir.isEmpty()) {
        qtDirEdit->setText(dir);
        savePathsAndValidate();
    }
}

void QtLiteApp::browseForVsDir() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Visual Studio Directory", vsDirEdit->text());
    if (!dir.isEmpty()) {
        vsDirEdit->setText(dir);
        savePathsAndValidate();
    }
}

void QtLiteApp::browseForBuildDir() {
    QString dir = QFileDialog::getExistingDirectory(this, "Select Build (Project) Directory", buildDirEdit->text());
    if (!dir.isEmpty()) {
        buildDirEdit->setText(dir);
        saveOtherPaths();
    }
}

void QtLiteApp::browseForPythonExe() {
    QString file = QFileDialog::getOpenFileName(this, "Select Python Executable", pythonExeEdit->text(), "Executables (*.exe)");
    if (!file.isEmpty()) {
        pythonExeEdit->setText(file);
        savePathsAndValidate();
    }
}

void QtLiteApp::updateFileBrowserRootDirectory(const QString& path) {
    uiController.updateFileBrowserRoot(path);
}

void QtLiteApp::openFileFromBrowser(const QModelIndex& index) {
    if (uiController.isDirectory(index)) return;

    QString filePath = uiController.getSelectedFilePath(index);
    if (filePath.isEmpty()) return;

    editorTabManager->openFile(filePath);
}

void QtLiteApp::runCompilation() {
    outputLog->clear();
    ProjectSettings settings = uiController.getProjectSettings();

    compilationEngine->setProjectSettings(settings);

    PathValidator validator;
    CompilerPaths paths = validator.validateAndExtract(settings);
    compilationEngine->setCompilerPaths(paths);

    const QString sourcePath = editorTabManager->saveCurrentEditor(settings.buildDir);
    if (sourcePath.isEmpty()) {
        outputLog->append("!!! FAILED to save the current file before building.");
        return;
    }

    if (settings.language == "C++ (Qt)") {
        compilationEngine->compileQt(sourcePath);
    } else if (settings.language == "C/C++ (Console)") {
        compilationEngine->compileConsole(sourcePath);
    } else if (settings.language == "Python") {
        compilationEngine->compilePython(sourcePath);
    }
}

void QtLiteApp::saveCurrentFile() {
    const QString savedPath = editorTabManager->saveCurrentEditor(buildDirEdit->text());
    if (savedPath.isEmpty()) {
        outputLog->append("!!! FAILED to save the current file.");
    } else {
        outputLog->append("Saved: " + savedPath);
    }
}

void QtLiteApp::bundleCode() {
    outputLog->clear();
    
    CodeEditor* currentEditor = editorTabManager->getCurrentEditor();
    if (!currentEditor) {
        outputLog->append("Error: No editor tab is currently open.");
        return;
    }
    
    QString currentFilePath = currentEditor->filePath();
    if (currentFilePath.isEmpty()) {
        outputLog->append("Error: Current file has not been saved. Please save the file first.");
        QMessageBox::warning(this, "Bundle Code", 
                           "Please save the current file before bundling.\n"
                           "The bundler needs a file path to resolve local includes.");
        return;
    }
    
    QString outputPath;
    bool success = codeBundler.bundleCode(currentFilePath, outputPath);
    
    if (success) {
        outputLog->append("Code bundling successful!");
        outputLog->append("Bundled file created at: " + outputPath);
        QMessageBox::information(this, "Bundle Code", 
                               "Code bundled successfully!\n\n"
                               "Output file: " + outputPath);
    } else {
        outputLog->append("Error: Failed to bundle code.");
        outputLog->append("Could not read the source file or write the output file: " + outputPath);
        QMessageBox::warning(this, "Bundle Code", 
                           "Failed to bundle code.\n"
                           "Check the output log for details.");
    }
}

void QtLiteApp::insertSnippet(QAction* action) {
    if (!action) return;

    QString snippetName = action->text();
    QString snippetCode = snippetManager.getSnippet(snippetName);

    if (!snippetCode.isEmpty()) {
        editorTabManager->insertSnippet(snippetCode);
    }
}
