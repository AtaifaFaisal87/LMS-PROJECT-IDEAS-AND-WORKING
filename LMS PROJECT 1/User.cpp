// ============================================================
// User.cpp
// Contains: User class function implementations,
//           Student/Faculty/Guest printTo,
//           file I/O functions, and all user menus.
// ============================================================

#include "User.h"
#include "Book.h"      // needed by menus (SearchbyID, ViewBookCatalog, etc.)
#include "library.h"   // needed by menus

// ---- Static member definition ----
int User::next_id = 1;

// ============================================================
//                    USER CLASS
// ============================================================

User::User(string n, string c, string e, string p)
{
    userid      = next_id++;
    name        = n;
    CNIC        = c;
    email       = e;
    phonenumber = p;
    membership  = nullptr;
}

User::~User()
{
    delete membership;
}

// Print user using polymorphism (calls subclass printTo)
ostream &operator<<(ostream &os, const User &u)
{
    u.printTo(os);
    return os;
}

// Borrow a book (adds book ID to borrowed list)
bool User::borrowbook(int id)
{
    if ((int)borrowedbook.size() < getborrowlimit())
    {
        borrowedbook.push_back(id);
        return true;
    }
    if (!RuntimeLogger::isSilent())
        cout << "BORROW LIMIT REACHED!" << endl;
    return false;
}

// Return a book (removes book ID from borrowed list)
bool User::returnbook(int id)
{
    for (int i = 0; i < (int)borrowedbook.size(); i++)
    {
        if (borrowedbook[i] == id)
        {
            borrowedbook.erase(borrowedbook.begin() + i);
            return true;
        }
    }
    cout << "BOOK NOT BORROWED BY THIS USER!" << endl;
    return false;
}

// Activate a new membership for this user
void User::activateMembership(int durationDays)
{
    if (hasMembership())
    {
        cout << "USER ALREADY HAS AN ACTIVE MEMBERSHIP!" << endl;
        return;
    }
    delete membership;
    membership = new Membership(userid, get_usertype(), name, durationDays);
    cout << "\nMEMBERSHIP ACTIVATED SUCCESSFULLY!" << endl;
    cout << *membership;
}

// Cancel the membership
void User::cancelMembership()
{
    if (!membership)
    {
        cout << "NO MEMBERSHIP TO CANCEL!" << endl;
        return;
    }
    membership->cancel();
}

// View membership details
void User::viewMembership()
{
    if (!membership)
    {
        cout << "YOU DO NOT HAVE A MEMBERSHIP.PURCHASE ONE FROM THE MENU!" << endl;
        return;
    }
    cout << *membership;
}

// Restore membership from saved file data
void User::restoreMembership(time_t activation, time_t expiry)
{
    delete membership;
    membership = new Membership(userid, get_usertype(), name, activation, expiry);
}

// Borrow limit including membership boost
int User::getEffectiveBorrowLimit()
{
    return getborrowlimit() + (hasMembership() ? membership->getBorrowBoost() : 0);
}

// Apply membership discount to a fine amount
double User::applyMembershipDiscount(double fine)
{
    if (hasMembership())
        return membership->applyDiscount(fine);
    return fine;
}

// ============================================================
//              STUDENT / FACULTY / GUEST printTo
// ============================================================

void Student::printTo(ostream &os) const
{
    os << "============= STUDENT DETAILS =============" << endl
       << "| ID             : " << userid << endl
       << "| NAME           : " << name << endl
       << "| SEAT NO        : " << seatno << endl
       << "| MAJOR          : " << major << endl
       << "| USER TYPE      : STUDENT" << endl
       << "===========================================" << endl;
}

void Faculty::printTo(ostream &os) const
{
    os << "============= FACULTY DETAILS =============" << endl
       << "| ID             : " << userid << endl
       << "| NAME           : " << name << endl
       << "| DEPARTMENT     : " << department << endl
       << "| DESIGNATION    : " << designation << endl
       << "| USER TYPE      : FACULTY" << endl
       << "===========================================" << endl;
}

