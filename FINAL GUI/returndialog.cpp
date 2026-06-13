#include "returndialog.h"
#include "ui_returndialog.h"
#include <QMessageBox>

ReturnDialog::ReturnDialog(User* user, Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReturnDialog)
    , currentUser(user)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("Return Book");
    refreshBorrowedList();
}

ReturnDialog::~ReturnDialog()
{
    delete ui;
}

void ReturnDialog::refreshBorrowedList()
{
    ui->lstBooks->clear();
    activeBookIDs.clear();

    const QVector<int>& borrowed = currentUser->getBorrowedBooks();
    for (int bookID : borrowed) {
        Book* b = lib->getCatalog()->SearchbyID(bookID);
        if (b) {
            QString item = QString("ID: %1 | %2 by %3")
                .arg(b->get_bookid())
                .arg(b->get_title())
                .arg(b->get_author());
            ui->lstBooks->addItem(item);
            activeBookIDs.append(bookID);
        }
    }

    if (activeBookIDs.isEmpty()) {
        ui->lstBooks->addItem("No books currently borrowed.");
        ui->btnReturn->setEnabled(false);
    } else {
        ui->btnReturn->setEnabled(true);
    }
}

void ReturnDialog::on_btnReturn_clicked()
{
    int row = ui->lstBooks->currentRow();
    if (row < 0 || row >= activeBookIDs.size()) {
        QMessageBox::warning(this, "Error", "Please select a book to return.");
        return;
    }

    Book* b = lib->getCatalog()->SearchbyID(activeBookIDs[row]);
    if (!b) return;

    try {
        lib->returnBook(currentUser, b->get_title());
        lib->saveAllData();

        QString msg = "Book returned successfully!\nTitle: " + b->get_title();
        QMessageBox::information(this, "Success", msg);
        accept();
    } catch (const LibraryException& e) {
        QMessageBox::critical(this, "Error", e.getMessage());
    }
}

void ReturnDialog::on_btnCancel_clicked()
{
    reject();
}
