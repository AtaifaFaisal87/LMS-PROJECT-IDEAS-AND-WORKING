#ifndef SEARCHDIALOG_H
#define SEARCHDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class SearchDialog; }
QT_END_NAMESPACE

class SearchDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SearchDialog(Library* lib, QWidget *parent = nullptr);
    ~SearchDialog();

private slots:
    void on_btnSearch_clicked();
    void on_btnClose_clicked();
    void on_cmbCriteria_currentIndexChanged(int index);

private:
    Ui::SearchDialog *ui;
    Library* lib;
    void displayResults(const QVector<Book*>& books);
    void displayBook(Book* book);
};

#endif // SEARCHDIALOG_H
