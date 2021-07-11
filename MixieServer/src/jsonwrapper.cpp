#include "jsonwrapper.h"


QString JsonWrapper::getJsonResponce(QStringList& list)
{
	return QString();
}

void JsonWrapper::setRules(QMap<QString, QString>& newRules)
{
    rules = newRules;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("data/db.db");
    if(db.open()){
        qDebug() << "db open";
    }
    if (db.isOpen()) {

        QSqlQuery query2(db);
        query2.exec("DELETE FROM users");


        QSqlQuery query1(db);
        query1.prepare("INSERT INTO users (name) "
            "VALUES (:name)");
        //query1.bindValue(":id", 1001);
        //query1.bindValue(":forename", "Bart");
        query1.bindValue(":name", "MAx");
        query1.exec();

        QSqlQuery query(db);
        auto users = QString("[");
        if (query.exec("SELECT name FROM users")) {
            while (query.next())
            {
                users.append(rules["name"].arg(query.value(0).toString()) + ",");
            }
            users.remove(users.size() - 1, 1);
            users.append("]");
        }
        auto result = rules["UserList"].arg(users);
        qDebug() << result;
    }
}

void JsonWrapper::writeUsersToJsonFile(QStringList&& users)
{
    QFile file;
    file.setFileName("users.json");
    file.resize(0);//clear
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        file.write("[");
        auto count = users.size() - 1;//last not have ,
        for (const auto& name : users) {
            auto packName = rules["name"].arg(name);
            if (count--)
                packName.push_back(",");
            file.write(packName.toStdString().c_str());
        }
        file.write("]");
    }
    file.close();
}

QString JsonWrapper::getUsersFromJsonFile()
{
    QFile file;
    QString res;
    file.setFileName("users.json");
    if (file.open(QIODevice::ReadOnly | QFile::Text)) {
        res = rules["UserList"].arg(QString(file.readAll()));
    }
    else {
        qDebug() << "Error open";
    }
    file.close();
    return res;
}
