#include "projectview.h"

#include <QFile>
#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>

#include <QRegExp>


ProjectView::ProjectView(QWidget *parent)
    : QWidget(parent)
{
    ui.setupUi(this);
    QFile file(":/icons/projectviewer/style.qss");
    file.open(QFile::ReadOnly);
    QString style = file.readAll();

    ui.view->header()->hide();
    ui.view->setStyleSheet(style);


    connect(ui.search,SIGNAL(textChanged(const QString &)),
            &proxy, SLOT(setFilterRegExp(const QString &)));
    connect(ui.search,SIGNAL(textChanged(const QString &)),
            this, SLOT(printStuff()));
}

ProjectView::~ProjectView()
{
}

void ProjectView::printStuff()
{
    qDebug() << "";
    expandChildren(ui.view->model()->index(0,0), true);
}

void ProjectView::selectionChanged(
        const QItemSelection & selected,
        const QItemSelection & deselected)
{
    QModelIndex index = selected.indexes()[0];
    QStandardItem * item = ((QStandardItemModel * )index.model())->itemFromIndex(index);
    qDebug() << "BLAH" << item->text();


//    expandChildren(((QStandardItemModel *) index.model())->index(0,0),false);
    QList<QStandardItem *> items = ((QStandardItemModel *) index.model())->findItems(item->text());
    
    foreach (QStandardItem * i, items)
    {
        qDebug() << i->text();
    } 
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

    if (!ui.view->isExpanded(index)) {
        ui.view->setExpanded(index, true);
    }
}


void ProjectView::setModel(QStandardItemModel * model)
{
    proxy.setSourceModel(model);

    ui.view->setModel(&proxy);
//    ui.view->setExpanded(ui.view->model()->index(0,0), true);

    expandChildren(ui.view->model()->index(0,0), true);
}
