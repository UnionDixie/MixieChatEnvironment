#include "SqlWrapper.h"


SqlWrapper::SqlWrapper()//QMap<QString, QString>& userRules
{
    qDebug() << "SqlWrapper starting....";
    
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data/db.db");
    if (db.open()) {
        qDebug() << "db open";
    }
    else {
        qDebug() << "Bad dp is't open";
    }
}

void SqlWrapper::getAll(QMap<QString, QMap<QString, QStringList>>& hashDB)
{
    auto tables = db.tables();
    for (const auto& table : tables) {
        auto column = db.record(table);
        auto n = column.count();
        for (size_t i = 0; i < n; i++)
        {
            QSqlQuery query(db);
            QString currColumn = column.fieldName(i);
            if (currColumn != "null") {
                if (query.exec(QString("SELECT %1 FROM %2").arg(currColumn, table))) {
                    QString packet;
                    while (query.next())
                    {
                        packet.append(query.value(0).toString());
                    }
                    hashDB[table][currColumn].push_back(packet);
                }
            }
        }
    }
}

void SqlWrapper::saveAll(QMap<QString, QMap<QString, QStringList>>& hashDB)
{
    auto tables = db.tables();
    QMap<QString, bool> isTable;
    for (const auto& it : tables) {
        isTable[it] = true;
    }
    for (const auto& sender : hashDB.keys()) {
        if(!isTable[sender])
        {
            QSqlQuery createTable(db);
            createTable.prepare(QString("CREATE TABLE \"%1\" (\"null\" TEXT);").arg(sender));
            createTable.exec();
        }
        auto table = db.record(sender);
        auto count = table.count();
        QMap<QString, bool> fieldName;
        for (size_t i = 0; i < count; i++)
        {
            fieldName[table.fieldName(i)] = true;
        }
        QSqlQuery createColumn(db);
        for (const auto& receiver : hashDB[sender].keys()) {
            if (!fieldName[receiver]) {
                createColumn.prepare(QString("ALTER TABLE %1 ADD %2 TEXT NULL").arg(sender, receiver));
                createColumn.exec();
            }
        }
        QSqlQuery deleRows(db);
        deleRows.prepare(QString("DELETE FROM %1").arg(sender));
        deleRows.exec();
       for (const auto& receiver : hashDB[sender].keys()) {
           QString packet;
           for (const auto& mess : hashDB[sender][receiver])
           {
               packet += mess + "\n";
           }
           QSqlQuery createRow(db);
           createRow.prepare(QString("INSERT INTO %1 (%2) VALUES('%3'); ").arg(sender, receiver, packet));
           createRow.exec();
       }
    }
}
