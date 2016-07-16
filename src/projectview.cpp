#include "projectview.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>

#include <QRegExp>
#include <QKeyEvent>
#include <QMap>

#include <QTextDocument>
#include <QDirIterator>

ProjectView::ProjectView(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    ui.view->header()->hide();

    connect(ui.search,SIGNAL(textChanged(const QString &)),
            &proxy, SLOT(setFilterRegExp(const QString &)));
    connect(ui.search,SIGNAL(textChanged(const QString &)),
            this, SLOT(changeView()));
    connect(ui.view,SIGNAL(clicked(QModelIndex)),this,SLOT(clicked(QModelIndex)));

    updateColors();
    setFocusProxy(ui.search);
}

ProjectView::~ProjectView()
{
}

void ProjectView::updateColors(QColor background)
{
    QFile file(":/icons/projectview/style.qss");
    file.open(QFile::ReadOnly);
    QString style = file.readAll();

    style = style.arg(background.name());

    if (background.lightness() < 128)
        style = style.arg("dark");
    else
        style = style.arg("light");

    ui.view->setStyleSheet(style);
}

void ProjectView::clicked(QModelIndex index)
{
    const QStandardItemModel * model = (QStandardItemModel *) proxy.sourceModel();
    if (model == NULL) 
    {
        qCritical() << "Attempted to use NULL tree model.";
        return;
    }

    QStandardItem * item = model->itemFromIndex(proxy.mapToSource(index));

    if (item == NULL)
        return;

    QMap<QString, QVariant> data = item->data().toMap();
    QString filename = data["file"].toString();
    QString exact = data["exact"].toString();

    QFileInfo fi(filename);
    if (!fi.exists() || !fi.isFile())
    {
        qCritical() << "File in tree does not exist!";
        return;
    }

    QFile f(filename);

    f.open(QFile::ReadWrite);
    QString text = f.readAll();

    QTextDocument doc(text);
    int line = doc.find(exact).blockNumber();

    qDebug() << "highlight:"
             << qPrintable(QString("(%1, %2):")
                                   .arg(fi.fileName())
                                   .arg(line))
             << exact;

    emit showFileLine(filename, line);
}

void ProjectView::changeView()
{
    if (!ui.search->text().isEmpty())
    {
        ui.view->expandAll();
    }
    else
    {
        ui.view->collapseAll();
        ui.view->setExpanded(ui.view->model()->index(0,0), true);
    }
}

void ProjectView::selectionChanged(
        const QItemSelection & selected,
        const QItemSelection & deselected)
{
    Q_UNUSED(deselected);
    QModelIndex index = selected.indexes()[0];
    QStandardItem * item = ((QStandardItemModel * )index.model())->itemFromIndex(index);

    QList<QStandardItem *> items = ((QStandardItemModel *) index.model())->findItems(item->text());
}

void ProjectView::expandChildren(const QModelIndex &index, bool expandOrCollapse)
{
    if (!index.isValid()) {
        return;
    }

    int childCount = index.model()->rowCount(index);
    for (int i = 0; i < childCount; i++) {
        const QModelIndex &child = index.child(i, 0);
        expandChildren(child, expandOrCollapse);
    }


    ui.view->setExpanded(index, expandOrCollapse);
}

void ProjectView::setModel(QStandardItemModel * model)
{
    if (model == NULL)
    {
        qCritical() << "Attempted to set NULL tree model.";
        return;
    }
    proxy.setSourceModel(model);
    ui.view->setModel(&proxy);
    ui.view->setExpanded(ui.view->model()->index(0,0), true);
}

void ProjectView::clearSearch()
{
    ui.search->clear();
}
