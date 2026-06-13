#ifndef LIBRARIANDASHBOARD_H
#define LIBRARIANDASHBOARD_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class LibrarianDashboard; }
QT_END_NAMESPACE

class LibrarianDashboard : public QDialog
{
    Q_OBJECT

public:
    explicit LibrarianDashboard(librarian* lib_admin, Library* lib, QWidget *parent = nullptr);
    ~LibrarianDashboard();

private slots:
    void on_btnManageBooks_clicked();
    void on_btnManageTables_clicked();
    void on_btnManageUsers_clicked();
    void on_btnViewAll_clicked();
    void on_btnLogout_clicked();

private:
    Ui::LibrarianDashboard *ui;
    librarian* currentLibrarian;
    Library* lib;
};

#endif // LIBRARIANDASHBOARD_H
