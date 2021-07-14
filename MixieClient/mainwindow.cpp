#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    qDebug() << "client born";
    ui->setupUi(this);

    socket = new QTcpSocket(this);
    connect(socket,SIGNAL(readyRead()),this,SLOT(sockReady()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(sockDisc()));
    socket->connectToHost("127.0.0.1",2345);

    setWindowTitle("MixieChat");
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
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
        {"connect",[=]() { isConnect(); } },
        {"nameChanged",[=]() { nameIsChanged(); } },
        {"resSelect",[=]() { getUsers(); } },
        {"newMessages",[=]() { getMessage(); } },
        // {"dialog","{\"type\":\"dialog\",\"result\": %1 }"},
        {"dialog",[=]() { getDialog(); } },
    };


}

MainWindow::~MainWindow()
{
    delete ui;
}

#include "src/logger.h"

void MainWindow::sockReady(){
    qDebug() << "Client try read data";
    auto data = socket->readAll();
    qDebug() << "client get data" << data;
    QJsonParseError docErr;
    doc = QJsonDocument::fromJson(data,&docErr);
    if(docErr.errorString().toInt() == QJsonParseError::NoError){
       if(auto it = logicMap[doc.object().value("type").toString()];it!=nullptr){
            it();
       }else{
            ui->textEdit->append("Err parse json");
            logErr(data);
       }
    }else{
         ui->textEdit->append("Err read json");
         logErr(data);
    }
}

void MainWindow::sockDisc(){
    socket->deleteLater();
}



void MainWindow::on_pushButton_clicked()
{
    sendMessage();
}

void MainWindow::isConnect()
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
    changeName();
}

void MainWindow::changeName()
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

void MainWindow::nameIsChanged()
{
    if (socket->isOpen()) {
        socket->write(jsonRules["getUsers"].toStdString().c_str());
        socket->waitForBytesWritten(500);
    }
    else {
        ui->statusbar->showMessage("Error parse users");
    }
}

void MainWindow::getUsers()
{
    ui->listWidget->clear();
    QJsonArray docA = doc.object().value("result").toArray();
    for (const auto& it : docA) {
        //if(name != )
        ui->listWidget->addItem(it.toObject().value("name").toString());
    }
}

void MainWindow::getMessage()
{
    auto from = doc.object().value("from").toString();
    if(!dialog){
       whoRead = from;
       ui->label_5->setText(whoRead);
       //showMessageBar();
       dialog = true;
    }
    auto mess = doc.object().value("message").toString();
    ui->textEdit->append(jsonRules["from"].arg(from) + mess);
}

void MainWindow::sendMessage()
{
    QString receiver = whoRead;
    if(receiver != name){
        auto text = ui->lineEdit->text();
        if(!text.isEmpty()){
            ui->textEdit->append(text);
            auto mess = jsonRules["message"].arg(name, receiver, text);
            socket->write(mess.toStdString().c_str());
            ui->lineEdit->clear();
            ui->statusbar->showMessage("Ok sent...");
        }
    }else{
        ui->statusbar->showMessage("Block send yourself...");
    }
}


void MainWindow::getDialog()
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

void MainWindow::showMessageBar()
{
    ui->label_5->show();
    ui->pushButton->show();
    ui->lineEdit->show();
    ui->lineEdit->clear();
    ui->textEdit->show();
    ui->textEdit->clear();
}

void MainWindow::on_listWidget_itemClicked(QListWidgetItem *item)
{
    whoRead = item->text();
    if(whoRead!=name){
       ui->label_5->setText(whoRead);
       showMessageBar();
       socket->write(jsonRules["getDialog"].arg(name,whoRead).toStdString().c_str());
       socket->waitForBytesWritten(500);
    }
}

