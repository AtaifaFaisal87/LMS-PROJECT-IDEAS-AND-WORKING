#ifndef RETURNDIALOG_H
#define RETURNDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReturnDialog; }
QT_END_NAMESPACE

class ReturnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReturnDialog(User* user, Library* lib, QWidget *parent = nullptr);
    ~ReturnDialog();

private slots:
    void on_btnReturn_clicked();
    void on_btnCancel_clicked();
    void refreshBorrowedList();

private:
    Ui::ReturnDialog *ui;
    User* currentUser;
    Library* lib;
    QVector<int> activeBookIDs;
};

#endif // RETURNDIALOG_H
