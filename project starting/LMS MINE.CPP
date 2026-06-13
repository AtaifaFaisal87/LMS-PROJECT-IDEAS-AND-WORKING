#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <algorithm>
using namespace std;

class book;
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
    if (t == 0)
        return "N/A";
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
    vector<int> borrowedbook; // was: int borrowedbook[20]

public:
    // CLEAN CONSTRUCTOR
    User(string n = "NONE", string c = "xxxxx-xxxxxxx-x",
         string e = "abc@gmail.com", string p = "03xxxxxxxxx")
    {
        userid = next_id++; //  AUTO ID
        name = n;
        CNIC = c;
        email = e;
        phonenumber = p;
    }

    virtual ~User()
    {
        cout << "User " << name << " destroyed." << endl;
    }

    // ================= SETTERS =================
    void setid(int id) { userid = id; }
    void setname(string n) { name = n; }
    void setemail(string e) { email = e; }
    void setphone_no(string p) { phonenumber = p; }
    void setCNIC(string c) { CNIC = c; }
    void setUsername(string u) { username = u; }
    void setPassword(string p) { password = p; }

    // ================= GETTERS =================
    int get_userid() { return userid; }
    string get_name() { return name; }
    string get_email() { return email; }
    string get_phone() { return phonenumber; }
    string get_cnic() { return CNIC; }
    string getUsername() { return username; }
    string getPassword() { return password; }

    // USED BY LIBRARY CLASS
    const vector<int> &getBorrowedBooks() { return borrowedbook; }
    int getBorrowedCount() { return (int)borrowedbook.size(); }

    // ================= POLYMORPHISM =================
    virtual int getborrowlimit() = 0;
    virtual string get_usertype() = 0;
    virtual void viewdetails() = 0;
    virtual int getFineRate() = 0;

    // ================= BORROW =================
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

    // ================= RETURN =================
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

    // ================= FULL DASHBOARD =================
    void viewFullDetails(library *lib);
    void searchBooks(library *lib);
    void reservationMenu(library *lib);
    void bookActionMenu(library *lib);
    //=========USER MENU=================
    void userMenu(library *lib);
    // ================= REGISTER =================
    static User *registerUser();
};

int User::next_id = 1;
// ================= STUDENT =================
class Student : public User
{
private:
    string seatno;
    string major;

public:
    Student(string n, string c, string e, string p, string s, string m)
        : User(n, c, e, p), seatno(s), major(m) {}

    int getborrowlimit() override { return 3; }
    string get_usertype() override { return "STUDENT"; }

    void viewdetails() override
    {
        cout << "|NAME: " << name
             << " |SEAT: " << seatno
             << " |MAJOR: " << major
             << " |TYPE: STUDENT|" << endl;
    }

    int getFineRate() override { return 10; }
};

// ================= FACULTY =================
class Faculty : public User
{
private:
    string department;
    string designation;

public:
    Faculty(string n, string c, string e, string p, string dept, string des)
        : User(n, c, e, p), department(dept), designation(des) {}

    int getborrowlimit() override { return 5; }
    string get_usertype() override { return "FACULTY"; }

    void viewdetails() override
    {
        cout << "|NAME: " << name
             << " |DEPARTMENT: " << department
             << " |DESIGNATION: " << designation
             << " |TYPE: FACULTY|" << endl;
    }

    int getFineRate() override { return 15; }
};

// ================= GUEST =================
class Guest : public User
{
private:
    string purpose;

public:
    Guest(string n, string c, string e, string p, string ps)
        : User(n, c, e, p), purpose(ps) {}

    int getborrowlimit() override { return 1; }
    string get_usertype() override { return "GUEST"; }

    void viewdetails() override
    {
        cout << "|NAME: " << name
             << " |PURPOSE: " << purpose
             << " |TYPE: GUEST|" << endl;
    }

    int getFineRate() override { return 20; }
};

// ================= REGISTER FUNCTION =================
User *User::registerUser()
{
    int choice;
    cout << "Select User Type:\n1. Student\n2. Faculty\n3. Guest\nChoice: ";
    cin >> choice;
    cin.ignore();

    string name, CNIC, email, phone;
    string username, password;

    cout << "Enter Username: ";
    getline(cin, username);
    cin.ignore();
    cout << "Enter Password: ";
    getline(cin, password);
    cin.ignore();
    cout << "Enter Name: ";
    getline(cin, name);
    cin.ignore();
    cout << "Enter CNIC: ";
    getline(cin, CNIC);
    cin.ignore();
    cout << "Enter Email: ";
    getline(cin, email);
    cin.ignore();
    cout << "Enter Phone: ";
    getline(cin, phone);
    cin.ignore();

    if (choice == 1)
    {
        string seat, major;

        getline(cin, password);
        cout << "Enter Seat Number: ";
        getline(cin, seat);
        cout << "Enter Major: ";
        getline(cin, major);

        User *u = new Student(name, CNIC, email, phone, seat, major);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }
    else if (choice == 2)
    {
        string dept, des;

        cout << "Enter Department: ";
        getline(cin, dept);
        cout << "Enter Designation: ";
        getline(cin, des);

        User *u = new Faculty(name, CNIC, email, phone, dept, des);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }

    else if (choice == 3)
    {
        string purpose;

        cout << "Enter Purpose: ";
        getline(cin, purpose);

        User *u = new Guest(name, CNIC, email, phone, purpose);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }
    else
    {
        cout << "Invalid choice!\n";
        return nullptr;
    }
}

