// ============================================================
// library.cpp
// Contains: library, librarian, and Authentication class
//           implementations, plus librarian menus.
// ============================================================

#include "library.h"

// ============================================================
//                    LIBRARIAN CLASS
// ============================================================

librarian::librarian()
    : librarian_id(0), name("UNKNOWN"), email("UNKNOWN"), phone_no("UNKNOWN") {}

librarian::librarian(int id, string n, string e, string p)
    : librarian_id(id), name(n), email(e), phone_no(p) {}

librarian::~librarian() {}

void librarian::setCredentials(string u, string p)
{
    username = u;
    password = p;
}

void librarian::addbook(Book *b, BookCatalog *catalog)
{
    catalog->addbook(b);
}

void librarian::removeBook(string t, BookCatalog *catalog)
{
    catalog->removeBook(t);
}

ostream &operator<<(ostream &os, const librarian &l)
{
    os << "=============== LIBRARIAN DETAILS ===============" << endl
       << "| ID       : " << l.librarian_id << endl
       << "| NAME     : " << l.name << endl
       << "| EMAIL    : " << l.email << endl
       << "| PHONE #  : " << l.phone_no << endl
       << "=================================================" << endl;
    return os;
}

// ============================================================
//                    LIBRARY CLASS
// ============================================================

User *library::getUserByIndex(int index)
{
    if (index >= 0 && index < (int)users.size())
        return users[index];
    return nullptr;
}

void library::registerUserInLibrary()
{
    User *newUser = User::registerUser();
    if (newUser)
    {
        // Check for duplicates (CNIC, email, phone)
        for (int i = 0; i < (int)users.size(); i++)
        {
            if (users[i]->get_cnic() == newUser->get_cnic())
            {
                cout << "REGISTRATION FAILED! A USER WITH THIS CNIC ALREADY EXISTS." << endl;
                delete newUser;
                return;
            }
            if (users[i]->get_email() == newUser->get_email())
            {
                cout << "REGISTRATION FAILED! A USER WITH THIS EMAIL ALREADY EXISTS." << endl;
                delete newUser;
                return;
            }
            if (users[i]->get_phone() == newUser->get_phone())
            {
                cout << "REGISTRATION FAILED! A USER WITH THIS PHONE NUMBER ALREADY EXISTS." << endl;
                delete newUser;
                return;
            }
        }

        // Check for duplicate username (loop until unique)
        bool duplicate = true;
        while (duplicate)
        {
            duplicate = false;
            for (int i = 0; i < (int)users.size(); i++)
            {
                if (users[i]->getUsername() == newUser->getUsername())
                {
                    duplicate = true;
                    cout << "USERNAME ALREADY EXISTS! PLEASE CHOOSE A DIFFERENT ONE." << endl;
                    string newUsername = getValidString("ENTER A NEW USERNAME: ");
                    newUser->setUsername(newUsername);
                    break;
                }
            }
        }

        try
        {
            users.push_back(newUser);
            saveUserToFile(newUser);
            cout << "USER REGISTERED SUCCESSFULLY!\n";
        }
        catch (FileException &e)
        {
            cout << e.what() << endl;
            cout << "USER ADDED TO MEMORY BUT NOT SAVED TO FILE.\n";
        }
    }
    else
        cout << "REGISTRATION FAILED!\n";
}

bool library::readOnSite(User *u, string title)
{
    Book *book = catalog.SearchbyTitle(title);
    if (!book)
        throw BookException("BOOK NOT FOUND: " + title);

    book->incrementReadOnSiteCount();
    cout << "READING BOOK ONSITE!" << endl;
    return true;
}

bool library::downloadBook(User *u, string title)
{
    Book *book = catalog.SearchbyTitle(title);
    if (!book)
        throw BookException("BOOK NOT FOUND: " + title);

    book->incrementDownloadCount();
    cout << "BOOK DOWNLOADED!" << endl;
    return true;
}

