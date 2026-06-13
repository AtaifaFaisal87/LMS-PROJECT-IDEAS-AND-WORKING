#include "logindialog.h"
#include "ui_logindialog.h"
#include <QMessageBox>

LoginDialog::LoginDialog(Library* library, Authentication* authentication, bool librarianMode, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , lib(library)
    , auth(authentication)
    , isLibrarianMode(librarianMode)
    , loggedInUser(nullptr)
    , loggedInLibrarian(nullptr)
{
    ui->setupUi(this);
    if (isLibrarianMode) {
        setWindowTitle("Librarian Login");
        ui->lblTitle->setText("LIBRARIAN LOGIN");
    } else {
        setWindowTitle("User Login");
        ui->lblTitle->setText("USER LOGIN");
    }
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_btnLogin_clicked()
{
    QString username = ui->txtUsername->text().trimmed();
    QString password = ui->txtPassword->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login Error", "Please enter both username and password.");
        return;
    }

    try {
        if (isLibrarianMode) {
            loggedInLibrarian = auth->loginLibrarian(username, password);
            QMessageBox::information(this, "Success", "Librarian login successful!");
            accept();
        } else {
            loggedInUser = auth->loginUser(lib, username, password);
            QMessageBox::information(this, "Success", "Welcome " + loggedInUser->get_name() + "!");
            accept();
        }
    } catch (const AuthException& e) {
        QMessageBox::critical(this, "Login Failed", e.getMessage());
    }
}

void LoginDialog::on_btnCancel_clicked()
{
    reject();
}
