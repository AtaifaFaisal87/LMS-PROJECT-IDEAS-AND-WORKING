#include "bookmanagementdialog.h"
#include "ui_bookmanagementdialog.h"
#include <QMessageBox>
#include <QInputDialog>

BookManagementDialog::BookManagementDialog(Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::BookManagementDialog)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("Book Management");
    refreshBookList();
}

BookManagementDialog::~BookManagementDialog()
{
    delete ui;
}

void BookManagementDialog::refreshBookList()
{
    ui->lstBooks->clear();
    BookCatalog* catalog = lib->getCatalog();
    for (int i = 0; i < catalog->get_bookcount(); i++) {
        Book* b = catalog->getBookAtIndex(i);
        if (b) {
            QString item = QString("ID: %1 | %2 by %3 | Copies: %4 | Year: %5")
                .arg(b->get_bookid())
                .arg(b->get_title())
                .arg(b->get_author())
                .arg(b->getAvailableCopies())
                .arg(b->get_YOP());
            ui->lstBooks->addItem(item);
        }
    }
    ui->lblCount->setText("Total Books: " + QString::number(catalog->get_bookcount()));
}

void BookManagementDialog::on_btnAddBook_clicked()
{
    QString title = QInputDialog::getText(this, "Add Book", "Enter book title:");
    if (title.isEmpty()) return;

    QString author = QInputDialog::getText(this, "Add Book", "Enter author:");
    if (author.isEmpty()) return;

    QString category = QInputDialog::getText(this, "Add Book", "Enter category:");
    if (category.isEmpty()) return;

    QString isbn = QInputDialog::getText(this, "Add Book", "Enter ISBN:");
    if (isbn.isEmpty()) return;

    QString publisher = QInputDialog::getText(this, "Add Book", "Enter publisher:");
    if (publisher.isEmpty()) return;

    bool ok;
    int year = QInputDialog::getInt(this, "Add Book", "Enter year:", 2024, 1000, 2100, 1, &ok);
    if (!ok) return;

    int copies = QInputDialog::getInt(this, "Add Book", "Enter available copies:", 1, 1, 1000, 1, &ok);
    if (!ok) return;

    Book* existing = lib->getCatalog()->SearchbyTitle(title);
    if (existing) {
        QMessageBox::warning(this, "Error", "A book with this title already exists!");
        return;
    }

    Book* b = new Book(title, author, category, isbn, publisher, year);
    b->setAvailableCopies(copies);
    lib->getCatalog()->addbook(b);
    lib->saveAllData();

    QMessageBox::information(this, "Success", "Book added successfully! ID: " + QString::number(b->get_bookid()));
    refreshBookList();
}

void BookManagementDialog::on_btnRemoveBook_clicked()
{
    int row = ui->lstBooks->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Please select a book to remove.");
        return;
    }

    Book* b = lib->getCatalog()->getBookAtIndex(row);
    if (!b) return;

    int reply = QMessageBox::question(this, "Confirm", "Remove book: " + b->get_title() + "?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        lib->getCatalog()->removeBook(b->get_title());
        lib->saveAllData();
        QMessageBox::information(this, "Success", "Book removed!");
        refreshBookList();
    }
}

void BookManagementDialog::on_btnRefresh_clicked()
{
    refreshBookList();
}

void BookManagementDialog::on_btnClose_clicked()
{
    accept();
}
