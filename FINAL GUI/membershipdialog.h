#ifndef MEMBERSHIPDIALOG_H
#define MEMBERSHIPDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MembershipDialog; }
QT_END_NAMESPACE

class MembershipDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MembershipDialog(User* user, Library* lib, QWidget *parent = nullptr);
    ~MembershipDialog();

private slots:
    void on_btnViewStatus_clicked();
    void on_btnPurchase_clicked();
    void on_btnCancel_clicked();
    void on_btnClose_clicked();

private:
    Ui::MembershipDialog *ui;
    User* currentUser;
    Library* lib;
    void updateStatusDisplay();
};

#endif // MEMBERSHIPDIALOG_H
