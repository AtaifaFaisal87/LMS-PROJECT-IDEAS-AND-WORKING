#ifndef BOOKMANAGEMENTDIALOG_H
#define BOOKMANAGEMENTDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class BookManagementDialog; }
QT_END_NAMESPACE

class BookManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BookManagementDialog(Library* lib, QWidget *parent = nullptr);
    ~BookManagementDialog();

private slots:
    void on_btnAddBook_clicked();
    void on_btnRemoveBook_clicked();
    void on_btnRefresh_clicked();
    void on_btnClose_clicked();

private:
    Ui::BookManagementDialog *ui;
    Library* lib;
    void refreshBookList();
};

#endif // BOOKMANAGEMENTDIALOG_H
