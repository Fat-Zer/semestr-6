#ifndef DOCUMENTMEDIATOR_H
#define DOCUMENTMEDIATOR_H

#include <QObject>
#include <QList>

class DocumentMediator : public QObject
{
    Q_OBJECT
public:
    explicit DocumentMediator(QObject *parent = 0);

signals:

public slots:
private:
    //QList<>;
};

#endif // DOCUMENTMEDIATOR_H
