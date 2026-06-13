// admin("admin", "123");
#include <iostream>
#include <vector>
#include <ctime>
#include <string>
using namespace std;

class book;
class Transaction;
class Fines;
class librarian;
class BookCatalog;
class User;
class library;
class Membership;

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
    return ctime(&t);
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

class Membership
{
private:
    int membershipID;
    static int next_id;

    int userID;
    string userType;
    string userName;

    time_t activationTime;
    time_t expiryTime;
    bool active;

    // ---- CONSTANTS ----
    static const double FINE_DISCOUNT;      // 15%
    static const int DEFAULT_DURATION_DAYS; // 365 days

public:
    // DEFAULT CONSTRUCTOR
    Membership()
        : membershipID(next_id++), userID(0),
          userType("NONE"), userName("NONE"),
          activationTime(0), expiryTime(0), active(false) {}

    // PARAMETERIZED CONSTRUCTOR
    Membership(int uid, string utype, string uname, int durationDays = 365)
        : membershipID(next_id++), userID(uid),
          userType(utype), userName(uname), active(true)
    {
        activationTime = getCurrentTime();
        expiryTime = addMinutes(activationTime, durationDays * 24 * 60);
    }

    // DESTRUCTOR
    ~Membership()
    {
        cout << "MEMBERSHIP OF " << userName << " DESTROYED!" << endl;
    }

    // ---- GETTERS ----
    int getMembershipID() const { return membershipID; }
    int getUserID() const { return userID; }
    string getUserType() const { return userType; }
    string getUserName() const { return userName; }
    bool isActive() const { return active; }

    // Check if membership is still valid
    bool isValid() const
    {
        return active && (getCurrentTime() <= expiryTime);
    }

    // ---- FINE DISCOUNT ----
    double applyDiscount(double originalFine) const
    {
        if (!isValid())
            return originalFine;
        return originalFine * (1.0 - FINE_DISCOUNT); // 15% off
    }

    // ---- BORROW LIMIT BOOST ----
    int getBorrowBoost() const
    {
        if (!isValid())
            return 0;
        if (userType == "STUDENT")
            return 2; // 3 -> 5
        if (userType == "FACULTY")
            return 2; // 5 -> 7
        if (userType == "GUEST")
            return 1; // 1 -> 2
        return 0;
    }

    // ---- CANCEL ----
    void cancel()
    {
        active = false;
        cout << "MEMBERSHIP CANCELLED FOR " << userName << "." << endl;
    }

    // ---- DISPLAY ----
    friend ostream &operator<<(ostream &os, const Membership &m)
    {
        os << "=========== MEMBERSHIP DETAILS ===========" << endl
           << "| MEMBERSHIP ID  : " << m.membershipID << endl
           << "| MEMBER NAME    : " << m.userName << endl
           << "| USER TYPE      : " << m.userType << endl
           << "| STATUS         : " << (m.isValid() ? "ACTIVE" : "EXPIRED/INACTIVE") << endl
           << "| FINE DISCOUNT  : 15%" << endl
           << "| BORROW BOOST   : +" << m.getBorrowBoost() << " BOOKS" << endl
           << "| ACTIVATED ON   : " << timeToString(m.activationTime)
           << "| EXPIRES ON     : " << timeToString(m.expiryTime)
           << "==========================================" << endl;
        return os;
    }
};

// STATIC MEMBER DEFINITIONS
int Membership::next_id = 1;
const double Membership::FINE_DISCOUNT = 0.15;
const int Membership::DEFAULT_DURATION_DAYS = 365;

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

    Membership *membership;