void Guest::printTo(ostream &os) const
{
    os << "============== GUEST DETAILS ==============" << endl
       << "| ID             : " << userid << endl
       << "| NAME           : " << name << endl
       << "| PURPOSE        : " << purpose << endl
       << "| USER TYPE      : GUEST" << endl
       << "===========================================" << endl;
}

// ============================================================
//              USER FILE I/O
// ============================================================

// Save a single user to the binary file (append mode)
void saveUserToFile(User *u)
{
    ofstream file("users.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN users.dat FOR WRITING!");

    UserRecord ur = {};

    ur.id = u->get_userid();
    strncpy(ur.name,     u->get_name().c_str(),     sizeof(ur.name) - 1);
    strncpy(ur.email,    u->get_email().c_str(),    sizeof(ur.email) - 1);
    strncpy(ur.phone,    u->get_phone().c_str(),    sizeof(ur.phone) - 1);
    strncpy(ur.cnic,     u->get_cnic().c_str(),     sizeof(ur.cnic) - 1);
    strncpy(ur.type,     u->get_usertype().c_str(), sizeof(ur.type) - 1);
    strncpy(ur.username, u->getUsername().c_str(),  sizeof(ur.username) - 1);
    strncpy(ur.password, u->getPassword().c_str(),  sizeof(ur.password) - 1);

    // Downcast to access subclass-specific fields
    if (u->get_usertype() == "STUDENT")
    {
        Student *s = dynamic_cast<Student *>(u);
        strncpy(ur.seatno, s->getSeatNo().c_str(), sizeof(ur.seatno) - 1);
        strncpy(ur.major,  s->getMajor().c_str(),  sizeof(ur.major) - 1);
    }
    else if (u->get_usertype() == "FACULTY")
    {
        Faculty *f = dynamic_cast<Faculty *>(u);
        strncpy(ur.department,  f->getDepartment().c_str(),  sizeof(ur.department) - 1);
        strncpy(ur.designation, f->getDesignation().c_str(), sizeof(ur.designation) - 1);
    }
    else if (u->get_usertype() == "GUEST")
    {
        Guest *g = dynamic_cast<Guest *>(u);
        strncpy(ur.purpose, g->getPurpose().c_str(), sizeof(ur.purpose) - 1);
    }

    ur.borrowed_count = u->getBorrowedCount();
    for (int i = 0; i < ur.borrowed_count && i < 10; i++)
        ur.borrowed_ids[i] = u->getBorrowedBooks()[i];

    if (u->getMembership() && u->getMembership()->isValid())
    {
        ur.hasMembership         = true;
        ur.membershipActivation  = u->getMembership()->getActivationTime();
        ur.membershipExpiry      = u->getMembership()->getExpiryTime();
        ur.membershipDurationDays = 365;
    }
    else
    {
        ur.hasMembership         = false;
        ur.membershipActivation  = 0;
        ur.membershipExpiry      = 0;
        ur.membershipDurationDays = 0;
    }

    file.write((char *)&ur, sizeof(ur));
    file.close();
}

// Load all users from the binary file
void loadUsersFromFile(vector<User *> &users)
{
    ifstream file("users.dat", ios::binary);
    if (!file)
        return;

    UserRecord ur;

    while (file.read((char *)&ur, sizeof(ur)))
    {
        User *u = nullptr;
        string type = ur.type;

        if (type == "STUDENT")
            u = new Student(ur.name, ur.cnic, ur.email, ur.phone, ur.seatno, ur.major);
        else if (type == "FACULTY")
            u = new Faculty(ur.name, ur.cnic, ur.email, ur.phone, ur.department, ur.designation);
        else
            u = new Guest(ur.name, ur.cnic, ur.email, ur.phone, ur.purpose);

        u->setid(ur.id);
        u->setUsername(ur.username);
        u->setPassword(ur.password);

        if (ur.hasMembership)
            u->restoreMembership(ur.membershipActivation, ur.membershipExpiry);

        for (int i = 0; i < ur.borrowed_count; i++)
            u->borrowbook(ur.borrowed_ids[i]);

        users.push_back(u);
    }

    file.close();
}

