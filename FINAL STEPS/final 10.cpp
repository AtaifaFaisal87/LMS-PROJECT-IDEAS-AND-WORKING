// admin("admin", "123") password for librarian login

#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <cstring>
#include <fstream>
using namespace std;

// ============================================================
//                      RUNTIME LOGGER
// ============================================================

// used to suppress console messages during file loading
// so things like "BOOK ADDED" don't print when reading from disk
class RuntimeLogger
{
private:
    static bool suppressMessages;

public:
    static void enableSilentMode()
    {
        suppressMessages = true;
    }

    static void disableSilentMode()
    {
        suppressMessages = false;
    }

    static bool isSilent()
    {
        return suppressMessages;
    }
};

bool RuntimeLogger::suppressMessages = false;

// ============================================================
//                  CUSTOM EXCEPTION CLASSES
// ============================================================

// covers: File could not be opened for reading or writing
class FileException : public exception
{
    string message;

public:
    FileException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// covers: Book not found in catalog, or no copies available
class BookException : public exception
{
    string message;

public:
    BookException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// covers: user not found, duplicate username, borrow limit hit
class UserException : public exception
{
    string message;

public:
    UserException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// covers: Wrong username or password
class AuthException : public exception
{
    string message;

public:
    AuthException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// covers: table not found, full, or time conflict
class ReservationException : public exception
{
    string message;

public:
    ReservationException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// covers: Overdue books blocking a borrow, or fine calculation issue
class TransactionException : public exception
{
    string message;

public:
    TransactionException(string msg) : message(msg) {}
    const char *what() const noexcept override { return message.c_str(); }
};

// ============================================================
// STRUCTS - used for file I/O to flatten complex objects into plain data
// ============================================================

// All user types use the same struct.
// Unused fields stay empty depending on the user type.
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
    // Student
    char seatno[30];
    char major[50];
    // Faculty
    char department[50];
    char designation[50];
    // Guest
    char purpose[100];
    // tracks which books this user currently has borrowed
    int borrowed_count;
    int borrowed_ids[10]; // max 10 at a time
    // membership window
    bool hasMembership;
    int membershipDurationDays;
    time_t membershipActivation;
    time_t membershipExpiry;
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
    // usage stats for reports
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

struct TableRecord
{
    int id;
    char location[100];
    int capacity;
};

// forward declarations since these classes reference each other
class book;
class Transaction;
class Fines;
class librarian;
class BookCatalog;
class User;
class library;
class Membership;

// ============================================================
// TIME UTILITY FUNCTIONS
// ============================================================

time_t getCurrentTime()
{
    return time(nullptr);
}

string timeToString(time_t t)
{
    if (t == 0)
        return "N/A";
    return ctime(&t); // ctime returns a string with a newline at the end
}

// returns 0 if end is before start (avoids negative overdue minutes)
int minutesDiff(time_t start, time_t end)
{
    double diff = difftime(end, start);
    return (diff > 0) ? (int)(diff / 60) : 0;
}

// used to set due_time at borrow (e.g. now + loan period)
time_t addMinutes(time_t t, int minutes)
{
    return t + (time_t)(minutes * 60);
}

// ============================================================
// INPUT VALIDATION FUNCTIONS
// ============================================================

// ---- DATE (DD/MM/YYYY format) ----
string getValidDate(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool valid = true;

        if (value.size() != 10)
            valid = false;
        else if (value[2] != '/' || value[5] != '/')
            valid = false;
        else
        {
            for (int i = 0; i < 10; i++)
            {
                if (i == 2 || i == 5)
                    continue;
                if (!isdigit(value[i]))
                {
                    valid = false;
                    break;
                }
            }
            // check day range 01-31, month range 01-12, year range 2000-2100
            int day = stoi(value.substr(0, 2));
            int month = stoi(value.substr(3, 2));
            int year = stoi(value.substr(6, 4));

            if (day < 1 || day > 31)
                valid = false;
            if (month < 1 || month > 12)
                valid = false;
            if (year < 2000 || year > 2100)
                valid = false;
        }

        if (!valid)
            cout << "INVALID DATE! FORMAT MUST BE: DD/MM/YYYY" << endl;
        else
            return value;
    }
}

// ---- TIME (HH:MM format) ----
string getValidTime(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        bool valid = true;

        if (value.size() != 5)
            valid = false;
        else if (value[2] != ':')
            valid = false;
        else
        {
            for (int i = 0; i < 5; i++)
            {
                if (i == 2)
                    continue;
                if (!isdigit(value[i]))
                {
                    valid = false;
                    break;
                }
            }

            int hours = stoi(value.substr(0, 2));
            int minutes = stoi(value.substr(3, 2));

            if (hours < 0 || hours > 23)
                valid = false;
            if (minutes < 0 || minutes > 59)
                valid = false;
        }

        if (!valid)
            cout << "INVALID TIME! FORMAT MUST BE: HH:MM (24-hour)" << endl;
        else
            return value;
    }
}

// ---- INTEGER INPUT ----
// cin.fail() catches letters/symbols entered instead of a number
// clear + ignore needed to reset stream before next attempt
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
string getValidString(string prompt)
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

// ---- PUBLISHER (letters, spaces, dots, & allowed) ----
string getValidPublisher(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "INPUT CANNOT BE EMPTY!" << endl;
            continue;
        }

        bool valid = true;
        for (int i = 0; i < (int)value.size(); i++)
        {
            if (!isalpha(value[i]) && value[i] != ' ' &&
                value[i] != '.' && value[i] != '&' && value[i] != '-')
            {
                valid = false;
                break;
            }
        }

        if (!valid)
            cout << "INVALID PUBLISHER! ONLY LETTERS, SPACES, '.', '&', '-' ALLOWED." << endl;
        else
            return value;
    }
}

// ---- ISBN (10 or 13 digits, hyphens allowed) ----
string getValidISBN(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "INPUT CANNOT BE EMPTY!" << endl;
            continue;
        }

        string digitsOnly = "";
        bool valid = true;

        for (int i = 0; i < (int)value.size(); i++)
        {
            if (isdigit(value[i]))
                digitsOnly += value[i];
            else if (value[i] != '-')
            {
                valid = false;
                break;
            }
        }

        if (!valid || (digitsOnly.size() != 10 && digitsOnly.size() != 13))
            cout << "INVALID ISBN! MUST BE 10 OR 13 DIGITS (HYPHENS OPTIONAL)." << endl;
        else
            return value;
    }
}

