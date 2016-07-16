#pragma once

#include <QWidget>
#include <QTreeView>
#include <QStandardItemModel>
#include <QSortFilterProxyModel>
#include <QDebug>

#include ".ui/ui_projectview.h"

class ProjectFilter : public QSortFilterProxyModel 
{
    bool filterAcceptsRow(int source_row, const QModelIndex & source_parent) const
    {
        const QStandardItemModel * model = (QStandardItemModel * ) sourceModel();
        const QModelIndex & index = model->index(source_row, 0, source_parent);

        QStandardItem * item = model->itemFromIndex(index);
        
        if (!item->parent())
            return true;

        if (item->hasChildren())
        {
            int childCount = model->rowCount(index);
            for (int i = 0; i < childCount; i++)
            {
                if (filterAcceptsRow(i, index))
                    return true;
            }
        }

        bool result = model->data(index).toString().contains(filterRegExp());

        return result;
    }
public:
    ProjectFilter()
        : QSortFilterProxyModel()
    {
        setFilterCaseSensitivity(Qt::CaseInsensitive);
    }
    
};


class ProjectView : public QWidget
{
    Q_OBJECT

private:
    Ui::ProjectView ui;
    ProjectFilter proxy;

public:
    explicit ProjectView(QWidget *parent = 0);
    ~ProjectView();
    void selectionChanged(
        const QItemSelection & selected,
        const QItemSelection & deselected);
    void expandChildren(const QModelIndex &index, bool expandOrCollapse);
    void setModel(QStandardItemModel * model);

public slots:
    void changeView();
    void clicked(QModelIndex index);
    void updateColors(QColor background = QColor());
    void clearSearch();

signals:
    void showFileLine(QString filename, int linenumber);
};

