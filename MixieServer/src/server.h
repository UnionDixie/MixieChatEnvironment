#ifndef SERVER_H
#define SERVER_H


#include <QTcpServer>
#include <QTcpSocket>

#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QFile>

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server();
    ~Server() = default;
public slots:
    void run();
    void incomingConnection(qintptr socketDescriptor);
    void sockReady();
    void sockDisc();
private:
    QTcpSocket* socket;
    QByteArray data;
    QJsonDocument doc;
    QJsonParseError docErr;
};

#endif // SERVER_H
