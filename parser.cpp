#include "parser.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QRegularExpression>
#include <QStringList>
#include <QStringListIterator>
#include <QBrush>
#include <QColor>


Parser::Parser()
{
    treemodel = 0;
    treeroot  = 0;

    flatmodel = 0;
    flatroot  = 0;
}

Parser::~Parser()
{
    if (treemodel)
        delete treemodel;

    if (flatmodel)
        delete flatmodel;

    treemodel = 0;
    treeroot  = 0;

    flatmodel = 0;
    flatroot  = 0;
}



void Parser::setFile(const QString & name)
{
    filename = name;
    setSearchPaths();
}

void Parser::setLibraryPaths(QStringList paths)
{
    libraryPaths = paths;
    setSearchPaths();
}

void Parser::setSearchPaths()
{
    searchPaths.clear();

    QFileInfo fi(filename);
    searchPaths << fi.canonicalPath();

    foreach (QString p, libraryPaths)
    {
        searchPaths << QDir(p).canonicalPath();
    }
    qDebug() << "PATHS" << searchPaths;
}

void Parser::clearFileList()
{
    fileList.clear();
}

void Parser::appendFileList(const QString & name)
{
    QStringList dependencies = getDependencies(name);

    fileList.clear();
    for (int i = 0; i < searchPaths.size(); i++)
    {
        fileList << QStringList();
    }

    foreach (QString dependency, dependencies)
    {
        int fileindex = findFileIndex(dependency);
        fileList[fileindex].append(dependency);
    }

    qDebug() << fileList;
}

QString Parser::findFileName(const QString & name)
{
    int pathindex = findFileIndex(name);
    if (pathindex > -1)
        return QDir(searchPaths[pathindex]).filePath(name);
    else
        return QString();
}

int Parser::findFileIndex(const QString & name)
{
    QString path;
    int pathindex = 0;

    QStringListIterator paths(searchPaths);
    while (paths.hasNext())
    {
        path = paths.next();
        QDir dir(path);
        if (dir.exists(name))
        {
            return pathindex;
        }

        pathindex++;
    }
    qDebug() << "NOT FOUND" << name;
    return -1;
}

