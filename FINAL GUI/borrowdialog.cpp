#include "borrowdialog.h"
#include "ui_borrowdialog.h"
#include <QMessageBox>
#include <QInputDialog>

BorrowDialog::BorrowDialog(User* user, Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BorrowDialog)
    , currentUser(user)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("Borrow Book");
    refreshBookList();
}

BorrowDialog::~BorrowDialog()
{
    delete ui;
}

void BorrowDialog::refreshBookList()
{
    ui->lstBooks->clear();
    BookCatalog* catalog = lib->getCatalog();
    for (int i = 0; i < catalog->get_bookcount(); i++) {
        Book* b = catalog->getBookAtIndex(i);
        if (b && b->getAvailabilityStatus()) {
            QString item = QString("ID: %1 | %2 by %3 | Copies: %4")
                .arg(b->get_bookid())
                .arg(b->get_title())
                .arg(b->get_author())
                .arg(b->getAvailableCopies());
            ui->lstBooks->addItem(item);
        }
    }
}

void BorrowDialog::on_btnBorrow_clicked()
{
    int row = ui->lstBooks->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Please select a book to borrow.");
        return;
    }

    Book* selected = nullptr;
    int idx = 0;
    BookCatalog* catalog = lib->getCatalog();
    for (int i = 0; i < catalog->get_bookcount(); i++) {
        Book* b = catalog->getBookAtIndex(i);
        if (b && b->getAvailabilityStatus()) {
            if (idx == row) { selected = b; break; }
            idx++;
        }
    }

    if (!selected) return;

    bool ok;
    QStringList units;
    units << "Minutes" << "Hours" << "Days";
    QString unit = QInputDialog::getItem(this, "Borrow Duration", "Select time unit:", units, 0, false, &ok);
    if (!ok) return;

    int duration = 0;
    if (unit == "Minutes") {
        duration = QInputDialog::getInt(this, "Duration", "Enter minutes (1-10080):", 60, 1, 10080, 1, &ok);
    } else if (unit == "Hours") {
        duration = QInputDialog::getInt(this, "Duration", "Enter hours (1-168):", 1, 1, 168, 1, &ok);
        duration *= 60;
    } else {
        duration = QInputDialog::getInt(this, "Duration", "Enter days (1-7):", 1, 1, 7, 1, &ok);
        duration *= 24 * 60;
    }
    if (!ok) return;

    try {
        lib->borrowBook(currentUser, selected->get_title(), duration);
        lib->saveAllData();
        QMessageBox::information(this, "Success",
            "Book borrowed successfully!\nTitle: " + selected->get_title() +
            "\nDuration: " + QString::number(duration) + " minutes");
        accept();
    } catch (const LibraryException& e) {
        QMessageBox::critical(this, "Error", e.getMessage());
    }
}

void BorrowDialog::on_btnCancel_clicked()
{
    reject();
}
