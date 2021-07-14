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
#include <QtDebug>
#include <QInputDialog>
#include <QDir>
#include <QDesktopWidget>
#include <functional>
#include <QShortcut>

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

    void on_listWidget_itemClicked(QListWidgetItem *item);

private:
    void isConnect();
    void changeName();
    void nameIsChanged();
    void getUsers();
    void getMessage();
    void sendMessage();
    void getDialog();
    //void hideMessageBar();
    void showMessageBar();
private:
    bool dialog = false;
    QString name, whoRead, id;
    Ui::MainWindow *ui;
    QTcpSocket* socket;
    QMap<QString, QString> jsonRules;
    QMap<QString, std::function<void()>> logicMap;
    QJsonDocument doc;
};
#endif // MAINWINDOW_H