QStringList Parser::getConstants(const QString & name)
{
    QFile f(name);
    f.open(QFile::ReadOnly);
    QString s = f.readAll();
    f.close();

    QRegularExpression re("^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*=[ \t]*[0-9]+[ \t]*$");
    re.setPatternOptions(
            QRegularExpression::MultilineOption |
            QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator match = re.globalMatch(s);

    QStringList list;

    while (match.hasNext())
    {
        QRegularExpressionMatch m = match.next();
        list << m.captured(1);
    }

    return list;
}


QStringList Parser::getFunctions(const QString & name)
{
    QFile f(name);
    f.open(QFile::ReadOnly);
    QString s = f.readAll();
    f.close();

    QRegularExpression re("^(PUB|PRI)[ \t]+([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*(\\(.*\\))?(\\|.*|:.*)?.*$");
    re.setPatternOptions(
            QRegularExpression::MultilineOption |
            QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator match = re.globalMatch(s);

    QStringList list;

    while (match.hasNext())
    {
        QRegularExpressionMatch m = match.next();
        list << (m.captured(2) + " " + m.captured(3));
    }

    return list;
}

QStringList Parser::getDependencies(const QString & name)
{
    QFile f(name);
    f.open(QFile::ReadOnly);
    QString s = f.readAll();
    f.close();

    QRegularExpression re("^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*:[ \t]*\"(.*?)(.spin)?\"$");
    re.setPatternOptions(
            QRegularExpression::MultilineOption |
            QRegularExpression::CaseInsensitiveOption);

    QRegularExpressionMatchIterator match = re.globalMatch(s);

    QStringList list;

    while (match.hasNext())
    {
        QRegularExpressionMatch m = match.next();
        list << m.captured(2) + ".spin";
    }

    return list;
}

void Parser::buildModel()
{
    if (treemodel)
        delete treemodel;

    treemodel = new QStandardItemModel(); 
    treeroot = treemodel->invisibleRootItem();

    QStandardItem * item = new QStandardItem(QIcon(":/icons/projectviewer/icon.png"),QFileInfo(filename).fileName());
    QMap<QString, QVariant> data;
    data["file"] = QFileInfo(filename).canonicalFilePath();
    
    item->setData(QVariant(data));
    item->setEditable(false);

    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);


    treeroot->appendRow(item);

    if (flatmodel)
        delete flatmodel;

    flatmodel = new QStandardItemModel();
    flatroot = treemodel->invisibleRootItem();

//    for (int i = 0 ; i < searchPaths.size(); i++)
//    {
//        QStandardItem * path = new QStandardItem(QIcon(":/icons/projectviewer/block-obj.png"),QFileInfo(filename).fileName());
//    }

    appendModel(item, filename);
}

bool Parser::detectCircularReference(QStandardItem * item)
{
    QStandardItem * parent = item->parent();
    while (parent != 0)
    {
        if (!QString::compare(
                parent->data().toMap()["file"].toString(),
                item->data().toMap()["file"].toString(),
                Qt::CaseInsensitive
            ))
        {
            qDebug() << "CIRCULAR DEPENDENCY";
            qDebug() << "  PARENT" << parent->data().toMap()["file"].toString();
            qDebug() << "    ITEM" << item->data().toMap()["file"].toString();
            qDebug() << "";

            item->setForeground(QBrush(QColor("#FF0000")));
            item->parent()->setForeground(QBrush(QColor("#FF0000")));
            parent->setForeground(QBrush(QColor("#FF0000")));

            item->parent()->setText(item->parent()->text() + " -> " + parent->text());
            parent->setText(QFileInfo(
                        parent->data().toMap()["file"].toString()).fileName() + " (CIRCULAR DEPENDENCY)");
            item->parent()->removeRow(item->row());
            return true;
        }

        parent = parent->parent();
    }
    return false;
}

void Parser::appendModel(QStandardItem * parentItem, const QString & name)
{
    QList<QStandardItem *> items;


    // display functions
    foreach (QString function, getFunctions(name))
    {
        QStandardItem * item = new QStandardItem(QIcon(":/icons/projectviewer/block-pub.png"),function);
        item->setEditable(false);
        item->setForeground(QBrush(QColor("#0000FF")));

        parentItem->appendRow(item);
    }

    // display constants
    foreach (QString con, getConstants(name))
    {
        QStandardItem * item = new QStandardItem(QIcon(":/icons/projectviewer/block-con.png"),con);
        item->setEditable(false);
        item->setForeground(QBrush(QColor("#7F7F00")));

        parentItem->appendRow(item);
    }

    foreach (QString dep, getDependencies(name))
    {
        // tree view
        QMap<QString, QVariant> data;
        QStandardItem * item = new QStandardItem(QIcon(":/icons/projectviewer/block-obj.png"),dep);

        QString fulldep = findFileName(dep);
        if (fulldep.isEmpty())
        {
            item->setText(item->text() + " (NOT FOUND)");
            item->setForeground(QBrush(QColor("#FF0000")));
        }
        dep = fulldep;
        data["file"] = dep;

        item->setData(QVariant(data));
        item->setEditable(false);

        items.append(item);

        // flat view

//        QStandardItem * flatitem = new QStandardItem(QIcon(":/icons/projectviewer/block-obj.png"),dep);
//        int pathindex = findFileIndex(dep);
    }
    parentItem->appendRows(items);

    foreach (QStandardItem * item, items)
    {
        if (!detectCircularReference(item) && findFileIndex(item->text()) > -1)
        {
            appendModel(item, item->data().toMap()["file"].toString());
        }
    }
}


QStandardItemModel * Parser::flatModel()
{
    return flatmodel;
}
QStandardItemModel * Parser::treeModel()
{
    return treemodel;
};
