#include "compilationengine.h"
#include <QTextStream>

static QStringList splitPathList(const QString& text) {
    QStringList result;
    for (const QString& part : text.split(';')) {
        const QString trimmed = part.trimmed();
        if (!trimmed.isEmpty()) result << trimmed;
    }
    return result;
}

CompilationEngine::CompilationEngine(QTextEdit* log) : currentProcess(nullptr), outputLog(log) {}

void CompilationEngine::setCompilerPaths(const CompilerPaths& paths) {
    compilerPaths = paths;
}

void CompilationEngine::setProjectSettings(const ProjectSettings& settings) {
    projectSettings = settings;
}

bool CompilationEngine::rejectIfBusy() {
    if (!currentProcess) return false;
    outputLog->append("!!! A build or run is already in progress. Wait for it to finish.");
    return true;
}

void CompilationEngine::compilePython(const QString& filePath) {
    if (rejectIfBusy()) return;
    outputLog->append("Running Python script...");

    QProcess* process = new QProcess(this);
    currentProcess = process;
    process->setWorkingDirectory(projectSettings.buildDir);

    connect(process, &QProcess::readyReadStandardOutput, [this, process]() {
        outputLog->append(process->readAllStandardOutput());
    });
    connect(process, &QProcess::readyReadStandardError, [this, process]() {
        outputLog->append(process->readAllStandardError());
    });
    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, process]() {
                outputLog->append("\n...Script finished.");
                process->deleteLater();
                if (currentProcess == process) currentProcess = nullptr;
            });

    process->start(compilerPaths.pythonExePath, {filePath});
    if (!process->waitForStarted()) {
        outputLog->append("!!! FAILED to start process: " + compilerPaths.pythonExePath);
        process->disconnect(this);
        process->deleteLater();
        currentProcess = nullptr;
    }
}

void CompilationEngine::compileConsole(const QString& filePath) {
    if (rejectIfBusy()) return;
    outputLog->append("Starting C/C++ Console build process...");

    QString buildConfig = projectSettings.buildConfig.toLower();
    QString configPath = projectSettings.buildDir + "/" + buildConfig;

    if (!QDir(configPath).mkpath(".")) {
        outputLog->append("!!! FAILED to create config directory: " + configPath);
        return;
    }

    QStringList clArgs;
    clArgs << "/EHsc" << "/nologo";

    if (buildConfig == "debug") {
        clArgs << "/MDd" << "/Zi" << "/Od";
    } else {
        clArgs << "/MD" << "/O2";
    }

    clArgs << "/analyze";

    if (!projectSettings.includeDirs.isEmpty()) {
        for (const QString& path : splitPathList(projectSettings.includeDirs)) {
            clArgs << "/I\"" + QDir::fromNativeSeparators(path) + "\"";
        }
    }

    QString exeName = QFileInfo(filePath).completeBaseName() + ".exe";
    QString exePath = QDir::fromNativeSeparators(configPath + "/" + exeName);

    clArgs << "/Fo:\"" + QDir::fromNativeSeparators(configPath + "/") + "\"";
    clArgs << "/Fe:\"" + exePath + "\"";
    clArgs << "\"" + QDir::fromNativeSeparators(filePath) + "\"";

    if (!projectSettings.libDirs.isEmpty()) {
        for (const QString& path : splitPathList(projectSettings.libDirs)) {
            QString nativePath = QDir::fromNativeSeparators(path);
            if (nativePath.endsWith(".lib", Qt::CaseInsensitive)) {
                clArgs << "\"" + nativePath + "\"";
            } else if (QFileInfo(nativePath).isDir()) {
                clArgs << "/LIBPATH:\"" + nativePath + "\"";
            }
        }
    }

    QString cmdChain = QString("cd /d \"%1\" && \"%2\" x64 && %3")
                           .arg(QDir::toNativeSeparators(projectSettings.buildDir))
                           .arg(compilerPaths.vcvarsPath)
                           .arg("cl.exe " + clArgs.join(" "));

    executeProcess(QString("cmd /C '%1'").arg(cmdChain), exePath);
}