class Book
{
private:
    int bookid;
    string title;
    string author;
    string category;
    string ISBN;
    string publisher;
    int year_of_publication;
    int available_copies = 0;
    bool availability_status;
    int download_count = 0;
    int read_onsite_count = 0;
    int timesborrowed_count = 0;

public:
    // CONSTRUCTOR
    Book() : bookid(0), title("UNKNOWN"), author("UNKNOWN"), category("UNKNOWN"),
             ISBN("UNKNOWN"), publisher("UNKNOWN"), year_of_publication(0),
             available_copies(0), availability_status(false),
             download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

    Book(int id, string t, string a, string c, string isbn, string p, int yop)
        : bookid(id), title(t), author(a), category(c),
          ISBN(isbn), publisher(p), year_of_publication(yop),
          available_copies(0), availability_status(false),
          download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

    // DESTRUCTOR
    ~Book() { cout << "BOOK DESTROYED!" << endl; }

    // GETTERS
    int get_bookid() { return bookid; }
    string get_title() { return title; }
    string get_author() { return author; }
    string get_category() { return category; }
    string get_isbn() { return ISBN; }
    string get_publisher() { return publisher; }
    int get_YOP() { return year_of_publication; }
    int getAvailableCopies() { return available_copies; }
    int get_downloadcount() { return download_count; }
    int get_read_onsitecount() { return read_onsite_count; }
    int get_timesborrowed_count() { return timesborrowed_count; }
    void decreaseCopy()
    {
        if (available_copies > 0)
            available_copies--;
    }

    // SETTERS
    void set_bookid(int id) { bookid = id; }
    void set_author(string a) { author = a; }
    void set_title(string t) { title = t; }
    void set_category(string c) { category = c; }
    void set_isbn(string isbn) { ISBN = isbn; }
    void set_publisher(string p) { publisher = p; }
    void set_YOP(int yop) { year_of_publication = yop; }

    void setAvailableCopies(int ac)
    {
        available_copies = ac;

        //  AUTO STATUS UPDATE
        if (available_copies > 0)
            availability_status = true;
        else
            availability_status = false;
    }

    // ================= COUNTER FUNCTIONS =================

    void incrementBorrowCount()
    {
        //  only count valid borrows
        if (available_copies >= 0)
            timesborrowed_count++;
    }

    void incrementDownloadCount()
    {
        download_count++;
    }

    void incrementReadOnSiteCount()
    {
        read_onsite_count++;
    }

    // ================= DISPLAY =================

    void display()
    {
        cout << "BOOK ID: " << bookid << endl
             << "TITLE: " << title << endl
             << "AUTHOR: " << author << endl
             << "CATEGORY: " << category << endl
             << "ISBN: " << ISBN << endl
             << "PUBLISHER: " << publisher << endl
             << "YEAR: " << year_of_publication << endl
             << "AVAILABLE COPIES: " << available_copies << endl
             << "STATUS: " << (availability_status ? "AVAILABLE" : "NOT AVAILABLE") << endl
             << "DOWNLOADED: " << download_count << " times" << endl
             << "READ ON SITE: " << read_onsite_count << " times" << endl
             << "TIMES BORROWED: " << timesborrowed_count << " times" << endl;

        cout << "-----------------------------------------------------" << endl;
    }
};

class BookCatalog
{
private:
    vector<Book *> BK; // was: Book* BK[100]; int book_count

public:
    BookCatalog() {}
    ~BookCatalog()
    {
        for (int i = 0; i < (int)BK.size(); i++)
            delete BK[i];
        BK.clear();
        cout << "BOOKCATALOG DESTROYED!" << endl;
    }

    void addbook(Book *b)
    {
        BK.push_back(b);
        cout << "BOOK ADDED SUCCESSFULLY!" << endl;
    }

    void removeBook(string t)
    {
        if (BK.empty())
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return;
        }

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

    // ================= SEARCH FUNCTIONS =================