bool library::hasOverdueBooks(User *u)
{
    time_t now = getCurrentTime();
    for (int i = 0; i < (int)transactions.size(); i++)
    {
        if (transactions[i].getuserid() == u->get_userid() &&
            !transactions[i].isReturned() &&
            now > transactions[i].getDueTime())
            return true;
    }
    return false;
}

bool library::borrowBook(User *u, string title, int duration_minutes)
{
    if (hasOverdueBooks(u))
        throw TransactionException("YOU HAVE OVERDUE BOOKS! RETURN THEM BEFORE BORROWING.");

    Book *book = catalog.SearchbyTitle(title);
    if (!book)
        throw BookException("BOOK NOT FOUND: " + title);

    if (book->getAvailableCopies() == 0)
        throw BookException("NO COPIES AVAILABLE FOR: " + title);

    if (!u->borrowbook(book->get_bookid()))
        throw UserException("BORROW LIMIT REACHED! YOU CANNOT BORROW MORE BOOKS.");

    book->setAvailableCopies(book->getAvailableCopies() - 1);
    book->incrementBorrowCount();
    transactions.emplace_back(u->get_userid(), book->get_bookid(), duration_minutes);
    saveTransactionToFile(transactions.back());

    time_t now = getCurrentTime();
    cout << "BOOK BORROWED SUCCESSFULLY!" << endl;
    cout << "BORROW TIME : " << timeToString(now) << endl;
    cout << "DUE TIME    : " << timeToString(addMinutes(now, duration_minutes)) << endl;
    cout << "FINE RATE   : Rs." << u->getFineRate() << " PER MINUTE IF LATE!" << endl;
    return true;
}

bool library::returnBook(User *u, string title)
{
    Book *book = catalog.SearchbyTitle(title);
    if (!book)
        throw BookException("BOOK NOT FOUND: " + title);

    if (!u->returnbook(book->get_bookid()))
        throw TransactionException("YOU HAVE NOT BORROWED THIS BOOK: " + title);

    book->setAvailableCopies(book->getAvailableCopies() + 1);

    for (int i = 0; i < (int)transactions.size(); i++)
    {
        if (transactions[i].getbookid() == book->get_bookid() &&
            transactions[i].getuserid() == u->get_userid() &&
            !transactions[i].isReturned())
        {
            int overdue_mins = transactions[i].markreturned();
            rewriteTransactionsFile(transactions);
            cout << "BOOK RETURNED SUCCESSFULLY!" << endl;
            cout << "RETURN TIME: " << timeToString(getCurrentTime()) << endl;

            if (overdue_mins > 0)
            {
                fines.emplace_back(u->get_userid(), book->get_bookid(), overdue_mins, u);
                saveFineToFile(fines.back());
                cout << "FINE GENERATED!" << endl;
                cout << "OVERDUE    : " << overdue_mins << " MINUTES" << endl;

                double originalFine = u->getFineRate() * overdue_mins;
                if (u->hasMembership())
                {
                    double discountedFine = u->applyMembershipDiscount(originalFine);
                    cout << "ORIGINAL FINE: Rs." << originalFine << endl;
                    cout << "AFTER 15% MEMBERSHIP DISCOUNT: Rs." << discountedFine << endl;
                }
                else
                {
                    cout << "FINE AMOUNT: Rs." << originalFine << endl;
                }
            }
            else
                cout << "RETURNED ON TIME! NO FINE." << endl;

            break;
        }
    }
    return true;
}

int library::calculateLiveFine(Transaction &t, User *u)
{
    if (t.isReturned())
        return 0;

    time_t now = getCurrentTime();

    if (now <= t.getDueTime())
        return 0;

    int overdue = minutesDiff(t.getDueTime(), now);
    return overdue * u->getFineRate();
}

void library::viewTransactions()
{
    cout << "==== ALL TRANSACTIONS ====" << endl;
    for (int i = 0; i < (int)transactions.size(); i++)
        cout << transactions[i];
}

