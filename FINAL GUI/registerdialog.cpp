#include "registerdialog.h"
#include "ui_registerdialog.h"
#include <QMessageBox>
#include <QRegularExpression>

RegisterDialog::RegisterDialog(Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::RegisterDialog)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("User Registration");
    on_cmbType_currentIndexChanged(0);
}

RegisterDialog::~RegisterDialog()
{
    delete ui;
}

void RegisterDialog::on_cmbType_currentIndexChanged(int index)
{
    bool isStudent = (index == 0);
    bool isFaculty = (index == 1);
    bool isGuest = (index == 2);

    ui->lblField1->setText(isStudent ? "Seat Number:" : (isFaculty ? "Department:" : "Purpose:"));
    ui->lblField2->setText(isStudent ? "Major:" : (isFaculty ? "Designation:" : ""));
    ui->txtField2->setVisible(!isGuest);
    ui->lblField2->setVisible(!isGuest);
}

bool RegisterDialog::validateInputs()
{
    if (ui->txtName->text().isEmpty()) { QMessageBox::warning(this, "Error", "Name is required!"); return false; }
    if (ui->txtCNIC->text().isEmpty()) { QMessageBox::warning(this, "Error", "CNIC is required!"); return false; }
    if (ui->txtEmail->text().isEmpty()) { QMessageBox::warning(this, "Error", "Email is required!"); return false; }
    if (ui->txtPhone->text().isEmpty()) { QMessageBox::warning(this, "Error", "Phone is required!"); return false; }
    if (ui->txtUsername->text().isEmpty()) { QMessageBox::warning(this, "Error", "Username is required!"); return false; }
    if (ui->txtPassword->text().length() < 6) { QMessageBox::warning(this, "Error", "Password must be at least 6 characters!"); return false; }
    if (ui->txtField1->text().isEmpty()) { QMessageBox::warning(this, "Error", ui->lblField1->text() + " is required!"); return false; }
    if (ui->cmbType->currentIndex() != 2 && ui->txtField2->text().isEmpty()) {
        QMessageBox::warning(this, "Error", ui->lblField2->text() + " is required!");
        return false;
    }
    return true;
}

void RegisterDialog::on_btnRegister_clicked()
{
    if (!validateInputs()) return;

    QString name = ui->txtName->text();
    QString cnic = ui->txtCNIC->text();
    QString email = ui->txtEmail->text();
    QString phone = ui->txtPhone->text();
    QString username = ui->txtUsername->text();
    QString password = ui->txtPassword->text();
    double balance = ui->spnBalance->value();
    QString field1 = ui->txtField1->text();
    QString field2 = ui->txtField2->text();

    User* newUser = nullptr;
    try {
        int type = ui->cmbType->currentIndex();
        if (type == 0) {
            newUser = new Student(name, cnic, email, phone, field1, field2);
        } else if (type == 1) {
            newUser = new Faculty(name, cnic, email, phone, field1, field2);
        } else {
            newUser = new Guest(name, cnic, email, phone, field1);
        }
        newUser->setUsername(username);
        newUser->setPassword(password);
        newUser->setBalance(balance);

        lib->registerUserInLibrary(newUser);
        lib->saveAllData();

        QMessageBox::information(this, "Success", "User registered successfully!\\nYour ID is: " + QString::number(newUser->get_userid()));
        accept();
    } catch (const UserException& e) {
        delete newUser;
        QMessageBox::critical(this, "Registration Failed", e.getMessage());
    }
}

void RegisterDialog::on_btnCancel_clicked()
{
    reject();
}
