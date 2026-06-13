#include <iostream>
using namespace std;

class book;
class Transaction;
class Fines;
class librarian;
class BookCatalog;
class User;
class library;

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
    int borrowedbook[20];   //VECTORS
    int borrowedbookcount;

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

        borrowedbookcount = 0;
        for (int i = 0; i < 20; i++)
            borrowedbook[i] = 0;
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
    int *getBorrowedBooks() { return borrowedbook; }
    int getBorrowedCount() { return borrowedbookcount; }

    // ================= POLYMORPHISM =================
    virtual int getborrowlimit() = 0;
    virtual string get_usertype() = 0;
    virtual void viewdetails() = 0;
    virtual int getFineRate() = 0;

    // ================= BORROW =================
    bool borrowbook(int id)
    {
        if (borrowedbookcount < getborrowlimit())
        {
            borrowedbook[borrowedbookcount++] = id;
            return true;
        }
        cout << "Borrow limit reached!" << endl;
        return false;
    }

    // ================= RETURN =================
    bool returnbook(int id)
    {
        for (int i = 0; i < borrowedbookcount; i++)
        {
            if (borrowedbook[i] == id)
            {
                for (int j = i; j < borrowedbookcount - 1; j++)
                    borrowedbook[j] = borrowedbook[j + 1];

                borrowedbookcount--;
                borrowedbook[borrowedbookcount] = 0; // clean last slot
                return true;
            }
        }
        cout << "Book not borrowed by this user!" << endl;
        return false;
    }

    // ================= FULL DASHBOARD =================
    void viewFullDetails(library *lib);
    void searchBooks(library *lib);
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
    cout << "Enter Password: ";
    cout << "Enter Name: ";
    getline(cin, name);
    cout << "Enter CNIC: ";
    getline(cin, CNIC);
    cout << "Enter Email: ";
    getline(cin, email);
    cout << "Enter Phone: ";
    getline(cin, phone);

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
    Book *BK[100];  //VECTORS
    int book_count = 0;

