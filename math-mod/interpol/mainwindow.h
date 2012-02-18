#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DocumentMediator;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(DocumentMediator* dm_, QWidget *parent = 0);
    ~MainWindow();
protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    DocumentMediator *dm;
};

#endif // MAINWINDOW_H
