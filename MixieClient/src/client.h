#ifndef CLIENT_H
#define CLIENT_H

#include <QObject>

#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMap>
#include <QListWidgetItem>
#include <QtDebug>
#include <QInputDialog>
#include <QDir>
#include <QDesktopWidget>
#include <functional>
#include <QShortcut>
#include <QFile>
#include <QByteArray>

class Client : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(Client)
public:
    explicit Client(QObject *parent = nullptr);
public :
    void getNameFromUI(const QString& login);
    void getTextFromUI(const QString& text);
    void getReciever(const QString& reciever);
private slots:
      void sockReady();
      void sockDisc();
signals:
    void showDialogOnUI(const QString& mess);
    void reqNameFromUI();
    void showUsersOnUI(const QStringList& list);
    void showMessage(const QString& mess);
private:
    void isConnect();
    void changeName();
    void nameIsChanged();
    void getUsers();
    void getMessage();
    void sendMessage(const QString& mess);
    void getDialog();
private:
    bool dialog = false;
    QString id, name, whoRead;
    QTcpSocket* socket;
    QJsonDocument doc;
    QMap<QString, std::function<void()>> logicMap;
    QMap<QString, QString> jsonRules;
private:
    void logErr(const QByteArray& data);
};

#endif // CLIENT_H
