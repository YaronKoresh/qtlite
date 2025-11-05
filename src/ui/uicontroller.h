#ifndef UICONTROLLER_H
#define UICONTROLLER_H

#include <QWidget>
#include <QLineEdit>
#include <QTextEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QPushButton>
#include <QTreeView>
#include <QFileSystemModel>
#include <QFormLayout>
#include <QPalette>
#include <QColor>
#include <QModelIndex>
#include "../core/projectstructs.h"
#include "../core/pathvalidator.h"

class UIController {
private:
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
    QFormLayout* settingsFormLayout;

    QWidget* qtDirContainer;
    QWidget* vsDirContainer;
    QWidget* pythonExeContainer;
    QWidget* buildConfigContainer;
    QWidget* includeDirsContainer;
    QWidget* libDirsContainer;

    void setRowVisible(QLabel* label, QWidget* widget, bool visible);
    void setPaletteColor(QLineEdit* edit, const QColor& color);

public:
    UIController();

    void setQtDirEdit(QLineEdit* edit);
    void setVsDirEdit(QLineEdit* edit);
    void setPythonExeEdit(QLineEdit* edit);
    void setIncludeDirsEdit(QLineEdit* edit);
    void setLibDirsEdit(QLineEdit* edit);
    void setBuildDirEdit(QLineEdit* edit);
    void setBuildConfigCombo(QComboBox* combo);
    void setLanguageCombo(QComboBox* combo);
    void setBundleCheckBox(QCheckBox* check);
    void setQtDirLabel(QLabel* label);
    void setVsDirLabel(QLabel* label);
    void setPythonExeLabel(QLabel* label);
    void setBuildConfigLabel(QLabel* label);
    void setIncludeDirsLabel(QLabel* label);
    void setLibDirsLabel(QLabel* label);
    void setOutputLog(QTextEdit* log);
    void setRunButton(QPushButton* btn);
    void setBundleCodeButton(QPushButton* btn);
    void setFileBrowser(QTreeView* browser);
    void setFileSystemModel(QFileSystemModel* model);
    void setSettingsFormLayout(QFormLayout* layout);
    void setQtDirContainer(QWidget* container);
    void setVsDirContainer(QWidget* container);
    void setPythonExeContainer(QWidget* container);
    void setBuildConfigContainer(QWidget* container);
    void setIncludeDirsContainer(QWidget* container);
    void setLibDirsContainer(QWidget* container);

    ProjectSettings getProjectSettings();
    void updateProjectSettings(const ProjectSettings& settings);
    void updatePathValidationColors(const CompilerPaths& paths, const PathValidator& validator);
    void setRunButtonEnabled(bool enabled);
    void setOutputPlaceholder(const QString& text);
    void updateLanguageVisibility(const QString& lang);
    void updateFileBrowserRoot(const QString& path);
    QString getSelectedFilePath(const QModelIndex& index);
    bool isDirectory(const QModelIndex& index);
};

#endif // UICONTROLLER_H
