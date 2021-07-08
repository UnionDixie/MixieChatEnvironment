#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
     qDebug() << "client born";
    ui->setupUi(this);

    socket = new QTcpSocket(this);

    connect(socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sockReady(){
    if(socket->waitForConnected(500)){
        socket->waitForReadyRead(500);
        qDebug() << "client try read data";
        data = socket->readAll();
        qDebug() << data;
    }

}

void MainWindow::sockDisc(){
    socket->deleteLater();
}

void MainWindow::on_pushButton_clicked()
{
    socket->connectToHost("127.0.0.1",2345);
}

