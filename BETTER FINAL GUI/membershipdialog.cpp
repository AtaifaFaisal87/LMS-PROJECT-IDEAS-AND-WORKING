#include "membershipdialog.h"
#include "ui_membershipdialog.h"
#include <QMessageBox>

MembershipDialog::MembershipDialog(User* user, Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::MembershipDialog)
    , currentUser(user)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("Membership");
    updateStatusDisplay();
}

MembershipDialog::~MembershipDialog()
{
    delete ui;
}

void MembershipDialog::updateStatusDisplay()
{
    QString status;
    if (currentUser->hasMembership()) {
        Membership* m = currentUser->getMembership();
        status = "Status: ACTIVE\n\n";
        status += "Member Name: " + m->getUserName() + "\n";
        status += "User Type: " + m->getUserType() + "\n";
        status += "Fine Discount: 15%\n";
        status += "Borrow Boost: +" + QString::number(m->getBorrowBoost()) + " books\n";
        status += "Activated: " + m->getActivationTime().toString("yyyy-MM-dd hh:mm") + "\n";
        status += "Expires: " + m->getExpiryTime().toString("yyyy-MM-dd hh:mm");
    } else {
        status = "Status: NONE\n\n";
        status += "You do not have an active membership.\n\n";
        status += "Benefits for " + currentUser->get_usertype() + ":\n";
        status += "- 15% discount on all fines\n";
        if (currentUser->get_usertype() == "STUDENT")
            status += "- Borrow limit: 3 -> 5 books\n";
        else if (currentUser->get_usertype() == "FACULTY")
            status += "- Borrow limit: 5 -> 7 books\n";
        else
            status += "- Borrow limit: 1 -> 2 books\n";
        status += "- Duration: 1 year";
    }
    ui->txtStatus->setPlainText(status);
}

void MembershipDialog::on_btnViewStatus_clicked()
{
    updateStatusDisplay();
}

void MembershipDialog::on_btnPurchase_clicked()
{
    if (currentUser->hasMembership()) {
        QMessageBox::information(this, "Info", "You already have an active membership!");
        return;
    }

    int reply = QMessageBox::question(this, "Confirm Purchase",
        "Purchase 1-year membership?\n\nBenefits:\n- 15% fine discount\n- Increased borrow limit\n- Duration: 365 days",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        currentUser->activateMembership(365);
        lib->saveAllData();
        QMessageBox::information(this, "Success", "Membership activated successfully!");
        updateStatusDisplay();
        accept();
    }
}

void MembershipDialog::on_btnCancel_clicked()
{
    if (!currentUser->hasMembership()) {
        QMessageBox::warning(this, "Error", "You do not have an active membership!");
        return;
    }

    int reply = QMessageBox::question(this, "Confirm",
        "Are you sure you want to cancel your membership?",
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        currentUser->cancelMembership();
        lib->saveAllData();
        QMessageBox::information(this, "Success", "Membership cancelled.");
        updateStatusDisplay();
    }
}

void MembershipDialog::on_btnClose_clicked()
{
    accept();
}