// ---- LETTERS ONLY (for names) ----
string getValidAlphaString(string prompt)
{
    string value;
    while (true)
    {
        cout << prompt;
        getline(cin, value);

        if (value.empty())
        {
            cout << "INPUT CANNOT BE EMPTY!" << endl;
            continue;
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
            if (value[i] == '.' && hasAt) // dot must come after @
                hasDot = true;
        }

        if (value.empty() || !hasAt || !hasDot)
            cout << "INVALID EMAIL! MUST CONTAIN @ AND ." << endl;
        else
            return value;
    }
}

// ---- PHONE (Pakistani format: 03xxxxxxxxx) ----
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
        else if (value[0] != '0' || value[1] != '3')
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

// ============================================================
//                      MEMBERSHIP SYSTEM
// ============================================================
// membership gives users a fine discount and a higher borrow limit
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
    static const int DEFAULT_DURATION_DAYS; // 365

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
        // convert days to minutes for addMinutes()
        expiryTime = addMinutes(activationTime, durationDays * 24 * 60);
    }

    // RESTORE CONSTRUCTOR: used when loading from file — skips time recalculation
    Membership(int uid, string utype, string uname, time_t activation, time_t expiry)
        : membershipID(next_id++), userID(uid),
          userType(utype), userName(uname), active(true),
          activationTime(activation), expiryTime(expiry) {}

    // DESTRUCTOR
    ~Membership() {}

    //---- GETTERS ----
    int getMembershipID() const { return membershipID; }
    int getUserID() const { return userID; }
    string getUserType() const { return userType; }
    string getUserName() const { return userName; }
    bool isActive() const { return active; }
    time_t getActivationTime() { return activationTime; }
    time_t getExpiryTime() { return expiryTime; }

    //---- SETTERS ----
    void setActivationTime(time_t t) { activationTime = t; }
    void setExpiryTime(time_t t) { expiryTime = t; }

    // checks both the active flag and whether the expiry hasn't passed
    bool isValid() const
    {
        return active && (getCurrentTime() <= expiryTime);
    }

    // applies discount to a given fine amount if membership is valid
    double applyDiscount(double originalFine) const
    {
        if (!isValid())
            return originalFine;
        return originalFine * (1.0 - FINE_DISCOUNT); // 15% off
    }

    // extra books a member can borrow on top of base limit
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

    // cancels the membership immediately by setting active to false
    void cancel()
    {
        active = false;
        cout << "MEMBERSHIP CANCELLED FOR " << userName << "." << endl;
    }

    //---- MEMBERSHIP DISPLAY ----
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

// ============================================================
//                      USER CLASSES
// ============================================================
// base class for Student, Faculty, Guest
// uses pure virtual functions so each subclass enforces its own borrow limit, fine rate, etc.
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
    vector<int> borrowedbook; // holds book IDs currently borrowed
    Membership *membership;

public:
    // DEFAULT CONSTRUCTOR with dummy values to simplify file loading (overridden by subclasses)
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

    // polymorphic base destructor to ensure proper cleanup of derived classes
    virtual ~User()
    {
        delete membership;
    }

    //---- GETTERS AND SETTERS ----
    void setid(int id) { userid = id; }
    void setname(string n) { name = n; }
    void setemail(string e) { email = e; }
    void setphone_no(string p) { phonenumber = p; }
    void setCNIC(string c) { CNIC = c; }
    void setUsername(string u) { username = u; }
    void setPassword(string p) { password = p; }
    static void setNextID(int id) { next_id = id; }

    int get_userid() { return userid; }
    string get_name() { return name; }
    string get_email() { return email; }
    string get_phone() { return phonenumber; }
    string get_cnic() { return CNIC; }
    string getUsername() { return username; }
    string getPassword() { return password; }

    const vector<int> &getBorrowedBooks() { return borrowedbook; }
    int getBorrowedCount() { return (int)borrowedbook.size(); }

    // pure virtual functions so each subclass enforces its own borrow limit, fine rate, etc.
    virtual int getborrowlimit() = 0;
    virtual string get_usertype() = 0;
    virtual void printTo(ostream &os) const = 0;
    virtual int getFineRate() = 0;

    // overloaded << operator to print user details using the printTo() method
    friend ostream &operator<<(ostream &os, const User &u)
    {
        u.printTo(os);
        return os;
    }

    bool borrowbook(int id)
    {
        if ((int)borrowedbook.size() < getborrowlimit())
        {
            borrowedbook.push_back(id);
            return true;
        }
        // silent during file load to avoid misleading output
        if (!RuntimeLogger::isSilent())
            cout << "BORROW LIMIT REACHED!" << endl;
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
        cout << "BOOK NOT BORROWED BY THIS USER!" << endl;
        return false;
    }

    // membership-related methods
    Membership *getMembership() { return membership; }
    bool hasMembership() { return (membership != nullptr && membership->isValid()); }

    void activateMembership(int durationDays = 365)
    {
        if (hasMembership())
        {
            cout << "USER ALREADY HAS AN ACTIVE MEMBERSHIP!" << endl;
            return;
        }
        delete membership; // free expired membership if any before creating new one
        membership = new Membership(userid, get_usertype(), name, durationDays);
        cout << "\nMEMBERSHIP ACTIVATED SUCCESSFULLY!" << endl;
        cout << *membership;
    }

    void cancelMembership()
    {
        if (!membership)
        {
            cout << "NO MEMBERSHIP TO CANCEL!" << endl;
            return;
        }
        membership->cancel();
    }

    void viewMembership()
    {
        if (!membership)
        {
            cout << "YOU DO NOT HAVE A MEMBERSHIP. PURCHASE ONE FROM THE MENU!" << endl;
            return;
        }
        cout << *membership;
    }

    // rebuilds membership object from saved timestamps without resetting the clock
    void restoreMembership(time_t activation, time_t expiry)
    {
        delete membership;
        membership = new Membership(userid, get_usertype(), name, activation, expiry);
    }

    // base limit + membership boost (if active)
    int getEffectiveBorrowLimit()
    {
        return getborrowlimit() + (hasMembership() ? membership->getBorrowBoost() : 0);
    }

    double applyMembershipDiscount(double fine)
    {
        if (hasMembership())
            return membership->applyDiscount(fine);
        return fine;
    }

    // menu methods
    void viewFullDetails(library *lib);
    void searchBooks(library *lib);
    void reservationMenu(library *lib);
    void bookActionMenu(library *lib);
    void membershipMenu(library *lib);
    void userMenu(library *lib);
    // static method to handle user registration input and object creation
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

    int getborrowlimit() override { return 3; } // students can borrow up to 3 books
    string get_usertype() override { return "STUDENT"; }
    string getSeatNo() { return seatno; }
    string getMajor() { return major; }

    void printTo(ostream &os) const override
    {
        os << "============= STUDENT DETAILS =============" << endl
           << "| ID             : " << userid << endl
           << "| NAME           : " << name << endl
           << "| SEAT NO        : " << seatno << endl
           << "| MAJOR          : " << major << endl
           << "| USER TYPE      : STUDENT" << endl
           << "===========================================" << endl;
    }

    int getFineRate() override { return 10; } // Rs.10 per minute late
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

    int getborrowlimit() override { return 5; } // faculty can borrow 5 books
    string get_usertype() override { return "FACULTY"; }
    string getDepartment() { return department; }
    string getDesignation() { return designation; }

    void printTo(ostream &os) const override
    {
        os << "============= FACULTY DETAILS =============" << endl
           << "| ID             : " << userid << endl
           << "| NAME           : " << name << endl
           << "| DEPARTMENT     : " << department << endl
           << "| DESIGNATION    : " << designation << endl
           << "| USER TYPE      : FACULTY" << endl
           << "===========================================" << endl;
    }

    int getFineRate() override { return 15; } // Rs.15 per minute late
};

