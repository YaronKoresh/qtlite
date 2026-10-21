#ifndef COMPILATIONENGINE_H
#define COMPILATIONENGINE_H

#include <QObject>
#include <QProcess>
#include <QTextEdit>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>
#include <QRegularExpression>
#include <QSet>
#include <QDirIterator>
#include "../core/projectstructs.h"

class CompilationEngine : public QObject {
    Q_OBJECT
private:
    CompilerPaths compilerPaths;
    ProjectSettings projectSettings;
    QProcess* currentProcess;
    QTextEdit* outputLog;

public:
    CompilationEngine(QTextEdit* log);

    void setCompilerPaths(const CompilerPaths& paths);
    void setProjectSettings(const ProjectSettings& settings);
    void compilePython(const QString& filePath);
    void compileConsole(const QString& filePath);
    void compileQt(const QString& filePath);
    bool isBusy() const { return currentProcess != nullptr; }

private:
    bool rejectIfBusy();
    QString generateProFile(const QString& code);
    void executeProcess(const QString& command, const QString& exeToRun);
    void handleBuildCompletion(int exitCode, QProcess::ExitStatus exitStatus, const QString& exeToRun);
    void launchApplication(const QFileInfo& exeInfo);
    void launchInstallerBundler(const QString& builtAppDir, const QString& mainExecutableName);
};

#endif // COMPILATIONENGINE_H
