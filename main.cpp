#include "mainwindow.h"
#include <QApplication>
#include <QResource>
int main(int argc, char *argv[])
{

    QApplication a(argc, argv);

    //QResource::registerResource("resources.rcc");
    //Q_INIT_RESOURCE(resources);

    MainWindow w;
    w.show();
    return a.exec();
}
