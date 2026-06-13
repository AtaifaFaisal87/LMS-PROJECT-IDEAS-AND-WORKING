#ifndef TABLEMANAGEMENTDIALOG_H
#define TABLEMANAGEMENTDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TableManagementDialog; }
QT_END_NAMESPACE

class TableManagementDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TableManagementDialog(Library* lib, QWidget *parent = nullptr);
    ~TableManagementDialog();

private slots:
    void on_btnAddTable_clicked();
    void on_btnRemoveTable_clicked();
    void on_btnRefresh_clicked();
    void on_btnClose_clicked();

private:
    Ui::TableManagementDialog *ui;
    Library* lib;
    void refreshTableList();
};

#endif // TABLEMANAGEMENTDIALOG_H
