#pragma once

#include <QString>
#include <QStringList>

#include <QStandardItemModel>
#include <QStandardItem>

#include <QIcon>
#include <QColor>
#include <QVariant>

#include <QRegularExpression>


class Parser
{
public:
    struct Pattern
    {
        QString regex;
        QList<QVariant> capture; // int and string
        QRegularExpression::PatternOptions flags;
    };

    struct Rule
    {
        QString name;
        QList<Pattern> patterns;
        QIcon icon;
        QColor color;
    };

private:
    bool caseInsensitive;

    QList<Rule> rules;

    QStandardItemModel * model;
    QList<QStringList> fileList;
    QString filename;
    QStringList libraryPaths;
    QStringList searchPaths;
    QStringList wordList;

    void setSearchPaths();
    int findFileIndex(const QString & name);
    QString findFileName(const QString & name);
    bool detectCircularReference(QStandardItem * item);
    void appendModel(QStandardItem * parentItem, const QString & name);
public:
    Parser();
    ~Parser();
    void setFile(const QString & filename);
    void setLibraryPaths(QStringList paths);
    void clearFileList();
    void appendFileList(const QString & name);
    void buildModel();
    QStandardItemModel * treeModel();
    QStandardItemModel * searchModel();
    void setCaseInsensitive(bool enabled);
    QStringList getWordList();


    QList<Rule> getRules();
    void clearRules();
    void addRule(QString name, QList<Pattern> patterns, QIcon icon = QIcon(), QColor color = QColor());
    QStringList matchRule(const QString & name, const QString & text);
    QStringList matchRuleFromFile(const QString & name, const QString & filename);
    QStringList matchPattern(Pattern pattern, const QString & text);
};

