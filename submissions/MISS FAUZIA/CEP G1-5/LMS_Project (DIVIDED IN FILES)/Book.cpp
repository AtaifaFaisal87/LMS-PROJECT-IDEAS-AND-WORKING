// ============================================================
// Book.cpp
// Contains: Book and BookCatalog class implementations,
//           and book file I/O functions.
// ============================================================

#include "Book.h"

// ---- Static member definition ----
int Book::next_id = 1;

// ============================================================
//                    BOOK CLASS
// ============================================================

Book::Book()
    : bookid(next_id++), title("UNKNOWN"), author("UNKNOWN"), category("UNKNOWN"),
      ISBN("UNKNOWN"), publisher("UNKNOWN"), year_of_publication(0),
      available_copies(0), availability_status(false),
      download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

Book::Book(string t, string a, string c, string isbn, string p, int yop)
    : bookid(next_id++), title(t), author(a), category(c),
      ISBN(isbn), publisher(p), year_of_publication(yop),
      available_copies(0), availability_status(false),
      download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

Book::~Book() {}

// Decrease copy count by 1 (used when a copy is reserved)
void Book::decreaseCopy()
{
    if (available_copies > 0)
        available_copies--;
}

// Set available copies and update status automatically
void Book::setAvailableCopies(int ac)
{
    available_copies = ac;
    availability_status = (available_copies > 0);
}

void Book::incrementBorrowCount()
{
    if (available_copies >= 0)
        timesborrowed_count++;
}

void Book::incrementDownloadCount()
{
    download_count++;
}

void Book::incrementReadOnSiteCount()
{
    read_onsite_count++;
}

ostream &operator<<(ostream &os, const Book &b)
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

// ============================================================
//                    BOOK CATALOG CLASS
// ============================================================

BookCatalog::BookCatalog() {}

// Destructor deletes all dynamically allocated Book objects
BookCatalog::~BookCatalog()
{
    for (int i = 0; i < (int)BK.size(); i++)
        delete BK[i];
    BK.clear();
}

void BookCatalog::addbook(Book *b)
{
    BK.push_back(b);
    if (!RuntimeLogger::isSilent())
        cout << "BOOK ADDED SUCCESSFULLY!" << endl;
}

void BookCatalog::removeBook(string t)
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

// NOTE: Search functions return nullptr silently when no match
// is found. The caller is responsible for printing "BOOK NOT FOUND".
Book *BookCatalog::SearchbyID(int id)
{
    for (int i = 0; i < (int)BK.size(); i++)
        if (BK[i]->get_bookid() == id)
            return BK[i];
    return nullptr;
}

Book *BookCatalog::SearchbyTitle(string t)
{
    for (int i = 0; i < (int)BK.size(); i++)
        if (BK[i]->get_title() == t)
            return BK[i];
    return nullptr;
}

vector<Book *> BookCatalog::SearchbyAuthor(string a)
{
    vector<Book *> results;
    for (int i = 0; i < (int)BK.size(); i++)
        if (BK[i]->get_author() == a)
            results.push_back(BK[i]);
    return results;
}

vector<Book *> BookCatalog::SearchbyCategory(string c)
{
    vector<Book *> results;
    for (int i = 0; i < (int)BK.size(); i++)
        if (BK[i]->get_category() == c)
            results.push_back(BK[i]);
    return results;
}

vector<Book *> BookCatalog::SearchbyPublisher(string p)
{
    vector<Book *> results;
    for (int i = 0; i < (int)BK.size(); i++)
        if (BK[i]->get_publisher() == p)
            results.push_back(BK[i]);
    return results;
}

vector<Book *> BookCatalog::SearchbyYOP(int yop)
{
    vector<Book *> results;
    for (int i = 0; i < (int)BK.size(); i++)
        if (BK[i]->get_YOP() == yop)
            results.push_back(BK[i]);
    return results;
}

void BookCatalog::ViewBookCatalog() const
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

Book *BookCatalog::getBookAtIndex(int index)
{
    if (index >= 0 && index < (int)BK.size())
        return BK[index];
    return nullptr;
}

// ============================================================
//              BOOK FILE I/O
// ============================================================

void saveBookToFile(Book *b)
{
    ofstream file("books.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN books.dat FOR WRITING!");

    BookRecord br;
    br.id = b->get_bookid();
    strncpy(br.title,     b->get_title().c_str(),     sizeof(br.title) - 1);
    strncpy(br.author,    b->get_author().c_str(),    sizeof(br.author) - 1);
    strncpy(br.category,  b->get_category().c_str(),  sizeof(br.category) - 1);
    strncpy(br.isbn,      b->get_isbn().c_str(),      sizeof(br.isbn) - 1);
    strncpy(br.publisher, b->get_publisher().c_str(), sizeof(br.publisher) - 1);
    br.year              = b->get_YOP();
    br.copies            = b->getAvailableCopies();
    br.download_count    = b->get_downloadcount();
    br.read_onsite_count = b->get_read_onsitecount();
    br.times_borrowed    = b->get_timesborrowed_count();

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
        Book *b = new Book(br.title, br.author, br.category,
                           br.isbn, br.publisher, br.year);

        // Restore the ID to match what was saved
        if (br.id >= Book::getNextID())
            Book::setNextID(br.id + 1);

        b->set_bookid(br.id);
        b->setAvailableCopies(br.copies);

        // Restore counters
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
        strncpy(br.title,     b->get_title().c_str(),     sizeof(br.title) - 1);
        strncpy(br.author,    b->get_author().c_str(),    sizeof(br.author) - 1);
        strncpy(br.category,  b->get_category().c_str(),  sizeof(br.category) - 1);
        strncpy(br.isbn,      b->get_isbn().c_str(),      sizeof(br.isbn) - 1);
        strncpy(br.publisher, b->get_publisher().c_str(), sizeof(br.publisher) - 1);
        br.year              = b->get_YOP();
        br.copies            = b->getAvailableCopies();
        br.download_count    = b->get_downloadcount();
        br.read_onsite_count = b->get_read_onsitecount();
        br.times_borrowed    = b->get_timesborrowed_count();

        file.write((char *)&br, sizeof(br));
        if (!file)
            throw FileException("ERROR: FAILED TO WRITE DURING BOOK FILE REWRITE!");
    }
    file.close();
}
