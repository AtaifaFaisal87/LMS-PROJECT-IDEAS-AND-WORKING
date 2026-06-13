#ifndef GLOBALS_H
#define GLOBALS_H

// ============================================================
// Globals.h
// Contains: all #includes, structs (for file I/O),
//           forward declarations, utility functions,
//           and input validation functions.
// Every other file will include this.
// ============================================================

#include <iostream>
#include <vector>
#include <ctime>
#include <string>
#include <cstring>
#include <fstream>
using namespace std;

// ============================================================
//                       RUNTIME LOGGER
// ============================================================
// Controls which runtime messages are printed during sensitive
// operations (e.g. file loading). Prevents statements like
// BORROW LIMIT REACHED or BOOK ADDED from firing mid-load.
class RuntimeLogger
{
private:
    static bool suppressMessages;

public:
    static void enableSilentMode()  { suppressMessages = true; }
    static void disableSilentMode() { suppressMessages = false; }
    static bool isSilent()          { return suppressMessages; }
};

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
    int borrowed_ids[10];
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

struct ReservationRecord
{
    int reservationid;
    int userid;
    int bookid;
    int tableid;
    char date[15];
    char startTime[10];
    char endTime[10];
    bool active;
};

// ============================================================
// Forward declarations (classes reference each other so we
// declare their names here before defining them)
// ============================================================
class Book;
class Transaction;
class Fines;
class librarian;
class BookCatalog;
class User;
class library;
class Membership;

// ============================================================
//            REAL-TIME UTILITY FUNCTIONS
// ============================================================

// Gets current system time as time_t
time_t getCurrentTime();

// Converts time_t to human-readable string, or "N/A" if time is 0
string timeToString(time_t t);

// Converts 24-hour HH:MM to 12-hour format with AM/PM
string to12Hour(string timeStr);

// Converts date (DD/MM/YYYY) and time (HH:MM in 24-hour) to time_t
time_t reservationToTimeT(string date, string timeStr);

// Returns difference in minutes between two time_t values (end - start)
int minutesDiff(time_t start, time_t end);

// Adds given number of minutes to a time_t value and returns the new time_t
time_t addMinutes(time_t t, int minutes);

// ============================================================
// INPUT VALIDATION FUNCTIONS
// ============================================================
string getValidDate(string prompt);
string getValidTime(string prompt);
bool   isValidReservationTime(string date, string start, string end);
int    getValidInt(string prompt);
int    getValidIntInRange(string prompt, int min, int max);
string getValidString(string prompt);
string getValidPublisher(string prompt);
string getValidISBN(string prompt);
string getValidAlphaString(string prompt);
string getValidCNIC(string prompt);
string getValidEmail(string prompt);
string getValidPhone(string prompt);
string getValidPassword(string prompt);

#endif
