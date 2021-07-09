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
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    connect(socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));
    //ADD function for new user in users.json!!!
    qDebug() << socketDescriptor << "Client Connected";
    QString messageForNewClient = QString("{\"type\":\"connect\",\"name\":\"%1\"}").arg(socketDescriptor);
    socket->write(messageForNewClient.toStdString().c_str());
    qDebug() << "Sending InitMssage for new client";
}

void Server::sockReady(){
    data = socket->readAll();
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
                socket->write(res);
                qDebug() << "Sending...";
                socket->waitForBytesWritten(500);
            }
            else {
                qDebug() << "Error open";
            }
            file.close();
        }else if (doc.object().value("type").toString() == "message"){
            auto sender = doc.object().value("sender").toString();
            auto receiver = doc.object().value("receiver").toString();
            auto message = doc.object().value("message").toString();


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
    qDebug() << "Client disconnected";
    socket->deleteLater();
}