void library::viewFines()
{
    cout << "==== ALL FINES ====" << endl;
    for (int i = 0; i < (int)fines.size(); i++)
        cout << fines[i];
}

void library::viewUserTransactions(int userID)
{
    cout << "==== TRANSACTIONS FOR USER ID: " << userID << " ====" << endl;
    for (int i = 0; i < (int)transactions.size(); i++)
        if (transactions[i].getuserid() == userID)
            cout << transactions[i];
}

void library::viewUserFines(int userID)
{
    cout << "==== FINES FOR USER ID: " << userID << " ====" << endl;
    bool found = false;

    // Stored fines (after return)
    for (int i = 0; i < (int)fines.size(); i++)
    {
        if (fines[i].getUserID() == userID)
        {
            cout << fines[i];
            found = true;
        }
    }

    // Live fines (not yet returned)
    for (int i = 0; i < (int)transactions.size(); i++)
    {
        if (transactions[i].getuserid() == userID && !transactions[i].isReturned())
        {
            User *u = nullptr;
            for (int j = 0; j < (int)users.size(); j++)
                if (users[j]->get_userid() == userID)
                {
                    u = users[j];
                    break;
                }

            if (!u) continue;

            int fine = calculateLiveFine(transactions[i], u);
            if (fine > 0)
            {
                if (u->hasMembership())
                {
                    double discountedFine = u->applyMembershipDiscount(fine);
                    cout << "|USER ID: " << userID
                         << " |BOOK ID: " << transactions[i].getbookid()
                         << " |STATUS: UNPAID (LIVE)"
                         << " |ORIGINAL FINE: Rs." << fine
                         << " |AFTER 15% DISCOUNT: Rs." << discountedFine
                         << " |" << endl;
                }
                else
                {
                    cout << "|USER ID: " << userID
                         << " |BOOK ID: " << transactions[i].getbookid()
                         << " |STATUS: UNPAID (LIVE)"
                         << " |LIVE FINE: Rs." << fine
                         << " |" << endl;
                }
                found = true;
            }
        }
    }

    if (!found)
        cout << "NO FINES FOUND!" << endl;
}

void library::viewUserReservations(int uid)
{
    autoExpireReservations();
    cout << "==== USER RESERVATIONS ====" << endl;
    bool found = false;
    for (int i = 0; i < (int)reservations.size(); i++)
        if (reservations[i].get_userid() == uid && reservations[i].isActive())
        {
            cout << reservations[i];
            found = true;
        }
    if (!found)
        cout << "NO ACTIVE RESERVATIONS." << endl;
}

void library::viewAllReservations()
{
    autoExpireReservations();

    cout << "\n===== ACTIVE RESERVATIONS =====" << endl;
    bool found = false;
    for (int i = 0; i < (int)reservations.size(); i++)
        if (reservations[i].isActive())
        {
            cout << reservations[i];
            found = true;
        }
    if (!found)
        cout << "NO ACTIVE RESERVATIONS." << endl;

    cout << "\n===== EXPIRED / CANCELLED RESERVATIONS =====" << endl;
    bool foundExpired = false;
    for (int i = 0; i < (int)reservations.size(); i++)
        if (!reservations[i].isActive())
        {
            reservations[i].printInactive(cout);
            foundExpired = true;
        }
    if (!foundExpired)
        cout << "NO EXPIRED OR CANCELLED RESERVATIONS." << endl;
}

void library::viewTables()
{
    cout << "==== TABLES (" << tables.size() << "/" << MAX_TABLES << ") ====" << endl;

    if (tables.empty())
    {
        cout << "NO TABLES AVAILABLE." << endl;
        return;
    }

    for (int i = 0; i < (int)tables.size(); i++)
        cout << tables[i];
}

