#include "uicontroller.h"
#include <QDir>

UIController::UIController()
    : qtDirEdit(nullptr), vsDirEdit(nullptr), pythonExeEdit(nullptr),
    includeDirsEdit(nullptr), libDirsEdit(nullptr), buildDirEdit(nullptr),
    buildConfigCombo(nullptr), languageCombo(nullptr), bundleCheckBox(nullptr),
    qtDirLabel(nullptr), vsDirLabel(nullptr), pythonExeLabel(nullptr),
    buildConfigLabel(nullptr), includeDirsLabel(nullptr), libDirsLabel(nullptr),
    outputLog(nullptr), runButton(nullptr), bundleCodeButton(nullptr),
    fileBrowser(nullptr), fsModel(nullptr), settingsFormLayout(nullptr),
    qtDirContainer(nullptr), vsDirContainer(nullptr), pythonExeContainer(nullptr),
    buildConfigContainer(nullptr), includeDirsContainer(nullptr), libDirsContainer(nullptr)
{
}

void UIController::setQtDirEdit(QLineEdit* edit) {
    qtDirEdit = edit;
}

void UIController::setVsDirEdit(QLineEdit* edit) {
    vsDirEdit = edit;
}

void UIController::setPythonExeEdit(QLineEdit* edit) {
    pythonExeEdit = edit;
}

void UIController::setIncludeDirsEdit(QLineEdit* edit) {
    includeDirsEdit = edit;
}

void UIController::setLibDirsEdit(QLineEdit* edit) {
    libDirsEdit = edit;
}

void UIController::setBuildDirEdit(QLineEdit* edit) {
    buildDirEdit = edit;
}

void UIController::setBuildConfigCombo(QComboBox* combo) {
    buildConfigCombo = combo;
}

void UIController::setLanguageCombo(QComboBox* combo) {
    languageCombo = combo;
}

void UIController::setBundleCheckBox(QCheckBox* check) {
    bundleCheckBox = check;
}

void UIController::setQtDirLabel(QLabel* label) {
    qtDirLabel = label;
}

void UIController::setVsDirLabel(QLabel* label) {
    vsDirLabel = label;
}

void UIController::setPythonExeLabel(QLabel* label) {
    pythonExeLabel = label;
}

void UIController::setBuildConfigLabel(QLabel* label) {
    buildConfigLabel = label;
}

void UIController::setIncludeDirsLabel(QLabel* label) {
    includeDirsLabel = label;
}

void UIController::setLibDirsLabel(QLabel* label) {
    libDirsLabel = label;
}

void UIController::setOutputLog(QTextEdit* log) {
    outputLog = log;
}

void UIController::setRunButton(QPushButton* btn) {
    runButton = btn;
}

void UIController::setBundleCodeButton(QPushButton* btn) {
    bundleCodeButton = btn;
}

void UIController::setFileBrowser(QTreeView* browser) {
    fileBrowser = browser;
}

void UIController::setFileSystemModel(QFileSystemModel* model) {
    fsModel = model;
}

void UIController::setSettingsFormLayout(QFormLayout* layout) {
    settingsFormLayout = layout;
}

void UIController::setQtDirContainer(QWidget* container) {
    qtDirContainer = container;
}

void UIController::setVsDirContainer(QWidget* container) {
    vsDirContainer = container;
}

void UIController::setPythonExeContainer(QWidget* container) {
    pythonExeContainer = container;
}

void UIController::setBuildConfigContainer(QWidget* container) {
    buildConfigContainer = container;
}

void UIController::setIncludeDirsContainer(QWidget* container) {
    includeDirsContainer = container;
}

void UIController::setLibDirsContainer(QWidget* container) {
    libDirsContainer = container;
}

