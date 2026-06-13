#ifndef USER_H
#define USER_H

// ============================================================
// User.h
// Contains: User (abstract base class), Student, Faculty,
//           Guest class definitions, and file I/O declarations.
// ============================================================

#include "Globals.h"
#include "Membership.h"

// Forward declare library so User menus can use it
class library;

// ============================================================
//                    USER CLASS (ABSTRACT)
// ============================================================
// Base class for Student, Faculty, Guest.
// Pure virtual functions force each subclass to define
// its own borrow limit, fine rate, etc.
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
    // DEFAULT CONSTRUCTOR with dummy values to simplify file loading
    User(string n = "NONE", string c = "xxxxx-xxxxxxx-x",
         string e = "abc@gmail.com", string p = "03xxxxxxxxx");

    // Virtual destructor ensures proper cleanup of derived classes
    virtual ~User();

    // ================= SETTERS =================
    void setid(int id)           { userid = id; }
    void setname(string n)       { name = n; }
    void setemail(string e)      { email = e; }
    void setphone_no(string p)   { phonenumber = p; }
    void setCNIC(string c)       { CNIC = c; }
    void setUsername(string u)   { username = u; }
    void setPassword(string p)   { password = p; }
    static void setNextID(int id){ next_id = id; }

    // ================= GETTERS =================
    int    get_userid()  { return userid; }
    string get_name()    { return name; }
    string get_email()   { return email; }
    string get_phone()   { return phonenumber; }
    string get_cnic()    { return CNIC; }
    string getUsername() { return username; }
    string getPassword() { return password; }

    const vector<int> &getBorrowedBooks() { return borrowedbook; }
    int getBorrowedCount()                { return (int)borrowedbook.size(); }

    // ================= PURE VIRTUAL FUNCTIONS =================
    // (Must be implemented by each subclass)
    virtual int    getborrowlimit()              = 0;
    virtual string get_usertype()                = 0;
    virtual void   printTo(ostream &os) const    = 0;
    virtual int    getFineRate()                 = 0;

    // Overload << so we can print user details with cout << user
    friend ostream &operator<<(ostream &os, const User &u);

    // ================= BORROW / RETURN =================
    bool borrowbook(int id);
    bool returnbook(int id);

    // ================= MEMBERSHIP =================
    Membership *getMembership()   { return membership; }
    bool hasMembership()          { return (membership != nullptr && membership->isValid()); }

    void activateMembership(int durationDays = 365);
    void cancelMembership();
    void viewMembership();
    void restoreMembership(time_t activation, time_t expiry);

    int    getEffectiveBorrowLimit();
    double applyMembershipDiscount(double fine);

    // ================= MENUS (implemented in User.cpp) =================
    void viewFullDetails(library *lib);
    void searchBooks(library *lib);
    void reservationMenu(library *lib);
    void bookActionMenu(library *lib);
    void membershipMenu(library *lib);
    void userMenu(library *lib);

    // ================= REGISTER =================
    static User *registerUser();
};

// ============================================================
//                    STUDENT
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
    string getSeatNo()               { return seatno; }
    string getMajor()                { return major; }
    int    getFineRate()    override { return 10; }

    void printTo(ostream &os) const override;
};

// ============================================================
//                    FACULTY
// ============================================================
class Faculty : public User
{
private:
    string department;
    string designation;

public:
    Faculty(string n, string c, string e, string p, string dept, string des)
        : User(n, c, e, p), department(dept), designation(des) {}

    int    getborrowlimit()   override { return 5; }
    string get_usertype()     override { return "FACULTY"; }
    string getDepartment()             { return department; }
    string getDesignation()            { return designation; }
    int    getFineRate()      override { return 15; }

    void printTo(ostream &os) const override;
};

// ============================================================
//                    GUEST
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
    string getPurpose()              { return purpose; }
    int    getFineRate()    override { return 20; }

    void printTo(ostream &os) const override;
};

// ============================================================
//              USER FILE I/O FUNCTION DECLARATIONS
// ============================================================
void saveUserToFile(User *u);
void loadUsersFromFile(vector<User *> &users);
void rewriteUsersFile(vector<User *> &users);

#endif
