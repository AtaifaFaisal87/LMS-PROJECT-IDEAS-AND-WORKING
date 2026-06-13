#ifndef LIBRARYCORE_H
#define LIBRARYCORE_H

#include <QString>
#include <QVector>
#include <QDateTime>
#include <QException>

// ============================================================
// EXCEPTION CLASSES
// ============================================================
class LibraryException : public QException
{
public:
    LibraryException(const QString& msg) : message(msg) {}
    const char* what() const noexcept override { return message.toUtf8().constData(); }
    QString getMessage() const { return message; }
private:
    QString message;
};

class FileException : public LibraryException
{
public:
    FileException(const QString& msg) : LibraryException(msg) {}
};

class BookException : public LibraryException
{
public:
    BookException(const QString& msg) : LibraryException(msg) {}
};

class UserException : public LibraryException
{
public:
    UserException(const QString& msg) : LibraryException(msg) {}
};

class AuthException : public LibraryException
{
public:
    AuthException(const QString& msg) : LibraryException(msg) {}
};

class ReservationException : public LibraryException
{
public:
    ReservationException(const QString& msg) : LibraryException(msg) {}
};

class TransactionException : public LibraryException
{
public:
    TransactionException(const QString& msg) : LibraryException(msg) {}
};

// ============================================================
// MEMBERSHIP CLASS
// ============================================================
class Membership
{
private:
    int membershipID;
    static int next_id;
    int userID;
    QString userType;
    QString userName;
    QDateTime activationTime;
    QDateTime expiryTime;
    bool active;
    static const double FINE_DISCOUNT;
    static const int DEFAULT_DURATION_DAYS;

public:
    Membership();
    Membership(int uid, const QString& utype, const QString& uname, int durationDays = 365);
    Membership(int uid, const QString& utype, const QString& uname,
               const QDateTime& activation, const QDateTime& expiry);
    ~Membership() {}

    int getMembershipID() const { return membershipID; }
    int getUserID() const { return userID; }
    QString getUserType() const { return userType; }
    QString getUserName() const { return userName; }
    bool isActive() const { return active; }
    QDateTime getActivationTime() const { return activationTime; }
    QDateTime getExpiryTime() const { return expiryTime; }

    void setActivationTime(const QDateTime& t) { activationTime = t; }
    void setExpiryTime(const QDateTime& t) { expiryTime = t; }

    bool isValid() const;
    double applyDiscount(double originalFine) const;
    int getBorrowBoost() const;
    void cancel();
};

// ============================================================
// USER CLASS (ABSTRACT)
// ============================================================
class Library;

class User
{
protected:
    int userid;
    static int next_id;
    QString name;
    QString CNIC;
    QString email;
    QString username;
    QString password;
    QString phonenumber;
    bool isDeleted;
    QVector<int> borrowedbook;
    Membership* membership;
    int daily_borrows;
    QString last_borrow_date;
    double balance;
    double total_fines_paid;
    static const double MAX_UNPAID_FINE_LIMIT;

public:
    User(const QString& n = "NONE", const QString& c = "xxxxx-xxxxxxx-x",
         const QString& e = "abc@gmail.com", const QString& p = "03xxxxxxxxx");
    virtual ~User();

    void setid(int id) { userid = id; }
    void setname(const QString& n) { name = n; }
    void setemail(const QString& e) { email = e; }
    void setphone_no(const QString& p) { phonenumber = p; }
    void setCNIC(const QString& c) { CNIC = c; }
    void setUsername(const QString& u) { username = u; }
    void setPassword(const QString& p) { password = p; }
    static void setNextID(int id) { next_id = id; }
    void setDailyBorrows(int d) { daily_borrows = d; }
    void setLastBorrowDate(const QString& d) { last_borrow_date = d; }
    void setBalance(double b) { balance = b; }
    void setTotalFinesPaid(double f) { total_fines_paid = f; }
    void setIsDeleted(bool d) { isDeleted = d; }

