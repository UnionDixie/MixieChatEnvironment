#ifndef JSONWRAPPER_H
#define JSONWRAPPER_H

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>
#include <QMap>
#include <QByteArray>
#include <QtDebug>

class JsonWrapper
{
public:
    JsonWrapper() = default;
    QPair<QString,QJsonDocument> parseData(const QByteArray& data);
    void setRules(QMap<QString, QString>& newRules);
    void writeUsersToJsonFile(QStringList&& users);
    QString getUsersFromJsonFile();
    ~JsonWrapper() = default;
private:
    QMap<QString, QString> rules;
};

#endif // JSONWRAPPER_H