public:
    BookCatalog() : book_count(0) {}
    ~BookCatalog() { cout << "BOOKCATALOG DESTROYED!" << endl; }

    void addbook(Book *b)
    {
        if (book_count < 100)
        {
            BK[book_count++] = b;
            cout << "BOOK ADDED SUCCESSFULLY!" << endl;
        }
        else
            cout << "LIBRARY FULL!" << endl;
    }

    void removeBook(string t)
    {
        int pos = -1;

        if (book_count == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return;
        }

        for (int i = 0; i < book_count; i++)
        {
            if (BK[i]->get_title() == t)
            {
                pos = i;
                break;
            }
        }

        if (pos == -1)
        {
            cout << "BOOK NOT FOUND!" << endl;
            return;
        }

        delete BK[pos]; //  delete correct object

        for (int i = pos; i < book_count - 1; i++)
            BK[i] = BK[i + 1];

        book_count--;

        cout << "BOOK REMOVED!" << endl;
    }

    // ================= SEARCH FUNCTIONS =================

    Book *SearchbyID(int id)
    {
        for (int i = 0; i < book_count; i++)
            if (BK[i]->get_bookid() == id)
                return BK[i];

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyTitle(string t)
    {
        for (int i = 0; i < book_count; i++)
            if (BK[i]->get_title() == t)
                return BK[i];

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyAuthor(string a)
    {
        for (int i = 0; i < book_count; i++)
            if (BK[i]->get_author() == a)
                return BK[i];

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyCategory(string c)
    {
        for (int i = 0; i < book_count; i++)
            if (BK[i]->get_category() == c)
                return BK[i];

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyPublisher(string p)
    {
        for (int i = 0; i < book_count; i++)
            if (BK[i]->get_publisher() == p)
                return BK[i];

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Book *SearchbyYOP(int yop)
    {
        for (int i = 0; i < book_count; i++)
            if (BK[i]->get_YOP() == yop)
                return BK[i];

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    // ================= DISPLAY =================

    void ViewBookCatalog() const
    {
        if (book_count == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return;
        }

        cout << "==== BOOK CATALOG ====" << endl;

        for (int i = 0; i < book_count; i++)
            BK[i]->display();
    }

    // ================= GETTERS =================

    int get_bookcount() { return book_count; }

    // IMPORTANT ADD (for future flexibility)
    Book *getBookAtIndex(int index)
    {
        if (index >= 0 && index < book_count)
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

    void displaylibrarian_details()
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
    static int next_id; // auto ID generator
    int transaction_id;
    int user_id;
    int book_id;
    string issue_date;
    string return_date;
    string due_date;
    bool returned;

public:
    // CONSTRUCTOR
    Transaction()
        : transaction_id(next_id++), user_id(0), book_id(0),
          issue_date("UNKNOWN"), return_date("NOT RETURNED"),
          due_date("UNKNOWN"), returned(false) {}

    Transaction(int uid, int bid, string i, string d)
        : transaction_id(next_id++), user_id(uid), book_id(bid),
          issue_date(i), return_date("NOT RETURNED"),
          due_date(d), returned(false) {}

    // DESTRUCTOR
    ~Transaction() { cout << "TRANSACTION DESTROYED!" << endl; }

    // GETTERS
    int getuserid() { return user_id; }
    int getbookid() { return book_id; }
    int getTransactionID() { return transaction_id; }
    string getissuedate() { return issue_date; }
    string getduedate() { return due_date; }
    bool isReturned() { return returned; }

    // SETTERS
    void setIssueDate(string i) { issue_date = i; }
    void setDueDate(string d) { due_date = d; }

    // MARK RETURNED (flexible for future time system)
    void markreturned(string rdate = "RETURNED")
    {
        returned = true;
        return_date = rdate;
    }

    // DISPLAY FOR SPECIFIC USER
    void displayuser(int id)
    {
        if (user_id == id)
        {
            cout << "|TID:" << transaction_id
                 << " |UID:" << user_id
                 << " |BID:" << book_id
                 << " |ISSUE:" << issue_date
                 << " |DUE:" << due_date
                 << " |RETURN:" << return_date
                 << " |STATUS:" << (returned ? "YES" : "NO") << " |" << endl;
        }
    }

    // DISPLAY ALL
    void displayAll()
    {
        cout << "|TID:" << transaction_id
             << " |UID:" << user_id
             << " |BID:" << book_id
             << " |ISSUE:" << issue_date
             << " |DUE:" << due_date
             << " |RETURN:" << return_date
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
    int dayslate;
    double rate;
    double amount;

public:
    // CONSTRUCTOR
    Fines()
        : user_id(0), book_id(0), dayslate(0), rate(0), amount(0) {}

    Fines(int u, int b, int d, User *user)
        : user_id(u), book_id(b)
    {
        // safety check
        if (d < 0)
            dayslate = 0;
        else
            dayslate = d;

        rate = user->getFineRate(); // ✅ polymorphism
        amount = rate * dayslate;
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
        if (user_id == userID)
        {
            cout << "|USER ID: " << user_id
                 << " |BOOK ID: " << book_id
                 << " |DAYS LATE: " << dayslate
                 << " |RATE: " << rate
                 << " |AMOUNT: " << amount << " |" << endl;
        }
    }

    // DISPLAY (ALL)
    void displayAll()
    {
        cout << "|USER ID: " << user_id
             << " |BOOK ID: " << book_id
             << " |DAYS LATE: " << dayslate
             << " |RATE: " << rate
             << " |AMOUNT: " << amount << " |" << endl;
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
    string Reservationdate;
    string Reservationtime;
    bool active; // NEW (important)

public:
    // CONSTRUCTOR
    Reservation()
        : userid(0), bookid(0), tableid(0),
          Reservationdate("UNKNOWN"), Reservationtime("UNKNOWN"), active(true) {}

    Reservation(int uid, int bid, int tid, string date, string time)
    {
        reservationid = next_id++; // AUTO ID
        userid = uid;
        bookid = bid;
        tableid = tid;
        Reservationdate = date;
        Reservationtime = time;
        active = true;
    }

    // DESTRUCTOR
    ~Reservation() { cout << "RESERVATION DESTROYED!" << endl; }

    // GETTERS
    int get_reservationid() { return reservationid; }
    int get_userid() { return userid; }
    int get_bookid() { return bookid; }
    int get_tableid() { return tableid; }
    string get_date() { return Reservationdate; }
    string get_time() { return Reservationtime; }
    bool isActive() { return active; } // NEW

    // SETTERS
    void set_reservationid(int rid) { reservationid = rid; }
    void set_userid(int uid) { userid = uid; }
    void set_bookid(int bid) { bookid = bid; }
    void set_tableid(int tid) { tableid = tid; }
    void set_date(string date) { Reservationdate = date; }
    void set_time(string time) { Reservationtime = time; }

    // CANCEL (instead of direct delete logic dependency)
    void cancel()
    {
        active = false;
    }

    // TYPE CHECKERS (VERY IMPORTANT)
    bool isBookReservation()
    {
        return bookid != 0;
    }

    bool isTableReservation()
    {
        return tableid != 0;
    }

    // DISPLAY
    void display()
    {
        if (!active)
            return; // skip cancelled

        cout << "|RESERVATION ID: " << reservationid
             << " |USER ID: " << userid;

        if (bookid != 0)
            cout << " |BOOK ID: " << bookid;
        else
            cout << " |TABLE ID: " << tableid;

        cout << " |DATE: " << Reservationdate
             << " |TIME: " << Reservationtime << " |" << endl;
    }
};

int Reservation::next_id = 1;

class library
{
private:
    BookCatalog catalog;

    Reservation *reservations[100];  //VECTORS
    int reservation_count = 0;

    Transaction *transaction[100];  //VECTORS
    int transaction_count = 0;

    Fines *fines[100];      //VECTORS
    int fine_count = 0;

    User *users[100];    //VECTORS
    int user_count = 0;

public:
    // ---------------- USER ----------------
    User *getUserByIndex(int index)
    {
        if (index >= 0 && index < user_count)
            return users[index];
        return nullptr;
    }

    int getUserCount()
    {
        return user_count;
    }

    void registerUserInLibrary()
    {
        if (user_count >= 100)
        {
            cout << "USER LIMIT REACHED!" << endl;
            return;
        }

        User *newUser = User::registerUser();
        if (newUser)
        {
            users[user_count++] = newUser;
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

    bool borrowBook(User *u, string title, string issue, string due)
    {
        Book *book = catalog.SearchbyTitle(title);
        if (!book)
            return false;

        if (book->getAvailableCopies() == 0)
        {
            cout << "NO COPIES!" << endl;
            return false;
        }

        if (u->borrowbook(book->get_bookid()))
        {
            if (transaction_count >= 100)
            {
                cout << "TRANSACTION LIMIT REACHED!" << endl;
                return false;
            }

            book->setAvailableCopies(book->getAvailableCopies() - 1);
            book->incrementBorrowCount();

            transaction[transaction_count++] =
                new Transaction(u->get_userid(), book->get_bookid(), issue, due);

            cout << "BOOK BORROWED!" << endl;
            return true;
        }
        return false;
    }

    // ---------------- RETURN ----------------
    bool returnBook(User *u, string title, int daysLate = 0)
    {
        Book *book = catalog.SearchbyTitle(title);
        if (!book)
            return false;

        if (u->returnbook(book->get_bookid()))
        {
            book->setAvailableCopies(book->getAvailableCopies() + 1);

            if (daysLate > 0)
            {
                if (fine_count < 100)
                {
                    fines[fine_count++] =
                        new Fines(u->get_userid(), book->get_bookid(), daysLate, u);
                    cout << "FINE GENERATED!" << endl;
                }
            }

            for (int i = 0; i < transaction_count; i++)
            {
                if (transaction[i]->getbookid() == book->get_bookid() &&
                    transaction[i]->getuserid() == u->get_userid() &&
                    !transaction[i]->isReturned()) // 🔥 FIX
                {
                    transaction[i]->markreturned("RETURNED");
                    break;
                }
            }

            cout << "BOOK RETURNED!" << endl;
            return true;
        }
        return false;
    }

    // ---------------- VIEW ----------------
    void viewTransactions()
    {
        cout << "==== ALL TRANSACTIONS ====" << endl;
        for (int i = 0; i < transaction_count; i++)
            transaction[i]->displayAll();
    }

    void viewFines()
    {
        cout << "==== ALL FINES ====" << endl;
        for (int i = 0; i < fine_count; i++)
            fines[i]->displayAll();
    }

    void viewUserTransactions(int userID)
    {
        cout << "==== TRANSACTIONS FOR USER ID: " << userID << " ====" << endl;
        for (int i = 0; i < transaction_count; i++)
            transaction[i]->displayuser(userID);
    }

    void viewUserFines(int userID)
    {
        cout << "==== FINES FOR USER ID: " << userID << " ====" << endl;
        for (int i = 0; i < fine_count; i++)
            fines[i]->display(userID);
    }

    void viewUsers()
    {
        cout << "==== ALL USERS ====" << endl;
        for (int i = 0; i < user_count; i++)
        {
            cout << "|NAME: " << users[i]->get_name()
                 << " |EMAIL: " << users[i]->get_email()
                 << " |PHONE: " << users[i]->get_phone()
                 << " |USER TYPE: " << users[i]->get_usertype() << " |" << endl;
        }
    }

    void viewUsersByType(string type)
    {
        cout << "==== USERS OF TYPE: " << type << " ====" << endl;
        for (int i = 0; i < user_count; i++)
        {
            if (users[i]->get_usertype() == type)
            {
                cout << "|NAME: " << users[i]->get_name()
                     << " |EMAIL: " << users[i]->get_email()
                     << " |PHONE: " << users[i]->get_phone()
                     << " |USER TYPE: " << users[i]->get_usertype() << " |" << endl;
            }
        }
    }

    // ---------------- RESERVATION ----------------
    void reserveBook(int uid, int bkid, string date, string time)
    {
        if (reservation_count >= 100)
        {
            cout << "RESERVATION LIMIT REACHED!" << endl;
            return;
        }

        Book *book = catalog.SearchbyID(bkid);
        if (!book)
        {
            cout << "BOOK NOT FOUND!" << endl;
            return;
        }

        int rid = reservation_count;
        reservations[reservation_count++] =
            new Reservation(uid, bkid, 0, date, time);

        cout << "BOOK RESERVED! UserID: " << uid << " BookID: " << bkid << endl;
    }

    void reserveTable(int uid, int tblid, string date, string time)
    {
        if (reservation_count >= 100)
        {
            cout << "RESERVATION LIMIT REACHED!" << endl;
            return;
        }

        int rid = reservation_count;
        reservations[reservation_count++] =
            new Reservation(uid, 0, tblid, date, time);

        cout << "TABLE RESERVED! UserID: " << uid << " TableID: " << tblid << endl;
    }

    void cancelReservation(int rid)
    {
        for (int i = 0; i < reservation_count; i++)
        {
            if (reservations[i]->get_reservationid() == rid)
            {
                reservations[i]->cancel();
                cout << "RESERVATION CANCELLED! ID: " << rid << endl;
                return;
            }
        }

        cout << "INVALID RESERVATION ID!" << endl;
    }

    bool checkAvailability(int rid)
    {
        for (int i = 0; i < reservation_count; i++)
        {
            if (reservations[i]->get_reservationid() == rid)
            {
                if (!reservations[i]->isActive())
                {
                    cout << "AVAILABLE!" << endl;
                    return true;
                }

                cout << "NOT AVAILABLE!" << endl;
                return false;
            }
        }

        cout << "INVALID RESERVATION ID!" << endl;
        return false;
    }

    void viewAllReservations()
    {
        cout << "==== ALL RESERVATIONS ====" << endl;
        for (int i = 0; i < reservation_count; i++)
            if (reservations[i] != nullptr)
                reservations[i]->display();
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
        cout << "5. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            cout << "\n--- USER DETAILS ---" << endl;

            // POLYMORPHISM
            viewdetails();

            break;

        case 2:
        {
            cout << "\n--- BORROWED BOOKS ---" << endl;

            int *books = getBorrowedBooks();
            int count = getBorrowedCount();

            if (count == 0)
                cout << "None" << endl;
            else
            {
                for (int i = 0; i < count; i++)
                    cout << books[i] << " ";
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
            cout << "Exiting Dashboard..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 5);
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

        // If book found → show + actions
        if (b != nullptr)
        {
            cout << "\n--- BOOK FOUND ---\n";
            b->display();

            int action;
            cout << "\nWhat do you want to do?\n";
            cout << "1. Borrow Book\n";
            cout << "2. Read On Site\n";
            cout << "3. Download Book\n";
            cout << "4. Back\n";
            cout << "Enter choice: ";
            cin >> action;

            switch (action)
            {
            case 1:
            {
                string issue, due;
                cin.ignore();
                cout << "Enter Issue Date: ";
                getline(cin, issue);
                cout << "Enter Due Date: ";
                getline(cin, due);

                lib->borrowBook(this, b->get_title(), issue, due);
                break;
            }

            case 2:
                // FIXED (User + title)
                lib->readOnSite(this, b->get_title());
                break;

            case 3:
                // FIXED (User + title)
                lib->downloadBook(this, b->get_title());
                break;

            case 4:
                break;

            default:
                cout << "Invalid choice!" << endl;
            }
        }

    } while (choice != 7);
}

void User::userMenu(library *lib)
{
    int choice;

    do
    {
        cout << "\n===== USER MAIN MENU =====" << endl;
        cout << "1. View Your Data" << endl; // dashboard
        cout << "2. Book Search" << endl;    // search system
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            viewFullDetails(lib); // ✅ your dashboard
            break;

        case 2:
            searchBooks(lib); // ✅ separated search system
            break;

        case 3:
            cout << "Exiting User Menu..." << endl;
            break;

        default:
            cout << "Invalid choice!" << endl;
        }

    } while (choice != 3);
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
        cout << "1. View System Details" << endl; // your existing menu
        cout << "2. Manage Books" << endl;        // new menu
        cout << "3. Exit" << endl;
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
            viewFullDetails(lib); // your existing function
            break;

        case 2:
            manageBooks(lib); //  new function
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
                u->userMenu(&lib); //  user system
                auth.logout();
            }
            break;
        }

        case 3:
        {
            librarian *l = auth.loginLibrarian();
            if (l)
            {
                l->librarianMainMenu(&lib); //  librarian system
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