// ================= GUEST =================
class Guest : public User
{
private:
    string purpose;

public:
    Guest(string n, string c, string e, string p, string ps)
        : User(n, c, e, p), purpose(ps) {}

    int getborrowlimit() override { return 1; } // guests can only borrow 1 book
    string get_usertype() override { return "GUEST"; }
    string getPurpose() { return purpose; }

    void printTo(ostream &os) const override
    {
        os << "============== GUEST DETAILS ==============" << endl
           << "| ID             : " << userid << endl
           << "| NAME           : " << name << endl
           << "| PURPOSE        : " << purpose << endl
           << "| USER TYPE      : GUEST" << endl
           << "===========================================" << endl;
    }

    int getFineRate() override { return 20; } // highest rate since guests have least privileges
};

// ===================save user to file=======================
// helper function to convert User object to UserRecord struct and save to file
void saveUserToFile(User *u)
{
    ofstream file("users.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN users.dat FOR WRITING!");

    UserRecord ur = {}; // zero-initialize so unused fields don't contain garbage

    ur.id = u->get_userid();
    strncpy(ur.name, u->get_name().c_str(), sizeof(ur.name) - 1);
    ur.name[sizeof(ur.name) - 1] = '\0';
    strncpy(ur.email, u->get_email().c_str(), sizeof(ur.email) - 1);
    ur.email[sizeof(ur.email) - 1] = '\0';
    strncpy(ur.phone, u->get_phone().c_str(), sizeof(ur.phone) - 1);
    ur.phone[sizeof(ur.phone) - 1] = '\0';
    strncpy(ur.cnic, u->get_cnic().c_str(), sizeof(ur.cnic) - 1);
    ur.cnic[sizeof(ur.cnic) - 1] = '\0';
    strncpy(ur.type, u->get_usertype().c_str(), sizeof(ur.type) - 1);
    ur.type[sizeof(ur.type) - 1] = '\0';
    strncpy(ur.username, u->getUsername().c_str(), sizeof(ur.username) - 1);
    ur.username[sizeof(ur.username) - 1] = '\0';
    strncpy(ur.password, u->getPassword().c_str(), sizeof(ur.password) - 1);
    ur.password[sizeof(ur.password) - 1] = '\0';

    // downcast to access subclass-specific fields
    if (u->get_usertype() == "STUDENT")
    {
        Student *s = dynamic_cast<Student *>(u);
        strncpy(ur.seatno, s->getSeatNo().c_str(), sizeof(ur.seatno) - 1);
        ur.seatno[sizeof(ur.seatno) - 1] = '\0';
        strncpy(ur.major, s->getMajor().c_str(), sizeof(ur.major) - 1);
        ur.major[sizeof(ur.major) - 1] = '\0';
    }
    else if (u->get_usertype() == "FACULTY")
    {
        Faculty *f = dynamic_cast<Faculty *>(u);
        strncpy(ur.department, f->getDepartment().c_str(), sizeof(ur.department) - 1);
        ur.department[sizeof(ur.department) - 1] = '\0';
        strncpy(ur.designation, f->getDesignation().c_str(), sizeof(ur.designation) - 1);
        ur.designation[sizeof(ur.designation) - 1] = '\0';
    }
    else if (u->get_usertype() == "GUEST")
    {
        Guest *g = dynamic_cast<Guest *>(u);
        strncpy(ur.purpose, g->getPurpose().c_str(), sizeof(ur.purpose) - 1);
        ur.purpose[sizeof(ur.purpose) - 1] = '\0';
    }

    ur.borrowed_count = u->getBorrowedCount();
    for (int i = 0; i < ur.borrowed_count && i < 10; i++)
        ur.borrowed_ids[i] = u->getBorrowedBooks()[i];

    // only save membership if it's currently valid
    if (u->getMembership() && u->getMembership()->isValid())
    {
        ur.hasMembership = true;
        ur.membershipActivation = u->getMembership()->getActivationTime();
        ur.membershipExpiry = u->getMembership()->getExpiryTime();
        ur.membershipDurationDays = 365;
    }
    else
    {
        ur.hasMembership = false;
        ur.membershipActivation = 0;
        ur.membershipExpiry = 0;
        ur.membershipDurationDays = 0;
    }

    file.write((char *)&ur, sizeof(ur));
    file.close();
}

