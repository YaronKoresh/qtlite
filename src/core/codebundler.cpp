#include "codebundler.h"

CodeBundler::CodeBundler() {
    localIncludeRegex.setPattern("^\\s*#\\s*include\\s*\"([^\"]+)\"");
    systemIncludeRegex.setPattern("^\\s*#\\s*include\\s*<([^>]+)>");
}

bool CodeBundler::bundleCode(const QString& rootFilePath, QString& outputPath) {
    QFileInfo rootFileInfo(rootFilePath);
    QString rootDir = rootFileInfo.absolutePath();
    QString rootName = rootFileInfo.completeBaseName();
    QString rootSuffix = rootFileInfo.suffix();

    outputPath = rootDir + "/" + rootName + "_bundled." + rootSuffix;

    QFile rootFile(rootFileInfo.absoluteFilePath());
    if (!rootFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }
    rootFile.close();

    QSaveFile outFile(outputPath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream outStream(&outFile);
    QSet<QString> processedIncludes;
    processedIncludes.insert(QDir::cleanPath(rootFileInfo.absoluteFilePath()));

    parseAndBundleFile(rootFileInfo.absoluteFilePath(), outStream, processedIncludes, rootDir);

    outStream.flush();

    return outFile.commit();
}

void CodeBundler::parseAndBundleFile(const QString& filePath, QTextStream& outStream,
                        QSet<QString>& processedIncludes, const QString& rootDir) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outStream << "\n// !!! FAILED to open include file: " << filePath << " !!!\n";
        return;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();

        QRegularExpressionMatch localMatch = localIncludeRegex.match(line);
        QRegularExpressionMatch systemMatch = systemIncludeRegex.match(line);

        if (localMatch.hasMatch()) {
            QString includeName = localMatch.captured(1);
            QString includePath = QDir(rootDir).absoluteFilePath(includeName);
            includePath = QDir::cleanPath(QDir::fromNativeSeparators(includePath));

            if (!QFileInfo(includePath).isFile()) {
                outStream << line << "\n";
            } else if (!processedIncludes.contains(includePath)) {
                processedIncludes.insert(includePath);
                outStream << "\n// --- Start of bundled file: " << includeName << " ---\n";
                parseAndBundleFile(includePath, outStream, processedIncludes,
                                   QFileInfo(includePath).absolutePath());
                outStream << "// --- End of bundled file: " << includeName << " ---\n\n";
            } else {
                outStream << "// (Skipped already included file: " << includeName << ")\n";
            }
        } else if (systemMatch.hasMatch()) {
            QString systemHeader = systemMatch.captured(1).trimmed();
            if (!processedIncludes.contains(systemHeader)) {
                processedIncludes.insert(systemHeader);
                outStream << line << "\n";
            }
        } else if (!line.trimmed().startsWith("#pragma once")) {
            outStream << line << "\n";
        }
    }
    file.close();
}
