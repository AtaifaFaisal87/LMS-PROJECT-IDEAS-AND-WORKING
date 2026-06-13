#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class RegisterDialog; }
QT_END_NAMESPACE

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(Library* lib, QWidget *parent = nullptr);
    ~RegisterDialog();

private slots:
    void on_btnRegister_clicked();
    void on_btnCancel_clicked();
    void on_cmbType_currentIndexChanged(int index);

private:
    Ui::RegisterDialog *ui;
    Library* lib;
    bool validateInputs();
};

#endif // REGISTERDIALOG_H