//==================load users from file=======================
// reads binary records from disk and reconstructs User objects in memory
// borrowbook() calls here are silent (RuntimeLogger) so no false console output during load
void loadUsersFromFile(vector<User *> &users)
{
    ifstream file("users.dat", ios::binary);
    if (!file)
        return; // first run — file doesn't exist yet, nothing to load

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

//==================rewrite users file=======================
// overwrites the entire users file with the current in-memory state
// called after any update (return, membership change, etc.) to keep disk in sync
void rewriteUsersFile(vector<User *> &users)
{
    ofstream file("users.dat", ios::binary); // no ios::app — full overwrite intended

    for (int i = 0; i < (int)users.size(); i++)
    {
        User *u = users[i];
        UserRecord ur = {};

        ur.id = u->get_userid();
        strncpy(ur.name, u->get_name().c_str(), sizeof(ur.name) - 1);
        ur.name[sizeof(ur.name) - 1] = '\0';
        strncpy(ur.email, u->get_email().c_str(), sizeof(ur.email) - 1);
        ur.email[sizeof(ur.email) - 1] = '\0';
        strncpy(ur.phone, u->get_phone().c_str(), sizeof(ur.phone) - 1);
        ur.phone[sizeof(ur.phone) - 1] = '\0';
        strncpy(ur.cnic, u->get_cnic().c_str(), sizeof(ur.cnic) - 1);
        ur.cnic[sizeof(ur.cnic) - 1] = '\0';
        strncpy(ur.type, u->get_usertype().c_str(), sizeof(ur.type) - 1);
        ur.type[sizeof(ur.type) - 1] = '\0';
        strncpy(ur.username, u->getUsername().c_str(), sizeof(ur.username) - 1);
        ur.username[sizeof(ur.username) - 1] = '\0';
        strncpy(ur.password, u->getPassword().c_str(), sizeof(ur.password) - 1);
        ur.password[sizeof(ur.password) - 1] = '\0';

        // downcast to get subclass-specific fields
        if (u->get_usertype() == "STUDENT")
        {
            Student *s = dynamic_cast<Student *>(u);
            strncpy(ur.seatno, s->getSeatNo().c_str(), sizeof(ur.seatno) - 1);
            ur.seatno[sizeof(ur.seatno) - 1] = '\0';
            strncpy(ur.major, s->getMajor().c_str(), sizeof(ur.major) - 1);
            ur.major[sizeof(ur.major) - 1] = '\0';
        }
        else if (u->get_usertype() == "FACULTY")
        {
            Faculty *f = dynamic_cast<Faculty *>(u);
            strncpy(ur.department, f->getDepartment().c_str(), sizeof(ur.department) - 1);
            ur.department[sizeof(ur.department) - 1] = '\0';
            strncpy(ur.designation, f->getDesignation().c_str(), sizeof(ur.designation) - 1);
            ur.designation[sizeof(ur.designation) - 1] = '\0';
        }
        else if (u->get_usertype() == "GUEST")
        {
            Guest *g = dynamic_cast<Guest *>(u);
            strncpy(ur.purpose, g->getPurpose().c_str(), sizeof(ur.purpose) - 1);
            ur.purpose[sizeof(ur.purpose) - 1] = '\0';
        }

        ur.borrowed_count = u->getBorrowedCount();
        for (int j = 0; j < ur.borrowed_count && j < 10; j++)
            ur.borrowed_ids[j] = u->getBorrowedBooks()[j];

        if (u->getMembership() && u->getMembership()->isValid())
        {
            ur.hasMembership = true;
            ur.membershipActivation = u->getMembership()->getActivationTime();
            ur.membershipExpiry = u->getMembership()->getExpiryTime();
            ur.membershipDurationDays = 365;
        }
        else
        {
            ur.hasMembership = false;
            ur.membershipActivation = 0;
            ur.membershipExpiry = 0;
            ur.membershipDurationDays = 0;
        }

        file.write((char *)&ur, sizeof(ur));
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

// ============================================================
//                      BOOK CLASSES
// ============================================================
class Book
{
private:
    static int next_id;
    int bookid;
    string title;
    string author;
    string category;
    string ISBN;
    string publisher;
    int year_of_publication;
    int available_copies = 0;
    bool availability_status;
    // usage counters for librarian reports
    int download_count = 0;
    int read_onsite_count = 0;
    int timesborrowed_count = 0;

public:
    // DEFAULT CONSTRUCTOR
    Book() : bookid(next_id++), title("UNKNOWN"), author("UNKNOWN"), category("UNKNOWN"),
             ISBN("UNKNOWN"), publisher("UNKNOWN"), year_of_publication(0),
             available_copies(0), availability_status(false),
             download_count(0), read_onsite_count(0), timesborrowed_count(0) {}
    // PARAMETERIZED CONSTRUCTOR
    Book(string t, string a, string c, string isbn, string p, int yop)
        : bookid(next_id++), title(t), author(a), category(c),
          ISBN(isbn), publisher(p), year_of_publication(yop),
          available_copies(0), availability_status(false),
          download_count(0), read_onsite_count(0), timesborrowed_count(0) {}
    // DESTRUCTOR
    ~Book() {}
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

    static void setNextID(int id) { next_id = id; }
    static int getNextID() { return next_id; }

    // automatically updates availability_status based on copy count
    void setAvailableCopies(int ac)
    {
        available_copies = ac;
        availability_status = (available_copies > 0);
    }
    //==========counter funstions=================
    void incrementBorrowCount()
    {
        if (available_copies >= 0)
            timesborrowed_count++;
    }

    void incrementDownloadCount() { download_count++; }
    void incrementReadOnSiteCount() { read_onsite_count++; }

    // == overloaded to support searching by id, title, or year
    bool operator==(const Book &other) const { return bookid == other.bookid; }
    bool operator==(const string &t) const { return title == t; }
    bool operator==(const int &y) const { return year_of_publication == y; }

    //---------- display book details----------
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
// initialize static member for auto-incrementing book IDs
int Book::next_id = 1;

// ===============================================
//                      BOOK CATALOG
// ===============================================
class BookCatalog
{
private:
    vector<Book *> BK;

public:
    // DEFAULT CONSTRUCTOR
    BookCatalog() {}
    // DESTRUCTOR
    ~BookCatalog()
    {
        for (int i = 0; i < (int)BK.size(); i++)
            delete BK[i];
        BK.clear();
    }
    //------------------BOOK MANAGEMENT FUNCTIONS------------------
    // add book to catalog and print success message
    void addbook(Book *b)
    {
        BK.push_back(b);
        if (!RuntimeLogger::isSilent())
            cout << "BOOK ADDED SUCCESSFULLY!" << endl;
    }

    // remove book by title and print result message
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
    // all return nullptr on miss — callers print "not found" if needed
    // this avoids false "not found" messages when these are used internally
    // (e.g. checking for duplicate titles before adding a book)

    Book *SearchbyID(int id)
    {
        for (int i = 0; i < (int)BK.size(); i++)
            if (BK[i]->get_bookid() == id)
                return BK[i];
        return nullptr;
    }

    Book *SearchbyTitle(string t)
    {
        for (int i = 0; i < (int)BK.size(); i++)
            if (BK[i]->get_title() == t)
                return BK[i];
        return nullptr;
    }

    Book *SearchbyAuthor(string a)
    {
        for (int i = 0; i < (int)BK.size(); i++)
            if (BK[i]->get_author() == a)
                return BK[i];
        return nullptr;
    }

    Book *SearchbyCategory(string c)
    {
        for (int i = 0; i < (int)BK.size(); i++)
            if (BK[i]->get_category() == c)
                return BK[i];
        return nullptr;
    }

    Book *SearchbyPublisher(string p)
    {
        for (int i = 0; i < (int)BK.size(); i++)
            if (BK[i]->get_publisher() == p)
                return BK[i];
        return nullptr;
    }

    Book *SearchbyYOP(int yop)
    {
        for (int i = 0; i < (int)BK.size(); i++)
            if (BK[i]->get_YOP() == yop)
                return BK[i];
        return nullptr;
    }
    //------------------DISPLAY FUNCTIONS------------------
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
    //------------------GETTER FUNCTIONS------------------
    int get_bookcount() { return (int)BK.size(); }

    Book *getBookAtIndex(int index)
    {
        if (index >= 0 && index < (int)BK.size())
            return BK[index];
        return nullptr;
    }
};

//==================save book to file=======================
void saveBookToFile(Book *b)
{
    ofstream file("books.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN books.dat FOR WRITING!");

    BookRecord br;
    br.id = b->get_bookid();
    strncpy(br.title, b->get_title().c_str(), sizeof(br.title) - 1);
    br.title[sizeof(br.title) - 1] = '\0';
    strncpy(br.author, b->get_author().c_str(), sizeof(br.author) - 1);
    br.author[sizeof(br.author) - 1] = '\0';
    strncpy(br.category, b->get_category().c_str(), sizeof(br.category) - 1);
    br.category[sizeof(br.category) - 1] = '\0';
    strncpy(br.isbn, b->get_isbn().c_str(), sizeof(br.isbn) - 1);
    br.isbn[sizeof(br.isbn) - 1] = '\0';
    strncpy(br.publisher, b->get_publisher().c_str(), sizeof(br.publisher) - 1);
    br.publisher[sizeof(br.publisher) - 1] = '\0';
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

//==================load books from file=======================
void loadBooksFromFile(BookCatalog *catalog)
{
    ifstream file("books.dat", ios::binary);
    if (!file)
        return;

    BookRecord br;

    while (file.read((char *)&br, sizeof(br)))
    {
        Book *b = new Book(br.title, br.author, br.category, br.isbn, br.publisher, br.year);

        // if saved ID is ahead of next_id, sync it up to avoid ID collisions
        if (br.id >= Book::getNextID())
            Book::setNextID(br.id + 1);

        b->set_bookid(br.id);
        b->setAvailableCopies(br.copies);

        // counters have no setter so we replay increments to restore them
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

//==================rewrite books file=======================
// full overwrite (no ios::app) so deleted/edited books don't persist on disk
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
        strncpy(br.title, b->get_title().c_str(), sizeof(br.title) - 1);
        br.title[sizeof(br.title) - 1] = '\0';
        strncpy(br.author, b->get_author().c_str(), sizeof(br.author) - 1);
        br.author[sizeof(br.author) - 1] = '\0';
        strncpy(br.category, b->get_category().c_str(), sizeof(br.category) - 1);
        br.category[sizeof(br.category) - 1] = '\0';
        strncpy(br.isbn, b->get_isbn().c_str(), sizeof(br.isbn) - 1);
        br.isbn[sizeof(br.isbn) - 1] = '\0';
        strncpy(br.publisher, b->get_publisher().c_str(), sizeof(br.publisher) - 1);
        br.publisher[sizeof(br.publisher) - 1] = '\0';
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

// ============================================================
//                      LIBRARIAN CLASS
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
    // DEFAULT CONSTRUCTOR
    librarian() : librarian_id(0), name("UNKNOWN"), email("UNKNOWN"), phone_no("UNKNOWN") {}
    // PARAMETERIZED CONSTRUCTOR
    librarian(int id, string n, string e, string p) : librarian_id(id), name(n), email(e), phone_no(p) {}
    // DESTRUCTOR
    ~librarian() {}
    // credential getters/setters
    void setCredentials(string u, string p)
    {
        username = u;
        password = p;
    }
    string getUsername() { return username; }
    string getPassword() { return password; }

    // thin wrappers — actual logic is in BookCatalog
    void addbook(Book *b, BookCatalog *catalog) { catalog->addbook(b); }
    void removeBook(string t, BookCatalog *catalog) { catalog->removeBook(t); }

    //------ display librarian details------
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

    // librarian menu methods
    void viewFullDetails(library *lib);
    void manageBooks(library *lib);
    void manageTables(library *lib);
    void librarianMainMenu(library *lib);
};

// ============================================================
//                      TRANSACTION CLASS
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
    Transaction()
        : transaction_id(next_id++), user_id(0), book_id(0),
          borrow_time(0), due_time(0), return_time(0),
          return_date("NOT RETURNED"), returned(false) {}
    // PARAMETERIZED CONSTRUCTOR
    // duration_minutes lets us use minutes for testing without changing real logic
    Transaction(int uid, int bid, int duration_minutes)
        : transaction_id(next_id++), user_id(uid), book_id(bid),
          return_date("NOT RETURNED"), returned(false), return_time(0)
    {
        borrow_time = getCurrentTime();
        due_time = addMinutes(borrow_time, duration_minutes);
    }

    // DESTRUCTOR
    ~Transaction() {}
    // GETTERS AND SETTERS
    int getuserid() { return user_id; }
    int getbookid() { return book_id; }
    int getTransactionID() { return transaction_id; }
    bool isReturned() { return returned; }
    time_t getDueTime() { return due_time; }
    time_t getBorrowTime() { return borrow_time; }
    time_t getReturnTime() { return return_time; }
    string getReturnDate() { return return_date; }

    void setTransactionID(int id) { transaction_id = id; }
    void setUserID(int id) { user_id = id; }
    void setBookID(int id) { book_id = id; }
    void setBorrowTime(time_t t) { borrow_time = t; }
    void setDueTime(time_t t) { due_time = t; }
    void setReturnTime(time_t t) { return_time = t; }
    void setReturnDate(string d) { return_date = d; }
    void setReturned(bool r) { returned = r; }

    // returns 0 if book wasn't late or hasn't been returned yet
    int getOverdueMinutes()
    {
        if (!returned || return_time == 0 || return_time <= due_time)
            return 0;
        return minutesDiff(due_time, return_time);
    }

    // stamps the return time and returns overdue minutes for fine calculation
    int markreturned()
    {
        returned = true;
        return_time = getCurrentTime();
        return_date = timeToString(return_time);
        return getOverdueMinutes();
    }
    //---------- display transaction details----------
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
// initialize static member for auto-incrementing transaction IDs
int Transaction::next_id = 1;

//==================save transaction to file=======================
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
    strncpy(tr.return_date, t.getReturnDate().c_str(), sizeof(tr.return_date) - 1);
    tr.return_date[sizeof(tr.return_date) - 1] = '\0';

    file.write((char *)&tr, sizeof(tr));
    file.close();
}

//==================load transactions from file=======================
// also re-links unreturned books to their user objects so borrowedbook stays accurate
void loadTransactionsFromFile(vector<Transaction> &transactions, vector<User *> &users)
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

        // restore the user's borrow list for any book not yet returned
        if (!tr.returned)
        {
            for (int i = 0; i < (int)users.size(); i++)
            {
                if (users[i]->get_userid() == tr.uid)
                {
                    users[i]->borrowbook(tr.bid);
                    break;
                }
            }
        }
    }

    file.close();
}

//==================rewrite transactions file=======================
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
        strncpy(tr.return_date, transactions[i].getReturnDate().c_str(), sizeof(tr.return_date) - 1);
        tr.return_date[sizeof(tr.return_date) - 1] = '\0';

        file.write((char *)&tr, sizeof(tr));
    }

    file.close();
}

// ============================================================
//                      FINES CLASS
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
    Fines()
        : user_id(0), book_id(0), minuteslate(0),
          rate(0), originalAmount(0), finalAmount(0), discountApplied(false) {}

    // User* is needed here for two reasons:
    // 1. getFineRate() is polymorphic (Student/Faculty/Guest have different rates)
    // 2. membership discount is applied automatically if active
    Fines(int u, int b, int mins, User *user)
        : user_id(u), book_id(b)
    {
        minuteslate = (mins < 0) ? 0 : mins; // guard against negative diff edge case
        rate = user->getFineRate();
        originalAmount = rate * minuteslate;

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
    ~Fines() {}

    // GETTERS AND SETTERS
    double getAmount() { return finalAmount; }
    double getOriginal() { return originalAmount; }
    int getUserID() { return user_id; }
    int getBookID() { return book_id; }
    bool wasDiscounted() { return discountApplied; }
    int getMinutesLate() { return minuteslate; }
    double getRate() { return rate; }

    void setUserID(int id) { user_id = id; }
    void setBookID(int id) { book_id = id; }
    void setMinutesLate(int m) { minuteslate = m; }
    void setRate(double r) { rate = r; }
    void setOriginalAmount(double a) { originalAmount = a; }
    void setFinalAmount(double a) { finalAmount = a; }
    void setDiscountApplied(bool d) { discountApplied = d; }

    //---------- display fine details----------
    friend ostream &operator<<(ostream &os, const Fines &f)
    {
        os << "================= FINE DETAILS =================" << endl
           << "| USER ID        : " << f.user_id << endl
           << "| BOOK ID        : " << f.book_id << endl
           << "| MINUTES LATE   : " << f.minuteslate << endl
           << "| RATE           : RS. " << f.rate << " /MIN" << endl
           << "| ORIGINAL AMT   : RS. " << f.originalAmount << endl;

        if (f.discountApplied)
            os << "| DISCOUNT       : 15% (MEMBERSHIP)" << endl
               << "| FINAL AMT      : RS. " << f.finalAmount << endl;
        else
            os << "| FINAL AMT      : RS. " << f.finalAmount << endl;

        os << "=================================================" << endl;
        return os;
    }
};

//==================save fine to file=======================
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

//==================load fines from file=======================
void loadFinesFromFile(vector<Fines> &fines)
{
    ifstream file("fines.dat", ios::binary);
    if (!file)
        return;

    FineRecord fr;

    while (file.read((char *)&fr, sizeof(fr)))
    {
        // use setters instead of constructor since we're restoring, not recalculating
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

//==================rewrite fines file=======================
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

// ============================================================
//                      RESERVATION CLASS
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
    Reservation()
        : reservationid(next_id++), userid(0), bookid(0), tableid(0),
          date("UNKNOWN"), startTime("0"), endTime("0"), active(true) {}
    // PARAMETERIZED CONSTRUCTOR
    Reservation(int uid, int bid, int tid, string d, string start, string end)
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
    // getters and setters
    int get_userid() { return userid; }
    int get_tableid() { return tableid; }
    int get_bookid() { return bookid; }
    int get_reservationid() { return reservationid; }
    string get_date() { return date; }
    string get_start() { return startTime; }
    string get_end() { return endTime; }
    bool isActive() { return active; }
    void cancel() { active = false; }

    // string comparison works here because times are in HH:MM (lexicographic order = chronological)
    static bool overlap(string s1, string e1, string s2, string e2)
    {
        return !(e1 <= s2 || e2 <= s1);
    }

    //---------- display reservation details----------
    friend ostream &operator<<(ostream &os, const Reservation &r)
    {
        if (!r.active)
            return os; // skip cancelled reservations silently

        os << "=============== RESERVATION DETAILS ===============" << endl
           << "| RESERVATION ID : " << r.reservationid << endl
           << "| USER ID        : " << r.userid << endl;

        // a reservation is either for a book or a table, not both
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
// initialize static member for auto-incrementing reservation IDs
int Reservation::next_id = 1;

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
    Table()
    {
        tableid = next_id++;
        location = "UNKNOWN";
        capacity = 1;
    }
    // PARAMETERIZED CONSTRUCTOR
    Table(string loc, int cap)
    {
        tableid = next_id++;
        location = loc;
        capacity = cap;
    }

    // getters and setters
    int get_id() { return tableid; }
    string get_location() { return location; }
    int get_capacity() { return capacity; }
    void setID(int id) { tableid = id; }
    void setLocation(string loc) { location = loc; }
    void setCapacity(int cap) { capacity = cap; }
    //---------- display table details----------
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
// initialize static member for auto-incrementing table IDs
int Table::next_id = 1;

//==================save table to file=======================
void saveTableToFile(Table &t)
{
    ofstream file("tables.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN tables.dat FOR WRITING!");

    TableRecord tr = {};
    tr.id = t.get_id();
    strncpy(tr.location, t.get_location().c_str(), sizeof(tr.location) - 1);
    tr.location[sizeof(tr.location) - 1] = '\0';
    tr.capacity = t.get_capacity();

    file.write((char *)&tr, sizeof(tr));
    file.close();
}

//==================load tables from file=======================
void loadTablesFromFile(vector<Table> &tables)
{
    ifstream file("tables.dat", ios::binary);
    if (!file)
        return;

    TableRecord tr;

    while (file.read((char *)&tr, sizeof(tr)))
    {
        Table t;
        t.setID(tr.id);
        t.setLocation(tr.location);
        t.setCapacity(tr.capacity);
        tables.push_back(t);
    }

    file.close();
}
//==================rewrite tables file=======================
void rewriteTablesFile(vector<Table> &tables)
{
    ofstream file("tables.dat", ios::binary);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN tables.dat FOR REWRITING!");

    for (int i = 0; i < (int)tables.size(); i++)
    {
        TableRecord tr = {};
        tr.id = tables[i].get_id();
        strncpy(tr.location, tables[i].get_location().c_str(), sizeof(tr.location) - 1);
        tr.location[sizeof(tr.location) - 1] = '\0';
        tr.capacity = tables[i].get_capacity();

        file.write((char *)&tr, sizeof(tr));
        if (!file)
            throw FileException("ERROR: FAILED TO WRITE TABLE RECORD DURING REWRITE!");
    }

    file.close();
}

// ============================================================
//                      LIBRARY CLASS
// ============================================================
// central class — owns all data and coordinates between subsystems
class library
{
private:
    BookCatalog catalog;
    vector<Table> tables;
    vector<Reservation> reservations;
    vector<Transaction> transactions;
    vector<Fines> fines;
    vector<User *> users; // pointers for polymorphism (Student/Faculty/Guest)

public:
    static const int MAX_TABLES = 10;

    User *getUserByIndex(int index)
    {
        if (index >= 0 && index < (int)users.size())
            return users[index];
        return nullptr;
    }

    int getUserCount() { return (int)users.size(); }

    vector<User *> &getUsersVector() { return users; }
    vector<Table> &getTablesVector() { return tables; }
    vector<Transaction> &getTransactionsVector() { return transactions; }
    vector<Fines> &getFinesVector() { return fines; }
    // ================== USER MANAGEMENT FUNCTIONS =================
    void registerUserInLibrary()
    {
        User *newUser = User::registerUser();
        if (newUser)
        {
            // loop until we get a unique username — can't just reject once since
            // the user object is already built and we don't want to discard it
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
                        break; // restart check with new username
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

    //========basic interactions with books (read onsite/download/borrow/return)========
    bool readOnSite(User *u, string title)
    {
        Book *book = catalog.SearchbyTitle(title);
        if (!book)
            throw BookException("BOOK NOT FOUND: " + title);

        book->incrementReadOnSiteCount();
        cout << "READING BOOK ONSITE!" << endl;
        return true;
    }

    bool downloadBook(User *u, string title)
    {
        Book *book = catalog.SearchbyTitle(title);
        if (!book)
            throw BookException("BOOK NOT FOUND: " + title);

        book->incrementDownloadCount();
        cout << "BOOK DOWNLOADED!" << endl;
        return true;
    }

    // scans active transactions to check if any are past due_time
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

                    double originalFine = u->getFineRate() * overdue_mins;
                    if (u->hasMembership())
                    {
                        double discountedFine = u->applyMembershipDiscount(originalFine);
                        cout << "ORIGINAL FINE: Rs." << originalFine << endl;
                        cout << "AFTER 15% MEMBERSHIP DISCOUNT: Rs." << discountedFine << endl;
                    }
                    else
                        cout << "FINE AMOUNT: Rs." << originalFine << endl;
                }
                else
                    cout << "RETURNED ON TIME! NO FINE." << endl;

                break;
            }
        }
        return true;
    }

    // calculates fine for a book still out — used for live fine display before return
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
    //==================display methods for librarian//user menu=======================
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

    // shows both settled fines (after return) and live fines (still borrowed, overdue)
    void viewUserFines(int userID)
    {
        cout << "==== FINES FOR USER ID: " << userID << " ====" << endl;
        bool found = false;

        for (int i = 0; i < (int)fines.size(); i++)
        {
            if (fines[i].getUserID() == userID)
            {
                cout << fines[i];
                found = true;
            }
        }

        for (int i = 0; i < (int)transactions.size(); i++)
        {
            if (transactions[i].getuserid() == userID && !transactions[i].isReturned())
            {
                // look up the user object to get their fine rate
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
            cout << reservations[i];
    }

    void viewUsers()
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

    void viewUsersByType(string type)
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

    void viewAllMemberships()
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

    void viewTables()
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

    //==================reservation and table management functions=======================
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

        // check for time conflict on the same table and date
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

    // tries each table in order and assigns the first one with no time conflict
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
        for (int i = 0; i < (int)reservations.size(); i++)
        {
            if (reservations[i].get_reservationid() == rid)
            {
                reservations[i].cancel();
                cout << "RESERVATION CANCELLED!\n";
                return;
            }
        }
        throw ReservationException("RESERVATION ID NOT FOUND!");
    }

    void addTable(string loc, int cap)
    {
        if ((int)tables.size() >= MAX_TABLES)
            throw ReservationException("TABLE LIMIT REACHED! MAXIMUM " + to_string(MAX_TABLES) + " TABLES ALLOWED.");

        tables.push_back(Table(loc, cap));
        saveTableToFile(tables.back());
        cout << "TABLE ADDED! (" << tables.size() << "/" << MAX_TABLES << " TABLES)" << endl;
    }

    void removeTable(int tid)
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

    // filters tables by checking each against existing reservations for that date/time
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

    int getTableCount() { return (int)tables.size(); }

    // used before showing auto-reserve option so we don't offer it when nothing is free
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
        if (i < 0 || i >= (int)tables.size())
            throw ReservationException("TABLE INDEX OUT OF BOUNDS: " + to_string(i));
        return tables[i];
    }

    BookCatalog *getCatalog() { return &catalog; }
};

//============================================================
//                  AUTHENTICATION CLASS
//============================================================
// only one librarian exists in this system — lib_admin pointer handles it
class Authentication
{
private:
    librarian *lib_admin;

public:
    Authentication(librarian *l = nullptr)
    {
        lib_admin = l;
    }

    void setLibrarian(librarian *l)
    {
        lib_admin = l;
    }

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

    void logout()
    {
        cout << "LOGGED OUT SUCCESSFULLY!\n";
    }
};

//=============================================
//-----------------USER MENUS-----------------

//------------------MAIN DASHBOARD------------------
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

//------------------BOOK SEARCH MENU------------------
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
        else if (choice >= 1 && choice <= 6)
        {
            // only print not found for actual searches, not when user exits
            cout << "BOOK NOT FOUND!" << endl;
        }

    } while (choice != 7);
}

