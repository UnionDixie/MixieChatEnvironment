#ifndef JSONWRAPPER_H
#define JSONWRAPPER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QMap>

#include <QSqlDatabase>
#include <QSqlQuery>

class JsonWrapper
{
public:
    JsonWrapper() = default;
    QString getJsonResponce(QStringList& list);
    void setRules(QMap<QString, QString>& newRules);
    void writeUsersToJsonFile(QStringList&& users);
    QString getUsersFromJsonFile();
    ~JsonWrapper() = default;
private:
    QMap<QString, QString> rules;
    QSqlDatabase db;
};

#endif // JSONWRAPPER_H
