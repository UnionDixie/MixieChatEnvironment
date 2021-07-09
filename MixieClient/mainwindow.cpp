#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QInputDialog>
#include <QDir>


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

    setWindowTitle("MixieChat");
    move(900 / 2, 300 / 2);
    //setWindowIcon(QIcon(QString::fromUtf8(":/img/favicon.png")));

    ui->lineEdit->setFocusPolicy(Qt::StrongFocus);
    ui->textEdit->setFocusPolicy(Qt::NoFocus);
    ui->textEdit->setReadOnly(true);

    ui->listWidget->setFocusPolicy(Qt::NoFocus);
    ui->listWidget->addItem("You!");

    ui->statusbar->showMessage("Ok");
}

MainWindow::~MainWindow()
{
    delete ui;
}


//refactoring
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
               bool ok;
               QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                                     tr("User name:"), QLineEdit::Normal,
                                                     QDir::home().dirName(), &ok);
                qDebug() << text;
                if(socket->isOpen()){
                    socket->write( "{\"type\":\"getUsers\"}");
                    socket->waitForBytesWritten(500);
                }else{
                    ui->statusbar->showMessage("Error parse users");
                }
           }else if (doc.object().value("type").toString() == "resSelect"){
               QJsonArray docA = doc.object().value("result").toArray();
               for(const auto& it : docA){
                  ui->listWidget->addItem(it.toObject().value("name").toString());
               }
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
    }

}

void MainWindow::sockDisc(){
    socket->deleteLater();
}

void MainWindow::on_pushButton_2_clicked()
{
    QString receiver = "All";
    if(auto it = ui->listWidget->currentItem(); it != nullptr){
        receiver = it->text();
        ui->listWidget->clearSelection();
    }
    auto text = ui->lineEdit->text();
    auto mess =
         QString("{\"type\":\"message\",\"sender\":\"%1\",\"receiver\":\"%2\",\"message\":\"%3\" }").arg(name,receiver,text);
    socket->write(mess.toStdString().c_str());
    ui->lineEdit->clear();
    ui->statusbar->showMessage("Ok sent...");
}
