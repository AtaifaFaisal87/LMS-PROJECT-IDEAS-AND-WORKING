#include "librariandashboard.h"
#include "ui_librariandashboard.h"
#include "bookmanagementdialog.h"
#include "tablemanagementdialog.h"
#include "usermanagementdialog.h"
#include <QMessageBox>

LibrarianDashboard::LibrarianDashboard(librarian* admin, Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::LibrarianDashboard)
    , currentLibrarian(admin)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("Librarian Dashboard - " + admin->getName());
    ui->lblWelcome->setText("Welcome, " + admin->getName());
    ui->lblInfo->setText("Email: " + admin->getEmail() + " | Phone: " + admin->getPhone());
}

LibrarianDashboard::~LibrarianDashboard()
{
    delete ui;
}

void LibrarianDashboard::on_btnManageBooks_clicked()
{
    BookManagementDialog dlg(lib, this);
    dlg.exec();
}

void LibrarianDashboard::on_btnManageTables_clicked()
{
    TableManagementDialog dlg(lib, this);
    dlg.exec();
}

void LibrarianDashboard::on_btnManageUsers_clicked()
{
    UserManagementDialog dlg(lib, this);
    dlg.exec();
}

void LibrarianDashboard::on_btnViewAll_clicked()
{
    QString info = "=== LIBRARY STATISTICS ===\n\n";
    info += "Total Users: " + QString::number(lib->getUserCount()) + "\n";
    info += "Active Users: " + QString::number(lib->getActiveUserCount()) + "\n";
    info += "Total Books: " + QString::number(lib->getCatalog()->get_bookcount()) + "\n";
    info += "Total Tables: " + QString::number(lib->getTableCount()) + " / " + QString::number(Library::MAX_TABLES) + "\n";
    info += "Total Transactions: " + QString::number(lib->getTransactionsVector().size()) + "\n";
    info += "Total Fines: " + QString::number(lib->getFinesVector().size()) + "\n";
    info += "Total Reservations: " + QString::number(lib->getReservationsVector().size());

    QMessageBox::information(this, "Library Statistics", info);
}

void LibrarianDashboard::on_btnLogout_clicked()
{
    lib->saveAllData();
    QMessageBox::information(this, "Logout", "Logged out successfully!");
    accept();
}
