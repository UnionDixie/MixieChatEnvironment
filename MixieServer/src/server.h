#ifndef SERVER_H
#define SERVER_H


#include <QTcpServer>
#include <QTcpSocket>

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

};

#endif // SERVER_H
