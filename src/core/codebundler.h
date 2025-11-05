#ifndef CODEBUNDLER_H
#define CODEBUNDLER_H

#include <QString>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QSet>
#include <QDir>
#include <QRegularExpression>

class CodeBundler {
private:
    QRegularExpression localIncludeRegex;
    QRegularExpression systemIncludeRegex;

public:
    CodeBundler();

    bool bundleCode(const QString& rootFilePath, QString& outputPath);

private:
    void parseAndBundleFile(const QString& filePath, QTextStream& outStream,
                            QSet<QString>& processedIncludes, const QString& rootDir);
};

#endif // CODEBUNDLER_H