void library::viewUsers()
{
    cout << "================= ALL USERS =================" << endl;
    for (int i = 0; i < (int)users.size(); i++)
    {
        User *u = users[i];
        cout << "| NAME         : " << u->get_name() << endl
             << "| EMAIL        : " << u->get_email() << endl
             << "| PHONE        : " << u->get_phone() << endl
             << "| USER TYPE    : " << u->get_usertype() << endl
             << "| MEMBERSHIP   : " << (u->hasMembership() ? "ACTIVE" : "NONE") << endl
             << "| BORROW LIMIT : " << u->getEffectiveBorrowLimit() << endl
             << "--------------------------------------------" << endl;
    }
}

void library::viewUsersByType(string type)
{
    cout << "=========== USERS OF TYPE: " << type << " ===========" << endl;

    for (int i = 0; i < (int)users.size(); i++)
    {
        User *u = users[i];
        if (u->get_usertype() == type)
        {
            cout << "| NAME         : " << u->get_name() << endl
                 << "| EMAIL        : " << u->get_email() << endl
                 << "| PHONE        : " << u->get_phone() << endl
                 << "| USER TYPE    : " << u->get_usertype() << endl
                 << "| MEMBERSHIP   : " << (u->hasMembership() ? "ACTIVE" : "NONE") << endl
                 << "--------------------------------------------" << endl;
        }
    }
}

void library::viewAllMemberships()
{
    cout << "=============== ALL MEMBERSHIPS ===============" << endl;

    bool found = false;

    for (int i = 0; i < (int)users.size(); i++)
    {
        User *u = users[i];
        if (u->getMembership())
        {
            cout << *u->getMembership();
            found = true;
        }
    }

    if (!found)
        cout << "NO MEMBERSHIPS FOUND." << endl;
}

void library::reserveBook(int uid, int bkid, string date, string start, string end)
{
    Book *book = catalog.SearchbyID(bkid);
    if (!book)
        throw BookException("BOOK NOT FOUND WITH ID: " + to_string(bkid));

    if (book->getAvailableCopies() <= 0)
        throw BookException("NO COPIES AVAILABLE TO RESERVE FOR BOOK ID: " + to_string(bkid));

    reservations.emplace_back(uid, bkid, 0, date, start, end);
    saveReservationToFile(reservations.back());
    book->decreaseCopy();
    cout << "BOOK RESERVED SUCCESSFULLY!\n";
}

void library::reserveTableManual(int uid, int tid, string date, string start, string end)
{
    bool tableExists = false;
    for (int i = 0; i < (int)tables.size(); i++)
        if (tables[i].get_id() == tid)
        {
            tableExists = true;
            break;
        }

    if (!tableExists)
        throw ReservationException("TABLE ID " + to_string(tid) + " DOES NOT EXIST!");

    for (int i = 0; i < (int)reservations.size(); i++)
    {
        Reservation &r = reservations[i];
        if (r.get_tableid() == tid && r.get_date() == date && r.isActive())
        {
            if (Reservation::overlap(r.get_start(), r.get_end(), start, end))
                throw ReservationException("TABLE " + to_string(tid) + " IS ALREADY BOOKED FOR THAT TIME SLOT!");
        }
    }

    reservations.emplace_back(uid, 0, tid, date, start, end);
    saveReservationToFile(reservations.back());
    cout << "TABLE RESERVED MANUALLY!\n";
}

void library::reserveTableAuto(int uid, string date, string start, string end)
{
    if (tables.empty())
        throw ReservationException("NO TABLES EXIST IN THE LIBRARY!");

    for (int i = 0; i < (int)tables.size(); i++)
    {
        int tid      = tables[i].get_id();
        bool conflict = false;

        for (int j = 0; j < (int)reservations.size(); j++)
        {
            Reservation &r = reservations[j];
            if (r.get_tableid() == tid && r.get_date() == date && r.isActive())
            {
                if (Reservation::overlap(r.get_start(), r.get_end(), start, end))
                {
                    conflict = true;
                    break;
                }
            }
        }

        if (!conflict)
        {
            reservations.emplace_back(uid, 0, tid, date, start, end);
            saveReservationToFile(reservations.back());
            cout << "AUTO ASSIGNED TABLE ID: " << tid << endl;
            return;
        }
    }

    throw ReservationException("NO TABLES AVAILABLE FOR THE SELECTED TIME SLOT!");
}