    int get_userid() const { return userid; }
    QString get_name() const { return name; }
    QString get_email() const { return email; }
    QString get_phone() const { return phonenumber; }
    QString get_cnic() const { return CNIC; }
    QString getUsername() const { return username; }
    QString getPassword() const { return password; }
    int getDailyBorrows() const { return daily_borrows; }
    QString getLastBorrowDate() const { return last_borrow_date; }
    double getBalance() const { return balance; }
    double getTotalFinesPaid() const { return total_fines_paid; }
    bool getIsDeleted() const { return isDeleted; }
    const QVector<int>& getBorrowedBooks() const { return borrowedbook; }
    int getBorrowedCount() const { return borrowedbook.size(); }

    virtual int getborrowlimit() = 0;
    virtual QString get_usertype() = 0;
    virtual int getFineRate() = 0;

    bool borrowbook(int id);
    bool returnbook(int id);

    Membership* getMembership() { return membership; }
    bool hasMembership() const;
    void activateMembership(int durationDays = 365);
    void cancelMembership();
    void viewMembership();
    void restoreMembership(const QDateTime& activation, const QDateTime& expiry);
    int getEffectiveBorrowLimit() const;
    bool canBorrowToday();
    void recordDailyBorrow();
    double applyMembershipDiscount(double fine);

    bool deductBalance(double amount);
    void addBalance(double amount);
    bool canBorrowBasedOnBalance() const { return balance > -MAX_UNPAID_FINE_LIMIT; }
    double calculateTotalUnpaidFines(Library* lib);
};

class Student : public User
{
private:
    QString seatno;
    QString major;

public:
    Student(const QString& n, const QString& c, const QString& e, const QString& p,
            const QString& s, const QString& m);
    int getborrowlimit() override { return 3; }
    QString get_usertype() override { return "STUDENT"; }
    QString getSeatNo() const { return seatno; }
    QString getMajor() const { return major; }
    void setSeatNo(const QString& s) { seatno = s; }
    void setMajor(const QString& m) { major = m; }
    int getFineRate() override { return 10; }
};

class Faculty : public User
{
private:
    QString department;
    QString designation;

public:
    Faculty(const QString& n, const QString& c, const QString& e, const QString& p,
            const QString& dept, const QString& des);
    int getborrowlimit() override { return 5; }
    QString get_usertype() override { return "FACULTY"; }
    QString getDepartment() const { return department; }
    QString getDesignation() const { return designation; }
    void setDepartment(const QString& d) { department = d; }
    void setDesignation(const QString& d) { designation = d; }
    int getFineRate() override { return 15; }
};

class Guest : public User
{
private:
    QString purpose;

public:
    Guest(const QString& n, const QString& c, const QString& e, const QString& p,
          const QString& ps);
    int getborrowlimit() override { return 1; }
    QString get_usertype() override { return "GUEST"; }
    QString getPurpose() const { return purpose; }
    void setPurpose(const QString& p) { purpose = p; }
    int getFineRate() override { return 20; }
};

// ============================================================
// BOOK CLASS
// ============================================================
class Book
{
private:
    static int next_id;
    int bookid;
    QString title;
    QString author;
    QString category;
    QString ISBN;
    QString publisher;
    int year_of_publication;
    int available_copies;
    bool availability_status;
    int download_count;
    int read_onsite_count;
    int timesborrowed_count;

public:
    Book();
    Book(const QString& t, const QString& a, const QString& c,
         const QString& isbn, const QString& p, int yop);
    ~Book() {}

    int get_bookid() const { return bookid; }
    QString get_title() const { return title; }
    QString get_author() const { return author; }
    QString get_category() const { return category; }
    QString get_isbn() const { return ISBN; }
    QString get_publisher() const { return publisher; }
    int get_YOP() const { return year_of_publication; }
    int getAvailableCopies() const { return available_copies; }
    int get_downloadcount() const { return download_count; }
    int get_read_onsitecount() const { return read_onsite_count; }
    int get_timesborrowed_count() const { return timesborrowed_count; }
    bool getAvailabilityStatus() const { return availability_status; }

    void set_bookid(int id) { bookid = id; }
    void set_author(const QString& a) { author = a; }
    void set_title(const QString& t) { title = t; }
    void set_category(const QString& c) { category = c; }
    void set_isbn(const QString& isbn) { ISBN = isbn; }
    void set_publisher(const QString& p) { publisher = p; }
    void set_YOP(int yop) { year_of_publication = yop; }
    static void setNextID(int id) { next_id = id; }
    static int getNextID() { return next_id; }

