// admin("admin", "123");
#include <iostream> //INPUT OUTPUT STREAM 
#include <vector> // FOR DYNAMIC ARRAYS
#include<exception> // FOR CUSTOM EXCEPTIONS 
#include <ctime>  //FOR DATE TIME FNS
#include <string>  //FOR STRINGS
#include <cstring> // FOR CHAR ARRYS AND TO USE STRING FNS
#include <fstream>  //FOR FILE HANDLING 
using namespace std;
// ============================================================
// CUSTOM EXCEPTION CLASSES
// ============================================================

// 1. File could not be opened for reading or writing
class FileException : public exception
{
    string message;

public:
    FileException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// 2. Book not found in catalog, or no copies available
class BookException : public exception
{
    string message;

public:
    BookException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// 3. User not found, duplicate username, or borrow limit reached
class UserException : public exception
{
    string message;

public:
    UserException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// 4. Wrong username or password
class AuthException : public exception
{
    string message;

public:
    AuthException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// 5. Table not found, already full, or time conflict
class ReservationException : public exception
{
    string message;

public:
    ReservationException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// 6. Overdue books blocking a borrow, or fine calculation issue
class TransactionException : public exception
{
    string message;

public:
    TransactionException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

struct UserRecord
{
    int id;
    char name[50];
    char email[50];
    char phone[20];
    char cnic[20];
    char type[20];
    char username[50];
    char password[50];
    // Student fields
    char seatno[30];
    char major[50];
    // Faculty fields
    char department[50];
    char designation[50];
    // Guest fields
    char purpose[100];
};

struct BookRecord
{
    int id;
    char title[100];
    char author[100];
    char category[50];
    char isbn[30];
    char publisher[50];
    int year;
    int copies;

    int download_count;
    int read_onsite_count;
    int times_borrowed;
};

struct TransactionRecord
{
    int tid;
    int uid;
    int bid;
    time_t borrow_time;
    time_t due_time;
    time_t return_time;
    char return_date[30];
    bool returned;
};

struct FineRecord
{
    int user_id;
    int book_id;
    int minuteslate;
    double rate;
    double originalAmount;
    double finalAmount;
    bool discountApplied;
};

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

// ============================================================
// INPUT VALIDATION FUNCTIONS
// ============================================================

// ---- INTEGER INPUT ----
int getValidInt(string prompt)
{
    int value;
    while (true)
    {
        cout << prompt;
        cin >> value;

        if (cin.fail())
        {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "INVALID INPUT! PLEASE ENTER A NUMBER." << endl;
        }
        else
        {
            cin.ignore(1000, '\n');
            return value;
        }
    }
}

// ---- INTEGER IN RANGE ----
int getValidIntInRange(string prompt, int min, int max)
{
    int value;
    while (true)
    {
        value = getValidInt(prompt);

        if (value < min || value > max)
            cout << "PLEASE ENTER A NUMBER BETWEEN " << min << " AND " << max << "." << endl;
        else
            return value;
    }
}

// ---- NON EMPTY STRING ----
string getValidString(string prompt) //FOR EMPTY SPACES ONLY
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
            cout << "INPUT CANNOT BE EMPTY!" << endl;
        else
            return value;
    }
}

// ---- LETTERS ONLY (for names) ----
string getValidAlphaString(string prompt)  //TO VALIDATE ONLY LETTERS ARE ADDED NO EMPTY SPACES
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "INPUT CANNOT BE EMPTY!" << endl;
            continue; //SKIPPING THE REST OF THIS LOOP AND MOVING TO NEXT ITERATION
        }

        bool valid = true;
        for (int i = 0; i < (int)value.size(); i++)
        {
            if (!isalpha(value[i]) && value[i] != ' ')
            {
                valid = false;
                break;
            }
        }

        if (!valid)
            cout << "ONLY LETTERS ALLOWED!" << endl;
        else
            return value;
    }
}

// ---- CNIC (xxxxx-xxxxxxx-x format) ----
string getValidCNIC(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool valid = true; 

        if (value.size() != 15)
            valid = false;
        else
        {
            for (int i = 0; i < 15; i++)
            {
                if (i == 5 || i == 13)
                {
                    if (value[i] != '-')
                    {
                        valid = false;
                        break;
                    }
                }
                else
                {
                    if (!isdigit(value[i]))
                    {
                        valid = false;
                        break;
                    }
                }
            }
        }

        if (!valid)
            cout << "INVALID CNIC! FORMAT MUST BE: xxxxx-xxxxxxx-x" << endl;
        else
            return value;
    }
}

