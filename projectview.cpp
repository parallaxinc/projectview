#include "projectview.h"

#include <QDebug>
#include <QStandardItem>
#include <QStandardItemModel>

ProjectView::ProjectView(QWidget *parent)
    : QTreeView(parent)
{
}

void ProjectView::selectionChanged(
        const QItemSelection & selected,
        const QItemSelection & deselected)
{
    QModelIndex index = selectedIndexes()[0];
    QStandardItem * item = ((QStandardItemModel * )index.model())->itemFromIndex(index);
    qDebug() << item->text();


//    expandChildren(((QStandardItemModel *) index.model())->index(0,0),false);
//    setExpanded(index, true);
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

    if (!isExpanded(index)) {
        expand(index);
    }
}


