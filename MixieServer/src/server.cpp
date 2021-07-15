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
        {"UserList","{\"type\":\"resSelect\",\"result\": %1 }"},
        {"dialog","{\"type\":\"dialog\",\"result\": [%1] }"},
        {"mess","{\"mess\": \"%1\" }"}
    };  
    logicMap = {
        {"getUsers",[=](const QJsonDocument& doc) { sendUsers(doc); } },
        {"message",[=](const QJsonDocument& doc) { sendMessage(doc); } },
        {"changeName",[=](const QJsonDocument& doc) { changeName(doc); } },
        {"getDialog",[=](const QJsonDocument& doc) { sendDialog(doc); } },
        //getDialog
        
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
    //set connect
    auto client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    connect(client,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(client,SIGNAL(disconnected()),this,SLOT(sockDisc()));
    //send responce to client
    qDebug() << socketDescriptor << "Client Connected";
    auto messageForNewClient = jsonRules["connect"].arg(socketDescriptor);
    qDebug() << "Sending InitMssage for new client" 
             << messageForNewClient.toStdString().c_str();
    if(client->isOpen()){
        client->write(messageForNewClient.toStdString().c_str());
    }
    //client know only users login,for fast find current user
    //uses double map, key - socket and login for everyone user
    socketToClient[client].socketDescriptor = socketDescriptor;
    clientToSocket[QString::number(socketDescriptor)] = client;
    qDebug() << "Now Users - " << clientToSocket.size();
}

void Server::sendMessage(const QJsonDocument& doc) {
    const auto sender = doc.object().value("sender").toString(),
               receiver = doc.object().value("receiver").toString(),
               message = doc.object().value("message").toString(),
               packet = jsonRules["newMessages"].arg(sender, message);
    //save message in db
    messageStorage.write(sender, receiver, message);//it's duplicate receiver sender
    //
    const auto& socket = clientToSocket[receiver];
    if (socket != nullptr) {
        socket->write(packet.toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
    else {
        qDebug() << "nullptr - 75";
    }
}

//add sync with db
void Server::changeName(const QJsonDocument& doc) {
    const auto id = doc.object().value("name").toString(),
               newLogin = doc.object().value("newName").toString(),
               statusMess = jsonRules["nameChanged"];
    //check duplicate two users
    if (clientToSocket[newLogin] == nullptr) {
        const auto socket = clientToSocket[id];
        socketToClient[socket].login = newLogin;
        clientToSocket[newLogin] = std::move(clientToSocket[id]);
        clientToSocket.erase(clientToSocket.find(id));
        socket->write(statusMess.toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
}

//send all users to all
// write users to json,read from json and send json :)
void Server::sendUsers(const QJsonDocument& doc) {
    for (const auto& it : clientToSocket) {
        if (it != nullptr) {
            jsonWrapper.writeUsersToJsonFile(clientToSocket.keys());
            if (auto mess = jsonWrapper.getUsersFromJsonFile().toStdString(); !mess.empty()) {
                it->write(mess.c_str());
                qDebug() << "Sending all users to " << socketToClient[it].login;
                it->waitForBytesWritten(500);
            }
        }
        else {
            qDebug() << "Error nullptr";
        }
    }
}

void Server::sendDialog(const QJsonDocument& doc)
{
    const auto sender = doc.object().value("sender").toString(),
               receiver = doc.object().value("receiver").toString();
    const auto messages = messageStorage.get(sender, receiver);
    QString messageInJsonFormat;
    for (const auto& it : messages) {
        messageInJsonFormat.push_back(jsonRules["mess"].arg(it) + ",");
    }
    //[{"name":"1"},{"name":"2"}]
    messageInJsonFormat.remove(messageInJsonFormat.size() - 1, 1);
    if (messageInJsonFormat.isEmpty()) {
        messageInJsonFormat = "[{}]";
    }
    auto packet = jsonRules["dialog"].arg(messageInJsonFormat);
    const auto& socket = clientToSocket[sender];
    if (socket != nullptr) {
        socket->write(packet.toStdString().c_str());
        socket->waitForBytesWritten(500);
        qDebug() << "Send packet -" << packet;
    }
    else {
        qDebug() << "nullptr - 113";
    }
}

void Server::sockReady(){
    QTcpSocket* client = dynamic_cast<QTcpSocket*>(sender());
    if (client != nullptr) {
        auto data = client->readAll();
        qDebug() << "Server get data - " << data;
        const auto [typeMessage,doc] = jsonWrapper.parseData(data);
        if (auto it = logicMap[typeMessage]; it != nullptr) {
            it(doc);
        }
        else {
            qDebug() << "Error find logicMap : 105";
        }
    }
    else {
        qDebug() << "nullptr - 119 ";
    }
}

void Server::sockDisc() {
    QTcpSocket* client = dynamic_cast< QTcpSocket*>(sender());
    if (client != nullptr) {
        qDebug() << "Client " << socketToClient[client].login << " disconnected!";
        messageStorage.erase(socketToClient[client].login);
        if (auto it = clientToSocket.find(socketToClient[client].login); it != clientToSocket.end()) {
            clientToSocket.erase(it);
        }
        if (auto it = socketToClient.find(client); it != socketToClient.end()) {
            socketToClient.erase(it);
        }     
        client->deleteLater();
    }
    else {
        qDebug() << "nullptr - 122 ";
    }
}
