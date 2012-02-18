#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(DocumentMediator* dm_, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    dm(dm_)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