    void setAvailableCopies(int ac);
    void incrementBorrowCount();
    void incrementDownloadCount();
    void incrementReadOnSiteCount();
    void decreaseCopy() { if (available_copies > 0) available_copies--; }
};

// ============================================================
// BOOK CATALOG CLASS
// ============================================================
class BookCatalog
{
private:
    QVector<Book*> BK;

public:
    BookCatalog() {}
    ~BookCatalog();

    void addbook(Book* b);
    void removeBook(const QString& t);
    Book* SearchbyID(int id);
    Book* SearchbyTitle(const QString& t);
    QVector<Book*> SearchbyAuthor(const QString& a);
    QVector<Book*> SearchbyCategory(const QString& c);
    QVector<Book*> SearchbyPublisher(const QString& p);
    QVector<Book*> SearchbyYOP(int yop);
    void ViewBookCatalog() const;
    int get_bookcount() const { return BK.size(); }
    Book* getBookAtIndex(int index);
    const QVector<Book*>& getBooks() const { return BK; }
};

// ============================================================
// TRANSACTION CLASS
// ============================================================
class Transaction
{
private:
    static int next_id;
    int transaction_id;
    int user_id;
    int book_id;
    QDateTime borrow_time;
    QDateTime due_time;
    QDateTime return_time;
    QString return_date;
    bool returned;

public:
    Transaction();
    Transaction(int uid, int bid, int duration_minutes);
    ~Transaction() {}

    int getuserid() const { return user_id; }
    int getbookid() const { return book_id; }
    int getTransactionID() const { return transaction_id; }
    bool isReturned() const { return returned; }
    QDateTime getDueTime() const { return due_time; }
    QDateTime getBorrowTime() const { return borrow_time; }
    QDateTime getReturnTime() const { return return_time; }
    QString getReturnDate() const { return return_date; }

    void setTransactionID(int id) { transaction_id = id; }
    void setUserID(int id) { user_id = id; }
    void setBookID(int id) { book_id = id; }
    void setBorrowTime(const QDateTime& t) { borrow_time = t; }
    void setDueTime(const QDateTime& t) { due_time = t; }
    void setReturnTime(const QDateTime& t) { return_time = t; }
    void setReturnDate(const QString& d) { return_date = d; }
    void setReturned(bool r) { returned = r; }

    int getOverdueMinutes() const;
    int markreturned();
};

// ============================================================
// FINES CLASS
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
    Fines();
    Fines(int u, int b, int mins, User* user);
    ~Fines() {}

    double getAmount() const { return finalAmount; }
    double getOriginal() const { return originalAmount; }
    int getUserID() const { return user_id; }
    int getBookID() const { return book_id; }
    bool wasDiscounted() const { return discountApplied; }
    int getMinutesLate() const { return minuteslate; }
    double getRate() const { return rate; }

    void setUserID(int id) { user_id = id; }
    void setBookID(int id) { book_id = id; }
    void setMinutesLate(int m) { minuteslate = m; }
    void setRate(double r) { rate = r; }
    void setOriginalAmount(double a) { originalAmount = a; }
    void setFinalAmount(double a) { finalAmount = a; }
    void setDiscountApplied(bool d) { discountApplied = d; }
};

// ============================================================
// RESERVATION CLASS
// ============================================================
class Reservation
{
private:
    int reservationid;
    static int next_id;
    int userid;
    int bookid;
    int tableid;
    QString date;
    QString startTime;
    QString endTime;
    bool active;

public:
    Reservation();
    Reservation(int uid, int bid, int tid, const QString& d,
                const QString& start, const QString& end);
    ~Reservation() {}

    int get_userid() const { return userid; }
    int get_tableid() const { return tableid; }
    int get_bookid() const { return bookid; }
    int get_reservationid() const { return reservationid; }
    QString get_date() const { return date; }
    QString get_start() const { return startTime; }
    QString get_end() const { return endTime; }
    void setReservationID(int id) { reservationid = id; }
    bool isActive() const { return active; }
    bool isExpired() const;
    void cancel() { active = false; }

    static bool overlap(const QString& s1, const QString& e1,
                        const QString& s2, const QString& e2);
};