void CompilationEngine::compileQt(const QString& filePath) {
    if (rejectIfBusy()) return;
    outputLog->append("Starting Qt Widgets build process...");

    QFile codeFile(filePath);
    if (!codeFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outputLog->append("!!! FAILED to read code");
        return;
    }

    QString code = codeFile.readAll();
    codeFile.close();

    if (code.isEmpty()) {
        outputLog->append("!!! FAILED: the file is empty: " + filePath);
        return;
    }

    if (!QDir(projectSettings.buildDir).exists() &&
        !QDir().mkpath(projectSettings.buildDir)) {
        outputLog->append("!!! FAILED to create build directory: " + projectSettings.buildDir);
        return;
    }

    QFile mainCppFile(projectSettings.buildDir + "/main.cpp");
    if (mainCppFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        mainCppFile.write(code.toUtf8());
        mainCppFile.close();
    } else {
        outputLog->append("!!! FAILED to write main.cpp at " + projectSettings.buildDir);
        return;
    }

    QString proContent = generateProFile(code);

    QFile proFile(projectSettings.buildDir + "/temp_app.pro");
    if (proFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        proFile.write(proContent.toUtf8());
        proFile.close();
    } else {
        outputLog->append("!!! FAILED to write temp_app.pro at " + projectSettings.buildDir);
        return;
    }

    QString appExePath = QString("%1/temp_app.exe").arg(projectSettings.buildConfig);
    QString cmdChain = QString("cd /d \"%1\" && \"%2\" x64 && \"%3\" temp_app.pro && %4 && \"%5\" %6")
                           .arg(QDir::toNativeSeparators(projectSettings.buildDir))
                           .arg(compilerPaths.vcvarsPath)
                           .arg(compilerPaths.qmakePath)
                           .arg(compilerPaths.nmakePath)
                           .arg(compilerPaths.windeployqtPath)
                           .arg(appExePath);

    executeProcess(QString("cmd /C '%1'").arg(cmdChain),
                   QDir::fromNativeSeparators(projectSettings.buildDir + "/" + appExePath));
}

QString CompilationEngine::generateProFile(const QString& code) {
    QSet<QString> modules;
    modules << "widgets";

    static const QSet<QString> nonModuleHeaders = {
        "global", "math", "endian", "algorithms", "debug", "alloc"
    };
    static const QList<QPair<QString, QString>> classPrefixes = {
        {"qwebengine", "webenginewidgets"}, {"qnetwork", "network"}, {"qtcp", "network"},
        {"qudp", "network"}, {"qhttp", "network"}, {"qssl", "network"}, {"qlocalsocket", "network"},
        {"qlocalserver", "network"}, {"qabstractsocket", "network"}, {"qsql", "sql"},
        {"qmedia", "multimedia"}, {"qaudio", "multimedia"}, {"qvideo", "multimedia"},
        {"qcamera", "multimedia"}, {"qsoundeffect", "multimedia"}, {"qsvg", "svg"},
        {"qprint", "printsupport"}, {"qpagesetup", "printsupport"}, {"qxml", "xml"},
        {"qtest", "testlib"}, {"qsignalspy", "testlib"}, {"qopengl", "opengl"},
        {"qquick", "quick"}, {"qqml", "qml"}, {"qbluetooth", "bluetooth"},
        {"qserialport", "serialport"}, {"qwebsocket", "websockets"}, {"qdbus", "dbus"}
    };

    QRegularExpression re("#include\\s*<([a-zA-Z0-9_]+)(/?)");
    auto it = re.globalMatch(code);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        const QString original = match.captured(1);
        const QString header = original.toLower();
        const bool hasSlash = !match.captured(2).isEmpty();
        const bool umbrellaName = original.size() > 2 && original.startsWith("Qt") && original[2].isUpper();

        if ((hasSlash && header.startsWith("qt")) || (!hasSlash && umbrellaName)) {
            QString moduleName = header.mid(2);
            if (moduleName == "test") moduleName = "testlib";
            if (!moduleName.isEmpty() && (hasSlash || !nonModuleHeaders.contains(moduleName))) {
                modules << moduleName;
            }
        } else if (!hasSlash) {
            for (const auto& entry : classPrefixes) {
                if (header.startsWith(entry.first)) {
                    modules << entry.second;
                    break;
                }
            }
        }
    }

    QString proContent = "TEMPLATE = app\nTARGET = temp_app\nSOURCES += main.cpp\n";
    QStringList moduleList = modules.values();
    moduleList.sort();
    proContent += "QT += " + moduleList.join(" ") + "\n";

    QString buildConfig = projectSettings.buildConfig.toLower();
    proContent += "CONFIG += " + buildConfig + "\n";

    if (buildConfig == "debug") {
        proContent += "CONFIG += console\n";
    } else {
        proContent += "CONFIG += windows\n";
    }

    if (!projectSettings.includeDirs.isEmpty()) {
        for (const QString& path : splitPathList(projectSettings.includeDirs)) {
            proContent += "INCLUDEPATH += \"" + QDir::fromNativeSeparators(path) + "\"\n";
        }
    }

    if (!projectSettings.libDirs.isEmpty()) {
        for (const QString& path : splitPathList(projectSettings.libDirs)) {
            QString nativePath = QDir::fromNativeSeparators(path);
            if (nativePath.endsWith(".lib", Qt::CaseInsensitive)) {
                proContent += "LIBS += \"" + nativePath + "\"\n";
            } else if (QFileInfo(nativePath).isDir()) {
                proContent += "LIBS += -L\"" + nativePath + "\"\n";
            } else {
                proContent += "LIBS += " + path + "\n";
            }
        }
    }

    return proContent;
}

