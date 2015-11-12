#include <QDebug>
#include <QApplication>
#include <QList>
#include <QVariant>
#include <QHeaderView>
#include <QFile>

#include <QPalette>
#include <QColor>

#include <ProjectParser>
#include <ProjectView>


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    ProjectParser p;
    p.setCaseInsensitive(true);

    QList<ProjectParser::Pattern> constants;
    QList<ProjectParser::Pattern> functions;
    QList<ProjectParser::Pattern> privatefunctions;
    QList<ProjectParser::Pattern> _includes_;

    ProjectParser::Pattern c1;
    c1.regex = "^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*=[ \t]*(.+?)('.*?)?[ \t]*$";
    c1.capture << 1;
    constants.append(c1);

    ProjectParser::Pattern c2;
    c2.regex = "^[ \t]*#[0-9_]+[ \t]*,(([ \t]*[a-zA-Z_]+[a-zA-Z0-9_]*[ \t]*,[ \t]*)*([ \t]*[a-zA-Z_]+[a-zA-Z0-9_]*[ \t]*)+).*$";
    c2.capture << 1;
    constants.append(c2);

    QString freg = "[ \t]+([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*(\\(.*\\))?(\\|.*|:.*)?.*$";

    ProjectParser::Pattern f1;
    f1.regex = "^(PUB)"+freg;
    f1.capture << 2 << " " << 3;
    functions.append(f1);

    ProjectParser::Pattern f2;
    f2.regex = "^(PRI)"+freg;
    f2.capture << 2 << " " << 3;
    privatefunctions.append(f2);

    ProjectParser::Pattern d1;
    d1.regex = "^[ \t]*([a-zA-Z_]+[a-zA-Z0-9_]*)[ \t]*:[ \t]*\"(.*?)(\\.spin)?\".*$";
    d1.capture << 2 << ".spin";
    _includes_.append(d1);

    p.addRule("public",functions,
            QIcon(":/icons/projectview/block-pub.png"),
            QColor("#0000FF"));
    p.addRule("private",privatefunctions,
            QIcon(":/icons/projectview/block-pri.png"),
            QColor("#007FFF"));

    p.addRule("constants",constants,
            QIcon(":/icons/projectview/block-con.png"),
            QColor("#7F7F00"));
    p.addRule("_includes_",_includes_);

//    p.setLibraryPaths(QStringList() << "../../../lamestation-sdk");
    p.setFile("FrappyBard/FrappyBard.spin");
    p.buildModel();

    ProjectView *tree= new ProjectView();
    tree->setModel(p.treeModel());
    tree->show();

    tree->updateColors(QColor("#24003B"));
    p.styleRule("public",QIcon(),QColor( "#9595FE"));
    p.styleRule("private",QIcon(),QColor("#9595FE"));
    p.styleRule("constants",QIcon(),QColor("#EDEDED"));
    p.styleRule("_includes_",QIcon(),QColor("#FFFFFF"));

    QFont font = QFont("Monospace");
    font.setPointSize(12);

    p.setFont(font);
    p.buildModel();

    app.exec();
}