ProjectSettings UIController::getProjectSettings() {
    ProjectSettings settings;
    settings.qtDirectory = qtDirEdit ? qtDirEdit->text() : "";
    settings.vsDirectory = vsDirEdit ? vsDirEdit->text() : "";
    settings.pythonExecutable = pythonExeEdit ? pythonExeEdit->text() : "";
    settings.includeDirs = includeDirsEdit ? includeDirsEdit->text() : "";
    settings.libDirs = libDirsEdit ? libDirsEdit->text() : "";
    settings.buildDir = buildDirEdit ? buildDirEdit->text() : "";
    settings.buildConfig = buildConfigCombo ? buildConfigCombo->currentText() : "Release";
    settings.language = languageCombo ? languageCombo->currentText() : "C++ (Qt)";
    settings.bundleToExe = bundleCheckBox ? bundleCheckBox->isChecked() : false;
    return settings;
}

void UIController::updateProjectSettings(const ProjectSettings& settings) {
    if (qtDirEdit) qtDirEdit->setText(settings.qtDirectory);
    if (vsDirEdit) vsDirEdit->setText(settings.vsDirectory);
    if (pythonExeEdit) pythonExeEdit->setText(settings.pythonExecutable);
    if (includeDirsEdit) includeDirsEdit->setText(settings.includeDirs);
    if (libDirsEdit) libDirsEdit->setText(settings.libDirs);
    if (buildDirEdit) buildDirEdit->setText(settings.buildDir);
    if (buildConfigCombo) buildConfigCombo->setCurrentText(settings.buildConfig);
    if (languageCombo) languageCombo->setCurrentText(settings.language);
    if (bundleCheckBox) bundleCheckBox->setChecked(settings.bundleToExe);
}

void UIController::updatePathValidationColors(const CompilerPaths& paths, const PathValidator& validator) {
    bool qtValid = validator.validateQtPaths(paths);
    bool vsValid = validator.validateVsPaths(paths);
    bool pyValid = validator.validatePythonPath(paths);

    setPaletteColor(qtDirEdit, validator.getValidationColor(qtValid));
    setPaletteColor(vsDirEdit, validator.getValidationColor(vsValid));
    setPaletteColor(pythonExeEdit, validator.getValidationColor(pyValid));
}

void UIController::setRunButtonEnabled(bool enabled) {
    if (runButton) {
        runButton->setEnabled(enabled);
    }
}

void UIController::setOutputPlaceholder(const QString& text) {
    if (outputLog) {
        outputLog->setPlaceholderText(text);
    }
}

void UIController::updateLanguageVisibility(const QString& lang) {
    bool isQt = (lang == "C++ (Qt)");
    bool isCpp = (lang == "C++ (Qt)" || lang == "C/C++ (Console)");
    bool isPy = (lang == "Python");

    setRowVisible(qtDirLabel, qtDirContainer, isQt);
    setRowVisible(vsDirLabel, vsDirContainer, isCpp);
    setRowVisible(buildConfigLabel, buildConfigContainer, isCpp);
    setRowVisible(includeDirsLabel, includeDirsContainer, isCpp);
    setRowVisible(libDirsLabel, libDirsContainer, isCpp);
    setRowVisible(pythonExeLabel, pythonExeContainer, isPy);

    if (bundleCheckBox) {
        bundleCheckBox->setVisible(isCpp);
    }
    if (bundleCodeButton) {
        bundleCodeButton->setVisible(isCpp);
    }

    if (runButton) {
        runButton->setText(isPy ? "Run Script" : "Build and Run");
    }
}

void UIController::updateFileBrowserRoot(const QString& path) {
    if (fsModel && fileBrowser && QDir(path).exists()) {
        fsModel->setRootPath(path);
        fileBrowser->setRootIndex(fsModel->index(path));
    }
}

QString UIController::getSelectedFilePath(const QModelIndex& index) {
    return fsModel ? fsModel->filePath(index) : "";
}

bool UIController::isDirectory(const QModelIndex& index) {
    return fsModel ? fsModel->isDir(index) : false;
}

void UIController::setRowVisible(QLabel* label, QWidget* widget, bool visible) {
    if (label) {
        label->setVisible(visible);
    }
    if (widget) {
        widget->setVisible(visible);
    }
}

void UIController::setPaletteColor(QLineEdit* edit, const QColor& color) {
    if (edit) {
        QPalette palette = edit->palette();
        palette.setColor(QPalette::Base, color);
        edit->setPalette(palette);
    }
}
