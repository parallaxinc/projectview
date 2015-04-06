#include "projectparser.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <QStringListIterator>
#include <QBrush>
#include <QColor>


ProjectParser::ProjectParser()
{
    caseInsensitive = false;

    model = 0;
}

ProjectParser::~ProjectParser()
{
    if (model)
        delete model;

    model = 0;
}

void ProjectParser::setCaseInsensitive(bool enabled)
{
    caseInsensitive = enabled;
}


void ProjectParser::setFile(const QString & name)
{
    filename = name;
    setSearchPaths();
}

void ProjectParser::setLibraryPaths(QStringList paths)
{
    libraryPaths = paths;
    setSearchPaths();
}

void ProjectParser::setSearchPaths()
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

void ProjectParser::clearFileList()
{
    fileList.clear();
}

void ProjectParser::appendFileList(const QString & name)
{
    QStringList dependencies = matchRuleFromFile("_includes_", name);

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

//    qDebug() << fileList;
}

QString ProjectParser::findFileName(const QString & name)
{
    int pathindex = findFileIndex(name);
    if (pathindex > -1)
        return QDir(searchPaths[pathindex]).filePath(name);
    else
        return QString();
}

int ProjectParser::findFileIndex(const QString & name)
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

QStringList ProjectParser::matchPattern(Pattern pattern, const QString & text)
{
    QStringList result;

    QRegularExpression re(pattern.regex);

    int options = QRegularExpression::MultilineOption;
    if (caseInsensitive)
        options |= QRegularExpression::CaseInsensitiveOption;
    re.setPatternOptions((QRegularExpression::PatternOptions) options);
    
    QRegularExpressionMatchIterator match = re.globalMatch(text);
    while (match.hasNext())
    {
        QRegularExpressionMatch m = match.next();

        QString capture;
        
        foreach (QVariant v, pattern.capture)
        {
            QMetaType::Type t = (QMetaType::Type) v.type();
            if (t == QMetaType::Int)
            {
                capture += m.captured(v.toInt());
            }
            else if (t == QMetaType::QString)
            {
                capture += v.toString();
            }
        }
        result << capture.simplified();
    }

    return result;
}

QStringList ProjectParser::matchRuleFromFile(const QString & name, const QString & filename)
{
    QFile f(filename);
    f.open(QFile::ReadOnly);
    QString s = f.readAll();
    f.close();
    return matchRule(name, s);
}

QStringList ProjectParser::matchRule(const QString & name, const QString & text)
{
    QStringList result;

    foreach (Rule r, rules)
    {
        if (!r.name.compare(name))
        {
            foreach (Pattern p, r.patterns)
            {
                result.append(matchPattern(p, text));
            }
        }
    }

    return result;
}

void ProjectParser::addRule(QString name, QList<Pattern> patterns, QIcon icon, QColor color)
{
    // test if rule already defined; if so, use that rule instead
    QList<Rule>::iterator i;
    for (i = rules.begin(); i != rules.end(); i++)
    {
        if (!i->name.compare(name))
        {
            i->patterns.append(patterns);
            return;
        }
    }

    Rule r;

    r.name = name;
    r.patterns = patterns;
    r.icon = icon;
    r.color = color;

    rules.append(r);
}

QList<ProjectParser::Rule> ProjectParser::getRules()
{
    return rules;
}

void ProjectParser::clearRules()
{
    rules.clear();
}

void ProjectParser::buildModel()
{
    if (model)
        delete model;

    wordList.clear();

    model = new QStandardItemModel(); 
    QStandardItem * root = model->invisibleRootItem();

    QStandardItem * item = new QStandardItem(QIcon(":/icons/projectviewer/icon.png"),QFileInfo(filename).fileName());
    QMap<QString, QVariant> data;
    data["file"] = QFileInfo(filename).canonicalFilePath();
    
    item->setData(QVariant(data));
    item->setEditable(false);

    QFont font = item->font();
    font.setBold(true);
    item->setFont(font);

    root->appendRow(item);

    appendModel(item, filename);
}

bool ProjectParser::detectCircularReference(QStandardItem * item)
{
    QStandardItem * parent = item->parent();
    while (parent != 0)
    {
        if (!QString::compare(
                parent->data().toMap()["file"].toString(),
                item->data().toMap()["file"].toString(),
                (Qt::CaseSensitivity) (!caseInsensitive)
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

void ProjectParser::appendModel(QStandardItem * parentItem, const QString & name)
{
    QList<QStandardItem *> items;
//    qDebug() << parentItem->row() << parentItem->text();

    foreach (Rule r, rules)
    {
        if (QString::compare(r.name, "_includes_"))
        {
            foreach (QString text, matchRuleFromFile(r.name, name))
            {
                QStandardItem * item = new QStandardItem(r.icon, text);
                item->setEditable(false);
                item->setForeground(QBrush(r.color));
                item->setData(text);

                parentItem->appendRow(item);
                wordList.append(item->text());
            }
        }
        else
        {
            foreach (QString dep, matchRuleFromFile("_includes_", name))
            {
                QMap<QString, QVariant> data;
                QStandardItem * item = new QStandardItem(r.icon, dep);

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
                wordList.append(item->text());
            }
        }
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


QStandardItemModel * ProjectParser::treeModel()
{
    if (model == 0)
        model = new QStandardItemModel();

    return model;
}

QStringList ProjectParser::getWordList()
{
    return wordList;
}
