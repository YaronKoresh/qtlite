#ifndef SNIPPETMANAGER_H
#define SNIPPETMANAGER_H

#include <QString>
#include <QStringList>
#include <QMap>

class SnippetManager {
private:
    QMap<QString, QStringList> languageSnippets;

public:
    SnippetManager();

    QString getSnippet(const QString& name);
    QStringList getSnippetNames(const QString& language);
    QStringList getAllLanguages();

private:
    QString getSnippetContent(const QString& name);
    void initializeSnippets();
};

#endif // SNIPPETMANAGER_H