//------------------RESERVATION MENU------------------
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
            string date  = getValidDate("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidTime("ENTER START TIME (HH:MM): ");
            string end   = getValidTime("ENTER END TIME (HH:MM): ");

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
            string date  = getValidDate("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidTime("ENTER START TIME (HH:MM): ");
            string end   = getValidTime("ENTER END TIME (HH:MM): ");

            cout << "\n--------------- AVAILABLE TABLES ---------------" << endl;
            lib->viewAvailableTables(date, start, end);
            cout << "------------------------------------------------" << endl;

            // no point asking for a table ID if nothing is free
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
            int bid      = getValidInt("ENTER BOOK ID: ");
            string date  = getValidDate("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidTime("ENTER START TIME (HH:MM): ");
            string end   = getValidTime("ENTER END TIME (HH:MM): ");

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

//------------------BOOK ACTIONS MENU------------------
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

            title = getValidString("ENTER BOOK TITLE: ");

            // convert user's chosen unit to minutes for the Transaction constructor
            int unit = getValidIntInRange("\nSELECT TIME UNIT:\n1. MINUTES\n2. HOURS\n3. DAYS\nCHOICE: ", 1, 3);
            int duration = 0;

            if (unit == 1)
                duration = getValidIntInRange("ENTER MINUTES: ", 1, 10080); // max 1 week
            else if (unit == 2)
                duration = getValidIntInRange("ENTER HOURS: ", 1, 168) * 60;
            else if (unit == 3)
                duration = getValidIntInRange("ENTER DAYS: ", 1, 7) * 24 * 60;

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

            // build a list of active borrows so we can let user pick by number
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
                    // rewrite all three files since return affects books, transactions, and users
                    rewriteBooksFile(lib->getCatalog());
                    rewriteTransactionsFile(lib->getTransactionsVector());
                    rewriteUsersFile(lib->getUsersVector());
                }
                catch (BookException &e)
                {
                    cout << "BOOK ERROR: " << e.what() << endl;
                }
                catch (TransactionException &e)
                {
                    cout << "TRANSACTION ERROR: " << e.what() << endl;
                }
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

//------------------MEMBERSHIP MENU------------------
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
                // show the user what they actually get before they confirm
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

//------------------MAIN USER MENU------------------
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
        case 1: viewFullDetails(lib); break;
        case 2: searchBooks(lib); break;
        case 3: bookActionMenu(lib); break;
        case 4: reservationMenu(lib); break;
        case 5: membershipMenu(lib); break;
        case 6: cout << "EXITING..." << endl; break;
        default: cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 6);
}

