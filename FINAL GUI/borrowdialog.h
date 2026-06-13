#ifndef BORROWDIALOG_H
#define BORROWDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BorrowDialog; }
QT_END_NAMESPACE

class BorrowDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BorrowDialog(User* user, Library* lib, QWidget *parent = nullptr);
    ~BorrowDialog();

private slots:
    void on_btnBorrow_clicked();
    void on_btnCancel_clicked();
    void refreshBookList();

private:
    Ui::BorrowDialog *ui;
    User* currentUser;
    Library* lib;
};

#endif // BORROWDIALOG_H
