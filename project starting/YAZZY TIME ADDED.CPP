#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <algorithm>
using namespace std;

// ============================================================
// FORWARD DECLARATIONS
// ============================================================
class Book;
class Transaction;
class Fines;
class librarian;
class BookCatalog;
class User;
class library;

// ============================================================
// REAL-TIME UTILITY FUNCTIONS
// ============================================================
time_t getCurrentTime()
{
    return time(nullptr);
}

string timeToString(time_t t)
{
    if (t == 0) return "N/A";
    struct tm *tm_info = localtime(&t);
    char buffer[30];
    strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", tm_info);
    return string(buffer);
}

int minutesDiff(time_t start, time_t end)
{
    double diff = difftime(end, start);
    return (diff > 0) ? (int)(diff / 60) : 0;
}

time_t addMinutes(time_t t, int minutes)
{
    return t + (time_t)(minutes * 60);
}

// ============================================================
// USER BASE CLASS
// ============================================================
class User
{
protected:
    int userid;
    static int next_id;
    string name;
    string CNIC;
    string email;
    string username;
    string password;
    string phonenumber;
    vector<int> borrowedbook;

public:
    User(string n = "NONE", string c = "xxxxx-xxxxxxx-x",
         string e = "abc@gmail.com", string p = "03xxxxxxxxx")
    {
        userid      = next_id++;
        name        = n;
        CNIC        = c;
        email       = e;
        phonenumber = p;
    }

    virtual ~User() {}

    // SETTERS
    void setid(int id)          { userid      = id; }
    void setname(string n)      { name        = n;  }
    void setemail(string e)     { email       = e;  }
    void setphone_no(string p)  { phonenumber = p;  }
    void setCNIC(string c)      { CNIC        = c;  }
    void setUsername(string u)  { username    = u;  }
    void setPassword(string p)  { password    = p;  }

    // GETTERS
    int    get_userid()  { return userid;      }
    string get_name()    { return name;        }
    string get_email()   { return email;       }
    string get_phone()   { return phonenumber; }
    string get_cnic()    { return CNIC;        }
    string getUsername() { return username;    }
    string getPassword() { return password;    }

    const vector<int>& getBorrowedBooks()  { return borrowedbook;           }
    int                getBorrowedCount()  { return (int)borrowedbook.size(); }

    // PURE VIRTUALS
    virtual int    getborrowlimit() = 0;
    virtual string get_usertype()   = 0;
    virtual void   viewdetails()    = 0;
    virtual int    getFineRate()    = 0;

    bool borrowbook(int id)
    {
        if ((int)borrowedbook.size() < getborrowlimit())
        {
            borrowedbook.push_back(id);
            return true;
        }
        cout << "Borrow limit reached!" << endl;
        return false;
    }

    bool returnbook(int id)
    {
        for (int i = 0; i < (int)borrowedbook.size(); i++)
        {
            if (borrowedbook[i] == id)
            {
                borrowedbook.erase(borrowedbook.begin() + i);
                return true;
            }
        }
        cout << "Book not borrowed by this user!" << endl;
        return false;
    }

    void viewFullDetails(library* lib);
    void searchBooks(library* lib);
    void userMenu(library* lib);

    static User* registerUser();
};

int User::next_id = 1;

// ============================================================
// STUDENT
// ============================================================
class Student : public User
{
private:
    string seatno;
    string major;

public:
    Student(string n, string c, string e, string p, string s, string m)
        : User(n, c, e, p), seatno(s), major(m) {}

    int    getborrowlimit() override { return 3; }
    string get_usertype()   override { return "STUDENT"; }
    int    getFineRate()    override { return 10; }

    void viewdetails() override
    {
        cout << "|NAME: "  << name
             << " |SEAT: " << seatno
             << " |MAJOR: "<< major
             << " |TYPE: STUDENT|" << endl;
    }
};

// ============================================================
// FACULTY
// ============================================================
class Faculty : public User
{
private:
    string department;
    string designation;

public:
    Faculty(string n, string c, string e, string p, string dept, string des)
        : User(n, c, e, p), department(dept), designation(des) {}

    int    getborrowlimit() override { return 5; }
    string get_usertype()   override { return "FACULTY"; }
    int    getFineRate()    override { return 15; }

    void viewdetails() override
    {
        cout << "|NAME: "         << name
             << " |DEPARTMENT: "  << department
             << " |DESIGNATION: " << designation
             << " |TYPE: FACULTY|" << endl;
    }
};

// ============================================================
// GUEST
// ============================================================
class Guest : public User
{
private:
    string purpose;

public:
    Guest(string n, string c, string e, string p, string ps)
        : User(n, c, e, p), purpose(ps) {}

    int    getborrowlimit() override { return 1; }
    string get_usertype()   override { return "GUEST"; }
    int    getFineRate()    override { return 20; }

    void viewdetails() override
    {
        cout << "|NAME: "    << name
             << " |PURPOSE: "<< purpose
             << " |TYPE: GUEST|" << endl;
    }
};