// Overwrite the entire users file (used after edits)
void rewriteUsersFile(vector<User *> &users)
{
    ofstream file("users.dat", ios::binary); // no ios::app — overwrites entire file

    for (int i = 0; i < (int)users.size(); i++)
    {
        User *u = users[i];
        UserRecord ur = {};

        ur.id = u->get_userid();
        strncpy(ur.name,     u->get_name().c_str(),     sizeof(ur.name) - 1);
        strncpy(ur.email,    u->get_email().c_str(),    sizeof(ur.email) - 1);
        strncpy(ur.phone,    u->get_phone().c_str(),    sizeof(ur.phone) - 1);
        strncpy(ur.cnic,     u->get_cnic().c_str(),     sizeof(ur.cnic) - 1);
        strncpy(ur.type,     u->get_usertype().c_str(), sizeof(ur.type) - 1);
        strncpy(ur.username, u->getUsername().c_str(),  sizeof(ur.username) - 1);
        strncpy(ur.password, u->getPassword().c_str(),  sizeof(ur.password) - 1);

        if (u->get_usertype() == "STUDENT")
        {
            Student *s = dynamic_cast<Student *>(u);
            strncpy(ur.seatno, s->getSeatNo().c_str(), sizeof(ur.seatno) - 1);
            strncpy(ur.major,  s->getMajor().c_str(),  sizeof(ur.major) - 1);
        }
        else if (u->get_usertype() == "FACULTY")
        {
            Faculty *f = dynamic_cast<Faculty *>(u);
            strncpy(ur.department,  f->getDepartment().c_str(),  sizeof(ur.department) - 1);
            strncpy(ur.designation, f->getDesignation().c_str(), sizeof(ur.designation) - 1);
        }
        else if (u->get_usertype() == "GUEST")
        {
            Guest *g = dynamic_cast<Guest *>(u);
            strncpy(ur.purpose, g->getPurpose().c_str(), sizeof(ur.purpose) - 1);
        }

        ur.borrowed_count = u->getBorrowedCount();
        for (int j = 0; j < ur.borrowed_count && j < 10; j++)
            ur.borrowed_ids[j] = u->getBorrowedBooks()[j];

        if (u->getMembership() && u->getMembership()->isValid())
        {
            ur.hasMembership          = true;
            ur.membershipActivation   = u->getMembership()->getActivationTime();
            ur.membershipExpiry       = u->getMembership()->getExpiryTime();
            ur.membershipDurationDays = 365;
        }
        else
        {
            ur.hasMembership          = false;
            ur.membershipActivation   = 0;
            ur.membershipExpiry       = 0;
            ur.membershipDurationDays = 0;
        }

        file.write((char *)&ur, sizeof(ur));
    }

    file.close();
}

// ============================================================
//              REGISTER NEW USER
// ============================================================
User *User::registerUser()
{
    int choice = getValidIntInRange("SELECT USER TYPE:\n1. STUDENT\n2. FACULTY\n3. GUEST\nCHOICE: ", 1, 3);

    string name     = getValidAlphaString("ENTER NAME: ");
    string CNIC     = getValidCNIC("ENTER CNIC: ");
    string email    = getValidEmail("ENTER EMAIL: ");
    string phone    = getValidPhone("ENTER PHONE: ");
    string username = getValidString("ENTER USERNAME: ");
    string password = getValidPassword("ENTER PASSWORD: ");

    if (choice == 1)
    {
        string seat  = getValidString("ENTER SEAT NUMBER: ");
        string major = getValidAlphaString("ENTER MAJOR: ");
        User *u = new Student(name, CNIC, email, phone, seat, major);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }
    else if (choice == 2)
    {
        string dept = getValidAlphaString("ENTER DEPARTMENT: ");
        string des  = getValidAlphaString("ENTER DESIGNATION: ");
        User *u = new Faculty(name, CNIC, email, phone, dept, des);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }
    else
    {
        string purpose = getValidString("ENTER PURPOSE: ");
        User *u = new Guest(name, CNIC, email, phone, purpose);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }
}

