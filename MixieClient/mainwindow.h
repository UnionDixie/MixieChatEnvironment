#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTcpSocket>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QMap>
#include <QListWidgetItem>

#include <functional>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
public slots:
    void sockReady();
    void sockDisc();
private slots:
    void on_pushButton_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

private:
    void isConnect(QJsonDocument& doc);
    void changeName(QJsonDocument& doc);
    void nameIsChanged(QJsonDocument& doc);
    void getUsers(QJsonDocument& doc);
    void getMessage(QJsonDocument& doc);
    void sendMessage(QJsonDocument& doc);
    void hideMessageBar();
    void showMessageBar();
private:
    bool dialog = false;
    QString name, whoRead, id;
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QMap<QString, QString> jsonRules;
    QMap<QString, std::function<void(QJsonDocument&)>> logicMap;
};
#endif // MAINWINDOW_H
