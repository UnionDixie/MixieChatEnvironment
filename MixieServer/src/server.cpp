#include "server.h"

Server::Server() : Server(2345) {}

Server::Server(size_t port) : port(port)
{
    qDebug() << "I'm born 0_0";
}


void Server::run()
{
    if (listen(QHostAddress::LocalHost, port))
    {
        qDebug() << "Listening....";
    }
    else 
    {
        qDebug() << "Can't listen....";
    }
    //connect(this, SIGNAL(newConnection()), this, SLOT(incomingConnection()));
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << "New Connected" << socketDescriptor;
    QTcpSocket* client = new QTcpSocket(this);
    client->setSocketDescriptor(socketDescriptor);
    //client->setSocketDescriptor(socketDescriptor);
    connect(client,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(client,SIGNAL(disconnected()),this,SLOT(sockDisc()));
    //ADD function for new user in users.json!!!
    qDebug() << socketDescriptor << "Client Connected";
    QString messageForNewClient = QString("{\"type\":\"connect\",\"name\":\"%1\"}").arg(socketDescriptor);
    auto t = messageForNewClient.toStdString();
    qDebug() << t.c_str();
    if(client->isOpen()){
        client->write(t.c_str());
    }
    qDebug() << "Sending InitMssage for new client";
    //sockets.push_back({ socket,QString::number(socketDescriptor) });
    m_clients[client].id = socketDescriptor;

    clients[QString::number(socketDescriptor)].first = client;
    clients[QString::number(socketDescriptor)].second.id = socketDescriptor;
}


void Server::writeUsersToJsonFile() {
    QFile file;
    file.setFileName("users.json");
    file.resize(0);
    if (file.open(QIODevice::WriteOnly | QFile::Text)) {
        //[{"name":"sergey"},{"name":"max"},{"name":"ivan"}]
        file.write("[");
        auto count = clients.size() - 1;//last not have ,
        for (auto& name : clients.keys()) {
            auto packName = QString("{\"name\":\"%1\"}").arg(name);
            if (count--) {
                packName.push_back(",");
            }
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
        QByteArray fromFile = file.readAll();
        res = "{\"type\":\"resSelect\",\"result\":" + fromFile + "}";
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
        for (auto& socket : m_clients.keys()) {
            if (socket != nullptr) {
                socket->write(packet.toStdString().c_str());
                socket->waitForBytesWritten(500);
            }
        }
    }
    else {
        auto& socket = clients[receiver].first;
        socket->write(packet.toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
    //messageToUserFromClient:
    //{"type":"message", "sender" : "I", "receiver" : "you", "message" : "Hello,bro" };
    //messageToUserFromServerOfuser:
    //{"type":"newMessages", "from" : "i", "message" : "hello bro"}
}

void Server::changeName(QJsonDocument& events) {
    auto id = doc.object().value("name").toString();
    auto newName = doc.object().value("newName").toString();
    auto statusMess =
        QString("{\"type\":\"nameChanged\"}");
    {
        auto& it = clients[id];
        it.second.m_login = newName;

        clients[newName] = std::move(clients[id]);
        clients.erase(clients.find(id));

        m_clients[clients[newName].first].m_login = newName;
    }

    auto& it = clients[newName];

    it.first->write(statusMess.toStdString().c_str());
    it.first->waitForBytesWritten(5000);
}

void Server::sendUsers(QTcpSocket* client) {
    qDebug() << "Try open users.json";
    writeUsersToJsonFile();
    client->write(getUsersFromJsonFile());
    qDebug() << "Sending...";
    client->waitForBytesWritten(1000);
}

void Server::sockReady(){
    QTcpSocket* client = dynamic_cast< QTcpSocket*>(sender());
    if(!client)
        return;
    data = client->readAll();
    qDebug() << "Server get data - " << data;
    //
    doc = QJsonDocument::fromJson(data,&docErr);
    if(docErr.errorString().toInt() == QJsonParseError::NoError){
        if (doc.object().value("type").toString() == "getUsers") {
            for (const auto& it : clients) {
                sendUsers(it.first);
            }
        }else if (doc.object().value("type").toString() == "message"){
            sendMessage(doc);
        }
        else if (doc.object().value("type").toString() == "changeName") {
            changeName(doc);   
        }else{
            qDebug() << "Wrong json parse";
        }
     }else{
         qDebug() << "Waring jsonError";
     }
}

void Server::sockDisc(){
    QTcpSocket* client = dynamic_cast< QTcpSocket*>(sender());
    qDebug() << "Client disconnected";
    client->deleteLater();
    auto& it = m_clients[client];
    clients.erase(clients.find(it.m_login));
    if (!clients.empty()) {
        for (const auto& it : clients) {
            sendUsers(it.first);
        }
    }
}
