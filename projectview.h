#pragma once

#include <QWidget>
#include <QTreeView>

class ProjectView : public QTreeView
{
    Q_OBJECT

public:
    explicit ProjectView(QWidget *parent = 0);
    void selectionChanged(
        const QItemSelection & selected,
        const QItemSelection & deselected);
    void expandChildren(const QModelIndex &index, bool expandOrCollapse);
};

