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
    model = new QStandardItemModel();
    caseInsensitive = false;
    _error = NoError;
}

ProjectParser::~ProjectParser()
{
    model->clear();
    delete model;
    model = 0;
}

ProjectParser::ParserError ProjectParser::status()
{
    return _error;
}

void ProjectParser::setFont(QFont font)
{
    this->font = font;
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
}

QStringList ProjectParser::getFileList()
{
    QList<QStandardItem *> items = model->findItems("*",Qt::MatchWildcard | Qt::MatchRecursive);
    fileList.clear();

    foreach (QStandardItem * item, items)
    {
        fileList << item->data().toMap()["file"].toString();
    }
    fileList.removeDuplicates();

    return fileList;
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

    _error = NotFoundError;
    qCritical() << "File not found:" << name;
    return -1;
}

QList<ProjectParser::Match> ProjectParser::matchPattern(Pattern pattern, const QString & text)
{
    QList<Match> result;

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

        Match r;
        r.exact = m.captured().trimmed();
        r.pretty = capture.simplified();
        result << r;
    }

    return result;
}

QList<ProjectParser::Match> ProjectParser::matchRuleFromFile(const QString & name, const QString & filename)
{
    QFile f(filename);
    f.open(QFile::ReadOnly);
    QString s = f.readAll();
    f.close();
    return matchRule(name, s);
}

QList<ProjectParser::Match> ProjectParser::matchRule(const QString & name, const QString & text)
{
    QList<Match> result;

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

void ProjectParser::styleRule(QString name, QIcon icon, QColor color)
{
    QList<Rule>::iterator i;
    for (i = rules.begin(); i != rules.end(); i++)
    {
        if (!i->name.compare(name))
        {
            if (!icon.isNull())  i->icon = icon;
            if (color.isValid()) i->color = color;
            return;
        }
    }
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

ProjectParser::Rule ProjectParser::getRule(const QString & name)
{
    foreach (Rule r, rules)
        if (!r.name.compare(name))
            return r;

    Rule r;
    return r;
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
    _error = NoError;

    wordList.clear();
    model->clear();

    QFileInfo fi(filename);
    if (!fi.exists() || !fi.isFile())
        return;

    QStandardItem * root = model->invisibleRootItem();

    QStandardItem * item = new QStandardItem(QIcon(":/icons/projectviewer/icon.png"),fi.fileName());
    QMap<QString, QVariant> data;
    data["file"] = fi.canonicalFilePath();
    
    item->setData(QVariant(data));
    item->setEditable(false);

    foreach (Rule r, rules)
        if (!QString::compare(r.name, "_includes_"))
            item->setForeground(QBrush(r.color));

    QFont rootfont = font;
    rootfont.setBold(true);
    item->setFont(rootfont);

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
            qCritical() << "Circular dependency:\n"
                        << "  parent:" << parent->data().toMap()["file"].toString() << "\n"
                        << "    item:" << item->data().toMap()["file"].toString();

            item->setForeground(QBrush(QColor("#FF0000")));
            item->parent()->setForeground(QBrush(QColor("#FF0000")));
            parent->setForeground(QBrush(QColor("#FF0000")));

            item->parent()->setText(item->parent()->text() + " -> " + parent->text());
            parent->setText(QFileInfo(
                        parent->data().toMap()["file"].toString()).fileName() + " (CIRCULAR DEPENDENCY)");
            item->parent()->removeRow(item->row());
            _error = CircularDependencyError;
            return true;
        }

        parent = parent->parent();
    }
    return false;
}

void ProjectParser::appendModel(QStandardItem * parentItem, const QString & name)
{
    QList<QStandardItem *> children;
    QList<QStandardItem *> items;

    foreach (Rule r, rules)
    {
        if (QString::compare(r.name, "_includes_"))
        {
            foreach (ProjectParser::Match match, matchRuleFromFile(r.name, name))
            {
                QMap<QString, QVariant> data;
                data["file"] = name;
                data["exact"] = match.exact;
                data["line"] = -1;

                QStandardItem * item = new QStandardItem(r.icon, match.pretty);
                item->setEditable(false);
                item->setForeground(QBrush(r.color));
                item->setFont(font);
                item->setData(data);
                
                items.append(item);
                wordList.append(item->text());
            }
        }
        else
        {
            foreach (ProjectParser::Match match, matchRuleFromFile("_includes_", name))
            {
                QString dep = match.pretty;
                QMap<QString, QVariant> data;

                QStandardItem * item = new QStandardItem(r.icon, QFileInfo(dep).fileName());
                item->setForeground(QBrush(r.color));
                item->setFont(font);

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

                children.append(item);
                wordList.append(item->text());
            }
        }
    }

    if (children.count() > 0)
        parentItem->appendRows(children);

    if (items.count() > 0)
        parentItem->appendRows(items);

    foreach (QStandardItem * item, children)
    {
        if (!detectCircularReference(item) && findFileIndex(item->text()) > -1)
        {
            appendModel(item, item->data().toMap()["file"].toString());
        }
    }
}


QStandardItemModel * ProjectParser::treeModel()
{
    return model;
}

QStringList ProjectParser::getWordList()
{
    return wordList;
}
