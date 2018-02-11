#include <QApplication>
#include "mainpanel.h"

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    app.setApplicationDisplayName(QApplication::tr("MEMS oriented image testing technology"));
    MainPanel w;
    w.show();
    return app.exec();
}