void CompilationEngine::executeProcess(const QString& command, const QString& exeToRun) {
    outputLog->append("Running build...\nThis may take a moment...\n");
    qDebug() << "Executing:" << "powershell.exe -Command " << command;

    QProcess* process = new QProcess(this);
    currentProcess = process;

    connect(process, &QProcess::readyReadStandardOutput, [this, process]() {
        outputLog->append(process->readAllStandardOutput());
    });
    connect(process, &QProcess::readyReadStandardError, [this, process]() {
        outputLog->append(process->readAllStandardError());
    });

    connect(process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [this, exeToRun](int exitCode, QProcess::ExitStatus exitStatus) {
                handleBuildCompletion(exitCode, exitStatus, exeToRun);
            });
    connect(process, &QProcess::errorOccurred, [this, process](QProcess::ProcessError error) {
        if (error != QProcess::FailedToStart) return;
        outputLog->append("!!! FAILED to start process: powershell.exe");
        process->disconnect(this);
        process->deleteLater();
        if (currentProcess == process) currentProcess = nullptr;
    });

    process->start("powershell.exe", {"-Command", command});
}

void CompilationEngine::handleBuildCompletion(int exitCode, QProcess::ExitStatus exitStatus, const QString& exeToRun) {
    QString logText = outputLog->toPlainText();
    bool buildFailed = exitStatus == QProcess::CrashExit || exitCode != 0 ||
                       logText.contains("NMAKE : fatal error", Qt::CaseInsensitive) ||
                       logText.contains("LINK : fatal error", Qt::CaseInsensitive) ||
                       logText.contains(QRegularExpression("error C\\d+", QRegularExpression::CaseInsensitiveOption));

    if (buildFailed) {
        outputLog->append("\n!!! Build FAILED !!!");
    } else if (logText.contains("windeployqt.exe... failed", Qt::CaseInsensitive)) {
        outputLog->append("\n!!! Build Succeeded, but Deployment FAILED !!!");
    } else {
        outputLog->append("\n...Build Succeeded.");
        QFileInfo exeInfo(exeToRun);

        if (!exeInfo.exists()) {
            outputLog->append("!!! FAILED: Executable not found at " + exeToRun);
            currentProcess->deleteLater();
            currentProcess = nullptr;
            return;
        }

        if (projectSettings.bundleToExe) {
            launchInstallerBundler(exeInfo.absolutePath(), exeInfo.fileName());
        } else {
            launchApplication(exeInfo);
        }
    }

    currentProcess->deleteLater();
    currentProcess = nullptr;
}