// ---- EMAIL ----
string getValidEmail(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool hasAt = false;
        bool hasDot = false;

        for (int i = 0; i < (int)value.size(); i++)
        {
            if (value[i] == '@')
                hasAt = true;
            if (value[i] == '.' && hasAt)
                hasDot = true;
        }

        if (value.empty() || !hasAt || !hasDot)
            cout << "INVALID EMAIL! MUST CONTAIN @ AND ." << endl;
        else
            return value;
    }
}

// ---- PHONE (03xxxxxxxxx, 11 digits) ----
string getValidPhone(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool valid = true;

        if (value.size() != 11)
            valid = false;
        else if (value[0] != '0' || value[1] != '3') //ONLY PAKISTANI NUMBER
            valid = false;
        else
        {
            for (int i = 0; i < 11; i++)
            {
                if (!isdigit(value[i]))
                {
                    valid = false;
                    break;
                }
            }
        }

        if (!valid)
            cout << "INVALID PHONE! FORMAT MUST BE: 03xxxxxxxxx (11 digits)" << endl;
        else
            return value;
    }
}

// ---- PASSWORD (min 6 characters) ----
string getValidPassword(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.size() < 6)
            cout << "PASSWORD TOO SHORT! MINIMUM 6 CHARACTERS." << endl;
        else
            return value;
    }
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
           << "| BORROW BOOST    : +" << m.getBorrowBoost() << " BOOKS" << endl
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

    Membership *membership;   //COMPOSITION

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
    static void setNextID(int id) { next_id = id; }

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
    string getSeatNo() { return seatno; }
    string getMajor() { return major; }
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
    string getDepartment() { return department; }
    string getDesignation() { return designation; }
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
    string getPurpose() { return purpose; }

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
// To save the user in binary file
void saveUserToFile(User *u)
{
    ofstream file("users.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN users.dat FOR WRITING!");

    UserRecord ur = {}; // zero out everything first

    ur.id = u->get_userid();
    strcpy(ur.name, u->get_name().c_str());
    strcpy(ur.email, u->get_email().c_str());
    strcpy(ur.phone, u->get_phone().c_str());
    strcpy(ur.cnic, u->get_cnic().c_str());
    strcpy(ur.type, u->get_usertype().c_str());
    strcpy(ur.username, u->getUsername().c_str());
    strcpy(ur.password, u->getPassword().c_str());

    // type-specific fields
    if (u->get_usertype() == "STUDENT")
    {
        Student *s = dynamic_cast<Student *>(u);
        strcpy(ur.seatno, s->getSeatNo().c_str());
        strcpy(ur.major, s->getMajor().c_str());
    }
    else if (u->get_usertype() == "FACULTY")
    {
        Faculty *f = dynamic_cast<Faculty *>(u);
        strcpy(ur.department, f->getDepartment().c_str());
        strcpy(ur.designation, f->getDesignation().c_str());
    }
    else if (u->get_usertype() == "GUEST")
    {
        Guest *g = dynamic_cast<Guest *>(u);
        strcpy(ur.purpose, g->getPurpose().c_str());
    }

    file.write((char *)&ur, sizeof(ur));
    file.close();
}

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

        users.push_back(u);
    }

    file.close();
}

