#pragma once

#include <QString>
#include <QStringList>
#include <QList>

#include <QStandardItemModel>
#include <QStandardItem>

#include <QIcon>
#include <QColor>
#include <QVariant>

#include <QRegularExpression>


class ProjectParser
{
public:
    struct Match
    {
        QString exact;
        QString pretty;
    };

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
    QFont font;
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
    ProjectParser();
    ~ProjectParser();
    void setFile(const QString & filename);
    void setLibraryPaths(QStringList paths);
    void clearFileList();
    void appendFileList(const QString & name);
    void buildModel();
    QStandardItemModel * treeModel();
    void setCaseInsensitive(bool enabled);
    QStringList getWordList();


    QList<Rule> getRules();
    void clearRules();
    void addRule(QString name, QList<Pattern> patterns, QIcon icon = QIcon(), QColor color = QColor());
    void styleRule(QString name, QIcon icon = QIcon(), QColor color = QColor());
    void setFont(QFont font);
    QList<Match> matchRule(const QString & name, const QString & text);
    QList<Match> matchRuleFromFile(const QString & name, const QString & filename);
    QList<Match> matchPattern(Pattern pattern, const QString & text);
};

