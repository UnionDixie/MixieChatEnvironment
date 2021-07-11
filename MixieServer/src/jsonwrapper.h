#ifndef JSONWRAPPER_H
#define JSONWRAPPER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QMap>

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
};

#endif // JSONWRAPPER_H
