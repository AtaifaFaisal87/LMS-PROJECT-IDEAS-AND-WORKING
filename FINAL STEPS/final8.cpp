// admin("admin", "123");
#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <cstring>
#include <fstream>
using namespace std;

// CHANGE 2.0: Replaced global silentMode with OOP-compliant DestructorLogger class
// This class manages destructor message suppression using static members (encapsulation)
class DestructorLogger
{
private:
    static bool suppressMessages;

public:
    // Enable message suppression (for file loading)
    static void enableSilentMode()
    {
        suppressMessages = true;
    }

    // Disable message suppression (normal operation)
    static void disableSilentMode()
    {
        suppressMessages = false;
    }

    // Check if messages should be suppressed
    static bool isSilent()
    {
        return suppressMessages;
    }
};

// Initialize static member
bool DestructorLogger::suppressMessages = false;

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
    // Borrowed books
    int borrowed_count;
    int borrowed_ids[10]; // max 10 books (adjust if needed)
    // Membership
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
            // check all other positions are digits
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

            // check day range 01-31
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

        // Strip hyphens and count digits
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
    // RESTORE CONSTRUCTOR (for loading from file — does not reset times)
    Membership(int uid, string utype, string uname, time_t activation, time_t expiry)
        : membershipID(next_id++), userID(uid),
          userType(utype), userName(uname), active(true),
          activationTime(activation), expiryTime(expiry) {}
    // DESTRUCTOR
    // CHANGE 3.0: Added DestructorLogger guard — fires during vector resize/copy without this
    ~Membership()
    {
        if (!DestructorLogger::isSilent())
            cout << "MEMBERSHIP OF " << userName << " DESTROYED!" << endl;
    }

    // ---- GETTERS ----
    int getMembershipID() const { return membershipID; }
    int getUserID() const { return userID; }
    string getUserType() const { return userType; }
    string getUserName() const { return userName; }
    bool isActive() const { return active; }
    time_t getActivationTime() { return activationTime; }
    time_t getExpiryTime() { return expiryTime; }

    // Setters
    void setActivationTime(time_t t) { activationTime = t; }
    void setExpiryTime(time_t t) { expiryTime = t; }

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

    // CHANGE 3.0: Added DestructorLogger guard — User destructor fires on vector<User*> cleanup
    virtual ~User()
    {
        delete membership;
        if (!DestructorLogger::isSilent())
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
    // Restore Membership
    void restoreMembership(time_t activation, time_t expiry)
    {
        delete membership;
        membership = new Membership(userid, get_usertype(), name, activation, expiry);
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
           << "| ID             : " << userid << endl
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
           << "| ID             : " << userid << endl
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
           << "| ID             : " << userid << endl
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

    // type-specific fields
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
    {
        ur.borrowed_ids[i] = u->getBorrowedBooks()[i];
    }
    // Save membership
    if (u->getMembership() && u->getMembership()->isValid())
    {
        ur.hasMembership = true;
        ur.membershipActivation = u->getMembership()->getActivationTime();
        ur.membershipExpiry = u->getMembership()->getExpiryTime();
        ur.membershipDurationDays = 365; // default
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
        // Restore membership
        if (ur.hasMembership)
        {
            u->restoreMembership(ur.membershipActivation, ur.membershipExpiry);
        }
        for (int i = 0; i < ur.borrowed_count; i++)
        {
            u->borrowbook(ur.borrowed_ids[i]);
        }
        users.push_back(u);
    }

    file.close();
}

void rewriteUsersFile(vector<User *> &users)
{
    ofstream file("users.dat", ios::binary); // no ios::app — overwrites entire file

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
    int download_count = 0;
    int read_onsite_count = 0;
    int timesborrowed_count = 0;

public:
    // CONSTRUCTOR
    Book() : bookid(next_id++), title("UNKNOWN"), author("UNKNOWN"), category("UNKNOWN"),
             ISBN("UNKNOWN"), publisher("UNKNOWN"), year_of_publication(0),
             available_copies(0), availability_status(false),
             download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

    Book(string t, string a, string c, string isbn, string p, int yop)
        : bookid(next_id++), title(t), author(a), category(c),
          ISBN(isbn), publisher(p), year_of_publication(yop),
          available_copies(0), availability_status(false),
          download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

    // DESTRUCTOR
    ~Book()
    {
        if (!DestructorLogger::isSilent()) // CHANGE 2.0: Using DestructorLogger class
            cout << "BOOK DESTROYED!" << endl;
    }

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

    // STATIC ID MANAGEMENT (similar to User class)
    static void setNextID(int id) { next_id = id; }
    static int getNextID() { return next_id; }

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
int Book::next_id = 1;

class BookCatalog
{
private:
    vector<Book *> BK;

public:
    BookCatalog() {}
    // CHANGE 3.0: Added DestructorLogger guard — BookCatalog destructor should only print at program end
    ~BookCatalog()
    {
        for (int i = 0; i < (int)BK.size(); i++)
            delete BK[i];
        BK.clear();
        if (!DestructorLogger::isSilent())
            cout << "BOOKCATALOG DESTROYED!" << endl;
    }

    void addbook(Book *b)
    {
        BK.push_back(b);
        if (!DestructorLogger::isSilent()) // CHANGE 2.0: Using DestructorLogger class
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

    // ---------------------------------------------------------------
    // NOTE: All Search* functions return nullptr silently when no
    // match is found. They do NOT print "BOOK NOT FOUND!" themselves
    // because they are also used internally (e.g. duplicate-title
    // check in manageBooks). Printing here would produce a false
    // "BOOK NOT FOUND!" message every time a brand-new book is added.
    // Callers that need a user-facing error must print it themselves.
    // ---------------------------------------------------------------

    Book *SearchbyID(int id)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_bookid() == id)
                return b;
        }

        return nullptr; // caller decides whether to report "not found"
    }

    Book *SearchbyTitle(string t)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_title() == t)
                return b;
        }

        return nullptr; // caller decides whether to report "not found"
    }

    Book *SearchbyAuthor(string a)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_author() == a)
                return b;
        }

        return nullptr; // caller decides whether to report "not found"
    }

    Book *SearchbyCategory(string c)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_category() == c)
                return b;
        }

        return nullptr; // caller decides whether to report "not found"
    }

    Book *SearchbyPublisher(string p)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_publisher() == p)
                return b;
        }

        return nullptr; // caller decides whether to report "not found"
    }

    Book *SearchbyYOP(int yop)
    {
        for (int i = 0; i < BK.size(); i++)
        {
            Book *b = BK[i];

            if (b->get_YOP() == yop)
                return b;
        }

        return nullptr; // caller decides whether to report "not found"
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
void loadBooksFromFile(BookCatalog *catalog)
{
    ifstream file("books.dat", ios::binary);

    if (!file)
        return;

    BookRecord br;

    while (file.read((char *)&br, sizeof(br)))
    {
        Book *b = new Book(
            br.title,
            br.author,
            br.category,
            br.isbn,
            br.publisher,
            br.year);

        // Restore the book ID if it's higher than current next_id
        if (br.id >= Book::getNextID())
        {
            Book::setNextID(br.id + 1);
        }
        // Manually set the ID to match the saved record
        b->set_bookid(br.id);

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
    // CHANGE 3.0: Added DestructorLogger guard — librarian destructor was printing during normal flow
    ~librarian()
    {
        if (!DestructorLogger::isSilent())
            cout << "LIBRARIAN DESTROYED!" << endl;
    }

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
    ~Transaction()
    {
        if (!DestructorLogger::isSilent()) // CHANGE 2.0: Using DestructorLogger class
            cout << "TRANSACTION DESTROYED!" << endl;
    }

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

    strncpy(tr.return_date, t.getReturnDate().c_str(), sizeof(tr.return_date) - 1);
    tr.return_date[sizeof(tr.return_date) - 1] = '\0';

    file.write((char *)&tr, sizeof(tr));
    file.close();
}
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

        // CHANGE 3.0: Suppress destructor noise — push_back may resize vector, destroying temp copies
        DestructorLogger::enableSilentMode();
        transactions.push_back(t);
        DestructorLogger::disableSilentMode();

        // ---- RESTORE BORROWED BOOKS INTO USER OBJECT ----
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
    ~Fines()
    {
        if (!DestructorLogger::isSilent()) // CHANGE 2.0: Using DestructorLogger class
            cout << "FINES DESTROYED!" << endl;
    }

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

        // CHANGE 3.0: Suppress destructor noise — push_back may resize vector, destroying temp copies
        DestructorLogger::enableSilentMode();
        fines.push_back(f);
        DestructorLogger::disableSilentMode();
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
    string get_location() { return location; }
    int get_capacity() { return capacity; }
    void setID(int id) { tableid = id; }
    void setLocation(string loc) { location = loc; }
    void setCapacity(int cap) { capacity = cap; }

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

        // CHANGE 3.0: Suppress destructor noise — push_back may resize vector, destroying temp copies
        DestructorLogger::enableSilentMode();
        tables.push_back(t);
        DestructorLogger::disableSilentMode();
    }

    file.close();
}

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

    int getUserCount() { return (int)users.size(); }

    vector<User *> &getUsersVector() { return users; }
    vector<Table> &getTablesVector() { return tables; }
    vector<Transaction> &getTransactionsVector() { return transactions; }
    vector<Fines> &getFinesVector() { return fines; }

    void registerUserInLibrary()
    {
        User *newUser = User::registerUser();
        if (newUser)
        {
            // Check duplicate username — retry loop until unique username is entered
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
                        break; // restart the duplicate check with the new username
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

    // ---------------- BOOK INTERACTIONS ----------------
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
        // CHANGE 3.0: Wrapped with DestructorLogger — vector resize during emplace_back
        // moves existing Transaction objects internally, firing their destructors falsely
        DestructorLogger::enableSilentMode();
        // CHANGE 3.0: Suppress destructor noise — emplace_back may resize vector, triggering destructors on existing elements
        DestructorLogger::enableSilentMode();
        transactions.emplace_back(u->get_userid(), book->get_bookid(), duration_minutes);
        DestructorLogger::disableSilentMode();
        DestructorLogger::disableSilentMode();
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
                    // CHANGE 3.0: Wrapped with DestructorLogger — vector resize during emplace_back
                    // moves existing Fines objects internally, firing their destructors falsely
                    DestructorLogger::enableSilentMode();
                    // CHANGE 3.0: Suppress destructor noise — emplace_back may resize vector, triggering destructors on existing elements
                    DestructorLogger::enableSilentMode();
                    fines.emplace_back(u->get_userid(), book->get_bookid(), overdue_mins, u);
                    DestructorLogger::disableSilentMode();
                    DestructorLogger::disableSilentMode();
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
                    if (u->hasMembership())
                    {
                        // Show both original and discounted amount
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
                        // No membership, show only the fine
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

        // CHANGE 3.0: Wrapped with DestructorLogger — vector resize during emplace_back
        // moves existing Reservation objects internally, firing their destructors falsely
        DestructorLogger::enableSilentMode();
        reservations.emplace_back(uid, bkid, 0, date, start, end);
        DestructorLogger::disableSilentMode();
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

        // CHANGE 3.0: Wrapped with DestructorLogger — vector resize during emplace_back
        // moves existing Reservation objects internally, firing their destructors falsely
        DestructorLogger::enableSilentMode();
        reservations.emplace_back(uid, 0, tid, date, start, end);
        DestructorLogger::disableSilentMode();
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
                // CHANGE 3.0: Wrapped with DestructorLogger — vector resize during emplace_back
                // moves existing Reservation objects internally, firing their destructors falsely
                DestructorLogger::enableSilentMode();
                reservations.emplace_back(uid, 0, tid, date, start, end);
                DestructorLogger::disableSilentMode();
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

        throw ReservationException("RESERVATION ID NOT FOUND!");
    }

    //----------------TABLE INTERATIONS----------------
    void addTable(string loc, int cap)
    {
        if ((int)tables.size() >= MAX_TABLES)
            throw ReservationException("TABLE LIMIT REACHED! MAXIMUM " + to_string(MAX_TABLES) + " TABLES ALLOWED.");

        // CHANGE 3.0: Wrapped with DestructorLogger — vector resize during push_back
        // moves existing Table objects internally, firing their destructors falsely
        DestructorLogger::enableSilentMode();
        tables.push_back(Table(loc, cap));
        DestructorLogger::disableSilentMode();
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
        if (i < 0 || i >= (int)tables.size())
            throw ReservationException("TABLE INDEX OUT OF BOUNDS: " + to_string(i));
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
            // Only shown for actual user searches (choices 1-6), not on back/exit
            cout << "BOOK NOT FOUND!" << endl;
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
            string date = getValidDate("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidTime("ENTER START TIME (HH:MM): ");
            string end = getValidTime("ENTER END TIME (HH:MM): ");

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
            string date = getValidDate("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidTime("ENTER START TIME (HH:MM): ");
            string end = getValidTime("ENTER END TIME (HH:MM): ");

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
            string date = getValidDate("ENTER DATE (DD/MM/YYYY): ");
            string start = getValidTime("ENTER START TIME (HH:MM): ");
            string end = getValidTime("ENTER END TIME (HH:MM): ");

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
            lib->getCatalog()->ViewBookCatalog();
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
            // Show user their currently borrowed books
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
            try
            {
                lib->readOnSite(this, title);
            }
            catch (BookException &e)
            {
                cout << e.what() << endl;
            }
            break;
        }

        case 4:
        {
            title = getValidString("ENTER BOOK TITLE: ");
            try
            {
                lib->downloadBook(this, title);
            }
            catch (BookException &e)
            {
                cout << e.what() << endl;
            }
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
        // illegal to declare a variable inside a switch without a new scope, so we add {}
          {  
            cout << "\n--------------- USERS BY TYPE ---------------" << endl;
            int typeChoice = getValidIntInRange("SELECT USER TYPE:\n1. STUDENT\n2. FACULTY\n3. GUEST\nCHOICE: ", 1, 3);
            string type;
            if (typeChoice == 1)
                type = "STUDENT";
            else if (typeChoice == 2)
                type = "FACULTY";
            else
                type= "GUEST";

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
            string title = getValidString("ENTER TITLE: ");
            string author = getValidAlphaString("ENTER AUTHOR: ");
            string category = getValidAlphaString("ENTER CATEGORY: ");
            string isbn = getValidISBN("ENTER ISBN: ");
            string publisher = getValidPublisher("ENTER PUBLISHER: ");
            int yop = getValidIntInRange("ENTER YEAR: ", 1000, 2026);
            int copies = getValidIntInRange("ENTER AVAILABLE COPIES: ", 1, 1000);

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
    // CHANGE 2.0: Using DestructorLogger class to suppress messages during file loading (OOP-compliant)
    DestructorLogger::enableSilentMode();
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
        loadTransactionsFromFile(lib.getTransactionsVector(), lib.getUsersVector());
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
        cout << "FINES FILE ERROR: " << e.what() << endl;
    }

    try
    {
        loadTablesFromFile(lib.getTablesVector());
    }
    catch (FileException &e)
    {
        cout << "TABLES FILE ERROR: " << e.what() << endl;
    }
    DestructorLogger::disableSilentMode(); // CHANGE 2.0: Restore normal output after loading

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
            try
            {
                lib.registerUserInLibrary();
            }
            catch (UserException &e)
            {
                cout << e.what() << endl;
            }
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