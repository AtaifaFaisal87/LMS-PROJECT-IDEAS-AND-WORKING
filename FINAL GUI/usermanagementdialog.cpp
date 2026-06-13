#include "usermanagementdialog.h"
#include "ui_usermanagementdialog.h"
#include <QMessageBox>
#include <QInputDialog>

UserManagementDialog::UserManagementDialog(Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::UserManagementDialog)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("User Management");
    refreshUserList();
}

UserManagementDialog::~UserManagementDialog()
{
    delete ui;
}

void UserManagementDialog::refreshUserList()
{
    ui->lstUsers->clear();
    for (int i = 0; i < lib->getUserCount(); i++) {
        User* u = lib->getUserByIndex(i);
        if (u) {
            QString status = u->getIsDeleted() ? " [DELETED]" : "";
            QString item = QString("ID: %1 | %2 | %3 | %4%5")
                .arg(u->get_userid())
                .arg(u->get_name())
                .arg(u->get_usertype())
                .arg(u->getUsername())
                .arg(status);
            ui->lstUsers->addItem(item);
        }
    }
    ui->lblCount->setText("Total Users: " + QString::number(lib->getUserCount()) +
                           " | Active: " + QString::number(lib->getActiveUserCount()));
}

void UserManagementDialog::on_btnViewUsers_clicked()
{
    refreshUserList();
}

void UserManagementDialog::on_btnDeleteUser_clicked()
{
    bool ok;
    int uid = QInputDialog::getInt(this, "Delete User", "Enter User ID to delete:", 1, 1, 999999, 1, &ok);
    if (!ok) return;

    User* target = nullptr;
    for (int i = 0; i < lib->getUserCount(); i++) {
        if (lib->getUserByIndex(i)->get_userid() == uid) {
            target = lib->getUserByIndex(i);
            break;
        }
    }

    if (!target) {
        QMessageBox::warning(this, "Error", "User not found!");
        return;
    }

    if (target->getIsDeleted()) {
        QMessageBox::warning(this, "Error", "User is already deleted!");
        return;
    }

    if (target->getBorrowedCount() > 0) {
        QMessageBox::warning(this, "Error", "User has unreturned books! Cannot delete.");
        return;
    }

    int reply = QMessageBox::question(this, "Confirm Delete",
        "Delete user: " + target->get_name() + " (ID: " + QString::number(uid) + ")?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        if (lib->deleteUser(uid)) {
            lib->saveAllData();
            QMessageBox::information(this, "Success", "User deleted successfully!");
            refreshUserList();
        }
    }
}

void UserManagementDialog::on_btnRefresh_clicked()
{
    refreshUserList();
}

void UserManagementDialog::on_btnClose_clicked()
{
    accept();
}
