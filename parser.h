#pragma once

#include <QString>
#include <QStringList>

#include <QStandardItemModel>
#include <QStandardItem>


class Parser
{
private:
    QStandardItemModel * flatmodel;
    QStandardItemModel * treemodel;
    QStandardItem * flatroot;
    QStandardItem * treeroot;
    QList<QStringList> fileList;
    QString filename;
    QStringList libraryPaths;
    QStringList searchPaths;

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
    QStringList getDependencies(const QString & name);
    QStringList getFunctions(const QString & name);
    QStringList getConstants(const QString & name);
    void clearFileList();
    void appendFileList(const QString & name);
    void buildModel();
    QStandardItemModel * flatModel();
    QStandardItemModel * treeModel();
};