// ============================================================
//              USER MENUS
// ============================================================

// View user dashboard (data, borrowed books, transactions, fines, reservations)
void User::viewFullDetails(library *lib)
{
    int choice;

    do
    {
        cout << "\n================ USER DASHBOARD ================" << endl;
        cout << "1. VIEW YOUR DATA" << endl;
        cout << "2. VIEW BORROWED BOOKS" << endl;
        cout << "3. VIEW TRANSACTIONS" << endl;
        cout << "4. VIEW FINES" << endl;
        cout << "5. VIEW RESERVATIONS" << endl;
        cout << "6. EXIT" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 6);

        switch (choice)
        {
        case 1:
            cout << "\n--------------- USER DETAILS ---------------" << endl;
            cout << *this;
            cout << "BORROW LIMIT  : " << getEffectiveBorrowLimit()
                 << (hasMembership() ? " [MEMBERSHIP ACTIVE]" : " [NO MEMBERSHIP]") << endl;
            cout << "FINE DISCOUNT : " << (hasMembership() ? "15% (MEMBERSHIP ACTIVE)" : "NONE") << endl;
            break;

        case 2:
        {
            cout << "\n| BORROWED BOOKS : " << getBorrowedCount() << endl;

            if (borrowedbook.empty())
            {
                cout << "|   NO BOOKS BORROWED YET." << endl;
            }
            else
            {
                cout << "|" << endl;
                for (int i = 0; i < (int)borrowedbook.size(); i++)
                {
                    Book *b = lib->getCatalog()->SearchbyID(borrowedbook[i]);
                    if (b)
                    {
                        cout << "|   " << (i + 1) << ". " << b->get_title()
                             << " by " << b->get_author()
                             << " (ID: " << b->get_bookid() << ")" << endl;
                    }
                }
            }
            break;
        }

        case 3:
            cout << "\n------------ TRANSACTIONS ------------" << endl;
            lib->viewUserTransactions(userid);
            break;

        case 4:
            cout << "\n--------------- FINES ---------------" << endl;
            lib->viewUserFines(userid);
            break;

        case 5:
            cout << "\n----------- RESERVATIONS -----------" << endl;
            lib->viewUserReservations(userid);
            break;

        case 6:
            cout << "EXITING DASHBOARD..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 6);
}

// Search books by various criteria
void User::searchBooks(library *lib)
{
    int choice = 0;

    do
    {
        cout << "\n============= BOOK SEARCH MENU =============" << endl;
        cout << "1. SEARCH BY ID" << endl;
        cout << "2. SEARCH BY TITLE" << endl;
        cout << "3. SEARCH BY AUTHOR" << endl;
        cout << "4. SEARCH BY CATEGORY" << endl;
        cout << "5. SEARCH BY PUBLISHER" << endl;
        cout << "6. SEARCH BY YEAR" << endl;
        cout << "7. EXIT" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 7);

        Book *b = nullptr;

        switch (choice)
        {
        case 1:
        {
            int id = getValidInt("ENTER BOOK ID: ");
            b = lib->getCatalog()->SearchbyID(id);
            break;
        }
        case 2:
        {
            string t = getValidString("ENTER TITLE: ");
            b = lib->getCatalog()->SearchbyTitle(t);
            break;
        }
        case 3:
        {
            string a = getValidString("ENTER AUTHOR: ");
            vector<Book *> results = lib->getCatalog()->SearchbyAuthor(a);
            if (results.empty())
                cout << "NO BOOKS FOUND!" << endl;
            else
                for (int i = 0; i < (int)results.size(); i++)
                    cout << *results[i];
            break;
        }
        case 4:
        {
            string c = getValidString("ENTER CATEGORY: ");
            vector<Book *> results = lib->getCatalog()->SearchbyCategory(c);
            if (results.empty())
                cout << "NO BOOKS FOUND!" << endl;
            else
                for (int i = 0; i < (int)results.size(); i++)
                    cout << *results[i];
            break;
        }
        case 5:
        {
            string p = getValidString("ENTER PUBLISHER: ");
            vector<Book *> results = lib->getCatalog()->SearchbyPublisher(p);
            if (results.empty())
                cout << "NO BOOKS FOUND!" << endl;
            else
                for (int i = 0; i < (int)results.size(); i++)
                    cout << *results[i];
            break;
        }
        case 6:
        {
            int y = getValidIntInRange("ENTER YEAR: ", 1000, 2026);
            vector<Book *> results = lib->getCatalog()->SearchbyYOP(y);
            if (results.empty())
                cout << "NO BOOKS FOUND!" << endl;
            else
                for (int i = 0; i < (int)results.size(); i++)
                    cout << *results[i];
            break;
        }
        case 7:
            cout << "EXITING SEARCH MENU..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

        if (b != nullptr)
        {
            cout << "\n--------------- BOOK FOUND ---------------" << endl;
            cout << *b;
        }
        else if (choice == 1 || choice == 2)
            cout << "BOOK NOT FOUND!" << endl;

    } while (choice != 7);
}

// Reservation menu (tables and books)
void User::reservationMenu(library *lib)
{
    int choice = 0;

    do
    {
        cout << "\n============= RESERVATION MENU =============" << endl;
        cout << "1. AUTO RESERVE TABLE" << endl;
        cout << "2. MANUAL RESERVE TABLE" << endl;
        cout << "3. RESERVE BOOK" << endl;
        cout << "4. VIEW MY RESERVATIONS" << endl;
        cout << "5. CANCEL A RESERVATION" << endl;
        cout << "6. EXIT" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 6);

        switch (choice)
        {
        case 1:
        {
            while (true)
            {
                string date  = getValidDate("ENTER DATE (DD/MM/YYYY): ");
                string start = getValidTime("ENTER START TIME (HH:MM): ");
                string end   = getValidTime("ENTER END TIME (HH:MM): ");

                if (!isValidReservationTime(date, start, end))
                    continue;

                try
                {
                    lib->reserveTableAuto(userid, date, start, end);
                }
                catch (ReservationException &e)
                {
                    cout << e.what() << endl;
                }
                break;
            }
            break;
        }
        case 2:
        {
            while (true)
            {
                string date  = getValidDate("ENTER DATE (DD/MM/YYYY): ");
                string start = getValidTime("ENTER START TIME (HH:MM): ");
                string end   = getValidTime("ENTER END TIME (HH:MM): ");

                if (!isValidReservationTime(date, start, end))
                    continue;

                cout << "\n--------------- AVAILABLE TABLES ---------------" << endl;
                lib->viewAvailableTables(date, start, end);
                cout << "------------------------------------------------" << endl;

                if (!lib->hasAvailableTable(date, start, end))
                    break;

                int tid = getValidInt("ENTER TABLE ID: ");
                try
                {
                    lib->reserveTableManual(userid, tid, date, start, end);
                }
                catch (ReservationException &e)
                {
                    cout << e.what() << endl;
                }
                break;
            }
            break;
        }
        case 3:
        {
            while (true)
            {
                int bid      = getValidInt("ENTER BOOK ID: ");
                string date  = getValidDate("ENTER DATE (DD/MM/YYYY): ");
                string start = getValidTime("ENTER START TIME (HH:MM): ");
                string end   = getValidTime("ENTER END TIME (HH:MM): ");

                if (!isValidReservationTime(date, start, end))
                    continue;

                try
                {
                    lib->reserveBook(userid, bid, date, start, end);
                }
                catch (BookException &e)
                {
                    cout << e.what() << endl;
                }
                catch (ReservationException &e)
                {
                    cout << e.what() << endl;
                }
                break;
            }
            break;
        }
        case 4:
            cout << "\n----------- YOUR RESERVATIONS -----------" << endl;
            lib->viewUserReservations(userid);
            break;

        case 5:
        {
            cout << "\n----------- YOUR RESERVATIONS -----------" << endl;
            lib->viewUserReservations(userid);

            int rid = getValidInt("ENTER RESERVATION ID TO CANCEL: ");
            try
            {
                lib->cancelReservation(rid, userid);
                rewriteReservationsFile(lib->getReservationsVector());
            }
            catch (ReservationException &e)
            {
                cout << e.what() << endl;
            }
            break;
        }
        case 6:
            cout << "EXITING RESERVATION MENU..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 6);
}

// Book actions menu (borrow, return, read on site, download)
void User::bookActionMenu(library *lib)
{
    int choice = 0;
    string title;

    do
    {
        cout << "\n================ BOOK ACTIONS ================" << endl;
        cout << "1. BORROW BOOK" << endl;
        cout << "2. RETURN BOOK" << endl;
        cout << "3. READ ON SITE" << endl;
        cout << "4. DOWNLOAD BOOK" << endl;
        cout << "5. EXIT" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 5);

        switch (choice)
        {
        case 1:
        {
            lib->getCatalog()->ViewBookCatalog();
            int duration = 0;

            title = getValidString("ENTER BOOK TITLE: ");

            int unit = getValidIntInRange("\nSELECT TIME UNIT:\n1. MINUTES\n2. HOURS\n3. DAYS\nCHOICE: ", 1, 3);

            if (unit == 1)
            {
                int mins = getValidIntInRange("ENTER MINUTES: ", 1, 10080);
                duration = mins;
            }
            else if (unit == 2)
            {
                int hours = getValidIntInRange("ENTER HOURS: ", 1, 168);
                duration = hours * 60;
            }
            else if (unit == 3)
            {
                int days = getValidIntInRange("ENTER DAYS: ", 1, 7);
                duration = days * 24 * 60;
            }

            try
            {
                lib->borrowBook(this, title, duration);
                rewriteBooksFile(lib->getCatalog());
            }
            catch (TransactionException &e) { cout << e.what() << endl; }
            catch (BookException &e)        { cout << e.what() << endl; }
            catch (UserException &e)        { cout << e.what() << endl; }
            catch (FileException &e)        { cout << e.what() << endl; }
            break;
        }

        case 2:
        {
            cout << "\n===== YOUR CURRENTLY BORROWED BOOKS =====" << endl;

            vector<int> activeBorrows;
            bool found = false;

            for (int i = 0; i < (int)lib->getTransactionsVector().size(); i++)
            {
                Transaction &t = lib->getTransactionsVector()[i];

                if (t.getuserid() == userid && !t.isReturned())
                {
                    Book *b = lib->getCatalog()->SearchbyID(t.getbookid());
                    if (b)
                    {
                        cout << (int)activeBorrows.size() + 1
                             << ". " << b->get_title()
                             << " (Book ID: " << b->get_bookid() << ")"
                             << " | Due: " << timeToString(t.getDueTime());
                        activeBorrows.push_back(t.getbookid());
                        found = true;
                    }
                }
            }

            if (!found)
            {
                cout << "YOU HAVE NO BOOKS TO RETURN!" << endl;
                break;
            }

            cout << "=========================================" << endl;

            int pick = getValidIntInRange("SELECT BOOK TO RETURN (number): ", 1, (int)activeBorrows.size());
            int selectedBookID = activeBorrows[pick - 1];

            Book *b = lib->getCatalog()->SearchbyID(selectedBookID);
            if (b)
            {
                try
                {
                    lib->returnBook(this, b->get_title());
                    rewriteBooksFile(lib->getCatalog());
                    rewriteTransactionsFile(lib->getTransactionsVector());
                    rewriteUsersFile(lib->getUsersVector());
                }
                catch (BookException &e)        { cout << "BOOK ERROR: " << e.what() << endl; }
                catch (TransactionException &e) { cout << "TRANSACTION ERROR: " << e.what() << endl; }
                catch (FileException &e)
                {
                    cout << "FILE ERROR: " << e.what() << endl;
                    cout << "RETURN WAS PROCESSED BUT SOME FILES MAY NOT BE UPDATED." << endl;
                }
            }
            break;
        }

        case 3:
        {
            title = getValidString("ENTER BOOK TITLE: ");
            try { lib->readOnSite(this, title); }
            catch (BookException &e) { cout << e.what() << endl; }
            break;
        }

        case 4:
        {
            title = getValidString("ENTER BOOK TITLE: ");
            try { lib->downloadBook(this, title); }
            catch (BookException &e) { cout << e.what() << endl; }
            break;
        }

        case 5:
            cout << "EXITING BOOK ACTIONS..." << endl;
            return;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 5);
}

// Membership menu (view, purchase, cancel)
void User::membershipMenu(library *lib)
{
    int choice = 0;

    do
    {
        cout << "\n================ MEMBERSHIP MENU ================" << endl;
        cout << "1. VIEW MEMBERSHIP STATUS" << endl;
        cout << "2. PURCHASE / ACTIVATE MEMBERSHIP" << endl;
        cout << "3. CANCEL MEMBERSHIP" << endl;
        cout << "4. BACK" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 4);

        switch (choice)
        {
        case 1:
            viewMembership();
            break;

        case 2:
            if (hasMembership())
            {
                cout << "YOU ALREADY HAVE AN ACTIVE MEMBERSHIP!" << endl;
                viewMembership();
            }
            else
            {
                cout << "\n=========== MEMBERSHIP BENEFITS FOR YOU ("
                     << get_usertype() << ") ===========" << endl;
                cout << "* 15% DISCOUNT ON ALL FINES" << endl;

                if (get_usertype() == "STUDENT")
                    cout << "* BORROW LIMIT: 3 -> 5 BOOKS" << endl;
                else if (get_usertype() == "FACULTY")
                    cout << "* BORROW LIMIT: 5 -> 7 BOOKS" << endl;
                else if (get_usertype() == "GUEST")
                    cout << "* BORROW LIMIT: 1 -> 2 BOOKS" << endl;

                cout << "* DURATION: 1 YEAR" << endl;

                int confirm = getValidIntInRange("\nCONFIRM MEMBERSHIP PURCHASE? (1=YES / 0=NO): ", 0, 1);

                if (confirm == 1)
                {
                    activateMembership(365);
                    rewriteUsersFile(lib->getUsersVector());
                }
                else
                    cout << "MEMBERSHIP PURCHASE CANCELLED." << endl;
            }
            break;

        case 3:
            cancelMembership();
            rewriteUsersFile(lib->getUsersVector());
            break;

        case 4:
            cout << "RETURNING TO MAIN MENU..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 4);
}

// Main user menu
void User::userMenu(library *lib)
{
    int choice = 0;

    do
    {
        cout << "\n================ USER MAIN MENU ================" << endl;
        cout << "1. VIEW DASHBOARD" << endl;
        cout << "2. BOOK SEARCH" << endl;
        cout << "3. BOOK ACTIONS (BORROW / RETURN / READ / DOWNLOAD)" << endl;
        cout << "4. RESERVATIONS" << endl;
        cout << "5. MEMBERSHIP" << endl;
        cout << "6. EXIT" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 6);

        switch (choice)
        {
        case 1: viewFullDetails(lib);  break;
        case 2: searchBooks(lib);      break;
        case 3: bookActionMenu(lib);   break;
        case 4: reservationMenu(lib);  break;
        case 5: membershipMenu(lib);   break;
        case 6: cout << "EXITING..." << endl; break;
        default: cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 6);
}
