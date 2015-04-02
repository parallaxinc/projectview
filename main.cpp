#include <QDebug>
#include <QApplication>
#include <QList>
#include <QVariant>
#include <QHeaderView>
#include <QFile>


#include "parser.h"
#include "projectview.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Parser p;
    p.setCaseInsensitive(true);

    QFile f("../../../LameAudio.spin");
    f.open(QFile::ReadOnly);
    QString s = f.readAll();
    f.close();

    QList<Parser::Pattern> constants;
    QList<Parser::Pattern> functions;
    QList<Parser::Pattern> _includes_;

    Parser::Pattern c1;
    c1.regex = "^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*=[ \t]*(.+?)('.*?)?[ \t]*$";
    c1.capture << 1;
    constants.append(c1);

    Parser::Pattern f1;
    f1.regex = "^(PUB|PRI)[ \t]+([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*(\\(.*\\))?(\\|.*|:.*)?.*$";
    f1.capture << 2 << " " << 3;
    functions.append(f1);

    Parser::Pattern d1;
    d1.regex = "^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*:[ \t]*\"(.*?)(.spin)?\"$";
    d1.capture << 2 << ".spin";
    _includes_.append(d1);

    p.addRule("functions",functions,
            QIcon(":/icons/projectviewer/block-pub.png"),
            QColor("#0000FF"));
    p.addRule("constants",constants,
            QIcon(":/icons/projectviewer/block-con.png"),
            QColor("#7F7F00"));
    p.addRule("_includes_",_includes_);

    QStringList x = p.matchRule("constants", s);

    foreach (QString y, x)
    {
        qDebug() << y;
    }

    p.setLibraryPaths(QStringList() << "../../../");
    //    p.setFile("../../Pikemanz/00_Pikemanz.spin");
    //    p.clearFileList();
    //    p.appendFileList("../../Pikemanz/00_Pikemanz.spin");

    p.setFile("../../Pikemanz/00_Pikemanz.spin");
    //    p.setFile("../piXel.spin");
    p.buildModel();

    QFile file(":/icons/projectviewer/style.qss");
    file.open(QFile::ReadOnly);
    QString style = file.readAll();

    ProjectView *tree= new ProjectView();
    tree->setModel(p.getModel());
    tree->header()->hide();
    tree->setStyleSheet(style);
    tree->show();

//    expandChildren(p.getModel()->index(0,0), tree);
//    QModelIndex index = tree->selectedIndexes()[0];
//    QStandardItem * item = index.model().itemFromIndex(index);
//    qDebug()


    app.exec();
}
