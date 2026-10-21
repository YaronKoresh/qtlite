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

    parseAndBundleFile(rootFileInfo.absoluteFilePath(), outStream, processedIncludes, rootDir,
                       QStringList());

    outStream.flush();

    return outFile.commit();
}

static QString conditionKey(const QStringList& conditions) {
    return conditions.join(" && ");
}

void CodeBundler::parseAndBundleFile(const QString& filePath, QTextStream& outStream,
                        QSet<QString>& processedIncludes, const QString& rootDir,
                        const QStringList& activeConditions) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        outStream << "\n// !!! FAILED to open include file: " << filePath << " !!!\n";
        return;
    }

    QStringList lines;
    {
        QTextStream in(&file);
        while (!in.atEnd()) lines << in.readLine();
    }
    file.close();

    static const QRegularExpression openRegex("^\\s*#\\s*(if|ifdef|ifndef)\\b");
    static const QRegularExpression guardRegex("^\\s*#\\s*ifndef\\s+(\\w+)\\s*$");
    static const QRegularExpression elseRegex("^\\s*#\\s*(else|elif|elifdef|elifndef)\\b");
    static const QRegularExpression endRegex("^\\s*#\\s*endif\\b");

    // A header wrapped in "#ifndef X / #define X ... #endif" is not conditional content.
    int guardOpen = -1;
    int guardClose = -1;
    {
        int first = -1;
        int second = -1;
        for (int i = 0; i < lines.size(); ++i) {
            const QString t = lines[i].trimmed();
            if (t.isEmpty() || t.startsWith("//")) continue;
            if (first < 0) first = i;
            else { second = i; break; }
        }
        if (first >= 0 && second >= 0) {
            const QRegularExpressionMatch g = guardRegex.match(lines[first]);
            if (g.hasMatch()) {
                const QRegularExpression defineRegex(
                    "^\\s*#\\s*define\\s+" + QRegularExpression::escape(g.captured(1)) + "\\b");
                if (defineRegex.match(lines[second]).hasMatch()) {
                    int depth = 0;
                    for (int i = first; i < lines.size(); ++i) {
                        if (openRegex.match(lines[i]).hasMatch()) ++depth;
                        else if (endRegex.match(lines[i]).hasMatch() && --depth == 0) {
                            guardClose = i;
                            break;
                        }
                    }
                    bool contentAfterGuard = false;
                    for (int i = guardClose + 1; guardClose >= 0 && i < lines.size(); ++i) {
                        if (!lines[i].trimmed().isEmpty()) contentAfterGuard = true;
                    }
                    if (guardClose >= 0 && !contentAfterGuard) guardOpen = first;
                    else guardClose = -1;
                }
            }
        }
    }

    // Each entry is one open conditional; an empty entry is a transparent include guard.
    QStringList conditions = activeConditions;
    QList<int> ownLevels;

    for (int lineNo = 0; lineNo < lines.size(); ++lineNo) {
        const QString& line = lines[lineNo];

        if (lineNo == guardOpen) {
            conditions << QString();
            ownLevels << conditions.size() - 1;
            outStream << line << "\n";
            continue;
        }
        if (openRegex.match(line).hasMatch()) {
            conditions << line.simplified();
            ownLevels << conditions.size() - 1;
            outStream << line << "\n";
            continue;
        }
        if (elseRegex.match(line).hasMatch()) {
            if (!ownLevels.isEmpty() && !conditions[ownLevels.last()].isEmpty()) {
                conditions[ownLevels.last()] += " / " + line.simplified();
            }
            outStream << line << "\n";
            continue;
        }
        if (endRegex.match(line).hasMatch()) {
            if (!ownLevels.isEmpty()) {
                conditions.removeAt(ownLevels.takeLast());
            }
            outStream << line << "\n";
            continue;
        }

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
                                   QFileInfo(includePath).absolutePath(), conditions);
                outStream << "// --- End of bundled file: " << includeName << " ---\n\n";
            } else {
                outStream << "// (Skipped already included file: " << includeName << ")\n";
            }
        } else if (systemMatch.hasMatch()) {
            QStringList real;
            for (const QString& c : std::as_const(conditions)) {
                if (!c.isEmpty()) real << c;
            }
            const QString systemHeader = systemMatch.captured(1).trimmed();
            const QString key = real.isEmpty() ? systemHeader
                                               : conditionKey(real) + "\n" + systemHeader;
            if (!processedIncludes.contains(systemHeader) && !processedIncludes.contains(key)) {
                processedIncludes.insert(key);
                outStream << line << "\n";
            }
        } else if (!line.trimmed().startsWith("#pragma once")) {
            outStream << line << "\n";
        }
    }
}
