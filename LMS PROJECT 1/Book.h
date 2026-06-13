#ifndef BOOK_H
#define BOOK_H

// ============================================================
// Book.h
// Contains: Book class definition, BookCatalog class
//           definition, and file I/O function declarations.
// ============================================================

#include "Globals.h"

// ============================================================
//                    BOOK CLASS
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
    int available_copies;
    bool availability_status;
    int download_count;
    int read_onsite_count;
    int timesborrowed_count;

public:
    // DEFAULT CONSTRUCTOR
    Book();
    // PARAMETERIZED CONSTRUCTOR
    Book(string t, string a, string c, string isbn, string p, int yop);
    // DESTRUCTOR
    ~Book();

    // GETTERS
    int    get_bookid()              { return bookid; }
    string get_title()               { return title; }
    string get_author()              { return author; }
    string get_category()            { return category; }
    string get_isbn()                { return ISBN; }
    string get_publisher()           { return publisher; }
    int    get_YOP()                 { return year_of_publication; }
    int    getAvailableCopies()      { return available_copies; }
    int    get_downloadcount()       { return download_count; }
    int    get_read_onsitecount()    { return read_onsite_count; }
    int    get_timesborrowed_count() { return timesborrowed_count; }

    void decreaseCopy();

    // SETTERS
    void set_bookid(int id)        { bookid = id; }
    void set_author(string a)      { author = a; }
    void set_title(string t)       { title = t; }
    void set_category(string c)    { category = c; }
    void set_isbn(string isbn)     { ISBN = isbn; }
    void set_publisher(string p)   { publisher = p; }
    void set_YOP(int yop)          { year_of_publication = yop; }

    // Static ID management
    static void setNextID(int id) { next_id = id; }
    static int  getNextID()       { return next_id; }

    void setAvailableCopies(int ac);

    // Counter functions
    void incrementBorrowCount();
    void incrementDownloadCount();
    void incrementReadOnSiteCount();

    // Operator overloads for comparison
    bool operator==(const Book &other) const { return bookid == other.bookid; }
    bool operator==(const string &t)   const { return title == t; }
    bool operator==(const int &y)      const { return year_of_publication == y; }

    // Display
    friend ostream &operator<<(ostream &os, const Book &b);
};

// ============================================================
//                    BOOK CATALOG CLASS
// ============================================================
class BookCatalog
{
private:
    vector<Book *> BK;

public:
    BookCatalog();
    ~BookCatalog();

    // Book management
    void addbook(Book *b);
    void removeBook(string t);

    // Search functions (return nullptr if not found)
    Book *SearchbyID(int id);
    Book *SearchbyTitle(string t);
    vector<Book *> SearchbyAuthor(string a);
    vector<Book *> SearchbyCategory(string c);
    vector<Book *> SearchbyPublisher(string p);
    vector<Book *> SearchbyYOP(int yop);

    // Display all books
    void ViewBookCatalog() const;

    // Getters
    int   get_bookcount()          { return (int)BK.size(); }
    Book *getBookAtIndex(int index);
};

// ============================================================
//              BOOK FILE I/O FUNCTION DECLARATIONS
// ============================================================
void saveBookToFile(Book *b);
void loadBooksFromFile(BookCatalog *catalog);
void rewriteBooksFile(BookCatalog *catalog);

#endif
