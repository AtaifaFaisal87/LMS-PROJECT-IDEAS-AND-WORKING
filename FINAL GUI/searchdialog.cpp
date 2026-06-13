#include "searchdialog.h"
#include "ui_searchdialog.h"
#include <QMessageBox>

SearchDialog::SearchDialog(Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SearchDialog)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("Search Books");
    on_cmbCriteria_currentIndexChanged(0);
}

SearchDialog::~SearchDialog()
{
    delete ui;
}

void SearchDialog::on_cmbCriteria_currentIndexChanged(int index)
{
    ui->txtSearch->setPlaceholderText(
        index == 0 ? "Enter Book ID (number)" :
        index == 5 ? "Enter Year (e.g., 2024)" :
        "Enter search text"
    );
}

void SearchDialog::on_btnSearch_clicked()
{
    QString searchText = ui->txtSearch->text().trimmed();
    if (searchText.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please enter search text.");
        return;
    }

    ui->lstResults->clear();
    int criteria = ui->cmbCriteria->currentIndex();
    BookCatalog* catalog = lib->getCatalog();

    switch (criteria) {
    case 0: {
        bool ok;
        int id = searchText.toInt(&ok);
        if (ok) {
            Book* b = catalog->SearchbyID(id);
            if (b) displayBook(b);
            else ui->lstResults->addItem("Book not found.");
        } else {
            ui->lstResults->addItem("Invalid ID. Please enter a number.");
        }
        break;
    }
    case 1: {
        Book* b = catalog->SearchbyTitle(searchText);
        if (b) displayBook(b);
        else ui->lstResults->addItem("Book not found.");
        break;
    }
    case 2: {
        QVector<Book*> results = catalog->SearchbyAuthor(searchText);
        displayResults(results);
        break;
    }
    case 3: {
        QVector<Book*> results = catalog->SearchbyCategory(searchText);
        displayResults(results);
        break;
    }
    case 4: {
        QVector<Book*> results = catalog->SearchbyPublisher(searchText);
        displayResults(results);
        break;
    }
    case 5: {
        bool ok;
        int year = searchText.toInt(&ok);
        if (ok) {
            QVector<Book*> results = catalog->SearchbyYOP(year);
            displayResults(results);
        } else {
            ui->lstResults->addItem("Invalid year. Please enter a number.");
        }
        break;
    }
    }
}

void SearchDialog::displayBook(Book* book)
{
    if (!book) return;
    QString info = QString("ID: %1\nTitle: %2\nAuthor: %3\nCategory: %4\nISBN: %5\nPublisher: %6\nYear: %7\nCopies: %8\nStatus: %9\nBorrowed: %10 times")
        .arg(book->get_bookid())
        .arg(book->get_title())
        .arg(book->get_author())
        .arg(book->get_category())
        .arg(book->get_isbn())
        .arg(book->get_publisher())
        .arg(book->get_YOP())
        .arg(book->getAvailableCopies())
        .arg(book->getAvailabilityStatus() ? "AVAILABLE" : "NOT AVAILABLE")
        .arg(book->get_timesborrowed_count());
    ui->lstResults->addItem(info);
}

void SearchDialog::displayResults(const QVector<Book*>& books)
{
    if (books.isEmpty()) {
        ui->lstResults->addItem("No books found.");
        return;
    }
    for (Book* b : books) {
        displayBook(b);
        ui->lstResults->addItem("---");
    }
}

void SearchDialog::on_btnClose_clicked()
{
    accept();
}
