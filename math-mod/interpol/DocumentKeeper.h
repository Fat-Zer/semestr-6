#ifndef DOCUMENTKEEPER_H
#define DOCUMENTKEEPER_H

#include <QObject>
#include <QSet>

class Document;

class DocumentKeeper : public QObject
{
    Q_OBJECT
public:
    static DocumentKeeper* instance(QObject *parent = 0) {
        if( !_instance ) {
            _instance = new DocumentKeeper(parent);
        }
        return _instance;
    }

    Document* docByName(const QString & name);
signals:
    void documentAdded(Document* doc);
    void documentAboutToDelete(Document* doc);

public slots:
    void addDocument(Document* doc);
    void deleteDocument(Document* doc);

protected:
    explicit DocumentKeeper(QObject *parent = 0);

private:
    QSet<Document*> docs;
    static DocumentKeeper* _instance;
};

#endif // DOCUMENTKEEPER_H
