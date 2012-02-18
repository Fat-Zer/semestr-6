#ifndef CSVOPENDIALOG_H
#define CSVOPENDIALOG_H

#include <QDialog>

namespace Ui {
    class CSVOpenDialog;
}

class CSVOpenDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CSVOpenDialog(QWidget *parent = 0);
    ~CSVOpenDialog();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CSVOpenDialog *ui;
};

#endif // CSVOPENDIALOG_H
