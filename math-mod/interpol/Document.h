#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <QObject>

class Document : public QObject
{
    Q_OBJECT
public:
    explicit Document(QObject *parent = 0);
    virtual QString name() {
        return doc_name;
    }

    virtual void setName(const QString & name_) {
        doc_name = name_;
    }

    virtual QString defaultName() {
        return tr("Document");
    }

    virtual QWidget* settingWidget(QWidget *parent=0) = 0;

signals:

public slots:

private:
    QString doc_name;
};

#endif // DOCUMENT_H