    Book *SearchbyID(int id)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_bookid() == id)
                return b;
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyTitle(string t)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_title() == t)
                return b;
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyAuthor(string a)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_author() == a)
                return b;
        }
        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyCategory(string c)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_category() == c)
                return b;
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyPublisher(string p)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_publisher() == p)
                return b;
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyYOP(int yop)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_YOP() == yop)
                return b;
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    // ================= DISPLAY =================

    void ViewBookCatalog() const
    {
        if (BK.empty())
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return;
        }

        cout << "==== BOOK CATALOG ====" << endl;

        for (Book *b : BK)
            b->display();
    }

    // ================= GETTERS =================

    int get_bookcount() { return (int)BK.size(); } // TYPE CASTED AS VECTORS HAS NO LIMITS

    // IMPORTANT ADD (for future flexibility)
    Book *getBookAtIndex(int index)
    {
        if (index >= 0 && index < (int)BK.size())
            return BK[index];
        return nullptr;
    }
};

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
    // CONSTRUCTOR
    librarian() : librarian_id(0), name("UNKNOWN"), email("UNKNOWN"), phone_no("UNKNOWN") {}
    librarian(int id, string n, string e, string p) : librarian_id(id), name(n), email(e), phone_no(p) {}

    // DESTRUCTOR
    ~librarian() { cout << "LIBRARIAN DESTROYED!" << endl; }

    // CREDENTIALS
    void setCredentials(string u, string p)
    {
        username = u;
        password = p;
    }

    string getUsername() { return username; }
    string getPassword() { return password; }

    void addbook(Book *b, BookCatalog *catalog)
    {
        catalog->addbook(b);
    }

    void removeBook(string t, BookCatalog *catalog)
    {
        catalog->removeBook(t);
    }

    void displaylibrarian_details() // FRIEND OSTREAM
    {
        cout << "ID:" << librarian_id << endl
             << "NAME:" << name << endl
             << "EMAIL:" << email << endl
             << "PHONE#:" << endl;
    }

    //=============SYSTEM DETAILS=============
    void viewFullDetails(library *lib);
    //=====BOOK MENU======
    void manageBooks(library *lib);
    //===============LIBRARIAN DASHBOARD====================
    void librarianMainMenu(library *lib);
};

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
    // CONSTRUCTOR
    Transaction()
        : transaction_id(next_id++), user_id(0), book_id(0),
          borrow_time(0), due_time(0), return_time(0),
          return_date("NOT RETURNED"), returned(false) {}

    Transaction(int uid, int bid, int duration_minutes)
        : transaction_id(next_id++), user_id(uid), book_id(bid),
          return_date("NOT RETURNED"), returned(false), return_time(0)
    {
        borrow_time = getCurrentTime();
        due_time = addMinutes(borrow_time, duration_minutes);
    }

    // DESTRUCTOR
    ~Transaction() { cout << "TRANSACTION DESTROYED!" << endl; }

    // GETTERS
    int getuserid() { return user_id; }
    int getbookid() { return book_id; }
    int getTransactionID() { return transaction_id; }
    bool isReturned() { return returned; }
    time_t getDueTime() { return due_time; }
    time_t getBorrowTime() { return borrow_time; }
    int getOverdueMinutes()
    {
        if (!returned || return_time == 0 || return_time <= due_time)
            return 0;
        return minutesDiff(due_time, return_time);
    }

    // // SETTERS
    // void setIssueDate(string i) { issue_date = i; }
    // void setDueDate(string d) { due_date = d; }

    // MARK RETURNED (flexible for future time system)
    int markreturned()
    {
        returned = true;
        return_time = getCurrentTime();
        return_date = timeToString(return_time);
        return getOverdueMinutes();
    }

    // DISPLAY FOR SPECIFIC USER
    void displayuser(int id)
    {
        if (user_id != id)
            return;
        cout << " |TID:" << transaction_id
             << " |UID:" << user_id
             << " |BID:" << book_id
             << " |BORROWED:" << timeToString(borrow_time)
             << " |DUE:" << timeToString(due_time)
             << " |RETURN:" << return_date
             << " |STATUS:" << (returned ? "YES" : "NO") << " |" << endl;
    }

    // DISPLAY ALL
    void displayAll()
    {
        cout << " |TID:" << transaction_id
             << " |UID:" << user_id
             << " |BID:" << book_id
             << " |BORROWED:" << timeToString(borrow_time)
             << " |DUE:" << timeToString(due_time)
             << " |RETURN:" << return_date
             << " |OVERDUE MINS:" << getOverdueMinutes()
             << " |STATUS:" << (returned ? "YES" : "NO") << " |" << endl;
    }
};

// STATIC INITIALIZATION
int Transaction::next_id = 1;

class Fines
{
private:
    int user_id;
    int book_id;
    int minuteslate;
    double rate;
    double amount;

public:
    // CONSTRUCTOR
    Fines() : user_id(0), book_id(0), minuteslate(0), rate(0), amount(0) {}

    Fines(int u, int b, int mins, User *user)
        : user_id(u), book_id(b)
    {
        minuteslate = (mins < 0) ? 0 : mins;
        rate = user->getFineRate();
        amount = rate * minuteslate;
    }
    // DESTRUCTOR
    ~Fines() { cout << "FINES DESTROYED!" << endl; }

