#include <QtCore/QCoreApplication>
#include <QtCore/QtGlobal>
#include <QtCore/QDebug>
#include <QtCore/QTextStream>
#include <QtCore/QVariant>

#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <QtXml/QDomDocument>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL");
    db.setHostName("localhost");
    db.setDatabaseName("alexander");
    db.setUserName("alexander");
    // db.setPassword("");

    if(db.open()) {
        QSqlQuery q;
        QTextStream outs(stdout);

        // simple query
        if(q.exec("SELECT surname, name FROM clients LIMIT 10")) {
            while (q.next()) {
                outs << q.value(0).toString().toUtf8() << ' '
                     << q.value(1).toString().toUtf8() << '\n';
            }
        } else {
            qCritical() << "Couldn't execute query. Reason: " << q.lastError().text() ;
        }

        // xml query
        if(q.exec("SELECT query_to_xml($$SELECT surname, name FROM clients LIMIT 10;$$,"
                        "false, false, 'NULL' )")) {
            q.next();
            QDomDocument doc;
            QString err;
            outs << q.value(0).toString();
            if(doc.setContent(q.value(0).toString(), 1, &err)) {
                QDomElement docElem = doc.documentElement();    // "table element"

                for(QDomNode n = docElem.firstChild(); !n.isNull(); n = n.nextSibling()) {
                    QDomElement row = n.toElement(); // try to convert the node to an element.

                    if(!row.isNull() && row.tagName()=="row") {
                        //QDomElement
                        outs << row.firstChildElement("name").text() << ' '
                             << row.firstChildElement("surname").text() << ' '<<'\n';
                    }
                }
            } else {
                qCritical() << "QDomDocument failed to parse xml. Reason: " << err;
            }

        } else {
            qCritical() << "Couldn't execute query. Reason: " << q.lastError().text() ;
        }
    } else {
        qCritical() << "Couldn't connect to db. Reason: " << db.lastError().text() ;
    }

    return 0;
}
