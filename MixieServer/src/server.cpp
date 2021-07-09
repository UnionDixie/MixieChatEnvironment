#include "server.h"

Server::Server()
{
 qDebug() << "I'm born 0_0";

}


void Server::run(){
    if(listen(QHostAddress::LocalHost,2345))
    {
      qDebug() << "Listening...." ;
    }else{
         qDebug() << "Can't listen...." ;
    }
}

void Server::incomingConnection(qintptr socketDescriptor){
    socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    connect(socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));

    qDebug() << socketDescriptor << "Client Connected";

    QString jsonTest = "{\"type\":\"connect\",\"status\":\"YES\"}";

    //socket->write("You are connect >_<");
    socket->write(jsonTest.toStdString().c_str());
    qDebug() << "Send message to client";

}

void Server::sockReady(){
    data = socket->readAll();
    qDebug() << "Server get data - " << data;
    //{type:select,params:workers};
    doc = QJsonDocument::fromJson(data,&docErr);
    if(docErr.errorString().toInt() == QJsonParseError::NoError){
        if(doc.object().value("type").toString() == "select" && doc.object().value("params").toString() == "users"){
             qDebug() << "Parse serv ok";
             QFile file;
             file.setFileName("users.json");
             if(file.open(QIODevice::ReadOnly | QFile::Text)){
                 QByteArray fromFile = file.readAll();
                 //type resulselect result:....
                 QByteArray res = "{\"type\":\"resSelect\",\"result\":" + fromFile + "}";

                 socket->write(res);
                 socket->waitForBytesWritten(500);
             }else{
                  qDebug() << "Error open";
             }
             file.close();

        }else{
             qDebug() << "Parse noo serv ok";
        }
     }else{
         qDebug() << "Parse no serv ";
     }


}

void Server::sockDisc(){
    qDebug() << "Client disconnected";
    socket->deleteLater();
}