    // GETTERS (useful for future)
    double getAmount() { return amount; }
    int getUserID() { return user_id; }
    int getBookID() { return book_id; }

    // DISPLAY (USER-SPECIFIC)
    void display(int userID)
    {
        if (user_id != userID)
            return;
        cout << "|USER ID: " << user_id
             << " |BOOK ID: " << book_id
             << " |MINS LATE: " << minuteslate
             << " |RATE: Rs." << rate << "/min"
             << " |AMOUNT: Rs." << amount << " |" << endl;
    }

    // DISPLAY (ALL)
    void displayAll()
    {
        cout << "|USER ID: " << user_id
             << " |BOOK ID: " << book_id
             << " |MINS LATE: " << minuteslate
             << " |RATE: Rs." << rate << "/min"
             << " |AMOUNT: Rs." << amount << " |" << endl;
    }
};

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
    Reservation()
        : reservationid(next_id++), userid(0), bookid(0), tableid(0),
          date("UNKNOWN"), startTime("0"), endTime("0"), active(true) {}

    Reservation(int uid, int bid, int tid,
                string d, string start, string end)
    {
        reservationid = next_id++;
        userid = uid;
        bookid = bid;
        tableid = tid;
        date = d;
        startTime = start;
        endTime = end;
        active = true;
    }

    int get_userid() { return userid; }
    int get_tableid() { return tableid; }
    int get_bookid() { return bookid; }
    int get_reservationid() { return reservationid; }

    string get_date() { return date; }
    string get_start() { return startTime; }
    string get_end() { return endTime; }

    bool isActive() { return active; }

    void cancel() { active = false; }

    static bool overlap(string s1, string e1, string s2, string e2)
    {
        return !(e1 <= s2 || e2 <= s1);
    }

    void display()
    {
        if (!active)
            return;

        cout << "RID: " << reservationid
             << " UID: " << userid;

        if (bookid)
            cout << " BOOK: " << bookid;
        else
            cout << " TABLE: " << tableid;

        cout << " DATE: " << date
             << " START: " << startTime
             << " END: " << endTime << endl;
    }
};

int Reservation::next_id = 1;

class Table
{
private:
    int tableid;
    static int next_id;

    string location;
    int capacity;

public:
    // DEFAULT CONSTRUCTOR
    Table()
    {
        tableid = next_id++; // AUTO ID
        location = "UNKNOWN";
        capacity = 1;
    }

    // PARAMETERIZED CONSTRUCTOR
    Table(string loc, int cap)
    {
        tableid = next_id++; // AUTO ID
        location = loc;
        capacity = cap;
    }

    int get_id() { return tableid; }

    void display()
    {
        cout << "TABLE ID: " << tableid
             << " | LOCATION: " << location
             << " | CAPACITY: " << capacity << endl;
    }
};
int Table::next_id = 1;

class library
{
private:
    BookCatalog catalog;
    vector<Table> tables;
    vector<Reservation> reservations; // was: Reservation* reservations[100]
    vector<Transaction> transactions; // was: Transaction* transaction[100]
    vector<Fines> fines;              // was: Fines* fines[100]
    vector<User *> users;             // was: User* users[100] (polymorphic — kept as pointer)

public:
    // ---------------- USER ----------------
    User *getUserByIndex(int index)
    {
        if (index >= 0 && index < (int)users.size())
            return users[index];
        return nullptr;
    }

    int getUserCount()
    {
        return (int)users.size();
    }

    void registerUserInLibrary()
    {
        User *newUser = User::registerUser();
        if (newUser)
        {
            users.push_back(newUser);
            cout << "User Registered Successfully!\n";
        }
        else
        {
            cout << "Registration Failed.\n";
        }
    }

    // ---------------- BOOK INTERACTIONS ----------------
    bool readOnSite(User *u, string title)
    {
        Book *book = catalog.SearchbyTitle(title);
        if (!book)
            return false;

        book->incrementReadOnSiteCount();

        cout << "READING BOOK ONSITE!" << endl;
        return true;
    }

    bool downloadBook(User *u, string title)
    {
        Book *book = catalog.SearchbyTitle(title);
        if (!book)
            return false;

        book->incrementDownloadCount();

        cout << "BOOK DOWNLOADED!" << endl;
        return true;
    }

