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
            qDebug() << "Try open users.json";
            QFile file;
            file.setFileName("users.json");
            if (file.open(QIODevice::ReadOnly | QFile::Text)) {
                QByteArray fromFile = file.readAll();
                QByteArray res = "{\"type\":\"resSelect\",\"result\":" + fromFile + "}";
                client->write(res);
                qDebug() << "Sending...";
                client->waitForBytesWritten(500);
            }
            else {
                qDebug() << "Error open";
            }
            file.close();
        }else if (doc.object().value("type").toString() == "message"){
            auto sender = doc.object().value("sender").toString();
            auto receiver = doc.object().value("receiver").toString();
            auto message = doc.object().value("message").toString();
            auto packet =
            QString("{\"type\":\"newMessages\", \"from\" : \"%1\", \"message\" : \"%2\"}").arg(sender,message);
            for (auto& socket : m_clients.keys()) {
                socket->write(packet.toStdString().c_str());
                socket->waitForBytesWritten(500);
            }
            //messageToUserFromClient:
            //{"type":"message", "sender" : "I", "receiver" : "you", "message" : "Hello,bro" };
            //messageToUserFromServerOfuser:
            //{"type":"newMessages", "from" : "i", "message" : "hello bro"}
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
}
