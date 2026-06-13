#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "registerdialog.h"
#include "logindialog.h"
#include "userdashboard.h"
#include "librariandashboard.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    lib = new Library();
    lib->loadAllData();
    admin = new librarian(1, "Admin", "admin@lib.com", "03000000000");
    admin->setCredentials("admin", "123");
    auth = new Authentication(admin);
}

MainWindow::~MainWindow()
{
    lib->saveAllData();
    delete admin;
    delete auth;
    delete lib;
    delete ui;
}

void MainWindow::on_btnRegister_clicked()
{
    RegisterDialog dlg(lib, this);
    dlg.exec();
}

void MainWindow::on_btnUserLogin_clicked()
{
    LoginDialog dlg(lib, auth, false, this);
    if (dlg.exec() == QDialog::Accepted) {
        User* u = dlg.getLoggedInUser();
        if (u) {
            UserDashboard dashboard(u, lib, this);
            dashboard.exec();
        }
    }
}

void MainWindow::on_btnLibrarianLogin_clicked()
{
    LoginDialog dlg(lib, auth, true, this);
    if (dlg.exec() == QDialog::Accepted) {
        librarian* l = dlg.getLoggedInLibrarian();
        if (l) {
            LibrarianDashboard dashboard(l, lib, this);
            dashboard.exec();
        }
    }
}

void MainWindow::on_btnExit_clicked()
{
    lib->saveAllData();
    close();
}
