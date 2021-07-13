#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include <QTcpSocket>
#include <QMap>
#include <functional>

#include "src/json/jsonwrapper.h"
#include "src/storage/Storage.h"

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
    size_t port;
    struct ClientInfo {
            QString login;
            QByteArray bufferWhatSend;//remove???
            qintptr socketDescriptor;
    };
    QMap<QTcpSocket*, ClientInfo> socketToClient;
    QMap<QString, QTcpSocket*> clientToSocket;
    QMap<QString, QString> jsonRules;
    QMap<QString, std::function<void(const QJsonDocument&)>> logicMap;
    JsonWrapper jsonWrapper;
    Storage messageStorage;
private:
    void sendMessage(const QJsonDocument& doc);
    void changeName(const QJsonDocument& doc);
    void sendUsers(const QJsonDocument& doc);
    void sendDialog(const QJsonDocument& doc);
};

#endif // SERVER_H