void CompilationEngine::launchApplication(const QFileInfo& exeInfo) {
    outputLog->append("Launching app... (" + exeInfo.absoluteFilePath() + ")");
    QProcess* runProcess = new QProcess(this);
    runProcess->setWorkingDirectory(exeInfo.absolutePath());
    runProcess->start(exeInfo.fileName());

    connect(runProcess, &QProcess::started, [=]() { runProcess->deleteLater(); });
    connect(runProcess, &QProcess::errorOccurred, [=](QProcess::ProcessError) {
        outputLog->append("!!! FAILED to launch application!");
        runProcess->deleteLater();
    });
}

void CompilationEngine::launchInstallerBundler(const QString& builtAppDir, const QString& mainExecutableName) {
    outputLog->append("Starting installer bundler process...");

    QStringList sourceFiles;
    QDirIterator it(builtAppDir, QDir::Files, QDirIterator::Subdirectories);
    while (it.hasNext()) sourceFiles.append(QDir::toNativeSeparators(it.next()));

    if (sourceFiles.isEmpty()) {
        outputLog->append("!!! Bundler FAILED: No files found.");
        return;
    }

    QString bundledExeName = QFileInfo(mainExecutableName).completeBaseName() + "_Installer.exe";
    QString bundledExePath = QDir::toNativeSeparators(builtAppDir + "/../" + bundledExeName);
    QString sedFilePath = QDir::toNativeSeparators(builtAppDir + "/bundle.sed");

    QFile sedFile(sedFilePath);
    if (!sedFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        outputLog->append("!!! Bundler FAILED: Could not write .sed file.");
        return;
    }

    QTextStream out(&sedFile);
    out << "[Version]\nClass=IEXPRESS\nSEDVersion=3\n[Options]\n"
        << "PackagePurpose=InstallApp\nShowInstallProgramWindow=0\n"
        << "HideExtractAnimation=1\nUseLongFileNames=1\nRebootMode=N\n"
        << "TargetName=" << bundledExePath << "\n"
        << "FriendlyName=" << mainExecutableName << "\n"
        << "[Strings]\nInstallPrompt=\nDisplayLicense=\nFinishMessage=\n"
        << "TargetName=" << bundledExePath << "\n"
        << "[SourceFiles]\nSourceFiles0=" << QDir::toNativeSeparators(builtAppDir) << "\n"
        << "[SourceFiles0]\n";

    for (const QString& file : sourceFiles) {
        out << "\"" << QFileInfo(file).fileName() << "\"\n";
    }

    out << "[Info]\nPackageName=" << mainExecutableName << "\n"
        << "[Setup]\nRunProgram=" << mainExecutableName << "\n";
    out.flush();
    sedFile.close();

    QProcess* bundleProcess = new QProcess(this);
    QString cmd = QString("iexpress.exe /N /Q \"%1\"").arg(sedFilePath);
    outputLog->append("Running installer bundler: " + cmd);

    connect(bundleProcess, &QProcess::readyReadStandardOutput, [=]() {
        outputLog->append(bundleProcess->readAllStandardOutput());
    });
    connect(bundleProcess, &QProcess::readyReadStandardError, [=]() {
        outputLog->append(bundleProcess->readAllStandardError());
    });

    connect(bundleProcess, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            [=](int exitCode, QProcess::ExitStatus) {
                if (exitCode == 0 && QFileInfo::exists(bundledExePath)) {
                    outputLog->append("\n...Installer Bundling Succeeded!");
                    outputLog->append("Single file installer created at: " + bundledExePath);
                } else {
                    outputLog->append("\n!!! Installer Bundling FAILED !!!");
                }
                bundleProcess->deleteLater();
            });

    bundleProcess->start("cmd.exe", {"/C", cmd});
}
