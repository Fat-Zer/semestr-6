#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DocumentKeeper.h"
#include "Document.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(DocumentKeeper::instance(), SIGNAL(documentAdded(Document*)),
            this, SLOT(addDocumentView(Document*)));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::changeEvent(QEvent *e) {
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainWindow::addDocumentView(Document* doc) {
    ui->docs->addTab(doc->settingWidget(this), doc->name());

}
