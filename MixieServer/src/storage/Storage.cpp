#include "Storage.h"

Storage::Storage()
{
    dbWrapper.getAll(hashDB);
}

void Storage::write(QString sender, QString receiver, QString message)
{
    hashDB[sender][receiver].push_back("@" + message);
    if (sender != receiver) {
        hashDB[receiver][sender].push_back(message);
    }
    saveDB();//critical!!!
}

QStringList Storage::get(QString sender, QString receiver)
{
    return hashDB[sender][receiver];
}

//critical!!!
void Storage::erase(QString user)
{
    qDebug() << hashDB.size() << " " << sizeof(hashDB) * hashDB.size();
    saveDB();
    if (auto it = hashDB.find(user); it != hashDB.end()) {
        //hashDB.erase(it);
    }
}

void Storage::saveDB()
{
    qDebug() << "Save mess to db";
    dbWrapper.saveAll(hashDB);
}
