#ifndef SETTINGSPERSISTENCE_H
#define SETTINGSPERSISTENCE_H

#include <QSettings>
#include <QStandardPaths>
#include "projectstructs.h"

class SettingsPersistence {
private:
    static constexpr const char* ORGANIZATION = "YaronKoresh";
    static constexpr const char* APPLICATION = "QtLite";

public:
    static ProjectSettings loadSettings() {
        QSettings settings(ORGANIZATION, APPLICATION);
        ProjectSettings proj;

        proj.qtDirectory = settings.value("paths/qtDir", "C:/Qt").toString();
        proj.vsDirectory = settings.value("paths/vsDir",
                                          "C:/Program Files/Microsoft Visual Studio/2022/Community").toString();
        proj.pythonExecutable = settings.value("paths/pythonExe", "C:/Python311/python.exe").toString();
        proj.includeDirs = settings.value("paths/includeDirs", "").toString();
        proj.libDirs = settings.value("paths/libDirs", "").toString();
        proj.buildDir = settings.value("paths/buildDir",
                                       QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/QtLiteProject").toString();
        proj.buildConfig = settings.value("paths/buildConfig", "Release").toString();
        proj.language = settings.value("paths/language", "C++ (Qt)").toString();
        proj.bundleToExe = settings.value("ui/bundleApp", false).toBool();

        return proj;
    }

    static void saveSettings(const ProjectSettings& settings) {
        QSettings qsettings(ORGANIZATION, APPLICATION);
        qsettings.setValue("paths/qtDir", settings.qtDirectory);
        qsettings.setValue("paths/vsDir", settings.vsDirectory);
        qsettings.setValue("paths/pythonExe", settings.pythonExecutable);
        qsettings.setValue("paths/includeDirs", settings.includeDirs);
        qsettings.setValue("paths/libDirs", settings.libDirs);
        qsettings.setValue("paths/buildDir", settings.buildDir);
        qsettings.setValue("paths/buildConfig", settings.buildConfig);
        qsettings.setValue("paths/language", settings.language);
        qsettings.setValue("ui/bundleApp", settings.bundleToExe);
    }
};

#endif // SETTINGSPERSISTENCE_H
