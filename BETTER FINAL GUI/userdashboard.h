#ifndef USERDASHBOARD_H
#define USERDASHBOARD_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UserDashboard; }
QT_END_NAMESPACE

class UserDashboard : public QDialog
{
    Q_OBJECT

public:
    explicit UserDashboard(User* user, Library* lib, QWidget *parent = nullptr);
    ~UserDashboard();

private slots:
    void on_btnViewProfile_clicked();
    void on_btnSearchBooks_clicked();
    void on_btnBorrowBook_clicked();
    void on_btnReturnBook_clicked();
    void on_btnReservations_clicked();
    void on_btnMembership_clicked();
    void on_btnUpdateProfile_clicked();
    void on_btnLogout_clicked();
    void refreshData();

private:
    Ui::UserDashboard *ui;
    User* currentUser;
    Library* lib;
    void updateUserInfo();
};

#endif // USERDASHBOARD_H