// ============================================================
// REGISTER USER â€” FIXED (no double password read)
// ============================================================
User* User::registerUser()
{
    int choice;
    cout << "\nSelect User Type:\n1. Student\n2. Faculty\n3. Guest\nChoice: ";
    cin >> choice;
    cin.ignore();

    string name, CNIC, email, phone, username, password;

    cout << "Enter Username: ";
    getline(cin, username);
    cout << "Enter Password: ";
    getline(cin, password);
    cout << "Enter Name: ";
    getline(cin, name);
    cout << "Enter CNIC: ";
    getline(cin, CNIC);
    cout << "Enter Email: ";
    getline(cin, email);
    cout << "Enter Phone: ";
    getline(cin, phone);

    User* u = nullptr;

    if (choice == 1)
    {
        string seat, major;
        cout << "Enter Seat Number: ";
        getline(cin, seat);
        cout << "Enter Major: ";
        getline(cin, major);
        u = new Student(name, CNIC, email, phone, seat, major);
    }
    else if (choice == 2)
    {
        string dept, des;
        cout << "Enter Department: ";
        getline(cin, dept);
        cout << "Enter Designation: ";
        getline(cin, des);
        u = new Faculty(name, CNIC, email, phone, dept, des);
    }
    else if (choice == 3)
    {
        string purpose;
        cout << "Enter Purpose: ";
        getline(cin, purpose);
        u = new Guest(name, CNIC, email, phone, purpose);
    }
    else
    {
        cout << "Invalid choice!\n";
        return nullptr;
    }

    u->setUsername(username);
    u->setPassword(password);
    return u;
}

// ============================================================
// BOOK
// ============================================================
class Book
{
private:
    int    bookid;
    string title;
    string author;
    string category;
    string ISBN;
    string publisher;
    int    year_of_publication;
    int    available_copies;
    bool   availability_status;
    int    download_count;
    int    read_onsite_count;
    int    timesborrowed_count;

public:
    Book() : bookid(0), title("UNKNOWN"), author("UNKNOWN"), category("UNKNOWN"),
             ISBN("UNKNOWN"), publisher("UNKNOWN"), year_of_publication(0),
             available_copies(0), availability_status(false),
             download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

    Book(int id, string t, string a, string c, string isbn, string p, int yop)
        : bookid(id), title(t), author(a), category(c),
          ISBN(isbn), publisher(p), year_of_publication(yop),
          available_copies(0), availability_status(false),
          download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

    ~Book() {}

    // GETTERS
    int    get_bookid()             { return bookid;              }
    string get_title()              { return title;               }
    string get_author()             { return author;              }
    string get_category()           { return category;            }
    string get_isbn()               { return ISBN;                }
    string get_publisher()          { return publisher;           }
    int    get_YOP()                { return year_of_publication; }
    int    getAvailableCopies()     { return available_copies;    }
    int    get_downloadcount()      { return download_count;      }
    int    get_read_onsitecount()   { return read_onsite_count;   }
    int    get_timesborrowed_count(){ return timesborrowed_count; }

    // SETTERS
    void set_bookid(int id)         { bookid              = id;  }
    void set_author(string a)       { author              = a;   }
    void set_title(string t)        { title               = t;   }
    void set_category(string c)     { category            = c;   }
    void set_isbn(string isbn)      { ISBN                = isbn;}
    void set_publisher(string p)    { publisher           = p;   }
    void set_YOP(int yop)           { year_of_publication = yop; }

    void setAvailableCopies(int ac)
    {
        available_copies   = (ac < 0) ? 0 : ac;
        availability_status = (available_copies > 0);
    }

    void incrementBorrowCount()     { timesborrowed_count++; }
    void incrementDownloadCount()   { download_count++;      }
    void incrementReadOnSiteCount() { read_onsite_count++;   }

    void display()
    {
        cout << "-----------------------------------------------------" << endl;
        cout << "BOOK ID        : " << bookid              << endl
             << "TITLE          : " << title               << endl
             << "AUTHOR         : " << author              << endl
             << "CATEGORY       : " << category            << endl
             << "ISBN           : " << ISBN                << endl
             << "PUBLISHER      : " << publisher           << endl
             << "YEAR           : " << year_of_publication << endl
             << "COPIES         : " << available_copies    << endl
             << "STATUS         : " << (availability_status ? "AVAILABLE" : "NOT AVAILABLE") << endl
             << "DOWNLOADED     : " << download_count      << " times" << endl
             << "READ ON SITE   : " << read_onsite_count   << " times" << endl
             << "TIMES BORROWED : " << timesborrowed_count << " times" << endl;
        cout << "-----------------------------------------------------" << endl;
    }
};

// ============================================================
// BOOK CATALOG â€” FIXED (destructor cleans up memory)
// ============================================================
class BookCatalog
{
private:
    vector<Book*> BK;

public:
    BookCatalog() {}

