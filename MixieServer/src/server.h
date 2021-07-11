#ifndef SERVER_H
#define SERVER_H


#include <QTcpServer>
#include <QTcpSocket>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>

#include <QVector>
#include <QHostAddress>
#include <QObject>
#include <QMap>

//class QTcpServer;
//class QTcpSocket;
//class ChatMessage;

//#include <thread>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    Server(size_t port);
    ~Server() = default;
public slots:
    void run();
    void incomingConnection(qintptr socketDescriptor);
    void sockReady();
    void sockDisc();
    //void sendMessage(QString reciever,QString message);
private:
    //QVector<QPair<QTcpSocket*,QString>> sockets;
    QByteArray data;
    QJsonDocument doc;
    QJsonParseError docErr;
private:
    size_t port;
    //QTcpSocket* socket;
    //QTcpServer * const m_listener;
    struct ConnectionInfo {
            QString m_login;
            QByteArray m_clientBuffer;
            qintptr id;
    };
    QMap<QTcpSocket*, ConnectionInfo> m_clients;
    QMap<QString, QPair<QTcpSocket*,ConnectionInfo>> clients;
private:
    void writeUsersToJsonFile();
    QByteArray getUsersFromJsonFile();
    void sendMessage(QJsonDocument& newMessage);
    void changeName(QJsonDocument& events);
    void sendUsers(QTcpSocket* user);
};

#endif // SERVER_H
