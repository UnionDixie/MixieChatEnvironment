#include "client.h"

Client::Client(QObject *parent) : QObject(parent) , socket(new QTcpSocket(this))
{
    connect(socket, SIGNAL(readyRead()), this, SLOT(sockReady()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(sockDisc()));
    socket->connectToHost("127.0.0.1", 2345);

    logicMap = {
       {"connect",[=]() { isConnect(); } },
       {"nameChanged",[=]() { nameIsChanged(); } },
       {"resSelect",[=]() { getUsers(); } },
       {"newMessages",[=]() { getMessage(); } },
       {"dialog",[=]() { getDialog(); } },
    };

    jsonRules = {
       {"changeName","{\"type\":\"changeName\",\"name\":\"%1\",\"newName\":\"%2\"}"},
       {"getUsers","{\"type\":\"getUsers\"}"},
       {"from","From %1 "},
       {"message","{\"type\":\"message\",\"sender\":\"%1\",\"receiver\":\"%2\",\"message\":\"%3\n\" }"},
       {"getDialog","{\"type\":\"getDialog\",\"sender\":\"%1\",\"receiver\":\"%2\" }"}
    };
    
}

void Client::sockReady() {
    qDebug() << "Client try read data";
    auto data = socket->readAll();
    qDebug() << "client get data" << data;
    QJsonParseError docErr;
    doc = QJsonDocument::fromJson(data, &docErr);
    if (docErr.errorString().toInt() == QJsonParseError::NoError) {
        if (auto it = logicMap[doc.object().value("type").toString()]; it != nullptr) {
            it();
        }
        else {
            logErr(data);
        }
    }
    else {
        logErr(data);
    }
}


void Client::getTextFromUI(const QString& text)
{
    sendMessage(text);
}

void Client::getReciever(const QString& reciever)
{
    whoRead = reciever;
    if (whoRead != name) {
        socket->write(jsonRules["getDialog"].arg(name, whoRead).toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
}

void Client::sockDisc() {
    socket->deleteLater();
}

void Client::logErr(const QByteArray& data)
{
    QFile file;
    file.setFileName("logJson.json");
    if (file.open(QIODevice::Append | QFile::Text)) {
        file.write(data);
        file.write("\n");
    }
    file.close();
}


void Client::isConnect() {
    id = doc.object().value("name").toString();
    emit requestName();
}

void Client::getNameFromUI(const QString& login) {
    name = login;
    changeName();
}

void Client::changeName()
{
    auto packetForChangeName = jsonRules["changeName"].arg(id, name);
    if (socket->isOpen()) {
        socket->write(packetForChangeName.toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
    else {
        logErr("Error socket close");
    }
}

void Client::nameIsChanged()
{
    if (socket->isOpen()) {
        socket->write(jsonRules["getUsers"].toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
    else {
        logErr("Error parse users");
    }
}

void Client::getUsers()
{
    QStringList users;
    QJsonArray docA = doc.object().value("result").toArray();
    for (const auto& it : docA) {
        users.push_back(it.toObject().value("name").toString());
    }
    emit showUsers(users);
}

void Client::getMessage()
{
    auto from = doc.object().value("from").toString();
    if (!dialog) {
        whoRead = from;
        dialog = true;
    }
    auto mess = doc.object().value("message").toString();
    emit showMessage(jsonRules["from"].arg(from) + mess);
}

void Client::sendMessage(const QString& text)
{
    QString receiver = whoRead;
    if (receiver != name) {
        if (!text.isEmpty()) {
            auto mess = jsonRules["message"].arg(name, receiver, text);
            socket->write(mess.toStdString().c_str());
        }
    }
    else {
        logErr("Block send yourself...");
    }
}

void Client::getDialog()
{
    QString dialog;
    QJsonArray docA = doc.object().value("result").toArray();
    for (const auto& it : docA) {
        auto mess = it.toObject().value("mess").toString();
        if (!mess.isEmpty()) {
            auto pos = mess.indexOf('@');
            auto notFound = -1;
            if (pos == notFound) {
                dialog.push_back("From " + whoRead + " - " + mess);
            }
            else {
                mess.remove(pos, 1);
                dialog.push_back(mess);
            }
        }
    }
    emit showDialog(dialog);
}
