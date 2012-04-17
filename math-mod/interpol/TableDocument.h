#ifndef TABLEDOCUMENT_H
#define TABLEDOCUMENT_H
#include "Document.h"

class TableDocument : public Document
{
    Q_OBJECT
public:
    explicit TableDocument(QObject *parent = 0);

    virtual QString defaultName() {
        return tr("Table");
    }
signals:

public slots:

};

#endif // TABLEDOCUMENT_H
