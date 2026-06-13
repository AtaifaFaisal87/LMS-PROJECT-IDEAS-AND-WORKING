#include "tablemanagementdialog.h"
#include "ui_tablemanagementdialog.h"
#include <QMessageBox>
#include <QInputDialog>

TableManagementDialog::TableManagementDialog(Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::TableManagementDialog)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("Table Management");
    refreshTableList();
}

TableManagementDialog::~TableManagementDialog()
{
    delete ui;
}

void TableManagementDialog::refreshTableList()
{
    ui->lstTables->clear();
    QVector<Table>& tables = lib->getTablesVector();
    for (int i = 0; i < tables.size(); i++) {
        QString item = QString("ID: %1 | Location: %2 | Capacity: %3")
            .arg(tables[i].get_id())
            .arg(tables[i].get_location())
            .arg(tables[i].get_capacity());
        ui->lstTables->addItem(item);
    }
    ui->lblCount->setText("Tables: " + QString::number(tables.size()) + " / " + QString::number(Library::MAX_TABLES));
}

void TableManagementDialog::on_btnAddTable_clicked()
{
    if (lib->getTableCount() >= Library::MAX_TABLES) {
        QMessageBox::warning(this, "Error", "Maximum table limit reached!");
        return;
    }

    QString location = QInputDialog::getText(this, "Add Table", "Enter location:");
    if (location.isEmpty()) return;

    bool ok;
    int capacity = QInputDialog::getInt(this, "Add Table", "Enter capacity:", 4, 1, 100, 1, &ok);
    if (!ok) return;

    try {
        lib->addTable(location, capacity);
        lib->saveAllData();
        QMessageBox::information(this, "Success", "Table added successfully!");
        refreshTableList();
    } catch (const ReservationException& e) {
        QMessageBox::critical(this, "Error", e.getMessage());
    }
}

void TableManagementDialog::on_btnRemoveTable_clicked()
{
    int row = ui->lstTables->currentRow();
    if (row < 0) {
        QMessageBox::warning(this, "Error", "Please select a table to remove.");
        return;
    }

    Table t = lib->getTable(row);
    int reply = QMessageBox::question(this, "Confirm", "Remove Table ID: " + QString::number(t.get_id()) + "?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        try {
            lib->removeTable(t.get_id());
            lib->saveAllData();
            QMessageBox::information(this, "Success", "Table removed!");
            refreshTableList();
        } catch (const ReservationException& e) {
            QMessageBox::critical(this, "Error", e.getMessage());
        }
    }
}

void TableManagementDialog::on_btnRefresh_clicked()
{
    refreshTableList();
}

void TableManagementDialog::on_btnClose_clicked()
{
    accept();
}