void library::cancelReservation(int rid, int uid)
{
    for (int i = 0; i < (int)reservations.size(); i++)
    {
        if (reservations[i].get_reservationid() == rid)
        {
            if (reservations[i].get_userid() != uid)
                throw ReservationException("YOU CAN ONLY CANCEL YOUR OWN RESERVATIONS!");

            reservations[i].cancel();
            cout << "RESERVATION CANCELLED!" << endl;
            return;
        }
    }
    throw ReservationException("RESERVATION ID NOT FOUND!");
}

void library::autoExpireReservations()
{
    bool anyExpired = false;
    for (int i = 0; i < (int)reservations.size(); i++)
    {
        if (reservations[i].isActive() && reservations[i].isExpired())
        {
            reservations[i].cancel();
            anyExpired = true;
        }
    }
    if (anyExpired)
    {
        try
        {
            rewriteReservationsFile(reservations);
        }
        catch (FileException &e)
        {
            cout << e.what() << endl;
        }
    }
}

void library::addTable(string loc, int cap)
{
    if ((int)tables.size() >= MAX_TABLES)
        throw ReservationException("TABLE LIMIT REACHED! MAXIMUM " + to_string(MAX_TABLES) + " TABLES ALLOWED.");

    tables.push_back(Table(loc, cap));
    saveTableToFile(tables.back());
    cout << "TABLE ADDED! (" << tables.size() << "/" << MAX_TABLES << " TABLES)" << endl;
}

void library::removeTable(int tid)
{
    for (int i = 0; i < (int)tables.size(); i++)
    {
        if (tables[i].get_id() == tid)
        {
            tables.erase(tables.begin() + i);
            try
            {
                rewriteTablesFile(tables);
            }
            catch (FileException &e)
            {
                cout << "FILE ERROR: " << e.what() << endl;
                cout << "TABLE REMOVED FROM MEMORY BUT FILE MAY NOT BE UPDATED." << endl;
            }
            cout << "TABLE " << tid << " REMOVED!" << endl;
            return;
        }
    }
    throw ReservationException("TABLE ID NOT FOUND!");
}

void library::viewAvailableTables(string date, string start, string end)
{
    cout << "==== AVAILABLE TABLES (" << tables.size() << "/" << MAX_TABLES << " TOTAL) ====" << endl;

    if (tables.empty())
    {
        cout << "NO TABLES IN LIBRARY. CONTACT LIBRARIAN." << endl;
        return;
    }

    bool anyAvailable = false;
    for (int i = 0; i < (int)tables.size(); i++)
    {
        int tid       = tables[i].get_id();
        bool conflict = false;

        for (int j = 0; j < (int)reservations.size(); j++)
        {
            Reservation &r = reservations[j];
            if (r.get_tableid() == tid && r.get_date() == date && r.isActive())
            {
                if (Reservation::overlap(r.get_start(), r.get_end(), start, end))
                {
                    conflict = true;
                    break;
                }
            }
        }

        if (!conflict)
        {
            cout << tables[i];
            anyAvailable = true;
        }
    }

    if (!anyAvailable)
        cout << "NO TABLES AVAILABLE FOR THE SELECTED TIME SLOT." << endl;
}

bool library::hasAvailableTable(string date, string start, string end)
{
    for (int i = 0; i < (int)tables.size(); i++)
    {
        int tid       = tables[i].get_id();
        bool conflict = false;
        for (int j = 0; j < (int)reservations.size(); j++)
        {
            Reservation &r = reservations[j];
            if (r.get_tableid() == tid && r.get_date() == date && r.isActive())
            {
                if (Reservation::overlap(r.get_start(), r.get_end(), start, end))
                {
                    conflict = true;
                    break;
                }
            }
        }
        if (!conflict)
            return true;
    }
    return false;
}

