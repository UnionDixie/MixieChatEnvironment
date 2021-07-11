#include "jsonwrapper.h"


QString JsonWrapper::getJsonResponce(QStringList& list)
{
	return QString();
}

void JsonWrapper::setRules(QMap<QString, QString>& newRules)
{
    rules = newRules;
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
        res = rules["UserList"].arg(file.readAll());
    }
    else {
        qDebug() << "Error open";
    }
    file.close();
    return res;
}
