#ifndef LIBRARY_H
#define LIBRARY_H

// ============================================================
// library.h
// Contains: library, librarian, and Authentication
//           class definitions.
// ============================================================

#include "Globals.h"
#include "Book.h"
#include "Transaction.h"

// ============================================================
//                    LIBRARIAN CLASS
// ============================================================
class librarian
{
private:
    int librarian_id;
    string name;
    string email;
    string phone_no;
    string username;
    string password;

public:
    // CONSTRUCTORS
    librarian();
    librarian(int id, string n, string e, string p);
    // DESTRUCTOR
    ~librarian();

    // Set login credentials
    void setCredentials(string u, string p);

    string getUsername() { return username; }
    string getPassword() { return password; }

    // Thin wrappers — actual logic lives in BookCatalog
    void addbook(Book *b, BookCatalog *catalog);
    void removeBook(string t, BookCatalog *catalog);

    // Display
    friend ostream &operator<<(ostream &os, const librarian &l);

    // Menus (implemented in library.cpp)
    void viewFullDetails(library *lib);
    void manageBooks(library *lib);
    void manageTables(library *lib);
    void librarianMainMenu(library *lib);
};

// ============================================================
//                    LIBRARY CLASS
// ============================================================
class library
{
private:
    BookCatalog catalog;
    vector<Table> tables;
    vector<Reservation> reservations;
    vector<Transaction> transactions;
    vector<Fines> fines;
    vector<User *> users;

public:
    // Max number of tables allowed
    static const int MAX_TABLES = 10;

    // ---- USER ACCESS ----
    User *getUserByIndex(int index);
    int   getUserCount() { return (int)users.size(); }

    vector<User *>     &getUsersVector()        { return users; }
    vector<Table>      &getTablesVector()        { return tables; }
    vector<Transaction>&getTransactionsVector()  { return transactions; }
    vector<Fines>      &getFinesVector()         { return fines; }
    vector<Reservation>&getReservationsVector()  { return reservations; }

    // Register a new user (validates duplicates, saves to file)
    void registerUserInLibrary();

    // ---- BOOK INTERACTIONS ----
    bool readOnSite(User *u, string title);
    bool downloadBook(User *u, string title);
    bool hasOverdueBooks(User *u);
    bool borrowBook(User *u, string title, int duration_minutes);
    bool returnBook(User *u, string title);

    // ---- LIVE FINE CALCULATION ----
    int calculateLiveFine(Transaction &t, User *u);

    // ---- VIEW FUNCTIONS ----
    void viewTransactions();
    void viewFines();
    void viewUserTransactions(int userID);
    void viewUserFines(int userID);
    void viewUserReservations(int uid);
    void viewAllReservations();
    void viewTables();
    void viewUsers();
    void viewUsersByType(string type);
    void viewAllMemberships();

    // ---- RESERVATION ----
    void reserveBook(int uid, int bkid, string date, string start, string end);
    void reserveTableManual(int uid, int tid, string date, string start, string end);
    void reserveTableAuto(int uid, string date, string start, string end);
    void cancelReservation(int rid, int uid);
    void autoExpireReservations();

    // ---- TABLE MANAGEMENT ----
    void addTable(string loc, int cap);
    void removeTable(int tid);
    void viewAvailableTables(string date, string start, string end);
    bool hasAvailableTable(string date, string start, string end);
    Table getTable(int i);
    int getTableCount() { return (int)tables.size(); }

    // ---- CATALOG ACCESS ----
    BookCatalog *getCatalog() { return &catalog; }
};

// ============================================================
//                    AUTHENTICATION CLASS
// ============================================================
class Authentication
{
private:
    librarian *lib_admin;

public:
    // CONSTRUCTOR
    Authentication(librarian *l = nullptr);

    // SETTER
    void setLibrarian(librarian *l);

    // Login functions (throw AuthException on failure)
    User      *loginUser(library *lib);
    librarian *loginLibrarian();

    // Logout
    void logout();
};

#endif