Table library::getTable(int i)
{
    if (i < 0 || i >= (int)tables.size())
        throw ReservationException("TABLE INDEX OUT OF BOUNDS: " + to_string(i));
    return tables[i];
}

// ============================================================
//                    AUTHENTICATION CLASS
// ============================================================

Authentication::Authentication(librarian *l)
{
    lib_admin = l;
}

void Authentication::setLibrarian(librarian *l)
{
    lib_admin = l;
}

User *Authentication::loginUser(library *lib)
{
    cout << "===== USER LOGIN =====" << endl;
    string username = getValidString("ENTER USERNAME: ");
    string password = getValidString("ENTER PASSWORD: ");

    for (int i = 0; i < lib->getUserCount(); i++)
    {
        User *u = lib->getUserByIndex(i);
        if (u->getUsername() == username && u->getPassword() == password)
        {
            cout << "LOGIN SUCCESSFUL! WELCOME " << u->get_name() << endl;
            return u;
        }
    }
    throw AuthException("INVALID USERNAME OR PASSWORD!");
}

librarian *Authentication::loginLibrarian()
{
    if (!lib_admin)
        throw AuthException("NO LIBRARIAN REGISTERED IN THE SYSTEM!");

    cout << "\n===== LIBRARIAN LOGIN =====" << endl;
    string uname = getValidString("ENTER USERNAME: ");
    string pass  = getValidString("ENTER PASSWORD: ");

    if (lib_admin->getUsername() == uname && lib_admin->getPassword() == pass)
    {
        cout << "LIBRARIAN LOGIN SUCCESSFUL!" << endl;
        return lib_admin;
    }
    throw AuthException("INVALID LIBRARIAN CREDENTIALS!");
}

void Authentication::logout()
{
    cout << "LOGGED OUT SUCCESSFULLY!\n";
}

// ============================================================
//                    LIBRARIAN MENUS
// ============================================================

