#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class DocumentKeeper;
class Document;

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void changeEvent(QEvent *e);
public slots:
    void addDocumentView(Document* doc);
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
