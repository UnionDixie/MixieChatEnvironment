#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),
                                          client(new Client(this))
{
    qDebug() << "Client born";
    ui->setupUi(this);


    connect(client, &Client::showDialogOnUI, this, &MainWindow::showDialogOnUI);
    connect(client, &Client::reqNameFromUI, this, &MainWindow::reqName);
    connect(client, &Client::showUsersOnUI, this, &MainWindow::showUsersOnUI);
    connect(client, &Client::showMessage, this, &MainWindow::showMessage);
    
    // connect the connect button to a slot that will attempt the connection
    //connect(ui->connectButton, &QPushButton::clicked, this, &ChatWindow::attemptConnection);
    // connect the click of the "send" button and the press of the enter while typing to the slot that sends the message
    //connect(ui->sendButton, &QPushButton::clicked, this, &ChatWindow::sendMessage);
    //connect(ui->messageEdit, &QLineEdit::returnPressed, this, &ChatWindow::sendMessage);

    setWindowTitle("MixieChat");
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    setWindowIcon(QIcon(":/img/Data/favicon.png"));

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

    //auto enter = new QShortcut(Qt::Key_Return, this);//Key_enter
    //connect(enter,&QShortcut::activated,this,&MainWindow::sendMessage);

    //auto reload = new QShortcut(Qt::Key_F5, this);//Key_enter
    //connect(reload,&QShortcut::activated,this,&MainWindow::getUsers);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDialogOnUI(const QString& mess)
{
    ui->textEdit->clear();
    ui->textEdit->append(mess);
    ui->statusbar->showMessage("Ok getDiaglog...");
}

void MainWindow::reqName()
{
    QString name;
    bool ok = false;
    while (!ok) {
        name = QInputDialog::getText(this, tr("Please Enter Name"),
            tr("User name:"), QLineEdit::Normal,
            QDir::home().dirName(), &ok);
        ui->label_3->setText(name);
    }
    client->getNameFromUI(name);
}

void MainWindow::showUsersOnUI(const QStringList& list)
{
    ui->listWidget->clear();
    for (const auto& it : list) {
        ui->listWidget->addItem(it);
    }
}

void MainWindow::showMessage(const QString& mess)
{
   ui->textEdit->append(mess);
   ui->statusbar->showMessage("Ok get...");
}

void MainWindow::on_pushButton_clicked()
{
    ui->textEdit->append(ui->lineEdit->text());
    client->getTextFromUI(ui->lineEdit->text());
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
    client->getReciever(item->text());
    showMessageBar();
}

