#include "reservationdialog.h"
#include "ui_reservationdialog.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QDateTime>

ReservationDialog::ReservationDialog(User* user, Library* library, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::ReservationDialog)
    , currentUser(user)
    , lib(library)
{
    ui->setupUi(this);
    setWindowTitle("Reservations");
}

ReservationDialog::~ReservationDialog()
{
    delete ui;
}

bool ReservationDialog::validateDateTimeInputs(QString& date, QString& start, QString& end)
{
    date = ui->txtDate->text().trimmed();
    start = ui->txtStartTime->text().trimmed();
    end = ui->txtEndTime->text().trimmed();

    if (date.isEmpty() || start.isEmpty() || end.isEmpty()) {
        QMessageBox::warning(this, "Error", "Please fill in all date and time fields.");
        return false;
    }

    QDate d = QDate::fromString(date, "dd/MM/yyyy");
    QTime s = QTime::fromString(start, "hh:mm");
    QTime e = QTime::fromString(end, "hh:mm");

    if (!d.isValid()) {
        QMessageBox::warning(this, "Error", "Invalid date format. Use DD/MM/YYYY");
        return false;
    }
    if (!s.isValid() || !e.isValid()) {
        QMessageBox::warning(this, "Error", "Invalid time format. Use HH:MM (24-hour)");
        return false;
    }

    QDateTime startDT(d, s);
    QDateTime endDT(d, e);
    if (startDT <= QDateTime::currentDateTime()) {
        QMessageBox::warning(this, "Error", "Start time must be in the future.");
        return false;
    }
    if (endDT <= startDT) {
        QMessageBox::warning(this, "Error", "End time must be after start time.");
        return false;
    }

    int startHour = s.hour();
    int endHour = e.hour();
    if ((startHour >= 0 && startHour < 5) || (endHour >= 0 && endHour < 5)) {
        QMessageBox::warning(this, "Error", "Library is closed from 12 AM to 5 AM.");
        return false;
    }

    return true;
}

void ReservationDialog::on_btnAutoTable_clicked()
{
    QString date, start, end;
    if (!validateDateTimeInputs(date, start, end)) return;

    try {
        lib->reserveTableAuto(currentUser->get_userid(), date, start, end);
        lib->saveAllData();
        QMessageBox::information(this, "Success", "Table reserved successfully (Auto-assigned)!");
    } catch (const ReservationException& e) {
        QMessageBox::critical(this, "Error", e.getMessage());
    }
}

void ReservationDialog::on_btnManualTable_clicked()
{
    QString date, start, end;
    if (!validateDateTimeInputs(date, start, end)) return;

    bool ok;
    int tid = QInputDialog::getInt(this, "Table ID", "Enter Table ID:", 1, 1, 999, 1, &ok);
    if (!ok) return;

    try {
        lib->reserveTableManual(currentUser->get_userid(), tid, date, start, end);
        lib->saveAllData();
        QMessageBox::information(this, "Success", "Table reserved successfully!");
    } catch (const ReservationException& e) {
        QMessageBox::critical(this, "Error", e.getMessage());
    }
}

void ReservationDialog::on_btnReserveBook_clicked()
{
    QString date, start, end;
    if (!validateDateTimeInputs(date, start, end)) return;

    bool ok;
    int bid = QInputDialog::getInt(this, "Book ID", "Enter Book ID to reserve:", 1, 1, 999, 1, &ok);
    if (!ok) return;

    try {
        lib->reserveBook(currentUser->get_userid(), bid, date, start, end);
        lib->saveAllData();
        QMessageBox::information(this, "Success", "Book reserved successfully!");
    } catch (const LibraryException& e) {
        QMessageBox::critical(this, "Error", e.getMessage());
    }
}

void ReservationDialog::on_btnViewMyReservations_clicked()
{
    ui->lstReservations->clear();
    lib->autoExpireReservations();
    bool found = false;
    for (const Reservation& r : lib->getReservationsVector()) {
        if (r.get_userid() == currentUser->get_userid() && r.isActive()) {
            QString item = QString("ResID: %1 | %2 | Date: %3 | %4 to %5")
                .arg(r.get_reservationid())
                .arg(r.get_bookid() ? "Book ID: " + QString::number(r.get_bookid()) : "Table ID: " + QString::number(r.get_tableid()))
                .arg(r.get_date())
                .arg(r.get_start())
                .arg(r.get_end());
            ui->lstReservations->addItem(item);
            found = true;
        }
    }
    if (!found) ui->lstReservations->addItem("No active reservations.");
}

void ReservationDialog::on_btnCancelReservation_clicked()
{
    bool ok;
    int rid = QInputDialog::getInt(this, "Cancel Reservation", "Enter Reservation ID:", 1, 1, 99999, 1, &ok);
    if (!ok) return;

    try {
        lib->cancelReservation(rid, currentUser->get_userid());
        lib->saveAllData();
        QMessageBox::information(this, "Success", "Reservation cancelled!");
        on_btnViewMyReservations_clicked();
    } catch (const ReservationException& e) {
        QMessageBox::critical(this, "Error", e.getMessage());
    }
}

void ReservationDialog::on_btnClose_clicked()
{
    accept();
}
