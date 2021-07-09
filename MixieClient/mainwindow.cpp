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
            //"{\"type\":\"connect\",\"name\":\"%1\"}
           if(doc.object().value("type").toString() == "connect"){
               auto name1 = doc.object().value("name").toString();
               ui->textEdit->append("Соедение установлено");
               ui->textEdit->append(name1);
               name = name1;
                if(socket->isOpen()){
                    socket->write( "{\"type\":\"getUsers\"}");
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
           }else if (doc.object().value("type").toString() == "newMessages"){
               //messageToUserFromServerOfuser:
               //{"type":"newMessages", "from" : "i", "message" : "hello bro"}
               auto from = doc.object().value("from").toString();
               auto mess = doc.object().value("message").toString();
               ui->textEdit->append(QString("From %1 - ").arg(from) + mess);
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




void MainWindow::on_pushButton_2_clicked()
{
    auto text = ui->lineEdit->text();
    ui->lineEdit->clear();

//messageToUserFromClient:
//{"type":"message","sender":"I","receiver":"you","message":"Hello,bro" };

    auto mess = QString("{\"type\":\"message\",\"sender\":\"%1\",\"receiver\":\"you\",\"message\":\"%2\" }").arg(name,text);
    socket->write(mess.toStdString().c_str());

}

