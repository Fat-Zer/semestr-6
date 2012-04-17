#include <QtGui/QApplication>
#include "mainwindow.h"
#include "DocumentKeeper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w(0);
    DocumentKeeper::instance(&w);
    w.show();

    return a.exec();
}