// ================= REGISTER FUNCTION =================
User *User::registerUser()
{
    int choice = getValidIntInRange("SELECT USER TYPE:\n1. STUDENT\n2. FACULTY\n3. GUEST\nCHOICE: ", 1, 3);

    string name = getValidAlphaString("ENTER NAME: ");
    string CNIC = getValidCNIC("ENTER CNIC: ");
    string email = getValidEmail("ENTER EMAIL: ");
    string phone = getValidPhone("ENTER PHONE: ");
    string username = getValidString("ENTER USERNAME: ");
    string password = getValidPassword("ENTER PASSWORD: ");

    if (choice == 1)
    {
        string seat = getValidString("ENTER SEAT NUMBER: ");
        string major = getValidAlphaString("ENTER MAJOR: ");

        User *u = new Student(name, CNIC, email, phone, seat, major);
        u->setUsername(username);
        u->setPassword(password);
        return u;
    }
    else if (choice == 2)
    {
        string dept = getValidAlphaString("ENTER DEPARTMENT: ");
        string des = getValidAlphaString("ENTER DESIGNATION: ");

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
    vector<Book *> BK; //COMPOSTION

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
void saveBookToFile(Book *b)
{
    ofstream file("books.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN books.dat FOR WRITING!");

    BookRecord br;
    br.id = b->get_bookid();
    strcpy(br.title, b->get_title().c_str());
    strcpy(br.author, b->get_author().c_str());
    strcpy(br.category, b->get_category().c_str());
    strcpy(br.isbn, b->get_isbn().c_str());
    strcpy(br.publisher, b->get_publisher().c_str());
    br.year = b->get_YOP();
    br.copies = b->getAvailableCopies();
    br.download_count = b->get_downloadcount();
    br.read_onsite_count = b->get_read_onsitecount();
    br.times_borrowed = b->get_timesborrowed_count();

    file.write((char *)&br, sizeof(br));
    if (!file)
        throw FileException("ERROR: FAILED TO WRITE BOOK RECORD TO FILE!");

    file.close();
}
void loadBooksFromFile(BookCatalog *catalog)
{
    ifstream file("books.dat", ios::binary);

    if (!file)
        return;

    BookRecord br;

    while (file.read((char *)&br, sizeof(br)))
    {
        Book *b = new Book(
            br.id,
            br.title,
            br.author,
            br.category,
            br.isbn,
            br.publisher,
            br.year);

        b->setAvailableCopies(br.copies);

        // restore counters
        for (int i = 0; i < br.download_count; i++)
            b->incrementDownloadCount();

        for (int i = 0; i < br.read_onsite_count; i++)
            b->incrementReadOnSiteCount();

        for (int i = 0; i < br.times_borrowed; i++)
            b->incrementBorrowCount();

        catalog->addbook(b);
    }

    file.close();
}

void rewriteBooksFile(BookCatalog *catalog)
{
    ofstream file("books.dat", ios::binary);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN books.dat FOR REWRITING!");

    for (int i = 0; i < catalog->get_bookcount(); i++)
    {
        Book *b = catalog->getBookAtIndex(i);
        BookRecord br;
        br.id = b->get_bookid();
        strcpy(br.title, b->get_title().c_str());
        strcpy(br.author, b->get_author().c_str());
        strcpy(br.category, b->get_category().c_str());
        strcpy(br.isbn, b->get_isbn().c_str());
        strcpy(br.publisher, b->get_publisher().c_str());
        br.year = b->get_YOP();
        br.copies = b->getAvailableCopies();
        br.download_count = b->get_downloadcount();
        br.read_onsite_count = b->get_read_onsitecount();
        br.times_borrowed = b->get_timesborrowed_count();

        file.write((char *)&br, sizeof(br));
        if (!file)
            throw FileException("ERROR: FAILED TO WRITE DURING BOOK FILE REWRITE!");
    }
    file.close();
}

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
    time_t getReturnTime() { return return_time; }
    string getReturnDate() { return return_date; }
    // SETTERS
    void setTransactionID(int id) { transaction_id = id; }
    void setUserID(int id) { user_id = id; }
    void setBookID(int id) { book_id = id; }
    void setBorrowTime(time_t t) { borrow_time = t; }
    void setDueTime(time_t t) { due_time = t; }
    void setReturnTime(time_t t) { return_time = t; }
    void setReturnDate(string d) { return_date = d; }
    void setReturned(bool r) { returned = r; }
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
void saveTransactionToFile(Transaction &t)
{
    ofstream file("transactions.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN transactions.dat FOR WRITING!");

    TransactionRecord tr = {};

    tr.tid = t.getTransactionID();
    tr.uid = t.getuserid();
    tr.bid = t.getbookid();
    tr.borrow_time = t.getBorrowTime();
    tr.due_time = t.getDueTime();
    tr.return_time = t.getReturnTime();
    tr.returned = t.isReturned();

    strcpy(tr.return_date, t.getReturnDate().c_str());

    file.write((char *)&tr, sizeof(tr));
    file.close();
}
void loadTransactionsFromFile(vector<Transaction> &transactions)
{
    ifstream file("transactions.dat", ios::binary);
    if (!file)
        return;

    TransactionRecord tr;

    while (file.read((char *)&tr, sizeof(tr)))
    {
        Transaction t;
        t.setTransactionID(tr.tid);
        t.setUserID(tr.uid);
        t.setBookID(tr.bid);
        t.setBorrowTime(tr.borrow_time);
        t.setDueTime(tr.due_time);
        t.setReturnTime(tr.return_time);
        t.setReturnDate(tr.return_date);
        t.setReturned(tr.returned);

        transactions.push_back(t);
    }

    file.close();
}

void rewriteTransactionsFile(vector<Transaction> &transactions)
{
    ofstream file("transactions.dat", ios::binary);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN transactions.dat FOR REWRITING!");

    for (int i = 0; i < (int)transactions.size(); i++)
    {
        TransactionRecord tr = {};

        tr.tid = transactions[i].getTransactionID();
        tr.uid = transactions[i].getuserid();
        tr.bid = transactions[i].getbookid();
        tr.borrow_time = transactions[i].getBorrowTime();
        tr.due_time = transactions[i].getDueTime();
        tr.return_time = transactions[i].getReturnTime();
        tr.returned = transactions[i].isReturned();

        strcpy(tr.return_date, transactions[i].getReturnDate().c_str());

        file.write((char *)&tr, sizeof(tr));
    }

    file.close();
}

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
    int getMinutesLate() { return minuteslate; }
    double getRate() { return rate; }
    // SETTERS
    void setUserID(int id) { user_id = id; }
    void setBookID(int id) { book_id = id; }
    void setMinutesLate(int m) { minuteslate = m; }
    void setRate(double r) { rate = r; }
    void setOriginalAmount(double a) { originalAmount = a; }
    void setFinalAmount(double a) { finalAmount = a; }
    void setDiscountApplied(bool d) { discountApplied = d; }
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
void saveFineToFile(Fines &f)
{
    ofstream file("fines.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN fines.dat FOR WRITING!");

    FineRecord fr = {};

    fr.user_id = f.getUserID();
    fr.book_id = f.getBookID();
    fr.minuteslate = f.getMinutesLate();
    fr.rate = f.getRate();
    fr.originalAmount = f.getOriginal();
    fr.finalAmount = f.getAmount();
    fr.discountApplied = f.wasDiscounted();

    file.write((char *)&fr, sizeof(fr));
    file.close();
}

void loadFinesFromFile(vector<Fines> &fines)
{
    ifstream file("fines.dat", ios::binary);
    if (!file)
        return;

    FineRecord fr;

    while (file.read((char *)&fr, sizeof(fr)))
    {
        Fines f;
        f.setUserID(fr.user_id);
        f.setBookID(fr.book_id);
        f.setMinutesLate(fr.minuteslate);
        f.setRate(fr.rate);
        f.setOriginalAmount(fr.originalAmount);
        f.setFinalAmount(fr.finalAmount);
        f.setDiscountApplied(fr.discountApplied);

        fines.push_back(f);
    }

    file.close();
}

void rewriteFinesFile(vector<Fines> &fines)
{
    ofstream file("fines.dat", ios::binary);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN fines.dat FOR REWRITING!");

    for (int i = 0; i < (int)fines.size(); i++)
    {
        FineRecord fr = {};

        fr.user_id = fines[i].getUserID();
        fr.book_id = fines[i].getBookID();
        fr.minuteslate = fines[i].getMinutesLate();
        fr.rate = fines[i].getRate();
        fr.originalAmount = fines[i].getOriginal();
        fr.finalAmount = fines[i].getAmount();
        fr.discountApplied = fines[i].wasDiscounted();

        file.write((char *)&fr, sizeof(fr));
    }

    file.close();
}

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
    vector<User *> &getUsersVector()
    {
        return users;
    }
    vector<Transaction> &getTransactionsVector() { return transactions; }
    vector<Fines> &getFinesVector() { return fines; }
    void registerUserInLibrary()
    {
        User *newUser = User::registerUser();
        if (newUser)
        {
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

    // ---------------- RETURN ----------------
    bool returnBook(User *u, string title)
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
                    cout << "FINE AMOUNT: Rs." << (u->getFineRate() * overdue_mins) << endl;
                }
                else
                    cout << "RETURNED ON TIME! NO FINE." << endl;

                break;
            }
        }
        return true;
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
            throw BookException("BOOK NOT FOUND WITH ID: " + to_string(bkid));

        if (book->getAvailableCopies() <= 0)
            throw BookException("NO COPIES AVAILABLE TO RESERVE FOR BOOK ID: " + to_string(bkid));

        reservations.emplace_back(uid, bkid, 0, date, start, end);
        book->decreaseCopy();
        cout << "BOOK RESERVED SUCCESSFULLY!\n";
    }

    void reserveTableManual(int uid, int tid, string date, string start, string end)
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
        cout << "TABLE RESERVED MANUALLY!\n";
    }

    void reserveTableAuto(int uid, string date, string start, string end)
    {
        if (tables.empty())
            throw ReservationException("NO TABLES EXIST IN THE LIBRARY!");

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
                reservations.emplace_back(uid, 0, tid, date, start, end);
                cout << "AUTO ASSIGNED TABLE ID: " << tid << endl;
                return;
            }
        }

        throw ReservationException("NO TABLES AVAILABLE FOR THE SELECTED TIME SLOT!");
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
            throw ReservationException("TABLE LIMIT REACHED! MAXIMUM " + to_string(MAX_TABLES) + " TABLES ALLOWED.");

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

    // ---------------- LIBRARIAN LOGIN ----------------
    librarian *loginLibrarian()
    {
        if (!lib_admin)
            throw AuthException("NO LIBRARIAN REGISTERED IN THE SYSTEM!");

        cout << "\n===== LIBRARIAN LOGIN =====" << endl;
        string uname = getValidString("ENTER USERNAME: ");
        string pass = getValidString("ENTER PASSWORD: ");

        if (lib_admin->getUsername() == uname && lib_admin->getPassword() == pass)
        {
            cout << "LIBRARIAN LOGIN SUCCESSFUL!" << endl;
            return lib_admin;
        }
        throw AuthException("INVALID LIBRARIAN CREDENTIALS!");
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
            b = lib->getCatalog()->SearchbyAuthor(a);
            break;
        }
        case 4:
        {
            string c = getValidString("ENTER CATEGORY: ");
            b = lib->getCatalog()->SearchbyCategory(c);
            break;
        }
        case 5:
        {
            string p = getValidString("ENTER PUBLISHER: ");
            b = lib->getCatalog()->SearchbyPublisher(p);
            break;
        }
        case 6:
        {
            int y = getValidIntInRange("ENTER YEAR: ", 1000, 2025);
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

    } while (choice != 7);
}
void User::reservationMenu(library *lib)
{
    int choice = 0;

    do
    {
        cout << "\n============= RESERVATION MENU =============" << endl;
        cout << "1. AUTO RESERVE TABLE" << endl;
        cout << "2. MANUAL RESERVE TABLE" << endl;
        cout << "3. RESERVE BOOK" << endl;
        cout << "4. EXIT" << endl;

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 4);

        switch (choice)
        {
        case 1:
        {
            string date = getValidString("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidString("ENTER START TIME (HH:MM): ");
            string end = getValidString("ENTER END TIME (HH:MM): ");

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

        case 2:
        {
            string date = getValidString("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidString("ENTER START TIME (HH:MM): ");
            string end = getValidString("ENTER END TIME (HH:MM): ");

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

        case 3:
        {
            int bid = getValidInt("ENTER BOOK ID: ");
            string date = getValidString("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidString("ENTER START TIME (HH:MM): ");
            string end = getValidString("ENTER END TIME (HH:MM): ");

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
            int duration = 0;

            title = getValidString("ENTER BOOK TITLE: ");

            int unit = getValidIntInRange("\nSELECT TIME UNIT:\n1. MINUTES\n2. HOURS\n3. DAYS\nCHOICE: ", 1, 3);

            if (unit == 1)
            {
                int mins = getValidIntInRange("ENTER MINUTES: ", 1, 10080); // max 1 week
                duration = mins;
            }
            else if (unit == 2)
            {
                int hours = getValidIntInRange("ENTER HOURS: ", 1, 168); // max 1 week
                duration = hours * 60;
            }
            else if (unit == 3)
            {
                int days = getValidIntInRange("ENTER DAYS: ", 1, 7); // max 1 week
                duration = days * 24 * 60;
            }

            try
            {
                lib->borrowBook(this, title, duration);
                rewriteBooksFile(lib->getCatalog());
            }
            catch (TransactionException &e)
            {
                cout << e.what() << endl;
            }
            catch (BookException &e)
            {
                cout << e.what() << endl;
            }
            catch (UserException &e)
            {
                cout << e.what() << endl;
            }
            catch (FileException &e)
            {
                cout << e.what() << endl;
            }
            break;
        }

        case 2:
        {
            title = getValidString("ENTER BOOK TITLE: ");
            try
            {
                lib->returnBook(this, title);
                rewriteBooksFile(lib->getCatalog());
                rewriteTransactionsFile(lib->getTransactionsVector());
            }
            catch (BookException &e)
            {
                cout << e.what() << endl;
            }
            catch (TransactionException &e)
            {
                cout << e.what() << endl;
            }
            catch (FileException &e)
            {
                cout << e.what() << endl;
            }
            break;
        }

        case 3:
        {
            title = getValidString("ENTER BOOK TITLE: ");
            lib->readOnSite(this, title);
            break;
        }

        case 4:
        {
            title = getValidString("ENTER BOOK TITLE: ");
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
    int choice = 0;

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

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 8);

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
            int id = getValidInt("ENTER BOOK ID: ");
            string title = getValidString("ENTER TITLE: ");
            string author = getValidAlphaString("ENTER AUTHOR: ");
            string category = getValidAlphaString("ENTER CATEGORY: ");
            string isbn = getValidString("ENTER ISBN: ");
            string publisher = getValidString("ENTER PUBLISHER: ");
            int yop = getValidIntInRange("ENTER YEAR: ", 1000, 2025);
            int copies = getValidIntInRange("ENTER AVAILABLE COPIES: ", 1, 1000);

            try
            {
                Book *b = new Book(id, title, author, category, isbn, publisher, yop);
                b->setAvailableCopies(copies);
                addbook(b, lib->getCatalog());
                saveBookToFile(b);
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
            string loc;
            int cap;

            loc = getValidString("ENTER LOCATION: ");
            cap = getValidIntInRange("ENTER CAPACITY: ", 1, 100);

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
            int tid;
            tid = getValidInt("ENTER TABLE ID TO REMOVE: ");
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

        choice = getValidIntInRange("ENTER CHOICE: ", 1, 4);

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
    try
    {
        loadUsersFromFile(lib.getUsersVector());
    }
    catch (FileException &e)
    {
        cout << e.what() << endl;
    }
    int maxID = 0;
    for (int i = 0; i < lib.getUserCount(); i++)
    {
        if (lib.getUserByIndex(i)->get_userid() > maxID)
            maxID = lib.getUserByIndex(i)->get_userid();
    }
    User::setNextID(lib.getUserCount() + 1);
    try
    {
        loadBooksFromFile(lib.getCatalog());
    }
    catch (FileException &e)
    {
        cout << e.what() << endl;
    }

    try
    {
        loadTransactionsFromFile(lib.getTransactionsVector());
    }
    catch (FileException &e)
    {
        cout << e.what() << endl;
    }

    try
    {
        loadFinesFromFile(lib.getFinesVector());
    }
    catch (FileException &e)
    {
        cout << e.what() << endl;
    }
    // ===== SETUP LIBRARIAN =====
    librarian admin(1, "Admin", "admin@lib.com", "03000000000");
    admin.setCredentials("admin", "123");

    Authentication auth(&admin);
    int choice = 0;
    do
    {
        cout << "\n================ LIBRARY SYSTEM ================" << endl;
        cout << "1. REGISTER USER" << endl;
        cout << "2. LOGIN AS USER" << endl;
        cout << "3. LOGIN AS LIBRARIAN" << endl;
        cout << "4. EXIT" << endl;

        choice = getValidIntInRange("\nENTER CHOICE: ", 1, 4);

        switch (choice)
        {
        case 1:
            lib.registerUserInLibrary();
            break;

        case 2:
        {
            try
            {
                User *u = auth.loginUser(&lib);
                if (u)
                {
                    u->userMenu(&lib);
                    auth.logout();
                }
            }
            catch (AuthException &e)
            {
                cout << e.what() << endl;
            }
            break;
        }

        case 3:
        {
            try
            {
                librarian *l = auth.loginLibrarian();
                if (l)
                {
                    l->librarianMainMenu(&lib);
                    auth.logout();
                }
            }
            catch (AuthException &e)
            {
                cout << e.what() << endl;
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