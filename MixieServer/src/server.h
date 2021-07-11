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
private:
    QByteArray data;
    QJsonDocument doc;
    QJsonParseError docErr;
    size_t port;
    struct ClientInfo {
            QString login;
            QByteArray bufferWhatSend;
            qintptr socketDescriptor;
    };
    QMap<QTcpSocket*, ClientInfo> socketToClient;
    QMap<QString, QTcpSocket*> clientToSocket;
private:
    void writeUsersToJsonFile();
    QByteArray getUsersFromJsonFile();
    void sendMessage(QJsonDocument& newMessage);
    void changeName(QJsonDocument& events);
    void sendUsers(QTcpSocket* user);
};

#endif // SERVER_H
