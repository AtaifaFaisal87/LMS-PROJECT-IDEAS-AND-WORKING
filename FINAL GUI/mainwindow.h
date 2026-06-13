#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btnRegister_clicked();
    void on_btnUserLogin_clicked();
    void on_btnLibrarianLogin_clicked();
    void on_btnExit_clicked();

private:
    Ui::MainWindow *ui;
    Library* lib;
    Authentication* auth;
    librarian* admin;
};
#endif // MAINWINDOW_H
