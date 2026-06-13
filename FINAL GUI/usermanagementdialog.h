#ifndef USERMANAGEMENTDIALOG_H
#define USERMANAGEMENTDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class UserManagementDialog; }
QT_END_NAMESPACE

class UserManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit UserManagementDialog(Library* lib, QWidget *parent = nullptr);
    ~UserManagementDialog();

private slots:
    void on_btnViewUsers_clicked();
    void on_btnDeleteUser_clicked();
    void on_btnRefresh_clicked();
    void on_btnClose_clicked();

private:
    Ui::UserManagementDialog *ui;
    Library* lib;
    void refreshUserList();
};

#endif // USERMANAGEMENTDIALOG_H
