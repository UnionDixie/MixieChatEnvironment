#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow),
                                          client(new Client(this))
{
    ui->setupUi(this);
    ui->statusbar->showMessage("Starting...");

    connect(client, &Client::showDialog, this, &MainWindow::showDialog);
    connect(client, &Client::requestName, this, &MainWindow::requestName);
    connect(client, &Client::showUsers, this, &MainWindow::showUsers);
    connect(client, &Client::showMessage, this, &MainWindow::showMessage);
    
    setWindowTitle("MixieChat");
    QRect screenGeometry = QApplication::desktop()->screenGeometry();
    int x = (screenGeometry.width() - width()) / 2;
    int y = (screenGeometry.height() - height()) / 2;
    move(x, y);
    setWindowIcon(QIcon(":/img/Data/favicon.png"));

    ui->inputTextEdit->setFocusPolicy(Qt::StrongFocus);
    ui->historyMessageEdit->setFocusPolicy(Qt::NoFocus);
    ui->historyMessageEdit->setReadOnly(true);

    ui->userListWidget->setFocusPolicy(Qt::NoFocus);
    ui->userListWidget->addItem("Test(FAKE) User!");

    QPixmap sendButtonIcon(":/img/Data/send button.png");
    QIcon ButtonIcon(sendButtonIcon);
    ui->sendButton->setIcon(ButtonIcon);
    ui->sendButton->setIconSize(sendButtonIcon.rect().size());

    auto returnKey = new QShortcut(Qt::Key_Return, this);//Key_enter
    connect(returnKey,&QShortcut::activated,this,&MainWindow::on_sendButton_clicked);

    ui->statusbar->showMessage("Load ok");

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::showDialog(const QString& mess)
{
    ui->historyMessageEdit->clear();
    ui->historyMessageEdit->append(mess);
    ui->statusbar->showMessage("Ok getDiaglog...");
}

void MainWindow::requestName()
{
    QString name;
    bool setName = false;
    while (!setName) {
        name = QInputDialog::getText(this, tr("Please Enter Name"),
               tr("User name:"), QLineEdit::Normal,
               QDir::home().dirName(), &setName);
    }
    ui->name_label->setText(name);
    client->getNameFromUI(name);
}

void MainWindow::showUsers(const QStringList& users)
{
    ui->userListWidget->clear();
    for (const auto& user : users) {
        ui->userListWidget->addItem(user);
    }
}

void MainWindow::showMessage(const QString& message)
{
   ui->historyMessageEdit->append(message);
   ui->statusbar->showMessage("Ok get message...");
}


void MainWindow::showMessageBar()
{
    ui->whoseChatting->show();
    ui->sendButton->show();
    ui->inputTextEdit->show();
    ui->inputTextEdit->clear();
    ui->historyMessageEdit->show();
    ui->historyMessageEdit->clear();
}

void MainWindow::on_sendButton_clicked()
{
    auto text = ui->inputTextEdit->text();
    ui->inputTextEdit->clear();
    ui->historyMessageEdit->append(text);
    client->getTextFromUI(text);
}


void MainWindow::on_userListWidget_itemClicked(QListWidgetItem *item)
{
    showMessageBar();
    ui->whoseChatting->setText(item->text());
    client->getReciever(item->text());
}

