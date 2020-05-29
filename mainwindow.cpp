#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButtonLogin_clicked()
{
    QString loginString = ui->lineEditLogin->text();
    QString passwordString = ui->lineEditPassword->text();

    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC","connect");
    db.setDatabaseName("Driver={MySQL ODBC 8.0 ANSI Driver};DATABASE=tsod");
    //db.setUserName(loginString);
    //db.setPassword(passwordString);
    db.setUserName("Admin");
    db.setPassword("adminPas1");


    if (db.open())
    {
        ui->statusbar->showMessage("OK");
        hide();
        mapWindow = new MapWindow(this, &db);
        mapWindow->show();

    }
    else
    {
        ui->statusbar->showMessage("Не правелный логин или пароль");
    }
}
