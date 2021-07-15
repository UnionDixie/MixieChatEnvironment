#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>

#include "src/client.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void showDialog(const QString& mess);
    void requestName();
    void showUsers(const QStringList& list);
    void showMessage(const QString& mess);
private slots:
    void on_sendButton_clicked();
    void on_userListWidget_itemClicked(QListWidgetItem *item);
private:
    void showMessageBar();
    Client* client;
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
