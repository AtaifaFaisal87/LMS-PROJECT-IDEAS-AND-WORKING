#ifndef TRANSACTION_H
#define TRANSACTION_H

// ============================================================
// Transaction.h
// Contains: Transaction, Fines, Reservation, and Table
//           class definitions, plus file I/O declarations.
// ============================================================

#include "Globals.h"
#include "User.h"

// ============================================================
//                    TRANSACTION CLASS
// ============================================================
class Transaction
{
private:
    static int next_id;
    int transaction_id;
    int user_id;
    int book_id;
    time_t borrow_time;
    time_t due_time;
    time_t return_time;
    string return_date;
    bool returned;

public:
    // DEFAULT CONSTRUCTOR
    Transaction();
    // PARAMETERIZED CONSTRUCTOR
    Transaction(int uid, int bid, int duration_minutes);
    // DESTRUCTOR
    ~Transaction();

    // GETTERS
    int    getuserid()         { return user_id; }
    int    getbookid()         { return book_id; }
    int    getTransactionID()  { return transaction_id; }
    bool   isReturned()        { return returned; }
    time_t getDueTime()        { return due_time; }
    time_t getBorrowTime()     { return borrow_time; }
    time_t getReturnTime()     { return return_time; }
    string getReturnDate()     { return return_date; }

    // SETTERS
    void setTransactionID(int id)  { transaction_id = id; }
    void setUserID(int id)         { user_id = id; }
    void setBookID(int id)         { book_id = id; }
    void setBorrowTime(time_t t)   { borrow_time = t; }
    void setDueTime(time_t t)      { due_time = t; }
    void setReturnTime(time_t t)   { return_time = t; }
    void setReturnDate(string d)   { return_date = d; }
    void setReturned(bool r)       { returned = r; }

    // Calculate overdue minutes (0 if not late or not returned yet)
    int getOverdueMinutes();

    // Mark as returned and return overdue minutes
    int markreturned();

    // Display
    friend ostream &operator<<(ostream &os, Transaction &t);
};

// ============================================================
//                    FINES CLASS
// ============================================================
class Fines
{
private:
    int user_id;
    int book_id;
    int minuteslate;
    double rate;
    double originalAmount;
    double finalAmount;
    bool discountApplied;

public:
    // DEFAULT CONSTRUCTOR
    Fines();
    // PARAMETERIZED CONSTRUCTOR (User* is passed to get fine rate and membership discount)
    Fines(int u, int b, int mins, User *user);
    // DESTRUCTOR
    ~Fines();

    // GETTERS
    double getAmount()      { return finalAmount; }
    double getOriginal()    { return originalAmount; }
    int    getUserID()      { return user_id; }
    int    getBookID()      { return book_id; }
    bool   wasDiscounted()  { return discountApplied; }
    int    getMinutesLate() { return minuteslate; }
    double getRate()        { return rate; }

    // SETTERS
    void setUserID(int id)            { user_id = id; }
    void setBookID(int id)            { book_id = id; }
    void setMinutesLate(int m)        { minuteslate = m; }
    void setRate(double r)            { rate = r; }
    void setOriginalAmount(double a)  { originalAmount = a; }
    void setFinalAmount(double a)     { finalAmount = a; }
    void setDiscountApplied(bool d)   { discountApplied = d; }

    // Display
    friend ostream &operator<<(ostream &os, const Fines &f);
};

// ============================================================
//                    RESERVATION CLASS
// ============================================================
class Reservation
{
private:
    int reservationid;
    static int next_id;

    int userid;
    int bookid;
    int tableid;

    string date;
    string startTime;
    string endTime;

    bool active;

public:
    // DEFAULT CONSTRUCTOR
    Reservation();
    // PARAMETERIZED CONSTRUCTOR
    Reservation(int uid, int bid, int tid,
                string d, string start, string end);
    // DESTRUCTOR
    ~Reservation();

    // GETTERS
    int    get_userid()         { return userid; }
    int    get_tableid()        { return tableid; }
    int    get_bookid()         { return bookid; }
    int    get_reservationid()  { return reservationid; }
    string get_date()           { return date; }
    string get_start()          { return startTime; }
    string get_end()            { return endTime; }

    void setReservationID(int id) { reservationid = id; }

    bool isActive();
    bool isExpired();
    void cancel() { active = false; }

    // Check if two time slots overlap (static = no object needed)
    static bool overlap(string s1, string e1, string s2, string e2);

    // Display (only prints if active)
    friend ostream &operator<<(ostream &os, const Reservation &r);

    // Print including status (for expired/cancelled reservations)
    void printInactive(ostream &os) const;
};

// ============================================================
//                    TABLE CLASS
// ============================================================
class Table
{
private:
    int tableid;
    static int next_id;

    string location;
    int capacity;

public:
    // DEFAULT CONSTRUCTOR
    Table();
    // PARAMETERIZED CONSTRUCTOR
    Table(string loc, int cap);

    int    get_id()       { return tableid; }
    string get_location() { return location; }
    int    get_capacity() { return capacity; }

    void setID(int id)          { tableid = id; }
    void setLocation(string loc){ location = loc; }
    void setCapacity(int cap)   { capacity = cap; }

    // Display
    friend ostream &operator<<(ostream &os, const Table &t);
};

// ============================================================
//              FILE I/O FUNCTION DECLARATIONS
// ============================================================
void saveTransactionToFile(Transaction &t);
void loadTransactionsFromFile(vector<Transaction> &transactions, vector<User *> &users);
void rewriteTransactionsFile(vector<Transaction> &transactions);

void saveFineToFile(Fines &f);
void loadFinesFromFile(vector<Fines> &fines);
void rewriteFinesFile(vector<Fines> &fines);

void saveReservationToFile(Reservation &r);
void loadReservationsFromFile(vector<Reservation> &reservations);
void rewriteReservationsFile(vector<Reservation> &reservations);

void saveTableToFile(Table &t);
void loadTablesFromFile(vector<Table> &tables);
void rewriteTablesFile(vector<Table> &tables);

#endif
