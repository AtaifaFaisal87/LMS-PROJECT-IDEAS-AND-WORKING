#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LoginDialog; }
QT_END_NAMESPACE

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(Library* lib, Authentication* auth, bool isLibrarian, QWidget *parent = nullptr);
    ~LoginDialog();
    User* getLoggedInUser() const { return loggedInUser; }
    librarian* getLoggedInLibrarian() const { return loggedInLibrarian; }

private slots:
    void on_btnLogin_clicked();
    void on_btnCancel_clicked();

private:
    Ui::LoginDialog *ui;
    Library* lib;
    Authentication* auth;
    bool isLibrarianMode;
    User* loggedInUser;
    librarian* loggedInLibrarian;
};

#endif // LOGINDIALOG_H