public:
    // CONSTRUCTOR
    User(string n = "NONE", string c = "xxxxx-xxxxxxx-x",
         string e = "abc@gmail.com", string p = "03xxxxxxxxx")
    {
        userid = next_id++;
        name = n;
        CNIC = c;
        email = e;
        phonenumber = p;
        membership = nullptr;
    }

    virtual ~User()
    {
        delete membership;
        cout << "USER " << name << " DESTROYED." << endl;
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
    virtual void printTo(ostream &os) const = 0;
    virtual int getFineRate() = 0;

    friend ostream &operator<<(ostream &os, const User &u)
    {
        u.printTo(os);
        return os;
    }

    // ================= BORROW =================
    bool borrowbook(int id)
    {
        if ((int)borrowedbook.size() < getborrowlimit())
        {
            borrowedbook.push_back(id);
            return true;
        }
        cout << "BORROW LIMIT REACHED!" << endl;
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
        cout << "BOOK NOT BORROWED BY THIS USER!" << endl;
        return false;
    }

    //===========MEMBERSHIP INTERATIONS================
    // ---- MEMBERSHIP ACCESS ----
    Membership *getMembership() { return membership; }
    bool hasMembership() { return (membership != nullptr && membership->isValid()); }

    // Activate membership for this user
    void activateMembership(int durationDays = 365)
    {
        if (hasMembership())
        {
            cout << "USER ALREADY HAS AN ACTIVE MEMBERSHIP!" << endl;
            return;
        }
        delete membership; // clean up expired one if any
        membership = new Membership(userid, get_usertype(), name, durationDays);
        cout << "\nMEMBERSHIP ACTIVATED SUCCESSFULLY!" << endl;
        cout << *membership;
    }

    // Cancel membership
    void cancelMembership()
    {
        if (!membership)
        {
            cout << "NO MEMBERSHIP TO CANCEL!" << endl;
            return;
        }
        membership->cancel();
    }

    // View membership
    void viewMembership()
    {
        if (!membership)
        {
            cout << "YOU DO NOT HAVE A MEMBERSHIP.PURCHASE ONE FROM THE MENU!" << endl;
            return;
        }
        cout << *membership;
    }

    // ---- BORROW LIMIT (membership-aware) ----

    int getEffectiveBorrowLimit()
    {
        return getborrowlimit() + (hasMembership() ? membership->getBorrowBoost() : 0);
    }

    // ---- FINE DISCOUNT (membership-aware) ----
    double applyMembershipDiscount(double fine)
    {
        if (hasMembership())
            return membership->applyDiscount(fine);
        return fine;
    }

    // ================= FULL DASHBOARD =================
    void viewFullDetails(library *lib);
    void searchBooks(library *lib);
    void reservationMenu(library *lib);
    void bookActionMenu(library *lib);
    void membershipMenu(library *lib);
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

    void printTo(ostream &os) const override
    {
        os << "============= STUDENT DETAILS =============" << endl
           << "| NAME           : " << name << endl
           << "| SEAT NO        : " << seatno << endl
           << "| MAJOR          : " << major << endl
           << "| USER TYPE      : STUDENT" << endl
           << "===========================================" << endl;
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

    void printTo(ostream &os) const override
    {
        os << "============= FACULTY DETAILS =============" << endl
           << "| NAME           : " << name << endl
           << "| DEPARTMENT     : " << department << endl
           << "| DESIGNATION    : " << designation << endl
           << "| USER TYPE      : FACULTY" << endl
           << "===========================================" << endl;
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

    void printTo(ostream &os) const override
    {
        os << "============== GUEST DETAILS ==============" << endl
           << "| NAME           : " << name << endl
           << "| PURPOSE        : " << purpose << endl
           << "| USER TYPE      : GUEST" << endl
           << "===========================================" << endl;
    }
    int getFineRate() override { return 20; }
};

// ================= REGISTER FUNCTION =================
User *User::registerUser()
{
    int choice;
    cout << "SELECT USER TYPE:\n1. STUDENT\n2. FACULTY\n3. GUEST\nCHOICE: ";
    cin >> choice;
    cin.ignore();

    string name, CNIC, email, phone;
    string username, password;

    cout << "ENTER USERNAME: ";
    getline(cin, username);

    cout << "ENTER PASSWORD: ";
    getline(cin, password);

    cout << "ENTER NAME: ";
    getline(cin, name);

    cout << "ENTER CNIC: ";
    getline(cin, CNIC);

    cout << "ENTER EMAIL: ";
    getline(cin, email);

    cout << "ENTER PHONE: ";
    getline(cin, phone);

    if (choice == 1)
    {
        string seat, major;

        cout << "ENTER SEAT NUMBER: ";
        getline(cin, seat);
        cout << "ENTER MAJOR: ";
        getline(cin, major);

        User *u = new Student(name, CNIC, email, phone, seat, major);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }
    else if (choice == 2)
    {
        string dept, des;

        cout << "ENTER DEPARTMENT: ";
        getline(cin, dept);
        cout << "ENTER DESIGNATION: ";
        getline(cin, des);

        User *u = new Faculty(name, CNIC, email, phone, dept, des);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }

    else if (choice == 3)
    {
        string purpose;

        cout << "ENTER PURPOSE: ";
        getline(cin, purpose);

        User *u = new Guest(name, CNIC, email, phone, purpose);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }
    else
    {
        cout << "INVALID CHOICE!" << endl;
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

    //================= OPERATOR OVERLOADING ==================
    bool operator==(const Book &other) const { return bookid == other.bookid; }
    bool operator==(const string &t) const { return title == t; }
    bool operator==(const int &y) const { return year_of_publication == y; }

    // ================= DISPLAY =================

    friend ostream &operator<<(ostream &os, const Book &b)
    {
        os << "================= BOOK DETAILS =================" << endl
           << "| BOOK ID          : " << b.bookid << endl
           << "| TITLE            : " << b.title << endl
           << "| AUTHOR           : " << b.author << endl
           << "| CATEGORY         : " << b.category << endl
           << "| ISBN             : " << b.ISBN << endl
           << "| PUBLISHER        : " << b.publisher << endl
           << "| YEAR             : " << b.year_of_publication << endl
           << "| AVAILABLE COPIES : " << b.available_copies << endl
           << "| STATUS           : " << (b.availability_status ? "AVAILABLE" : "NOT AVAILABLE") << endl
           << "| DOWNLOADED       : " << b.download_count << " TIMES" << endl
           << "| READ ON SITE     : " << b.read_onsite_count << " TIMES" << endl
           << "| TIMES BORROWED   : " << b.timesborrowed_count << " TIMES" << endl
           << "================================================" << endl;
        return os;
    }
};

class BookCatalog
{
private:
    vector<Book *> BK;

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
            cout << *b;
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

    friend ostream &operator<<(ostream &os, const librarian &l)
    {
        os << "=============== LIBRARIAN DETAILS ===============" << endl
           << "| ID       : " << l.librarian_id << endl
           << "| NAME     : " << l.name << endl
           << "| EMAIL    : " << l.email << endl
           << "| PHONE #  : " << l.phone_no << endl
           << "=================================================" << endl;
        return os;
    }

    //=============SYSTEM DETAILS=============
    void viewFullDetails(library *lib);
    //=====BOOK MENU======
    void manageBooks(library *lib);
    //=====TABLE MENU======
    void manageTables(library *lib);
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

    int markreturned()
    {
        returned = true;
        return_time = getCurrentTime();
        return_date = timeToString(return_time);
        return getOverdueMinutes();
    }

    // DISPLAY via friend ostream
    friend ostream &operator<<(ostream &os, Transaction &t)
    {
        os << "=============== TRANSACTION DETAILS ===============" << endl
           << "| TRANSACTION ID : " << t.transaction_id << endl
           << "| USER ID        : " << t.user_id << endl
           << "| BOOK ID        : " << t.book_id << endl
           << "| BORROWED ON    : " << timeToString(t.borrow_time) << endl
           << "| DUE DATE       : " << timeToString(t.due_time) << endl
           << "| RETURN DATE    : " << t.return_date << endl
           << "| OVERDUE        : " << t.getOverdueMinutes() << endl
           << "| RETURNED       : " << (t.returned ? "YES" : "NO") << endl
           << "===================================================" << endl;
        return os;
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
    double originalAmount;
    double finalAmount;
    bool discountApplied;

public:
    // CONSTRUCTOR
    Fines()
        : user_id(0), book_id(0), minuteslate(0),
          rate(0), originalAmount(0), finalAmount(0), discountApplied(false) {}

    /*  User pointer passed so we can:
 1. Get fine rate  (polymorphism)
 2. Check membership and apply 15% discount automatically
*/

    Fines(int u, int b, int mins, User *user)
        : user_id(u), book_id(b)
    {
        minuteslate = (mins < 0) ? 0 : mins;
        rate = user->getFineRate();
        originalAmount = rate * minuteslate;

        // ---- MEMBERSHIP DISCOUNT ----
        if (user->hasMembership())
        {
            finalAmount = user->applyMembershipDiscount(originalAmount);
            discountApplied = true;
        }
        else
        {
            finalAmount = originalAmount;
            discountApplied = false;
        }
    }
    // DESTRUCTOR
    ~Fines() { cout << "FINES DESTROYED!" << endl; }

    // GETTERS (useful for future)
    double getAmount() { return finalAmount; }
    double getOriginal() { return originalAmount; }
    int getUserID() { return user_id; }
    int getBookID() { return book_id; }
    bool wasDiscounted() { return discountApplied; }

    // DISPLAY via friend ostream
    friend ostream &operator<<(ostream &os, const Fines &f)
    {
        os << "================= FINE DETAILS =================" << endl
           << "| USER ID        : " << f.user_id << endl
           << "| BOOK ID        : " << f.book_id << endl
           << "| MINUTES LATE   : " << f.minuteslate << endl
           << "| RATE           : RS. " << f.rate << " /MIN" << endl
           << "| ORIGINAL AMT   : RS. " << f.originalAmount << endl;

        if (f.discountApplied)
        {
            os << "| DISCOUNT       : 15% (MEMBERSHIP)" << endl
               << "| FINAL AMT      : RS. " << f.finalAmount << endl;
        }
        else
        {
            os << "| FINAL AMT      : RS. " << f.finalAmount << endl;
        }

        os << "=================================================" << endl;
        return os;
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

    friend ostream &operator<<(ostream &os, const Reservation &r)
    {
        if (!r.active)
            return os;

        os << "=============== RESERVATION DETAILS ===============" << endl
           << "| RESERVATION ID : " << r.reservationid << endl
           << "| USER ID        : " << r.userid << endl;

        if (r.bookid)
            os << "| BOOK ID        : " << r.bookid << endl;
        else
            os << "| TABLE ID       : " << r.tableid << endl;

        os << "| DATE           : " << r.date << endl
           << "| START TIME     : " << r.startTime << endl
           << "| END TIME       : " << r.endTime << endl
           << "===================================================" << endl;

        return os;
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

    friend ostream &operator<<(ostream &os, const Table &t)
    {
        os << "================= TABLE DETAILS =================" << endl
           << "| TABLE ID   : " << t.tableid << endl
           << "| LOCATION   : " << t.location << endl
           << "| CAPACITY   : " << t.capacity << endl
           << "=================================================" << endl;
        return os;
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
    vector<User *> users;             // was: User* users[100] (polymorphic â€” kept as pointer)

public:
    static const int MAX_TABLES = 10;

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
            cout << "USER REGISTERED SUCCESSFULLY!\n";
        }
        else
        {
            cout << "REGISTRATION FAILED!\n";
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
            cout << "ERROR: YOU HAVE OVERDUE BOOKS RETURN THEM FIRST!" << endl;
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
            cout << "BORROW TIME : " << timeToString(now) << endl;
            cout << "DUE TIME    : " << timeToString(addMinutes(now, duration_minutes)) << endl;
            cout << "FINE RATE   : Rs." << u->getFineRate() << " PER MINUTE IF LATE!" << endl;
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
                    cout << "RETURN TIME: " << timeToString(getCurrentTime()) << endl;

                    if (overdue_mins > 0)
                    {
                        fines.emplace_back(u->get_userid(), book->get_bookid(), overdue_mins, u);
                        cout << "FINE GENERATED!" << endl;
                        cout << "OVERDUE    : " << overdue_mins << " MINUTES" << endl;
                        cout << "FINE AMOUNT: Rs." << (u->getFineRate() * overdue_mins) << endl;
                    }
                    else
                    {
                        cout << "RETURNED ON TIME! NO FINE." << endl;
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
        for (int i = 0; i < (int)transactions.size(); i++)
            cout << transactions[i];
    }

    void viewFines()
    {
        cout << "==== ALL FINES ====" << endl;
        for (int i = 0; i < (int)fines.size(); i++)
            cout << fines[i];
    }

    void viewUserTransactions(int userID)
    {
        cout << "==== TRANSACTIONS FOR USER ID: " << userID << " ====" << endl;
        for (int i = 0; i < (int)transactions.size(); i++)
            if (transactions[i].getuserid() == userID)
                cout << transactions[i];
    }

    void viewUserFines(int userID)
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

                if (!u)
                    continue;

                int fine = calculateLiveFine(transactions[i], u);
                if (fine > 0)
                {
                    cout << "|USER ID: " << userID
                         << " |BOOK ID: " << transactions[i].getbookid()
                         << " |STATUS: UNPAID (LIVE)"
                         << " |LIVE FINE: Rs." << fine
                         << (u->hasMembership() ? " [15% discount applied]" : "")
                         << " |" << endl;
                    found = true;
                }
            }
        }

        if (!found)
            cout << "NO FINES FOUND!" << endl;
    }

    void viewUserReservations(int uid)
    {
        cout << "==== USER RESERVATIONS ====" << endl;
        bool found = false;
        for (int i = 0; i < (int)reservations.size(); i++)
            if (reservations[i].get_userid() == uid)
            {
                cout << reservations[i];
                found = true;
            }
        if (!found)
            cout << "NO RESERVATION FOUND!" << endl;
    }

    void viewAllReservations()
    {
        for (int i = 0; i < (int)reservations.size(); i++)
        {
            cout << reservations[i];
        }
    }

    void viewUsers()
    {
        cout << "================= ALL USERS =================" << endl;
        for (int i = 0; i < users.size(); i++)
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

    void viewUsersByType(string type)
    {
        cout << "=========== USERS OF TYPE: " << type << " ===========" << endl;

        for (int i = 0; i < users.size(); i++)
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
    void viewAllMemberships()
    {
        cout << "=============== ALL MEMBERSHIPS ===============" << endl;

        bool found = false;

        for (int i = 0; i < users.size(); i++)
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
        // CHECK TABLE EXISTS
        bool tableExists = false;
        for (int i = 0; i < (int)tables.size(); i++)
        {
            if (tables[i].get_id() == tid)
            {
                tableExists = true;
                break;
            }
        }

        if (!tableExists)
        {
            cout << "INVALID TABLE ID! TABLE DOES NOT EXIST.\n";
            return;
        }

        for (int i = 0; i < (int)reservations.size(); i++)
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

    //----------------TABLE INTERATIONS----------------
    void addTable(string loc, int cap)
    {
        if ((int)tables.size() >= MAX_TABLES)
        {
            cout << "TABLE LIMIT REACHED! MAXIMUM " << MAX_TABLES << " TABLES ALLOWED." << endl;
            return;
        }
        tables.push_back(Table(loc, cap));
        cout << "TABLE ADDED! (" << tables.size() << "/" << MAX_TABLES << " TABLES)" << endl;
    }

    void removeTable(int tid)
    {
        for (int i = 0; i < (int)tables.size(); i++)
        {
            if (tables[i].get_id() == tid)
            {
                tables.erase(tables.begin() + i);
                cout << "TABLE " << tid << " REMOVED!" << endl;
                return;
            }
        }
        cout << "TABLE NOT FOUND!" << endl;
    }

    void viewTables()
    {
        cout << "==== TABLES (" << tables.size() << "/" << MAX_TABLES << ") ====" << endl;

        if (tables.empty())
        {
            cout << "NO TABLES AVAILABLE." << endl;
            return;
        }

        for (int i = 0; i < (int)tables.size(); i++)
        {
            cout << tables[i];
        }
    }

    void viewAvailableTables(string date, string start, string end)
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
            int tid = tables[i].get_id();
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

    int getTableCount()
    {
        return (int)tables.size();
    }

    bool hasAvailableTable(string date, string start, string end)
    {
        for (int i = 0; i < (int)tables.size(); i++)
        {
            int tid = tables[i].get_id();
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

        cout << "ENTER USERNAME: ";
        getline(cin, uname);

        cout << "ENTER PASSWORD: ";
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
            cout << "NO LIBRARIAN REGISTERED!" << endl;
            return nullptr;
        }

        string uname, pass;

        cout << "\n===== LIBRARIAN LOGIN =====" << endl;

        cout << "ENTER USERNAME: ";
        getline(cin, uname);

        cout << "ENTER PASSWORD: ";
        getline(cin, pass);

        if (lib_admin->getUsername() == uname &&
            lib_admin->getPassword() == pass)
        {
            cout << "LIBRARIAN LOGIN SUCCESSFUL!" << endl;
            return lib_admin;
        }

        cout << "INVALID LIBRARIAN CREDENTIALS!" << endl;
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
        cout << "\n================ USER DASHBOARD ================" << endl;
        cout << "1. VIEW YOUR DATA" << endl;
        cout << "2. VIEW BORROWED BOOKS" << endl;
        cout << "3. VIEW TRANSACTIONS" << endl;
        cout << "4. VIEW FINES" << endl;
        cout << "5. VIEW RESERVATIONS" << endl;
        cout << "6. EXIT" << endl;

        cout << "ENTER CHOICE: ";
        cin >> choice;

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
            cout << "\n------------ BORROWED BOOKS ------------" << endl;

            const vector<int> &books = getBorrowedBooks();

            if (books.empty())
            {
                cout << "NONE" << endl;
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
void User::searchBooks(library *lib)
{
    int choice;

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

        cout << "ENTER CHOICE: ";
        cin >> choice;

        Book *b = nullptr;

        switch (choice)
        {
        case 1:
        {
            int id;
            cout << "ENTER BOOK ID: ";
            cin >> id;
            b = lib->getCatalog()->SearchbyID(id);
            break;
        }
        case 2:
        {
            string t;
            cin.ignore();
            cout << "ENTER TITLE: ";
            getline(cin, t);
            b = lib->getCatalog()->SearchbyTitle(t);
            break;
        }
        case 3:
        {
            string a;
            cin.ignore();
            cout << "ENTER AUTHOR: ";
            getline(cin, a);
            b = lib->getCatalog()->SearchbyAuthor(a);
            break;
        }
        case 4:
        {
            string c;
            cin.ignore();
            cout << "ENTER CATEGORY: ";
            getline(cin, c);
            b = lib->getCatalog()->SearchbyCategory(c);
            break;
        }
        case 5:
        {
            string p;
            cin.ignore();
            cout << "ENTER PUBLISHER: ";
            getline(cin, p);
            b = lib->getCatalog()->SearchbyPublisher(p);
            break;
        }
        case 6:
        {
            int y;
            cout << "ENTER YEAR: ";
            cin >> y;
            b = lib->getCatalog()->SearchbyYOP(y);
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
        else if (choice >= 1 && choice <= 6)
        {
            cout << "BOOK NOT FOUND!" << endl;
        }

    } while (choice != 7);
}
void User::reservationMenu(library *lib)
{
    int choice;

    do
    {
        cout << "\n============= RESERVATION MENU =============" << endl;
        cout << "1. AUTO RESERVE TABLE" << endl;
        cout << "2. MANUAL RESERVE TABLE" << endl;
        cout << "3. RESERVE BOOK" << endl;
        cout << "4. EXIT" << endl;

        cout << "ENTER CHOICE: ";
        cin >> choice;

        switch (choice)
        {
        case 1:
        {
            string date, start, end;

            cin.ignore();
            cout << "ENTER DATE(DD/MM/YYYY): ";
            getline(cin, date);

            cout << "ENTER START TIME(HH:MM): ";
            getline(cin, start);

            cout << "ENTER END TIME(HH:MM): ";
            getline(cin, end);

            lib->reserveTableAuto(userid, date, start, end);
            break;
        }

        case 2:
        {
            int tid;
            string date, start, end;

            cin.ignore();
            cout << "ENTER DATE(DD/MM/YYYY): ";
            getline(cin, date);

            cout << "ENTER START TIME(HH:MM): ";
            getline(cin, start);

            cout << "ENTER END TIME(HH:MM): ";
            getline(cin, end);

            cout << "\n--------------- AVAILABLE TABLES ---------------" << endl;
            lib->viewAvailableTables(date, start, end);
            cout << "------------------------------------------------" << endl;

            if (!lib->hasAvailableTable(date, start, end))
                break;

            cout << "ENTER TABLE ID: ";
            cin >> tid;

            cin.ignore();

            lib->reserveTableManual(userid, tid, date, start, end);
            break;
        }

        case 3:
        {
            int bid;
            string date, start, end;

            cout << "ENTER BOOK ID: ";
            cin >> bid;

            cin.ignore();

            cout << "ENTER DATE (YYYY-MM-DD): ";
            getline(cin, date);

            cout << "ENTER START TIME (HH:MM): ";
            getline(cin, start);

            cout << "ENTER END TIME (HH:MM): ";
            getline(cin, end);

            lib->reserveBook(userid, bid, date, start, end);
            break;
        }

        case 4:
            cout << "EXITING RESERVATION MENU..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 4);
}

void User::bookActionMenu(library *lib)
{
    int choice;
    string title;

    do
    {
        cout << "\n================ BOOK ACTIONS ================" << endl;
        cout << "1. BORROW BOOK" << endl;
        cout << "2. RETURN BOOK" << endl;
        cout << "3. READ ON SITE" << endl;
        cout << "4. DOWNLOAD BOOK" << endl;
        cout << "5. EXIT" << endl;

        cout << "ENTER CHOICE: ";
        cin >> choice;

        cin.ignore();

        switch (choice)
        {
        case 1:
        {
            int duration = 0;

            cout << "ENTER BOOK TITLE: ";
            getline(cin, title);

            bool valid = false;

            while (!valid)
            {
                cout << "\nSELECT TIME UNIT:\n";
                cout << "1. MINUTES\n";
                cout << "2. HOURS\n";
                cout << "3. DAYS\n";
                cout << "CHOICE: ";
                cin >> choice;

                if (choice == 1)
                {
                    int mins;
                    cout << "ENTER MINUTES: ";
                    cin >> mins;
                    duration = mins;
                    valid = true;
                }
                else if (choice == 2)
                {
                    int hours;
                    cout << "ENTER HOURS: ";
                    cin >> hours;
                    duration = hours * 60;
                    valid = true;
                }
                else if (choice == 3)
                {
                    int days;
                    cout << "ENTER DAYS: ";
                    cin >> days;
                    duration = days * 24 * 60;
                    valid = true;
                }
                else
                {
                    cout << "INVALID CHOICE! TRY AGAIN." << endl;
                }
            }

            cin.ignore();
            lib->borrowBook(this, title, duration);
            break;
        }

        case 2:
        {
            cout << "ENTER BOOK TITLE: ";
            getline(cin, title);

            lib->returnBook(this, title);
            break;
        }

        case 3:
        {
            cout << "ENTER BOOK TITLE: ";
            getline(cin, title);

            lib->readOnSite(this, title);
            break;
        }

        case 4:
        {
            cout << "ENTER BOOK TITLE: ";
            getline(cin, title);

            lib->downloadBook(this, title);
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
void User::membershipMenu(library *lib)
{
    int choice;

    do
    {
        cout << "\n================ MEMBERSHIP MENU ================" << endl;
        cout << "1. VIEW MEMBERSHIP STATUS" << endl;
        cout << "2. PURCHASE / ACTIVATE MEMBERSHIP" << endl;
        cout << "3. CANCEL MEMBERSHIP" << endl;
        cout << "4. BACK" << endl;

        cout << "ENTER CHOICE: ";
        cin >> choice;

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

                cout << "\nCONFIRM MEMBERSHIP PURCHASE? (1=YES / 0=NO): ";
                int confirm;
                cin >> confirm;

                if (confirm == 1)
                    activateMembership(365);
                else
                    cout << "MEMBERSHIP PURCHASE CANCELLED." << endl;
            }
            break;

        case 3:
            cancelMembership();
            break;

        case 4:
            cout << "RETURNING TO MAIN MENU..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 4);
}

void User::userMenu(library *lib)
{
    int choice;

    do
    {
        cout << "\n================ USER MAIN MENU ================" << endl;
        cout << "1. VIEW DASHBOARD" << endl;
        cout << "2. BOOK SEARCH" << endl;
        cout << "3. BOOK ACTIONS (BORROW / RETURN / READ / DOWNLOAD)" << endl;
        cout << "4. RESERVATIONS" << endl;
        cout << "5. MEMBERSHIP" << endl;
        cout << "6. EXIT" << endl;

        cout << "ENTER CHOICE: ";
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
            bookActionMenu(lib);
            break;

        case 4:
            reservationMenu(lib);
            break;

        case 5:
            membershipMenu(lib);
            break;

        case 6:
            cout << "EXITING..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 6);
}

//=======================================
//----------LIBRARY MENUS---------------
void librarian::viewFullDetails(library *lib)
{
    int choice;

    do
    {
        cout << "\n=============== LIBRARIAN DASHBOARD ===============" << endl;
        cout << "1. VIEW LIBRARIAN INFO" << endl;
        cout << "2. VIEW ALL USERS" << endl;
        cout << "3. VIEW ALL TRANSACTIONS" << endl;
        cout << "4. VIEW ALL FINES" << endl;
        cout << "5. VIEW ALL RESERVATIONS" << endl;
        cout << "6. VIEW BOOK CATALOG" << endl;
        cout << "7. VIEW ALL MEMBERSHIPS" << endl;
        cout << "8. EXIT" << endl;

        cout << "ENTER CHOICE: ";
        cin >> choice;

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
            cout << "\n------------ ALL TRANSACTIONS ------------" << endl;
            lib->viewTransactions();
            break;

        case 4:
            cout << "\n---------------- FINES ----------------" << endl;
            lib->viewFines();
            break;

        case 5:
            cout << "\n------------- RESERVATIONS -------------" << endl;
            lib->viewAllReservations();
            break;

        case 6:
            cout << "\n------------- BOOK CATALOG -------------" << endl;
            lib->getCatalog()->ViewBookCatalog();
            break;

        case 7:
            cout << "\n------------ ALL MEMBERSHIPS ------------" << endl;
            lib->viewAllMemberships();
            break;

        case 8:
            cout << "EXITING LIBRARIAN DASHBOARD..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 8);
}

void librarian::manageBooks(library *lib)
{
    int choice;

    do
    {
        cout << "\n================ BOOK MANAGEMENT ================" << endl;
        cout << "1. ADD BOOK" << endl;
        cout << "2. REMOVE BOOK" << endl;
        cout << "3. BACK" << endl;

        cout << "ENTER CHOICE: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
        {
            int id, yop, copies;
            string title, author, category, isbn, publisher;

            cout << "ENTER BOOK ID: ";
            cin >> id;
            cin.ignore();

            cout << "ENTER TITLE: ";
            getline(cin, title);

            cout << "ENTER AUTHOR: ";
            getline(cin, author);

            cout << "ENTER CATEGORY: ";
            getline(cin, category);

            cout << "ENTER ISBN: ";
            getline(cin, isbn);

            cout << "ENTER PUBLISHER: ";
            getline(cin, publisher);

            cout << "ENTER YEAR: ";
            cin >> yop;

            cout << "ENTER AVAILABLE COPIES: ";
            cin >> copies;

            Book *b = new Book(id, title, author, category, isbn, publisher, yop);
            b->setAvailableCopies(copies);

            addbook(b, lib->getCatalog());
            break;
        }

        case 2:
        {
            string title;

            cout << "ENTER TITLE TO REMOVE: ";
            cin.ignore();
            getline(cin, title);

            removeBook(title, lib->getCatalog());
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

        cout << "ENTER CHOICE: ";
        cin >> choice;
        cin.ignore();

        switch (choice)
        {
        case 1:
        {
            string loc;
            int cap;

            cout << "ENTER LOCATION: ";
            getline(cin, loc);

            cout << "ENTER CAPACITY: ";
            cin >> cap;
            cin.ignore();

            lib->addTable(loc, cap);
            break;
        }

        case 2:
            lib->viewTables();
            break;

        case 3:
        {
            lib->viewTables();
            int tid;
            cout << "ENTER TABLE ID TO REMOVE: ";
            cin >> tid;
            cin.ignore();
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

        cout << "ENTER CHOICE: ";
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
            manageTables(lib);
            break;

        case 4:
            cout << "EXITING LIBRARIAN SYSTEM..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 4);
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
        cout << "\n================ LIBRARY SYSTEM ================" << endl;
        cout << "1. REGISTER USER" << endl;
        cout << "2. LOGIN AS USER" << endl;
        cout << "3. LOGIN AS LIBRARIAN" << endl;
        cout << "4. EXIT" << endl;

        cout << "ENTER CHOICE: ";
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
            cout << "EXITING SYSTEM..." << endl;
            break;

        default:
            cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 4);

    return 0;
}