    bool hasOverdueBooks(User *u)
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
    bool borrowBook(User *u, string title, int duration_minutes)
    {
        if (hasOverdueBooks(u))
        {
            cout << "ERROR: You have overdue books! Return them first." << endl;
            return false;
        }

        Book *book = catalog.SearchbyTitle(title);
        if (!book)
            return false;

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
            cout << "Borrow Time : " << timeToString(now) << endl;
            cout << "Due Time    : " << timeToString(addMinutes(now, duration_minutes)) << endl;
            cout << "Fine Rate   : Rs." << u->getFineRate() << " per minute if late" << endl;
            return true;
        }
        return false;
    }

    // ---------------- RETURN ----------------
    bool returnBook(User *u, string title)
    {
        Book *book = catalog.SearchbyTitle(title);
        if (!book)
            return false;

        if (u->returnbook(book->get_bookid()))
        {
            book->setAvailableCopies(book->getAvailableCopies() + 1);

            for (int i = 0; i < (int)transactions.size(); i++)
            {
                if (transactions[i].getbookid() == book->get_bookid() &&
                    transactions[i].getuserid() == u->get_userid() &&
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

        int calculateLiveFine(Transaction &t, User *u)
    {
        if (t.isReturned())
            return 0;

        time_t now = getCurrentTime();

        if (now <= t.getDueTime())
            return 0;

        int overdue = minutesDiff(t.getDueTime(), now);
        return overdue * u->getFineRate();
    }
    // ---------------- VIEW ----------------
    void viewTransactions()
    {
        cout << "==== ALL TRANSACTIONS ====" << endl;
        for (int i = 0; i < transactions.size(); i++)
        {
            transactions[i].displayAll();
        }
    }

    void viewFines()
    {
        cout << "==== ALL FINES ====" << endl;
        for (int i = 0; i < fines.size(); i++)
        {
            fines[i].displayAll();
        }
    }

    void viewUserTransactions(int userID)
    {
        cout << "==== TRANSACTIONS FOR USER ID: " << userID << " ====" << endl;
        for (int i = 0; i < transactions.size(); i++)
        {
            transactions[i].displayuser(userID);
        }
    }

   void viewUserFines(int userID)
    {
        cout << "==== FINES FOR USER ID: " << userID << " ====" << endl;

        bool found = false;

        // 1. Show STORED fines (after return)
        for (int i = 0; i < (int)fines.size(); i++)
        {
            if (fines[i].getUserID() == userID)
            {
                fines[i].display(userID);
                found = true;
            }
        }

        // 2. Show LIVE fines (not returned yet)
        for (int i = 0; i < (int)transactions.size(); i++)
        {
            if (transactions[i].getuserid() == userID &&
                !transactions[i].isReturned())
            {
                User *u = nullptr;

                // find user pointer
                for (int j = 0; j < (int)users.size(); j++)
                {
                    if (users[j]->get_userid() == userID)
                    {
                        u = users[j];
                        break;
                    }
                }

                if (!u)
                    continue;

                int fine = calculateLiveFine(transactions[i], u);

                if (fine > 0)
                {
                    cout << "|USER ID: " << userID
                         << " |BOOK ID: " << transactions[i].getbookid()
                         << " |STATUS: UNPAID"
                         << " |LIVE FINE: Rs." << fine
                         << " |" << endl;

                    found = true;
                }
            }
        }

        if (!found)
            cout << "No fines found." << endl;
    }

    void viewUsers()
    {
        cout << "==== ALL USERS ====" << endl;
        for (User *u : users)
        {
            cout << "|NAME: " << u->get_name()
                 << " |EMAIL: " << u->get_email()
                 << " |PHONE: " << u->get_phone()
                 << " |USER TYPE: " << u->get_usertype() << " |" << endl;
        }
    }

    void viewUserReservations(int uid)
    {
        cout << "==== USER RESERVATIONS ====" << endl;

        bool found = false;

        for (int i = 0; i < reservations.size(); i++)
        {
            if (reservations[i].get_userid() == uid)
            {
                reservations[i].display();
                found = true;
            }
        }

        if (!found)
        {
            cout << "No reservations found." << endl;
        }
    }

    void viewUsersByType(string type)
    {
        cout << "==== USERS OF TYPE: " << type << " ====" << endl;
        for (User *u : users)
        {
            if (u->get_usertype() == type)
            {
                cout << "|NAME: " << u->get_name()
                     << " |EMAIL: " << u->get_email()
                     << " |PHONE: " << u->get_phone()
                     << " |USER TYPE: " << u->get_usertype() << " |" << endl;
            }
        }
    }

    // ---------------- RESERVATION ----------------
    void reserveBook(int uid, int bkid, string date, string start, string end)
    {
        Book *book = catalog.SearchbyID(bkid);

        if (!book)
        {
            cout << "BOOK NOT FOUND!\n";
            return;
        }

        if (book->getAvailableCopies() <= 0)
        {
            cout << "NO COPIES AVAILABLE!\n";
            return;
        }

        reservations.emplace_back(uid, bkid, 0, date, start, end);

        book->decreaseCopy(); // important

        cout << "BOOK RESERVED SUCCESSFULLY!\n";
    }

    void reserveTableManual(int uid, int tid, string date, string start, string end)
    {
        for (int i = 0; i < reservations.size(); i++)
        {
            Reservation &r = reservations[i];

            if (r.get_tableid() == tid && r.get_date() == date && r.isActive())
            {
                if (Reservation::overlap(r.get_start(), r.get_end(), start, end))
                {
                    cout << "TABLE NOT AVAILABLE!\n";
                    return;
                }
            }
        }

        reservations.emplace_back(uid, 0, tid, date, start, end);

        cout << "TABLE RESERVED MANUALLY!\n";
    }

    void reserveTableAuto(int uid, string date, string start, string end)
    {
        for (int i = 0; i < tables.size(); i++)
        {
            int tid = tables[i].get_id();
            bool conflict = false;

            for (int j = 0; j < reservations.size(); j++)
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

                cout << "AUTO ASSIGNED TABLE ID: " << tid << endl;
                return;
            }
        }

        cout << "NO TABLE AVAILABLE!\n";
    }

    void cancelReservation(int rid)
    {
        for (int i = 0; i < reservations.size(); i++)
        {
            if (reservations[i].get_reservationid() == rid)
            {
                reservations[i].cancel();
                cout << "RESERVATION CANCELLED!\n";
                return;
            }
        }

        cout << "INVALID ID!\n";
    }

    void viewAllReservations()
    {
        for (int i = 0; i < reservations.size(); i++)
        {
            reservations[i].display();
        }
    }

    //----------------TABLE INTERATIONS----------------
    void addTable(string loc, int cap)
    {
        tables.push_back(Table(loc, cap));
        cout << "TABLE ADDED!" << endl;
    }

    void viewTables()
    {
        cout << "==== TABLES ====" << endl;

        for (int i = 0; i < tables.size(); i++)
        {
            tables[i].display();
        }
    }

    int getTableCount()
    {
        return tables.size();
    }

    Table getTable(int i)
    {
        return tables[i];
    }

    // ---------------- CATALOG ACCESS ----------------
    BookCatalog *getCatalog()
    {
        return &catalog;
    }
};

class Authentication
{
private:
    librarian *lib_admin; // single librarian (or extend later)

public:
    Authentication(librarian *l = nullptr)
    {
        lib_admin = l;
    }

    void setLibrarian(librarian *l)
    {
        lib_admin = l;
    }

    // ---------------- USER LOGIN ----------------
    User *loginUser(library *lib)
    {
        string uname, pass;

        cout << "\n===== USER LOGIN =====" << endl;
        cout << "Enter Username: ";
        getline(cin, uname);
        cout << "Enter Password: ";
        getline(cin, pass);

        for (int i = 0; i < lib->getUserCount(); i++)
        {
            User *u = lib->getUserByIndex(i);

            if (u->getUsername() == uname && u->getPassword() == pass)
            {
                cout << "LOGIN SUCCESSFUL!\n";
                return u;
            }
        }

        cout << "INVALID CREDENTIALS!\n";
        return nullptr;
    }

    // ---------------- LIBRARIAN LOGIN ----------------
    librarian *loginLibrarian()
    {
        if (!lib_admin)
        {
            cout << "No librarian registered!\n";
            return nullptr;
        }

        string uname, pass;

        cout << "\n===== LIBRARIAN LOGIN =====" << endl;
        cout << "Enter Username: ";
        getline(cin, uname);
        cout << "Enter Password: ";
        getline(cin, pass);

        if (lib_admin->getUsername() == uname &&
            lib_admin->getPassword() == pass)
        {
            cout << "LIBRARIAN LOGIN SUCCESSFUL!\n";
            return lib_admin;
        }

        cout << "INVALID LIBRARIAN CREDENTIALS!\n";
        return nullptr;
    }

    // ---------------- LOGOUT ----------------
    void logout()
    {
        cout << "LOGGED OUT SUCCESSFULLY!\n";
    }
};

//=============================================
//-----------------USER MENUS-----------------
void User::viewFullDetails(library *lib)
{
    int choice;

    do
    {
        cout << "\n===== USER DASHBOARD =====" << endl;
        cout << "1. View Your Data" << endl;
        cout << "2. View Borrowed Books" << endl;
        cout << "3. View Transactions" << endl;
        cout << "4. View Fines" << endl;
        cout << "5. View Reservations" << endl;
        cout << "6. Exit" << endl;

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "\n--- USER DETAILS ---" << endl;
            viewdetails(); // polymorphism
            break;

        case 2:
        {
            cout << "\n--- BORROWED BOOKS ---" << endl;

            const vector<int> &books = getBorrowedBooks();

            if (books.empty())
            {
                cout << "None" << endl;
            }
            else
            {
                for (int i = 0; i < books.size(); i++)
                {
                    cout << books[i] << " ";
                }
                cout << endl;
            }
            break;
        }

        case 3:
            cout << "\n--- TRANSACTIONS ---" << endl;
            lib->viewUserTransactions(userid);
            break;

        case 4:
            cout << "\n--- FINES ---" << endl;
            lib->viewUserFines(userid);
            break;

        case 5:
            cout << "\n--- RESERVATIONS ---" << endl;
            lib->viewUserReservations(userid);
            break;

        case 6:
            cout << "Exiting Dashboard..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 6);
}

void User::searchBooks(library *lib)
{
    int choice;

    do
    {
        cout << "\n===== BOOK SEARCH MENU =====" << endl;
        cout << "1. Search by ID" << endl;
        cout << "2. Search by Title" << endl;
        cout << "3. Search by Author" << endl;
        cout << "4. Search by Category" << endl;
        cout << "5. Search by Publisher" << endl;
        cout << "6. Search by Year" << endl;
        cout << "7. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        Book *b = nullptr;

        switch (choice)
        {
        case 1:
        {
            int id;
            cout << "Enter Book ID: ";
            cin >> id;
            b = lib->getCatalog()->SearchbyID(id);
            break;
        }
        case 2:
        {
            string t;
            cin.ignore();
            cout << "Enter Title: ";
            getline(cin, t);
            b = lib->getCatalog()->SearchbyTitle(t);
            break;
        }
        case 3:
        {
            string a;
            cin.ignore();
            cout << "Enter Author: ";
            getline(cin, a);
            b = lib->getCatalog()->SearchbyAuthor(a);
            break;
        }
        case 4:
        {
            string c;
            cin.ignore();
            cout << "Enter Category: ";
            getline(cin, c);
            b = lib->getCatalog()->SearchbyCategory(c);
            break;
        }
        case 5:
        {
            string p;
            cin.ignore();
            cout << "Enter Publisher: ";
            getline(cin, p);
            b = lib->getCatalog()->SearchbyPublisher(p);
            break;
        }
        case 6:
        {
            int y;
            cout << "Enter Year: ";
            cin >> y;
            b = lib->getCatalog()->SearchbyYOP(y);
            break;
        }
        case 7:
            cout << "Exiting Search Menu..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

        if (b != nullptr)
        {
            cout << "\n--- BOOK FOUND ---\n";
            b->display();
        }
        else if (choice >= 1 && choice <= 6)
        {
            cout << "Book not found!\n";
        }

    } while (choice != 7);
}

void User::reservationMenu(library *lib)
{
    int choice;

    do
    {
        cout << "\n===== RESERVATION MENU =====" << endl;
        cout << "1. Auto Reserve Table" << endl;
        cout << "2. Manual Reserve Table" << endl;
        cout << "3. Reserve Book" << endl;
        cout << "4. Exit" << endl;

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string date, start, end;

            cin.ignore();
            cout << "Enter Date: ";
            getline(cin, date);

            cout << "Enter Start Time: ";
            getline(cin, start);

            cout << "Enter End Time: ";
            getline(cin, end);

            lib->reserveTableAuto(userid, date, start, end);
            break;
        }

        case 2:
        {
            int tid;
            string date, start, end;

            cout << "Enter Table ID: ";
            cin >> tid;

            cin.ignore();
            cout << "Enter Date: ";
            getline(cin, date);

            cout << "Enter Start Time: ";
            getline(cin, start);

            cout << "Enter End Time: ";
            getline(cin, end);

            lib->reserveTableManual(userid, tid, date, start, end);
            break;
        }

        case 3:
        {
            int bid;
            string date, start, end;

            cout << "Enter Book ID: ";
            cin >> bid;

            cin.ignore();

            cout << "Enter Date (YYYY-MM-DD): ";
            getline(cin, date);

            cout << "Enter Start Time (HH:MM): ";
            getline(cin, start);

            cout << "Enter End Time (HH:MM): ";
            getline(cin, end);

            lib->reserveBook(userid, bid, date, start, end);
            break;
        }

        case 4:
            cout << "Exiting Reservation Menu..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 4);
}

void User::bookActionMenu(library *lib)
{
    int choice;
    string title;

    do
    {
        cout << "\n===== BOOK ACTIONS =====" << endl;
        cout << "1. Borrow Book" << endl;
        cout << "2. Return Book" << endl;
        cout << "3. Read On Site" << endl;
        cout << "4. Download Book" << endl;
        cout << "5. Exit" << endl;

        cout << "Enter choice: ";
        cin >> choice;

        cin.ignore();

        switch (choice)
        {
        case 1:
        {
            int mins;

            cout << "Enter Book Title: ";
            getline(cin, title);

            cout << "Borrow Duration (minutes): ";
            cin >> mins;
            cin.ignore();

            lib->borrowBook(this, title, mins);
            break;
        }

        case 2:
        {
            cout << "Enter Book Title: ";
            getline(cin, title);

            lib->returnBook(this, title);
            break;
        }

        case 3:
        {
            cout << "Enter Book Title: ";
            getline(cin, title);

            lib->readOnSite(this, title);
            break;
        }

        case 4:
        {
            cout << "Enter Book Title: ";
            getline(cin, title);

            lib->downloadBook(this, title);
            break;
        }

        case 5:
            cout << "Exiting Book Actions..." << endl;
            return;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 5);
}

void User::userMenu(library *lib)
{
    int choice;

    do
    {
        cout << "\n===== USER MAIN MENU =====" << endl;
        cout << "1. View Dashboard" << endl;
        cout << "2. Book Search" << endl;
        cout << "3. Book Actions (Borrow / Return / Read / Download)" << endl;
        cout << "4. Reservations" << endl;
        cout << "5. Exit" << endl;

        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            viewFullDetails(lib);
            break;

        case 2:
            searchBooks(lib);
            break;

        case 3:
            bookActionMenu(lib); // NEW
            break;

        case 4:
            reservationMenu(lib);
            break;

        case 5:
            cout << "Exiting..." << endl;
            break;
        }

    } while (choice != 5);
}
//=======================================
//----------LIBRARY MENUS---------------

void librarian::viewFullDetails(library *lib)
{
    int choice;

    do
    {
        cout << "\n===== LIBRARIAN DASHBOARD =====" << endl;
        cout << "1. View Librarian Info" << endl;
        cout << "2. View All Users" << endl;
        cout << "3. View All Transactions" << endl;
        cout << "4. View All Fines" << endl;
        cout << "5. View All Reservations" << endl;
        cout << "6. View Book Catalog" << endl;
        cout << "7. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "\n--- LIBRARIAN INFO ---" << endl;
            displaylibrarian_details();
            break;

        case 2:
            cout << "\n--- ALL USERS ---" << endl;
            lib->viewUsers();
            break;

        case 3:
            cout << "\n--- ALL TRANSACTIONS ---" << endl;
            lib->viewTransactions();
            break;

        case 4:
            cout << "\n--- ALL FINES ---" << endl;
            lib->viewFines();
            break;

        case 5:
            cout << "\n--- ALL RESERVATIONS ---" << endl;
            lib->viewAllReservations();
            break;

        case 6:
            cout << "\n--- BOOK CATALOG ---" << endl;
            lib->getCatalog()->ViewBookCatalog();
            break;

        case 7:
            cout << "Exiting Librarian Dashboard..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 7);
}

void librarian::manageBooks(library *lib)
{
    int choice;

    do
    {
        cout << "\n===== BOOK MANAGEMENT =====" << endl;
        cout << "1. Add Book" << endl;
        cout << "2. Remove Book" << endl;
        cout << "3. Back" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
        {
            int id, yop, copies;
            string title, author, category, isbn, publisher;

            cout << "Enter Book ID: ";
            cin >> id;
            cin.ignore();

            cout << "Enter Title: ";
            getline(cin, title);

            cout << "Enter Author: ";
            getline(cin, author);

            cout << "Enter Category: ";
            getline(cin, category);

            cout << "Enter ISBN: ";
            getline(cin, isbn);

            cout << "Enter Publisher: ";
            getline(cin, publisher);

            cout << "Enter Year: ";
            cin >> yop;

            cout << "Enter Available Copies: ";
            cin >> copies;

            Book *b = new Book(id, title, author, category, isbn, publisher, yop);
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

        case 3:
            cout << "Returning..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 3);
}

void librarian::librarianMainMenu(library *lib)
{
    int choice;

    do
    {
        cout << "\n===== LIBRARIAN MAIN MENU =====" << endl;
        cout << "1. View System Details" << endl;
        cout << "2. Manage Books" << endl;
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            viewFullDetails(lib);
            break;

        case 2:
            manageBooks(lib);
            break;

        case 3:
            cout << "Exiting Librarian System..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 3);
}

int main()
{
    library lib;

    // ===== SETUP LIBRARIAN =====
    librarian admin(1, "Admin", "admin@lib.com", "03000000000");
    admin.setCredentials("admin", "123");

    Authentication auth(&admin);

    int choice;

    do
    {
        cout << "\n===== LIBRARY SYSTEM =====" << endl;
        cout << "1. Register User" << endl;
        cout << "2. Login as User" << endl;
        cout << "3. Login as Librarian" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;
        cin.ignore(); // important for getline

        switch (choice)
        {
        case 1:
            lib.registerUserInLibrary();
            break;

        case 2:
        {
            User *u = auth.loginUser(&lib);
            if (u)
            {
                u->userMenu(&lib);
                auth.logout();
            }
            break;
        }

        case 3:
        {
            librarian *l = auth.loginLibrarian();
            if (l)
            {
                l->librarianMainMenu(&lib);
                auth.logout();
            }
            break;
        }

        case 4:
            cout << "Exiting System..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 4);

    return 0;
}