//=======================================
//----------LIBRARIAN MENUS-------------

//------------------MAIN DASHBOARD------------------
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
            // braces needed — can't declare variables inside a switch case without a scope
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

//------------------BOOK MANAGEMENT MENU------------------
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
                // prevent duplicate titles before allocating a new Book
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

//------------------TABLE MANAGEMENT MENU------------------
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

//------------------MAIN LIBRARIAN MENU------------------
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
        case 2: manageBooks(lib); break;
        case 3: manageTables(lib); break;
        case 4: cout << "EXITING LIBRARIAN SYSTEM..." << endl; break;
        default: cout << "INVALID CHOICE!" << endl;
        }

    } while (choice != 4);
}

//============================================================
//                  MAIN FUNCTION
//============================================================
int main()
{
    library lib;

    // suppress console output while replaying saved data from disk
    RuntimeLogger::enableSilentMode();

    try { loadUsersFromFile(lib.getUsersVector()); }
    catch (FileException &e) { cout << e.what() << endl; }

    // sync next_id so new users don't get IDs that collide with loaded ones
    int maxID = 0;
    for (int i = 0; i < lib.getUserCount(); i++)
        if (lib.getUserByIndex(i)->get_userid() > maxID)
            maxID = lib.getUserByIndex(i)->get_userid();
    User::setNextID(lib.getUserCount() + 1);

    try { loadBooksFromFile(lib.getCatalog()); }
    catch (FileException &e) { cout << e.what() << endl; }

    try { loadTransactionsFromFile(lib.getTransactionsVector(), lib.getUsersVector()); }
    catch (FileException &e) { cout << e.what() << endl; }

    try { loadFinesFromFile(lib.getFinesVector()); }
    catch (FileException &e) { cout << "FINES FILE ERROR: " << e.what() << endl; }

    try { loadTablesFromFile(lib.getTablesVector()); }
    catch (FileException &e) { cout << "TABLES FILE ERROR: " << e.what() << endl; }

    RuntimeLogger::disableSilentMode(); // restore normal output after loading

    // hardcoded single librarian — credentials match the comment at the top of the file
    librarian admin(1, "Admin", "admin@lib.com", "03000000000");
    admin.setCredentials("admin", "123");

    Authentication auth(&admin);
    int choice = 0;

    //------------MAIN MENU LOOP------------
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
            try { lib.registerUserInLibrary(); }
            catch (UserException &e) { cout << e.what() << endl; }
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
            catch (AuthException &e) { cout << e.what() << endl; }
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
            catch (AuthException &e) { cout << e.what() << endl; }
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