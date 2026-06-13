#include "userdashboard.h"
#include "ui_userdashboard.h"
#include "searchdialog.h"
#include "borrowdialog.h"
#include "returndialog.h"
#include "reservationdialog.h"
#include "membershipdialog.h"
#include <QMessageBox>
#include <QInputDialog>

UserDashboard::UserDashboard(User* user, Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserDashboard)
    , currentUser(user)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("User Dashboard - " + user->get_name());
    updateUserInfo();
}

UserDashboard::~UserDashboard()
{
    delete ui;
}

void UserDashboard::updateUserInfo()
{
    ui->lblWelcome->setText("Welcome, " + currentUser->get_name());
    ui->lblUserType->setText("Type: " + currentUser->get_usertype());
    ui->lblBalance->setText("Balance: Rs. " + QString::number(currentUser->getBalance(), 'f', 2));
    ui->lblBorrowed->setText("Borrowed: " + QString::number(currentUser->getBorrowedCount()) +
                              " / " + QString::number(currentUser->getEffectiveBorrowLimit()));
    ui->lblMembership->setText("Membership: " + QString(currentUser->hasMembership() ? "ACTIVE" : "NONE"));
}

void UserDashboard::refreshData()
{
    updateUserInfo();
}

void UserDashboard::on_btnViewProfile_clicked()
{
    QString info = "Name: " + currentUser->get_name() + "\n" +
                   "ID: " + QString::number(currentUser->get_userid()) + "\n" +
                   "Type: " + currentUser->get_usertype() + "\n" +
                   "Email: " + currentUser->get_email() + "\n" +
                   "Phone: " + currentUser->get_phone() + "\n" +
                   "CNIC: " + currentUser->get_cnic() + "\n" +
                   "Username: " + currentUser->getUsername() + "\n" +
                   "Balance: Rs. " + QString::number(currentUser->getBalance(), 'f', 2) + "\n" +
                   "Borrow Limit: " + QString::number(currentUser->getEffectiveBorrowLimit()) + "\n" +
                   "Membership: " + QString(currentUser->hasMembership() ? "ACTIVE (15% discount)" : "None");

    if (currentUser->get_usertype() == "STUDENT") {
        Student* s = dynamic_cast<Student*>(currentUser);
        info += "\nSeat No: " + s->getSeatNo() + "\nMajor: " + s->getMajor();
    } else if (currentUser->get_usertype() == "FACULTY") {
        Faculty* f = dynamic_cast<Faculty*>(currentUser);
        info += "\nDepartment: " + f->getDepartment() + "\nDesignation: " + f->getDesignation();
    } else {
        Guest* g = dynamic_cast<Guest*>(currentUser);
        info += "\nPurpose: " + g->getPurpose();
    }

    QMessageBox::information(this, "User Profile", info);
}

void UserDashboard::on_btnSearchBooks_clicked()
{
    SearchDialog dlg(lib, this);
    dlg.exec();
}

void UserDashboard::on_btnBorrowBook_clicked()
{
    BorrowDialog dlg(currentUser, lib, this);
    if (dlg.exec() == QDialog::Accepted) {
        refreshData();
    }
}

void UserDashboard::on_btnReturnBook_clicked()
{
    ReturnDialog dlg(currentUser, lib, this);
    if (dlg.exec() == QDialog::Accepted) {
        refreshData();
    }
}

void UserDashboard::on_btnReservations_clicked()
{
    ReservationDialog dlg(currentUser, lib, this);
    dlg.exec();
}

void UserDashboard::on_btnMembership_clicked()
{
    MembershipDialog dlg(currentUser, lib, this);
    if (dlg.exec() == QDialog::Accepted) {
        refreshData();
    }
}

void UserDashboard::on_btnUpdateProfile_clicked()
{
    bool ok;
    QStringList options;
    options << "Update Phone" << "Update Password" << "Add Balance";
    QString choice = QInputDialog::getItem(this, "Update Profile", "Select option:", options, 0, false, &ok);
    if (!ok) return;

    if (choice == "Update Phone") {
        QString newPhone = QInputDialog::getText(this, "Update Phone", "Enter new phone:", QLineEdit::Normal, currentUser->get_phone(), &ok);
        if (ok && !newPhone.isEmpty()) {
            currentUser->setphone_no(newPhone);
            lib->saveAllData();
            QMessageBox::information(this, "Success", "Phone updated!");
            refreshData();
        }
    } else if (choice == "Update Password") {
        QString newPass = QInputDialog::getText(this, "Update Password", "Enter new password (min 6 chars):", QLineEdit::Password, "", &ok);
        if (ok && newPass.length() >= 6) {
            currentUser->setPassword(newPass);
            lib->saveAllData();
            QMessageBox::information(this, "Success", "Password updated!");
        }
    } else if (choice == "Add Balance") {
        double amount = QInputDialog::getDouble(this, "Add Balance", "Enter amount (min Rs. 10):", 10, 10, 999999, 2, &ok);
        if (ok) {
            currentUser->addBalance(amount);
            lib->saveAllData();
            QMessageBox::information(this, "Success", "Rs. " + QString::number(amount, 'f', 2) + " added!");
            refreshData();
        }
    }
}

void UserDashboard::on_btnLogout_clicked()
{
    lib->saveAllData();
    accept();
}
