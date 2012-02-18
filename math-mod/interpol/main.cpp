#include <QtGui/QApplication>
#include "mainwindow.h"
#include "documentmediator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DocumentMediator dm;
    MainWindow w(&dm);
    w.show();

    return a.exec();
}