// ============================================================
// TABLE CLASS
// ============================================================
class Table
{
private:
    int tableid;
    static int next_id;
    QString location;
    int capacity;

public:
    Table();
    Table(const QString& loc, int cap);

    int get_id() const { return tableid; }
    QString get_location() const { return location; }
    int get_capacity() const { return capacity; }
    void setID(int id) { tableid = id; }
    void setLocation(const QString& loc) { location = loc; }
    void setCapacity(int cap) { capacity = cap; }
};

// ============================================================
// LIBRARIAN CLASS
// ============================================================
class librarian
{
private:
    int librarian_id;
    QString name;
    QString email;
    QString phone_no;
    QString username;
    QString password;

public:
    librarian();
    librarian(int id, const QString& n, const QString& e, const QString& p);
    ~librarian() {}

    void setCredentials(const QString& u, const QString& p);
    QString getUsername() const { return username; }
    QString getPassword() const { return password; }
    QString getName() const { return name; }
    QString getEmail() const { return email; }
    QString getPhone() const { return phone_no; }
    int getID() const { return librarian_id; }

    void addbook(Book* b, BookCatalog* catalog) { catalog->addbook(b); }
    void removeBook(const QString& t, BookCatalog* catalog) { catalog->removeBook(t); }
};

// ============================================================
// LIBRARY CLASS
// ============================================================
class Library
{
private:
    BookCatalog catalog;
    QVector<Table> tables;
    QVector<Reservation> reservations;
    QVector<Transaction> transactions;
    QVector<Fines> fines;
    QVector<User*> users;

public:
    static const int MAX_TABLES = 10;

    Library() {}
    ~Library();

    User* getUserByIndex(int index);
    int getUserCount() const { return users.size(); }
    int getActiveUserCount() const;
    QVector<User*>& getUsersVector() { return users; }
    QVector<Table>& getTablesVector() { return tables; }
    QVector<Transaction>& getTransactionsVector() { return transactions; }
    QVector<Fines>& getFinesVector() { return fines; }
    QVector<Reservation>& getReservationsVector() { return reservations; }

    void registerUserInLibrary(User* newUser);
    bool deleteUser(int uid);

    bool readOnSite(User* u, const QString& title);
    bool downloadBook(User* u, const QString& title);
    bool hasOverdueBooks(User* u) const;
    bool borrowBook(User* u, const QString& title, int duration_minutes);
    bool returnBook(User* u, const QString& title);
    int calculateLiveFine(Transaction& t, User* u) const;

    void viewTransactions();
    void viewFines();
    void viewUserTransactions(int userID);
    void viewUserFines(int userID);
    void viewUserReservations(int uid);
    void viewAllReservations();
    void viewTables();
    void viewUsers();
    void viewUsersByType(const QString& type);
    void viewAllMemberships();

    void reserveBook(int uid, int bkid, const QString& date,
                     const QString& start, const QString& end);
    void reserveTableManual(int uid, int tid, const QString& date,
                            const QString& start, const QString& end);
    void reserveTableAuto(int uid, const QString& date,
                          const QString& start, const QString& end);
    void cancelReservation(int rid, int uid);
    void autoExpireReservations();

    void addTable(const QString& loc, int cap);
    void removeTable(int tid);
    void viewAvailableTables(const QString& date,
                             const QString& start, const QString& end);
    int getTableCount() const { return tables.size(); }
    bool hasAvailableTable(const QString& date,
                           const QString& start, const QString& end) const;
    Table getTable(int i) const;

    BookCatalog* getCatalog() { return &catalog; }

    void loadAllData();
    void saveAllData();
};

// ============================================================
// AUTHENTICATION CLASS
// ============================================================
class Authentication
{
private:
    librarian* lib_admin;

public:
    Authentication(librarian* l = nullptr);
    void setLibrarian(librarian* l) { lib_admin = l; }
    User* loginUser(Library* lib, const QString& username, const QString& password);
    librarian* loginLibrarian(const QString& username, const QString& password);
};

// Utility Functions
QString getTodayDateString();
int minutesDiff(const QDateTime& start, const QDateTime& end);
QDateTime reservationToDateTime(const QString& date, const QString& timeStr);
bool isValidReservationTime(const QString& date, const QString& start, const QString& end);

#endif // LIBRARYCORE_H
