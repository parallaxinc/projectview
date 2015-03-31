#include <QDebug>
#include <QTreeView>
#include <QApplication>
#include <QList>
#include <QVariant>
#include <QHeaderView>
#include <QFile>


#include "parser.h"



int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Parser p;
//    p.getDependencies("../../Pikemanz/00_Pikemanz.spin");
//    p.getConstants("../piXel.spin");
//    p.getFunctions("../piXel.spin");
//    p.getFunctions("../../Brettris/Brettris.spin");
    p.setLibraryPaths(QStringList() << "../../../");
//    p.setFile("../../Pikemanz/00_Pikemanz.spin");
//    p.clearFileList();
//    p.appendFileList("../../Pikemanz/00_Pikemanz.spin");

    //p.buildModel("../../Pikemanz/00_Pikemanz.spin");
    p.setFile("../../Pikemanz/00_Pikemanz.spin");
//    p.setFile("../piXel.spin");
    p.buildModel();

    QFile file(":/icons/projectviewer/style.qss");
    file.open(QFile::ReadOnly);
    QString style = file.readAll();

    QTreeView *tree= new QTreeView();
    tree->setModel(p.treeModel());
    tree->header()->hide();
    tree->setStyleSheet(style);
    tree->show();





//    QTreeView *tree2= new QTreeView();
//    tree2->setModel(p.flatModel());
//    tree2->header()->hide();
//    tree2->setStyleSheet(style);
//    tree2->show();


    app.exec();
}
