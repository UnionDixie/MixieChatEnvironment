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

    socket->write("You are connect >_<");
    qDebug() << "Send message to client";

}

void Server::sockReady(){



}

void Server::sockDisc(){
    qDebug() << "Client disconnected";
    socket->deleteLater();
}
