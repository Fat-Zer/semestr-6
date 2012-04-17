#include "DocumentKeeper.h"
#include "Document.h"

DocumentKeeper* DocumentKeeper::_instance=0;

DocumentKeeper::DocumentKeeper(QObject *parent) :
    QObject(parent)
{
}

void DocumentKeeper::addDocument(Document* doc) {
    docs.insert(doc);
    emit documentAdded(doc);
}

void DocumentKeeper::deleteDocument(Document* doc) {
    emit documentAboutToDelete(doc);
    if( docs.contains(doc) ) {
        delete doc;
        docs.remove(doc);
    }

}

Document* DocumentKeeper::docByName(const QString & name) {
    for(QSet<Document*>::iterator doc=docs.begin();
                doc != docs.end(); doc++) {
        if((*doc)->name() == name) {
            return *doc;
        }
    }
    return 0;
}
