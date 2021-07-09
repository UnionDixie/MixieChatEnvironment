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


    socket->connectToHost("127.0.0.1",2345);




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
        qDebug() << "client get data" << data;
        doc = QJsonDocument::fromJson(data,&docErr);

        if(docErr.errorString().toInt() == QJsonParseError::NoError){
           if(doc.object().value("type").toString() == "connect" && doc.object().value("status").toString() == "YES"){
                ui->textEdit->append("Соедение установлено");

                if(socket->isOpen()){
                    socket->write( "{\"type\":\"select\",\"params\":\"users\" }");
                    socket->waitForBytesWritten(500);
                }else{
                    ui->statusbar->showMessage("Error parse users");
                }


           }else if (doc.object().value("type").toString() == "resSelect"){
               QStandardItemModel* model = new QStandardItemModel(nullptr);
               model->setHorizontalHeaderLabels(QStringList() << "name");
               QJsonArray docA = doc.object().value("result").toArray();
               for(const auto& it : docA){
                   QStandardItem* col = new QStandardItem(it.toObject().value("name").toString());
                   model->appendRow(col);
               }
               ui->tableView->setModel(model);
           }else{
                ui->textEdit->append("Соедение2 не установлено");
           }
        }else{
             ui->textEdit->append("Соедение1 не установлено");
        }

        //qDebug() << data;
    }

}

void MainWindow::sockDisc(){
    socket->deleteLater();
}

void MainWindow::on_pushButton_clicked()
{

}


void MainWindow::on_pushButton_3_clicked()
{

}

