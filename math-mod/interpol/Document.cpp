#include "Document.h"
#include "DocumentKeeper.h"
Document::Document(QObject *parent) :
    QObject(parent)
{
    QString name(defaultName());
    if(DocumentKeeper::instance()->docByName(name)) {
        for(int i=1; i<100; i++) {
            name = QString("%1 %2").arg(defaultName()).arg(i);
            if(DocumentKeeper::instance()->docByName(name)) {
                break;
            }
        }
    }
    doc_name = name;
}