// View system details (librarian info, users, transactions, fines, reservations, catalog, memberships)
void librarian::viewFullDetails(library *lib)
{
    int choice = 0;

    do
    {
        cout << "\n=============== LIBRARIAN DASHBOARD ===============" << endl;
        cout << "1. VIEW LIBRARIAN INFO" << endl;
        cout << "2. VIEW ALL USERS" << endl;
        cout << "3. VIEW USER BY TYPE" << endl;
        cout << "4. VIEW ALL TRANSACTIONS" << endl;
        cout << "5. VIEW ALL FINES" << endl;
        cout << "6. VIEW ALL RESERVATIONS" << endl;
        cout << "7. VIEW BOOK CATALOG" << endl;
        cout << "8. VIEW ALL MEMBERSHIPS" << endl;
        cout << "9. EXIT" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 9);

        switch (choice)
        {
        case 1:
            cout << "\n--------------- LIBRARIAN INFO ---------------" << endl;
            cout << *this;
            break;

        case 2:
            cout << "\n--------------- ALL USERS ---------------" << endl;
            lib->viewUsers();
            break;

        case 3:
        {
            cout << "\n--------------- USERS BY TYPE ---------------" << endl;
            int typeChoice = getValidIntInRange("SELECT USER TYPE:\n1. STUDENT\n2. FACULTY\n3. GUEST\nCHOICE: ", 1, 3);
            string type;
            if (typeChoice == 1)      type = "STUDENT";
            else if (typeChoice == 2) type = "FACULTY";
            else                      type = "GUEST";

            lib->viewUsersByType(type);
            break;
        }
        case 4:
            cout << "\n------------ ALL TRANSACTIONS ------------" << endl;
            lib->viewTransactions();
            break;

        case 5:
            cout << "\n---------------- FINES ----------------" << endl;
            lib->viewFines();
            break;

        case 6:
            cout << "\n------------- RESERVATIONS -------------" << endl;
            lib->viewAllReservations();
            break;

        case 7:
            cout << "\n------------- BOOK CATALOG -------------" << endl;
            lib->getCatalog()->ViewBookCatalog();
            break;

        case 8:
            cout << "\n------------ ALL MEMBERSHIPS ------------" << endl;
            lib->viewAllMemberships();
            break;

        case 9:
            cout << "EXITING LIBRARIAN DASHBOARD..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 9);
}

// Manage books (add, remove)
void librarian::manageBooks(library *lib)
{
    int choice = 0;

    do
    {
        cout << "\n================ BOOK MANAGEMENT ================" << endl;
        cout << "1. ADD BOOK" << endl;
        cout << "2. REMOVE BOOK" << endl;
        cout << "3. BACK" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 3);

        switch (choice)
        {
        case 1:
        {
            string title     = getValidString("ENTER TITLE: ");
            string author    = getValidAlphaString("ENTER AUTHOR: ");
            string category  = getValidAlphaString("ENTER CATEGORY: ");
            string isbn      = getValidISBN("ENTER ISBN: ");
            string publisher = getValidPublisher("ENTER PUBLISHER: ");
            int yop          = getValidIntInRange("ENTER YEAR: ", 1000, 2026);
            int copies       = getValidIntInRange("ENTER AVAILABLE COPIES: ", 1, 1000);

            try
            {
                // Check if book with same title already exists
                Book *existing = lib->getCatalog()->SearchbyTitle(title);
                if (existing != nullptr)
                {
                    cout << "A BOOK WITH THIS TITLE ALREADY EXISTS!" << endl;
                    break;
                }
                Book *b = new Book(title, author, category, isbn, publisher, yop);
                b->setAvailableCopies(copies);
                addbook(b, lib->getCatalog());
                saveBookToFile(b);
                cout << "ALLOTTED ID: " << b->get_bookid() << endl;
            }
            catch (FileException &e)
            {
                cout << e.what() << endl;
            }
            break;
        }

        case 2:
        {
            string title = getValidString("ENTER TITLE TO REMOVE: ");
            try
            {
                removeBook(title, lib->getCatalog());
                rewriteBooksFile(lib->getCatalog());
            }
            catch (FileException &e)
            {
                cout << e.what() << endl;
            }
            break;
        }

        case 3:
            cout << "RETURNING..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 3);
}

// Manage tables (add, view, remove)
void librarian::manageTables(library *lib)
{
    int choice;

    do
    {
        cout << "\n================ TABLE MANAGEMENT ================" << endl;
        cout << "TOTAL TABLES: " << lib->getTableCount() << " / " << library::MAX_TABLES << endl;
        cout << "1. ADD TABLE" << endl;
        cout << "2. VIEW ALL TABLES" << endl;
        cout << "3. REMOVE TABLE" << endl;
        cout << "4. BACK" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 4);

        switch (choice)
        {
        case 1:
        {
            string loc = getValidString("ENTER LOCATION: ");
            int cap    = getValidIntInRange("ENTER CAPACITY: ", 1, 100);

            try
            {
                lib->addTable(loc, cap);
            }
            catch (ReservationException &e)
            {
                cout << e.what() << endl;
            }
            break;
        }

        case 2:
            lib->viewTables();
            break;

        case 3:
        {
            lib->viewTables();
            int tid = getValidInt("ENTER TABLE ID TO REMOVE: ");
            lib->removeTable(tid);
            break;
        }

        case 4:
            cout << "RETURNING..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 4);
}

// Main librarian menu
void librarian::librarianMainMenu(library *lib)
{
    int choice;

    do
    {
        cout << "\n================ LIBRARIAN MAIN MENU ================" << endl;
        cout << "1. VIEW SYSTEM DETAILS" << endl;
        cout << "2. MANAGE BOOKS" << endl;
        cout << "3. MANAGE TABLES" << endl;
        cout << "4. EXIT" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 4);

        switch (choice)
        {
        case 1: viewFullDetails(lib); break;
        case 2: manageBooks(lib);     break;
        case 3: manageTables(lib);    break;
        case 4: cout << "EXITING LIBRARIAN SYSTEM..." << endl; break;
        default: cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 4);
}
