#include "server.h"

Server::Server() : Server(2345) {}

Server::Server(size_t port) : port(port)
{ 
    qDebug() << "Create server..." 
             << "\nCurrent time - " << QDateTime::currentDateTime().toString() 
             << "\nThe version of Qt is " << qVersion();
    jsonRules = {
        {"connect","{\"type\":\"connect\",\"name\":\"%1\"}"} ,
        {"newMessages","{\"type\":\"newMessages\", \"from\" : \"%1\", \"message\" : \"%2\"}"},
        {"nameChanged","{\"type\":\"nameChanged\"}"},
        {"name","{\"name\":\"%1\"}"},
        {"UserList","{\"type\":\"resSelect\",\"result\": %1 }"}
    };  
    logicMap = {
        {"getUsers",[=](const QJsonDocument& doc) { sendUsers(doc); } },
        {"message",[=](const QJsonDocument& doc) { sendMessage(doc); } },
        {"changeName",[=](const QJsonDocument& doc) { changeName(doc); } },
    };
    jsonWrapper.setRules(jsonRules);
}

void Server::run()
{
    if (listen(QHostAddress::LocalHost, port))
        qDebug() << "Listening port " << port;
    else
        qDebug() << "Can't listen port " << port;
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "New User connecting";
    QTcpSocket* client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    connect(client,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(client,SIGNAL(disconnected()),this,SLOT(sockDisc()));
    qDebug() << socketDescriptor << "Client Connected";
    auto messageForNewClient = jsonRules["connect"].arg(socketDescriptor);
    qDebug() << "Sending InitMssage for new client" 
             << messageForNewClient.toStdString().c_str();
    if(client->isOpen()){
        client->write(messageForNewClient.toStdString().c_str());
    }
    //socketToClient, clientToSocket
    socketToClient[client].socketDescriptor = socketDescriptor;
    clientToSocket[QString::number(socketDescriptor)] = client;
}

void Server::sendMessage(const QJsonDocument& doc) {
    auto sender = doc.object().value("sender").toString();
    auto receiver = doc.object().value("receiver").toString();
    auto message = doc.object().value("message").toString();
    auto packet = jsonRules["newMessages"].arg(sender, message);
    const auto& socket = clientToSocket[receiver];
    if (socket != nullptr) {
        socket->write(packet.toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
}

void Server::changeName(const QJsonDocument& doc) {
    const auto id = doc.object().value("name").toString();
    const auto newName = doc.object().value("newName").toString();
    const auto statusMess = jsonRules["nameChanged"];
    const auto socket = clientToSocket[id];

    auto& it = socketToClient[socket];
    it.login = newName;
    clientToSocket[newName] = std::move(clientToSocket[id]);
    clientToSocket.erase(clientToSocket.find(id));

    socket->write(statusMess.toStdString().c_str());
    socket->waitForBytesWritten(500);
}

void Server::sendUsers(const QJsonDocument& doc) {
    if (!clientToSocket.empty()) {
        for (const auto& it : clientToSocket) {
            qDebug() << "Try open users.json";
            jsonWrapper.writeUsersToJsonFile(clientToSocket.keys());
            it->write(jsonWrapper.getUsersFromJsonFile().toStdString().c_str());
            qDebug() << "Sending...";
            it->waitForBytesWritten(500);
        }
    }
}

void Server::sockReady(){
    QTcpSocket* client = dynamic_cast<QTcpSocket*>(sender());
    if (client != nullptr) {
        QJsonParseError docErr;
        auto data = client->readAll();
        qDebug() << "Server get data - " << data;
        auto doc = QJsonDocument::fromJson(data, &docErr);
        if (docErr.errorString().toInt() == QJsonParseError::NoError) {
            if (auto it = logicMap[doc.object().value("type").toString()]; it != nullptr) {
                it(doc);
            }
            else {
                qDebug() << "Error json parse";
            }
        }
        else {
            qDebug() << "Warning jsonError";
        }
    }
}

void Server::sockDisc(){
    QTcpSocket* client = dynamic_cast< QTcpSocket*>(sender());
    qDebug() << "Client " << socketToClient[client].login << " disconnected!";
    
    clientToSocket.erase(clientToSocket.find(socketToClient[client].login));
    socketToClient.erase(socketToClient.find(client));

    client->deleteLater();
}
