#include "server.h"

Server::Server() : Server(2345) {}

Server::Server(size_t port) : port(port)
{ 
    qDebug() << "Create server..." 
             << "Current time - " << QDateTime::currentDateTime().toString() 
             << "The version of Qt is " << qVersion();
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
    auto messageForNewClient = QString("{\"type\":\"connect\",\"name\":\"%1\"}").arg(socketDescriptor);
    qDebug() << "Sending InitMssage for new client" 
             << messageForNewClient.toStdString().c_str();
    if(client->isOpen()){
        client->write(messageForNewClient.toStdString().c_str());
    }
    //socketToClient, clientToSocket
    socketToClient[client].socketDescriptor = socketDescriptor;
    clientToSocket[QString::number(socketDescriptor)] = client;
}


void Server::writeUsersToJsonFile() {
    QFile file;
    file.setFileName("users.json");
    file.resize(0);//clear
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        //[{"name":"sergey"},{"name":"max"},{"name":"ivan"}]
        file.write("[");
        auto count = clientToSocket.size() - 1;//last not have ,
        for (auto& name : clientToSocket.keys()) {
            auto packName = QString("{\"name\":\"%1\"}").arg(name);
            if (count--)
                packName.push_back(",");
            file.write(packName.toStdString().c_str());
        }
        file.write("]");
    }
    file.close();
}

QByteArray Server::getUsersFromJsonFile() {
    QFile file;
    QByteArray res;
    file.setFileName("users.json");
    if (file.open(QIODevice::ReadOnly | QFile::Text)) {
        res = "{\"type\":\"resSelect\",\"result\":" + file.readAll() + "}";
    }
    else {
        qDebug() << "Error open";
    }
    file.close();
    return res;
}

void Server::sendMessage(QJsonDocument& newMessage) {
    auto sender = doc.object().value("sender").toString();
    auto receiver = doc.object().value("receiver").toString();
    auto message = doc.object().value("message").toString();
    auto packet =
        QString("{\"type\":\"newMessages\", \"from\" : \"%1\", \"message\" : \"%2\"}").arg(sender, message);
    if (receiver == "All") {
        for (const auto& socket : socketToClient.keys()) {
            if (socket != nullptr) {
                socket->write(packet.toStdString().c_str());
                socket->waitForBytesWritten(500);
            }
        }
    }
    else {
        const auto& socket = clientToSocket[receiver];
        if (socket != nullptr) {
            socket->write(packet.toStdString().c_str());
            socket->waitForBytesWritten(500);
        }
    }
}

void Server::changeName(QJsonDocument& events) {
    const auto id = doc.object().value("name").toString();
    const auto newName = doc.object().value("newName").toString();
    const auto statusMess = QString("{\"type\":\"nameChanged\"}");
    const auto socket = clientToSocket[id];

    auto& it = socketToClient[socket];
    it.login = newName;
    clientToSocket[newName] = std::move(clientToSocket[id]);
    clientToSocket.erase(clientToSocket.find(id));

    socket->write(statusMess.toStdString().c_str());
    socket->waitForBytesWritten(500);
}

void Server::sendUsers(QTcpSocket* client) {
    qDebug() << "Try open users.json";
    writeUsersToJsonFile();
    client->write(getUsersFromJsonFile());
    qDebug() << "Sending...";
    client->waitForBytesWritten(500);
}

void Server::sockReady(){
    QTcpSocket* client = dynamic_cast<QTcpSocket*>(sender());
    if (client != nullptr) {
        data = client->readAll();
        qDebug() << "Server get data - " << data;
        doc = QJsonDocument::fromJson(data, &docErr);
        if (docErr.errorString().toInt() == QJsonParseError::NoError) {
            if (doc.object().value("type").toString() == "getUsers") {
                for (const auto& it : clientToSocket) {
                    sendUsers(it);
                }
            }
            else if (doc.object().value("type").toString() == "message") {
                sendMessage(doc);
            }
            else if (doc.object().value("type").toString() == "changeName") {
                changeName(doc);
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
   
    if (!clientToSocket.empty()) {
        for (const auto& it : clientToSocket) {
            sendUsers(it);
        }
    }
    client->deleteLater();
}
