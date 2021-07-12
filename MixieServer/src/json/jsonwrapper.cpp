#include "jsonwrapper.h"

QPair<QString, QJsonDocument> JsonWrapper::parseData(const QByteArray& data)
{
    QString result;
    QJsonParseError docErr;
    auto doc = QJsonDocument::fromJson(data, &docErr);
    if (docErr.errorString().toInt() == QJsonParseError::NoError) {
        result = doc.object().value("type").toString();
    }
    else {
        qDebug() << "Warning jsonError parse";
    }
    return { result,doc };
}

void JsonWrapper::setRules(QMap<QString, QString>& newRules)
{
    rules = newRules;
}

void JsonWrapper::writeUsersToJsonFile(QStringList&& users)
{
    qDebug() << "Try write to users.json";
    QFile file;
    file.setFileName("data/users.json");
    file.resize(0);//clear
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        QString names;
        for (const auto& name : users) {
            names += rules["name"].arg(name) + ",";
        }
        names.remove(names.size() - 1, 1);//remove last ,
        file.write(QString("[%1]").arg(names).toStdString().c_str());
    }
    else {
        qDebug() << "Error write to users.json";
    }
    file.close();
}

QString JsonWrapper::getUsersFromJsonFile()
{
    qDebug() << "Try read users.json";
    QFile file;
    QString res;
    file.setFileName("data/users.json");
    if (file.open(QIODevice::ReadOnly | QFile::Text)) {
        res = rules["UserList"].arg(QString(file.readAll()));
    }
    else {
        qDebug() << "Error read users.json";
    }
    file.close();
    qDebug() << "Return users to server";
    return res;
}
