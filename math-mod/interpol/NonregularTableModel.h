#ifndef NONREGULARTABLEMODEL_H
#define NONREGULARTABLEMODEL_H

#include <QAbstractTableModel>
#include <list>
#include <utility>

class NonregularTableModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    explicit NonregularTableModel(QObject *parent = 0);

signals:

public slots:

private:
    std::list< std::pair<double,double> > pntList;
};

#endif // NONREGULARTABLEMODEL_H
