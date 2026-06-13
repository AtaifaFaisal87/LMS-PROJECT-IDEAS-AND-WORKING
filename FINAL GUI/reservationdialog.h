#ifndef RESERVATIONDIALOG_H
#define RESERVATIONDIALOG_H

#include <QDialog>
#include "librarycore.h"

QT_BEGIN_NAMESPACE
namespace Ui { class ReservationDialog; }
QT_END_NAMESPACE

class ReservationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ReservationDialog(User* user, Library* lib, QWidget *parent = nullptr);
    ~ReservationDialog();

private slots:
    void on_btnAutoTable_clicked();
    void on_btnManualTable_clicked();
    void on_btnReserveBook_clicked();
    void on_btnViewMyReservations_clicked();
    void on_btnCancelReservation_clicked();
    void on_btnClose_clicked();

private:
    Ui::ReservationDialog *ui;
    User* currentUser;
    Library* lib;
    bool validateDateTimeInputs(QString& date, QString& start, QString& end);
};

#endif // RESERVATIONDIALOG_H
