#ifndef PATHVALIDATOR_H
#define PATHVALIDATOR_H

#include <QColor>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include "projectstructs.h"

class PathValidator {
public:
    PathValidator() {}

    CompilerPaths validateAndExtract(const ProjectSettings& settings) {
        CompilerPaths paths;
        paths.qmakePath = QDir::fromNativeSeparators(
            settings.qtDirectory + "/msvc2022_64/bin/qmake.exe");
        paths.vcvarsPath = QDir::fromNativeSeparators(
            settings.vsDirectory + "/VC/Auxiliary/Build/vcvarsall.bat");
        paths.nmakePath = "nmake.exe";

        QFileInfo qmakeInfo(paths.qmakePath);
        paths.windeployqtPath = QDir::fromNativeSeparators(
            qmakeInfo.absolutePath() + "/windeployqt.exe");
        paths.pythonExePath = settings.pythonExecutable;

        return paths;
    }

    bool validateQtPaths(const CompilerPaths& paths) const {
        return QFile::exists(paths.qmakePath) && QFile::exists(paths.windeployqtPath);
    }

    bool validateVsPaths(const CompilerPaths& paths) const {
        return QFile::exists(paths.vcvarsPath);
    }

    bool validatePythonPath(const CompilerPaths& paths) const {
        return QFile::exists(paths.pythonExePath);
    }

    QColor getValidationColor(bool isValid) const {
        return isValid ? Qt::white : QColor(255, 220, 220);
    }
};

#endif // PATHVALIDATOR_H