    ~BookCatalog()
    {
        for (int i = 0; i < (int)BK.size(); i++)
            delete BK[i];
        BK.clear();
    }

    void addbook(Book* b)
    {
        BK.push_back(b);
        cout << "BOOK ADDED SUCCESSFULLY!" << endl;
    }

    void removeBook(string t)
    {
        if (BK.empty()) { cout << "NO BOOKS IN CATALOG!" << endl; return; }

        for (int i = 0; i < (int)BK.size(); i++)
        {
            if (BK[i]->get_title() == t)
            {
                delete BK[i];
                BK.erase(BK.begin() + i);
                cout << "BOOK REMOVED!" << endl;
                return;
            }
        }
        cout << "BOOK NOT FOUND!" << endl;
    }

    Book* SearchbyID(int id)
    {
        for (Book* b : BK)
            if (b->get_bookid() == id) return b;
        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book* SearchbyTitle(string t)
    {
        for (Book* b : BK)
            if (b->get_title() == t) return b;
        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book* SearchbyAuthor(string a)
    {
        for (Book* b : BK)
            if (b->get_author() == a) return b;
        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book* SearchbyCategory(string c)
    {
        for (Book* b : BK)
            if (b->get_category() == c) return b;
        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book* SearchbyPublisher(string p)
    {
        for (Book* b : BK)
            if (b->get_publisher() == p) return b;
        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book* SearchbyYOP(int yop)
    {
        for (Book* b : BK)
            if (b->get_YOP() == yop) return b;
        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    void ViewBookCatalog() const
    {
        if (BK.empty()) { cout << "NO BOOKS IN CATALOG!" << endl; return; }
        cout << "==== BOOK CATALOG ====" << endl;
        for (Book* b : BK) b->display();
    }

    int    get_bookcount()       { return (int)BK.size(); }

    Book* getBookAtIndex(int index)
    {
        if (index >= 0 && index < (int)BK.size()) return BK[index];
        return nullptr;
    }
};

// ============================================================
// TRANSACTION
// ============================================================
class Transaction
{
private:
    static int next_id;
    int    transaction_id;
    int    user_id;
    int    book_id;
    time_t borrow_time;
    time_t due_time;
    time_t return_time;
    string return_date;
    bool   returned;

public:
    Transaction()
        : transaction_id(next_id++), user_id(0), book_id(0),
          borrow_time(0), due_time(0), return_time(0),
          return_date("NOT RETURNED"), returned(false) {}

    Transaction(int uid, int bid, int duration_minutes)
        : transaction_id(next_id++), user_id(uid), book_id(bid),
          return_date("NOT RETURNED"), returned(false), return_time(0)
    {
        borrow_time = getCurrentTime();
        due_time    = addMinutes(borrow_time, duration_minutes);
    }

    ~Transaction() {}

    int    getuserid()        { return user_id;        }
    int    getbookid()        { return book_id;        }
    int    getTransactionID() { return transaction_id; }
    bool   isReturned()       { return returned;       }
    time_t getDueTime()       { return due_time;       }
    time_t getBorrowTime()    { return borrow_time;    }

    int getOverdueMinutes()
    {
        if (!returned || return_time == 0 || return_time <= due_time) return 0;
        return minutesDiff(due_time, return_time);
    }

    int markreturned()
    {
        returned    = true;
        return_time = getCurrentTime();
        return_date = timeToString(return_time);
        return getOverdueMinutes();
    }

    void displayuser(int id)
    {
        if (user_id != id) return;
        cout << " |TID:"      << transaction_id
             << " |UID:"      << user_id
             << " |BID:"      << book_id
             << " |BORROWED:" << timeToString(borrow_time)
             << " |DUE:"      << timeToString(due_time)
             << " |RETURN:"   << return_date
             << " |STATUS:"   << (returned ? "YES" : "NO") << " |" << endl;
    }

    void displayAll()
    {
        cout << " |TID:"          << transaction_id
             << " |UID:"          << user_id
             << " |BID:"          << book_id
             << " |BORROWED:"     << timeToString(borrow_time)
             << " |DUE:"          << timeToString(due_time)
             << " |RETURN:"       << return_date
             << " |OVERDUE MINS:" << getOverdueMinutes()
             << " |STATUS:"       << (returned ? "YES" : "NO") << " |" << endl;
    }
};

int Transaction::next_id = 1;

// ============================================================
// FINES
// ============================================================
class Fines
{
private:
    int    user_id;
    int    book_id;
    int    minuteslate;
    double rate;
    double amount;

public:
    Fines() : user_id(0), book_id(0), minuteslate(0), rate(0), amount(0) {}

    Fines(int u, int b, int mins, User* user)
        : user_id(u), book_id(b)
    {
        minuteslate = (mins < 0) ? 0 : mins;
        rate        = user->getFineRate();
        amount      = rate * minuteslate;
    }

    ~Fines() {}

    double getAmount() { return amount;  }
    int    getUserID() { return user_id; }
    int    getBookID() { return book_id; }

    void display(int userID)
    {
        if (user_id != userID) return;
        cout << "|USER ID: "   << user_id
             << " |BOOK ID: "  << book_id
             << " |MINS LATE: "<< minuteslate
             << " |RATE: Rs."  << rate << "/min"
             << " |AMOUNT: Rs."<< amount << " |" << endl;
    }

    void displayAll()
    {
        cout << "|USER ID: "   << user_id
             << " |BOOK ID: "  << book_id
             << " |MINS LATE: "<< minuteslate
             << " |RATE: Rs."  << rate << "/min"
             << " |AMOUNT: Rs."<< amount << " |" << endl;
    }
};

// ============================================================
// RESERVATION
// ============================================================
class Reservation
{
private:
    static int next_id;
    int    reservationid;
    int    userid;
    int    bookid;
    int    tableid;
    time_t start_time;
    time_t end_time;
    int    duration_minutes;
    bool   active;

public:
    Reservation()
        : reservationid(next_id++), userid(0), bookid(0), tableid(0),
          start_time(0), end_time(0), duration_minutes(0), active(true) {}

    Reservation(int uid, int bid, int tid, int dur_mins)
    {
        reservationid    = next_id++;
        userid           = uid;
        bookid           = bid;
        tableid          = tid;
        duration_minutes = dur_mins;
        start_time       = getCurrentTime();
        end_time         = addMinutes(start_time, dur_mins);
        active           = true;
    }

    ~Reservation() {}

    int  get_reservationid() { return reservationid; }
    int  get_userid()        { return userid;        }
    int  get_bookid()        { return bookid;        }
    int  get_tableid()       { return tableid;       }
    bool isActive()          { return active;        }

    bool isExpired()
    {
        return active && (getCurrentTime() > end_time);
    }

    int overdueMinutes()
    {
        if (!isExpired() || !active) return 0;
        return minutesDiff(end_time, getCurrentTime());
    }

    void cancel() { active = false; }

    bool isBookReservation()  { return bookid  != 0; }
    bool isTableReservation() { return tableid != 0; }

    void display()
    {
        if (!active) return;
        cout << "|RESERVATION ID: " << reservationid
             << " |USER ID: "       << userid;

        if (bookid  != 0) cout << " |BOOK ID: "  << bookid;
        else              cout << " |TABLE ID: " << tableid;

        cout << " |START: "   << timeToString(start_time)
             << " |END: "     << timeToString(end_time)
             << " |SLOT: "    << duration_minutes << " mins"
             << " |EXPIRED: " << (isExpired() ? "YES" : "NO");

        if (isExpired())
            cout << " |OVERDUE: " << overdueMinutes() << " mins";

        cout << " |" << endl;
    }
};

int Reservation::next_id = 1;

// ============================================================
// LIBRARIAN
// ============================================================
class librarian
{
private:
    int    librarian_id;
    string name;
    string email;
    string phone_no;
    string username;
    string password;

public:
    librarian()
        : librarian_id(0), name("UNKNOWN"), email("UNKNOWN"), phone_no("UNKNOWN") {}

    librarian(int id, string n, string e, string p)
        : librarian_id(id), name(n), email(e), phone_no(p) {}

    ~librarian() {}

    void setCredentials(string u, string p) { username = u; password = p; }
    string getUsername() { return username; }
    string getPassword() { return password; }

    void addbook(Book* b, BookCatalog* catalog)   { catalog->addbook(b);    }
    void removeBook(string t, BookCatalog* catalog){ catalog->removeBook(t); }

    void displaylibrarian_details()
    {
        cout << "ID     : " << librarian_id << endl
             << "NAME   : " << name         << endl
             << "EMAIL  : " << email        << endl
             << "PHONE  : " << phone_no     << endl;
    }

    void viewFullDetails(library* lib);
    void manageBooks(library* lib);
    void librarianMainMenu(library* lib);
};

// ============================================================
// LIBRARY â€” FIXED (destructor cleans up User* pointers)
// ============================================================
class library
{
private:
    BookCatalog        catalog;
    vector<Reservation> reservations;
    vector<Transaction> transactions;
    vector<Fines>       fines;
    vector<User*>       users;

public:
    library() {}

    ~library()
    {
        for (int i = 0; i < (int)users.size(); i++)
            delete users[i];
        users.clear();
    }

    // USER
    User* getUserByIndex(int index)
    {
        if (index >= 0 && index < (int)users.size()) return users[index];
        return nullptr;
    }

    int getUserCount() { return (int)users.size(); }

    void registerUserInLibrary()
    {
        User* newUser = User::registerUser();
        if (newUser)
        {
            users.push_back(newUser);
            cout << "User Registered Successfully! (ID: " << newUser->get_userid() << ")\n";
        }
        else
        {
            cout << "Registration Failed.\n";
        }
    }

    // BOOK INTERACTIONS
    bool readOnSite(User* u, string title)
    {
        Book* book = catalog.SearchbyTitle(title);
        if (!book) return false;
        book->incrementReadOnSiteCount();
        cout << "READING BOOK ON SITE!" << endl;
        return true;
    }

    bool downloadBook(User* u, string title)
    {
        Book* book = catalog.SearchbyTitle(title);
        if (!book) return false;
        book->incrementDownloadCount();
        cout << "BOOK DOWNLOADED!" << endl;
        return true;
    }

    bool hasOverdueBooks(User* u)
    {
        time_t now = getCurrentTime();
        for (int i = 0; i < (int)transactions.size(); i++)
        {
            if (transactions[i].getuserid()  == u->get_userid() &&
                !transactions[i].isReturned()                    &&
                now > transactions[i].getDueTime())
                return true;
        }
        return false;
    }

    bool borrowBook(User* u, string title, int duration_minutes)
    {
        if (hasOverdueBooks(u))
        {
            cout << "ERROR: You have overdue books! Return them first." << endl;
            return false;
        }

        Book* book = catalog.SearchbyTitle(title);
        if (!book) return false;

        if (book->getAvailableCopies() == 0)
        {
            cout << "NO COPIES AVAILABLE!" << endl;
            return false;
        }

        if (u->borrowbook(book->get_bookid()))
        {
            book->setAvailableCopies(book->getAvailableCopies() - 1);
            book->incrementBorrowCount();

            transactions.emplace_back(u->get_userid(), book->get_bookid(), duration_minutes);

            time_t now = getCurrentTime();
            cout << "BOOK BORROWED SUCCESSFULLY!" << endl;
            cout << "Borrow Time : " << timeToString(now)                           << endl;
            cout << "Due Time    : " << timeToString(addMinutes(now, duration_minutes)) << endl;
            cout << "Fine Rate   : Rs." << u->getFineRate() << " per minute if late"   << endl;
            return true;
        }
        return false;
    }

    bool returnBook(User* u, string title)
    {
        Book* book = catalog.SearchbyTitle(title);
        if (!book) return false;

        if (u->returnbook(book->get_bookid()))
        {
            book->setAvailableCopies(book->getAvailableCopies() + 1);

            for (int i = 0; i < (int)transactions.size(); i++)
            {
                if (transactions[i].getbookid()  == book->get_bookid() &&
                    transactions[i].getuserid()   == u->get_userid()    &&
                    !transactions[i].isReturned())
                {
                    int overdue_mins = transactions[i].markreturned();

                    cout << "BOOK RETURNED SUCCESSFULLY!" << endl;
                    cout << "Return Time : " << timeToString(getCurrentTime()) << endl;

                    if (overdue_mins > 0)
                    {
                        fines.emplace_back(u->get_userid(), book->get_bookid(), overdue_mins, u);
                        cout << "FINE GENERATED!" << endl;
                        cout << "Overdue     : " << overdue_mins << " minutes" << endl;
                        cout << "Fine Amount : Rs." << (u->getFineRate() * overdue_mins) << endl;
                    }
                    else
                    {
                        cout << "Returned on time! No fine." << endl;
                    }
                    break;
                }
            }
            return true;
        }
        return false;
    }

    // VIEW FUNCTIONS
    void viewTransactions()
    {
        cout << "==== ALL TRANSACTIONS ====" << endl;
        if (transactions.empty()) { cout << "No transactions found." << endl; return; }
        for (int i = 0; i < (int)transactions.size(); i++)
            transactions[i].displayAll();
    }

    void viewFines()
    {
        cout << "==== ALL FINES ====" << endl;
        if (fines.empty()) { cout << "No fines found." << endl; return; }
        for (int i = 0; i < (int)fines.size(); i++)
            fines[i].displayAll();
    }

    void viewUserTransactions(int userID)
    {
        cout << "==== TRANSACTIONS FOR USER ID: " << userID << " ====" << endl;
        bool found = false;
        for (int i = 0; i < (int)transactions.size(); i++)
        {
            if (transactions[i].getuserid() == userID)
            {
                transactions[i].displayuser(userID);
                found = true;
            }
        }
        if (!found) cout << "No transactions found." << endl;
    }

    void viewUserFines(int userID)
    {
        cout << "==== FINES FOR USER ID: " << userID << " ====" << endl;
        bool found = false;
        for (int i = 0; i < (int)fines.size(); i++)
        {
            if (fines[i].getUserID() == userID)
            {
                fines[i].display(userID);
                found = true;
            }
        }
        if (!found) cout << "No fines found." << endl;
    }

    void viewUsers()
    {
        cout << "==== ALL USERS ====" << endl;
        if (users.empty()) { cout << "No users registered." << endl; return; }
        for (User* u : users)
        {
            cout << "|ID: "        << u->get_userid()
                 << " |NAME: "     << u->get_name()
                 << " |EMAIL: "    << u->get_email()
                 << " |PHONE: "    << u->get_phone()
                 << " |TYPE: "     << u->get_usertype() << " |" << endl;
        }
    }

    // RESERVATIONS
    void reserveBook(int uid, int bkid, int duration_minutes)
    {
        Book* book = catalog.SearchbyID(bkid);
        if (!book) { cout << "BOOK NOT FOUND!" << endl; return; }

        reservations.emplace_back(uid, bkid, 0, duration_minutes);
        time_t now = getCurrentTime();
        cout << "BOOK RESERVED!" << endl;
        cout << "Start : " << timeToString(now)                               << endl;
        cout << "End   : " << timeToString(addMinutes(now, duration_minutes)) << endl;
    }

    void reserveTable(int uid, int tblid, int duration_minutes)
    {
        reservations.emplace_back(uid, 0, tblid, duration_minutes);
        time_t now = getCurrentTime();
        cout << "TABLE RESERVED!" << endl;
        cout << "Start : " << timeToString(now)                               << endl;
        cout << "End   : " << timeToString(addMinutes(now, duration_minutes)) << endl;
    }

    void cancelReservation(int rid)
    {
        for (int i = 0; i < (int)reservations.size(); i++)
        {
            if (reservations[i].get_reservationid() == rid)
            {
                reservations[i].cancel();
                cout << "RESERVATION CANCELLED! ID: " << rid << endl;
                return;
            }
        }
        cout << "INVALID RESERVATION ID!" << endl;
    }

    bool checkAvailability(int rid)
    {
        for (int i = 0; i < (int)reservations.size(); i++)
        {
            if (reservations[i].get_reservationid() == rid)
            {
                if (!reservations[i].isActive())
                {
                    cout << "RESERVATION IS CANCELLED!" << endl;
                    return false;
                }
                if (reservations[i].isExpired())
                {
                    cout << "SLOT EXPIRED! Overdue by "
                         << reservations[i].overdueMinutes() << " minutes." << endl;
                    return false;
                }
                cout << "SLOT IS ACTIVE AND VALID!" << endl;
                return true;
            }
        }
        cout << "INVALID RESERVATION ID!" << endl;
        return false;
    }

    void viewAllReservations()
    {
        cout << "==== ALL RESERVATIONS ====" << endl;
        if (reservations.empty()) { cout << "No reservations found." << endl; return; }
        for (int i = 0; i < (int)reservations.size(); i++)
            reservations[i].display();
    }

    BookCatalog* getCatalog() { return &catalog; }
};

// ============================================================
// AUTHENTICATION
// ============================================================
class Authentication
{
private:
    librarian* lib_admin;

public:
    Authentication(librarian* l = nullptr) { lib_admin = l; }
    void setLibrarian(librarian* l)        { lib_admin = l; }

    User* loginUser(library* lib)
    {
        string uname, pass;
        cout << "\n===== USER LOGIN =====" << endl;
        cout << "Enter Username: ";
        getline(cin, uname);
        cout << "Enter Password: ";
        getline(cin, pass);

        for (int i = 0; i < lib->getUserCount(); i++)
        {
            User* u = lib->getUserByIndex(i);
            if (u->getUsername() == uname && u->getPassword() == pass)
            {
                cout << "LOGIN SUCCESSFUL! Welcome, " << u->get_name() << "!\n";
                return u;
            }
        }
        cout << "INVALID CREDENTIALS!\n";
        return nullptr;
    }

    librarian* loginLibrarian()
    {
        if (!lib_admin) { cout << "No librarian registered!\n"; return nullptr; }

        string uname, pass;
        cout << "\n===== LIBRARIAN LOGIN =====" << endl;
        cout << "Enter Username: ";
        getline(cin, uname);
        cout << "Enter Password: ";
        getline(cin, pass);

        if (lib_admin->getUsername() == uname && lib_admin->getPassword() == pass)
        {
            cout << "LIBRARIAN LOGIN SUCCESSFUL!\n";
            return lib_admin;
        }
        cout << "INVALID LIBRARIAN CREDENTIALS!\n";
        return nullptr;
    }

    void logout() { cout << "LOGGED OUT SUCCESSFULLY!\n"; }
};

// ============================================================
// USER MENUS
// ============================================================
void User::viewFullDetails(library* lib)
{
    int choice;
    do
    {
        cout << "\n===== USER DASHBOARD =====" << endl;
        cout << "1. View Your Details"        << endl;
        cout << "2. View Borrowed Books"       << endl;
        cout << "3. View My Transactions"      << endl;
        cout << "4. View My Fines"             << endl;
        cout << "5. Back"                      << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
            cout << "\n--- USER DETAILS ---" << endl;
            viewdetails();
            cout << "User ID : " << userid      << endl;
            cout << "Email   : " << email        << endl;
            cout << "Phone   : " << phonenumber  << endl;
            cout << "CNIC    : " << CNIC         << endl;
            break;

        case 2:
        {
            cout << "\n--- BORROWED BOOK IDs ---" << endl;
            const vector<int>& books = getBorrowedBooks();
            if (books.empty()) cout << "None currently borrowed." << endl;
            else
                for (int id : books) cout << "  Book ID: " << id << endl;
            break;
        }

        case 3:
            lib->viewUserTransactions(userid);
            break;

        case 4:
            lib->viewUserFines(userid);
            break;

        case 5:
            cout << "Going back..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }
    } while (choice != 5);
}

void User::searchBooks(library* lib)
{
    int choice;
    do
    {
        cout << "\n===== BOOK SEARCH =====" << endl;
        cout << "1. Search by ID"           << endl;
        cout << "2. Search by Title"        << endl;
        cout << "3. Search by Author"       << endl;
        cout << "4. Search by Category"     << endl;
        cout << "5. Search by Publisher"    << endl;
        cout << "6. Search by Year"         << endl;
        cout << "7. View All Books"         << endl;
        cout << "8. Back"                   << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        Book* b = nullptr;

        switch (choice)
        {
        case 1: { int id; cout << "Enter Book ID: "; cin >> id; cin.ignore(); b = lib->getCatalog()->SearchbyID(id);        break; }
        case 2: { string t; cout << "Enter Title: ";     getline(cin, t); b = lib->getCatalog()->SearchbyTitle(t);     break; }
        case 3: { string a; cout << "Enter Author: ";    getline(cin, a); b = lib->getCatalog()->SearchbyAuthor(a);    break; }
        case 4: { string c; cout << "Enter Category: ";  getline(cin, c); b = lib->getCatalog()->SearchbyCategory(c);  break; }
        case 5: { string p; cout << "Enter Publisher: "; getline(cin, p); b = lib->getCatalog()->SearchbyPublisher(p); break; }
        case 6: { int y;   cout << "Enter Year: ";      cin >> y; cin.ignore(); b = lib->getCatalog()->SearchbyYOP(y); break; }
        case 7: lib->getCatalog()->ViewBookCatalog(); break;
        case 8: cout << "Going back..." << endl; break;
        default: cout << "Invalid choice!" << endl;
        }

        if (b != nullptr)
        {
            cout << "\n--- BOOK FOUND ---" << endl;
            b->display();

            int action;
            cout << "\nWhat do you want to do?" << endl;
            cout << "1. Borrow Book"           << endl;
            cout << "2. Read On Site"           << endl;
            cout << "3. Download Book"          << endl;
            cout << "4. Reserve Book Slot"      << endl;
            cout << "5. Back"                   << endl;
            cout << "Enter choice: ";
            cin >> action;
            cin.ignore();

            switch (action)
            {
            case 1:
            {
                int mins;
                cout << "How many minutes to borrow? ";
                cin >> mins; cin.ignore();
                lib->borrowBook(this, b->get_title(), mins);
                break;
            }
            case 2: lib->readOnSite(this,   b->get_title()); break;
            case 3: lib->downloadBook(this, b->get_title()); break;
            case 4:
            {
                int mins;
                cout << "How many minutes to reserve? ";
                cin >> mins; cin.ignore();
                lib->reserveBook(userid, b->get_bookid(), mins);
                break;
            }
            case 5: break;
            default: cout << "Invalid choice!" << endl;
            }
        }

    } while (choice != 8);
}

void User::userMenu(library* lib)
{
    int choice;
    do
    {
        cout << "\n===== USER MAIN MENU =====" << endl;
        cout << "1. My Dashboard"              << endl;
        cout << "2. Search / Browse Books"     << endl;
        cout << "3. Return a Book"             << endl;
        cout << "4. Reserve a Table"           << endl;
        cout << "5. Check Reservation Status"  << endl;
        cout << "6. Cancel Reservation"        << endl;
        cout << "7. Exit"                      << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1: viewFullDetails(lib); break;
        case 2: searchBooks(lib);     break;

        case 3:
        {
            string title;
            cout << "Enter book title to return: ";
            getline(cin, title);
            lib->returnBook(this, title);
            break;
        }

        case 4:
        {
            int tblid, mins;
            cout << "Enter Table ID: ";    cin >> tblid; cin.ignore();
            cout << "How many minutes? "; cin >> mins;  cin.ignore();
            lib->reserveTable(userid, tblid, mins);
            break;
        }

        case 5:
        {
            int rid;
            cout << "Enter Reservation ID: ";
            cin >> rid; cin.ignore();
            lib->checkAvailability(rid);
            break;
        }

        case 6:
        {
            int rid;
            cout << "Enter Reservation ID to cancel: ";
            cin >> rid; cin.ignore();
            lib->cancelReservation(rid);
            break;
        }

        case 7: cout << "Exiting User Menu..." << endl; break;
        default: cout << "Invalid choice!" << endl;
        }

    } while (choice != 7);
}

// ============================================================
// LIBRARIAN MENUS
// ============================================================
void librarian::viewFullDetails(library* lib)
{
    int choice;
    do
    {
        cout << "\n===== LIBRARIAN DASHBOARD =====" << endl;
        cout << "1. View Librarian Info"            << endl;
        cout << "2. View All Users"                 << endl;
        cout << "3. View All Transactions"          << endl;
        cout << "4. View All Fines"                 << endl;
        cout << "5. View All Reservations"          << endl;
        cout << "6. View Book Catalog"              << endl;
        cout << "7. Back"                           << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1: cout << "\n--- LIBRARIAN INFO ---" << endl; displaylibrarian_details(); break;
        case 2: lib->viewUsers();          break;
        case 3: lib->viewTransactions();   break;
        case 4: lib->viewFines();          break;
        case 5: lib->viewAllReservations();break;
        case 6: lib->getCatalog()->ViewBookCatalog(); break;
        case 7: cout << "Going back..." << endl; break;
        default: cout << "Invalid choice!" << endl;
        }
    } while (choice != 7);
}

void librarian::manageBooks(library* lib)
{
    int choice;
    do
    {
        cout << "\n===== BOOK MANAGEMENT =====" << endl;
        cout << "1. Add Book"                   << endl;
        cout << "2. Remove Book"                << endl;
        cout << "3. Back"                       << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
        {
            int id, yop, copies;
            string title, author, category, isbn, publisher;

            cout << "Enter Book ID: ";       cin >> id;  cin.ignore();
            cout << "Enter Title: ";         getline(cin, title);
            cout << "Enter Author: ";        getline(cin, author);
            cout << "Enter Category: ";      getline(cin, category);
            cout << "Enter ISBN: ";          getline(cin, isbn);
            cout << "Enter Publisher: ";     getline(cin, publisher);
            cout << "Enter Year: ";          cin >> yop;    cin.ignore();
            cout << "Enter Copies: ";        cin >> copies; cin.ignore();

            Book* b = new Book(id, title, author, category, isbn, publisher, yop);
            b->setAvailableCopies(copies);
            addbook(b, lib->getCatalog());
            break;
        }

        case 2:
        {
            string title;
            cout << "Enter Title to Remove: ";
            getline(cin, title);
            removeBook(title, lib->getCatalog());
            break;
        }

        case 3: cout << "Going back..." << endl; break;
        default: cout << "Invalid choice!" << endl;
        }
    } while (choice != 3);
}

void librarian::librarianMainMenu(library* lib)
{
    int choice;
    do
    {
        cout << "\n===== LIBRARIAN MAIN MENU =====" << endl;
        cout << "1. View System Dashboard"          << endl;
        cout << "2. Manage Books"                   << endl;
        cout << "3. Exit"                           << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1: viewFullDetails(lib); break;
        case 2: manageBooks(lib);     break;
        case 3: cout << "Exiting Librarian System..." << endl; break;
        default: cout << "Invalid choice!" << endl;
        }
    } while (choice != 3);
}

// ============================================================
// MAIN
// ============================================================
int main()
{
    library lib;

    // DEFAULT LIBRARIAN
    librarian admin(1, "Admin", "admin@lib.com", "03000000000");
    admin.setCredentials("admin", "123");

    // SAMPLE BOOKS
    Book* b1 = new Book(101, "Data Structures", "Mark Allen", "CS", "ISBN-001", "Pearson", 2019);
    b1->setAvailableCopies(3);
    Book* b2 = new Book(102, "OOP in C++", "Bjarne Stroustrup", "CS", "ISBN-002", "Wiley", 2020);
    b2->setAvailableCopies(2);
    Book* b3 = new Book(103, "Calculus Vol 1", "James Stewart", "Math", "ISBN-003", "Cengage", 2018);
    b3->setAvailableCopies(1);

    lib.getCatalog()->addbook(b1);
    lib.getCatalog()->addbook(b2);
    lib.getCatalog()->addbook(b3);

    Authentication auth(&admin);

    int choice;
    do
    {
        cout << "\n========================================" << endl;
        cout << "     LIBRARY MANAGEMENT SYSTEM          " << endl;
        cout << "========================================" << endl;
        cout << "1. Register New User"                     << endl;
        cout << "2. Login as User"                         << endl;
        cout << "3. Login as Librarian"                    << endl;
        cout << "4. Exit"                                  << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
            lib.registerUserInLibrary();
            break;

        case 2:
        {
            User* u = auth.loginUser(&lib);
            if (u)
            {
                u->userMenu(&lib);
                auth.logout();
            }
            break;
        }

        case 3:
        {
            librarian* l = auth.loginLibrarian();
            if (l)
            {
                l->librarianMainMenu(&lib);
                auth.logout();
            }
            break;
        }

        case 4:
            cout << "\nThank you for using the Library System. Goodbye!\n" << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 4);

    return 0;
}