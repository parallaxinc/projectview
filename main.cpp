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
    QList<Parser::Pattern> privatefunctions;
    QList<Parser::Pattern> _includes_;

    Parser::Pattern c1;
    c1.regex = "^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*=[ \t]*(.+?)('.*?)?[ \t]*$";
    c1.capture << 1;
    constants.append(c1);

    Parser::Pattern c2;
    c2.regex = "^[ \t]*#[0-9_]+[ \t]*,(([ \t]*[a-zA-Z_]+[a-zA-Z0-9_]*[ \t]*,[ \t]*)*([ \t]*[a-zA-Z_]+[a-zA-Z0-9_]*[ \t]*)+).*$";
    c2.capture << 1;
    constants.append(c2);


    QString freg = "[ \t]+([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*(\\(.*\\))?(\\|.*|:.*)?.*$";

    Parser::Pattern f1;
    f1.regex = "^(PUB)"+freg;
    f1.capture << 2 << " " << 3;
    functions.append(f1);

    Parser::Pattern f2;
    f2.regex = "^(PRI)"+freg;
    f2.capture << 2 << " " << 3;
    privatefunctions.append(f2);

    Parser::Pattern d1;
    d1.regex = "^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*:[ \t]*\"(.*?)(.spin)?\"$";
    d1.capture << 2 << ".spin";
    _includes_.append(d1);

    p.addRule("public",functions,
            QIcon(":/icons/projectviewer/block-pub.png"),
            QColor("#0000FF"));
    p.addRule("private",privatefunctions,
            QIcon(":/icons/projectviewer/block-pri.png"),
            QColor("#007FFF"));

    p.addRule("constants",constants,
            QIcon(":/icons/projectviewer/block-con.png"),
            QColor("#7F7F00"));
    p.addRule("_includes_",_includes_);

//    QStringList x = p.matchRule("constants", s);

//    foreach (QString y, x)
//    {
//        qDebug() << y;
//    }

    p.setLibraryPaths(QStringList() << "../../../");
    //    p.setFile("../../Pikemanz/00_Pikemanz.spin");
    //    p.clearFileList();
    //    p.appendFileList("../../Pikemanz/00_Pikemanz.spin");

    p.setFile("../../Pikemanz/00_Pikemanz.spin");
    //p.setFile("../../../games/Brettris/Brettris.spin");
    //    p.setFile("../piXel.spin");
    p.buildModel();

//    qDebug() << "STUFF";
//    qDebug() << p.getWordList();

    ProjectView *tree= new ProjectView();
    tree->setModel(p.treeModel());
    tree->show();

    app.exec();
}
