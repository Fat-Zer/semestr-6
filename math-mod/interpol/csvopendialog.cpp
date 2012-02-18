#include "csvopendialog.h"
#include "ui_csvopendialog.h"

CSVOpenDialog::CSVOpenDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CSVOpenDialog)
{
    ui->setupUi(this);
}

CSVOpenDialog::~CSVOpenDialog()
{
    delete ui;
}

void CSVOpenDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
