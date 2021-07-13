#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtDebug>
#include <QInputDialog>
#include <QDir>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
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
    ui->listWidget->addItem("Test User!");

    ui->statusbar->showMessage("Ok");

    QPixmap pixmap(":/img/Data/send button.png");
    QIcon ButtonIcon(pixmap);
    ui->pushButton->setIcon(ButtonIcon);
    ui->pushButton->setIconSize(pixmap.rect().size());

    jsonRules = {
       {"changeName","{\"type\":\"changeName\",\"name\":\"%1\",\"newName\":\"%2\"}"},
       {"getUsers","{\"type\":\"getUsers\"}"},
       {"from","From %1 "},
       {"message","{\"type\":\"message\",\"sender\":\"%1\",\"receiver\":\"%2\",\"message\":\"%3\" }"},
       {"getDialog","{\"type\":\"getDialog\",\"sender\":\"%1\",\"receiver\":\"%2\" }"}
    };

    logicMap = {
        {"connect",[=](QJsonDocument& doc) { isConnect(doc); } },
        {"nameChanged",[=](QJsonDocument& doc) { nameIsChanged(doc); } },
        {"resSelect",[=](QJsonDocument& doc) { getUsers(doc); } },
        {"newMessages",[=](QJsonDocument& doc) { getMessage(doc); } },
        // {"dialog","{\"type\":\"dialog\",\"result\": %1 }"},
        {"dialog",[=](QJsonDocument& doc) { getDialog(doc); } },
    };


}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "src/logger.h"

//refactoring this pls!!!
void MainWindow::sockReady(){
    if(socket->waitForConnected(500)){
        socket->waitForReadyRead(500);
        qDebug() << "Client try read data";
        auto data = socket->readAll();
        qDebug() << "client get data" << data;
        QJsonParseError docErr;
        auto doc = QJsonDocument::fromJson(data,&docErr);
        if(docErr.errorString().toInt() == QJsonParseError::NoError){
           if(auto it = logicMap[doc.object().value("type").toString()];it!=nullptr){
                it(doc);
           }else{
                ui->textEdit->append("Err parse json");
                logErr(data);
           }
        }else{
             ui->textEdit->append("Err read json");
             logErr(data);
        }
    }

}

void MainWindow::sockDisc(){
    socket->deleteLater();
}



void MainWindow::on_pushButton_clicked()
{
    auto tmp = QJsonDocument();
    sendMessage(tmp);
}



void MainWindow::isConnect(QJsonDocument& doc)
{
    id = doc.object().value("name").toString();;
    ui->textEdit->append("Соедение установлено");
    ui->textEdit->append(id);
    bool ok = false;
    while(!ok) {
        name = QInputDialog::getText(this, tr("Please Enter Name"),
               tr("User name:"), QLineEdit::Normal,
               QDir::home().dirName(), &ok);
        ui->label_3->setText(name);
    }
    changeName(doc);
}

void MainWindow::changeName(QJsonDocument& doc)
{
    auto packetForChangeName = jsonRules["changeName"].arg(id, name);
    if (socket->isOpen()) {
        socket->write(packetForChangeName.toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
    else {
        ui->statusbar->showMessage("Error socket close");
    }
}

void MainWindow::nameIsChanged(QJsonDocument& doc)
{
    if (socket->isOpen()) {
        socket->write(jsonRules["getUsers"].toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
    else {
        ui->statusbar->showMessage("Error parse users");
    }
}

void MainWindow::getUsers(QJsonDocument& doc)
{
    ui->listWidget->clear();
    QJsonArray docA = doc.object().value("result").toArray();
    for (const auto& it : docA) {
        ui->listWidget->addItem(it.toObject().value("name").toString());
    }
}

void MainWindow::getMessage(QJsonDocument& doc)
{
    auto from = doc.object().value("from").toString();
    if(!dialog){
       whoRead = from;
       ui->label_5->setText(whoRead);
       showMessageBar();
       dialog = true;
    }
    auto mess = doc.object().value("message").toString();
    ui->textEdit->append(jsonRules["from"].arg(from) + mess);
}

void MainWindow::sendMessage(QJsonDocument& doc)
{
    QString receiver = whoRead;;
    if(receiver != name){
        auto text = ui->lineEdit->text();
        ui->textEdit->append(text);
        auto mess = jsonRules["message"].arg(name, receiver, text);
        socket->write(mess.toStdString().c_str());
        ui->lineEdit->clear();
        ui->statusbar->showMessage("Ok sent...");
    }else{
        ui->statusbar->showMessage("Block send yourself...");
    }
}


void MainWindow::getDialog(QJsonDocument &doc)
{
    ui->textEdit->clear();
    QJsonArray docA = doc.object().value("result").toArray();
    for (const auto& it : docA) {
        auto mess = it.toObject().value("mess").toString();
        if(!mess.isEmpty()){
            if(mess[0] != '@'){
                 ui->textEdit->append("From " + whoRead + " - " + mess);
            }else{
                mess.remove(0,1);
                ui->textEdit->append(mess);
            }
        }
    }
    ui->statusbar->showMessage("Ok getDiaglog...");
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
     whoRead = item->text();
     if(whoRead!=name){
        ui->label_5->setText(whoRead);
        showMessageBar();
        socket->write(jsonRules["getDialog"].arg(name,whoRead).toStdString().c_str());
        socket->waitForBytesWritten(500);
     }
}



void MainWindow::hideMessageBar()
{
    ui->pushButton->hide();
    ui->lineEdit->hide();
    ui->textEdit->hide();
    ui->label_5->hide();
}
void MainWindow::showMessageBar()
{
    ui->label_5->show();
    ui->pushButton->show();
    ui->lineEdit->show();
    ui->lineEdit->clear();
    ui->textEdit->show();
    ui->textEdit->clear();
}
