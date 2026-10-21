#ifndef PROJECTSTRUCTS_H
#define PROJECTSTRUCTS_H

#include <QString>
#include <QRegularExpression>
#include <QTextCharFormat>

struct HighlightingRule {
    QRegularExpression pattern;
    QTextCharFormat format;
};

struct CompilerPaths {
    QString qmakePath;
    QString vcvarsPath;
    QString nmakePath;
    QString windeployqtPath;
    QString pythonExePath;
};

struct ProjectSettings {
    QString qtDirectory;
    QString vsDirectory;
    QString pythonExecutable;
    QString includeDirs;
    QString libDirs;
    QString buildDir;
    QString buildConfig;
    QString language;
    bool bundleToExe = false;
};

#endif // PROJECTSTRUCTS_H
