#include <iostream>
#include <string>
using namespace std;

class BookCatalog;
class PhysicalBook;
class Ebook;

class user
{
protected:
    int Userid;
    string name;
    string email;
    int borrowedbook[20];
    int borrowedbook_count = 0;

public:
    // CONSTRUCTOR
    user() : Userid(0), name("unknown"), email("abc@gmail.com"), borrowedbook_count(0) {}
    user(int id, string n, string e) : Userid(id), name(n), email(e), borrowedbook_count(0) {}
    // DESTRUCTOR
    virtual ~user() {}
    // SETTERS
    void setid(int id) { Userid = id; }
    void setemail(string e) { email = e; }
    void setname(string n) { name = n; }
    // GETTERS
    int getid() { return Userid; }
    string getemail() { return email; }
    string getname() { return name; }
    int getborrowedbook_count() { return borrowedbook_count; }
    // PURE VIRTUAL FUNCTIONS
    virtual int getBorrowedlimit() = 0;
    virtual string getUserType() = 0;
    // FUNCTIONS
    bool borrowBook(int bookID)
    {
        if (borrowedbook_count < getBorrowedlimit())
        {
            borrowedbook[borrowedbook_count] = bookID;
            borrowedbook_count++;
            return true;
        }
        else
        {
            cout << "BORROW LIMIT REACHED!" << endl;
            return false;
        }
    }

    bool returnbook(int bookID)
    {
        int pos = -1;
        for (int i = 0; i < borrowedbook_count; i++)
        {
            if (borrowedbook[i] == bookID)
            {
                pos = i;
                break;
            }
        }
        if (pos == -1)
        {
            cout << "BOOK NOT FOUND!" << endl;
            return false;
        }

        for (int i = pos; i < borrowedbook_count - 1; i++)
            borrowedbook[i] = borrowedbook[i + 1];

        borrowedbook_count--;
        cout << "BOOK RETURNED SUCCESSFULLY!" << endl;
        return true;
    }

    void viewborrowedbooks(BookCatalog &catalog) const
    {
        if (borrowedbook_count == 0)
        {
            cout << "NO BOOKS BORROWED!" << endl;
            return;
        }

        for (int i = 0; i < borrowedbook_count; i++)
        {
            PhysicalBook *book = catalog.SearchPBbyID(borrowedbook[i]);
            if (book)
                cout << "ID: " << book->getID() << "|TITLE: " << book->getTitle() << endl;
            else
                cout << "ID: " << borrowedbook[i] << " (BOOK NOT FOUND)" << endl;
        }
    }

    void viewFines(Fine *fines[], int fine_count) const
    {
        bool hasFines = false;
        for (int i = 0; i < fine_count; i++)
        {
            if (fines[i]->getUserID() == Userid)
            {
                fines[i]->displayFine();
                hasFines = true;
            }
        }
        if (!hasFines)
            cout << "NO FINES FOR THIS USER!" << endl;
    }
};

class Student : public user
{
public:
    // CONSTRUCTOR
    Student()
    {
        Userid = 0;
        name = "unknown";
        email = "unknown";
    }
    Student(string n, string e, int id)
    {
        name = n;
        email = e;
        Userid = id;
    }
    // GETTERS
    int getBorrowedlimit() override { return 5; }
    string getUserType() override { return "Student"; }
};

class Guest : public user
{
    string purpose;

public:
    // CONSTRUCTOR
    Guest()
    {
        Userid = 0;
        name = "unknown";
        purpose = "unknown";
        email = "unknown";
    }
    Guest(string n, string e, int id, string p)
    {
        name = n;
        email = e;
        Userid = id;
        purpose = p;
    }
    // GETTERS
    int getBorrowedlimit() override { return 0; }
    string getUserType() override { return "Guest"; }
};

class Faculty : public user
{
    string department;

public:
    // CONSTRUCTORS
    Faculty()
    {
        Userid = 0;
        name = "unknown";
        email = "unknown";
        department = "unknown";
    }
    Faculty(string n, string e, int id, string d)
    {
        name = n;
        email = e;
        Userid = id;
        department = d;
    }
    // GETTERS
    int getBorrowedlimit() override { return 10; }
    string getUserType() override { return "Faculty"; }
};

// Book classes
class Book
{
protected:
    int BookID;
    string Title;
    string Author;

public:
    // CONSTRUCTORS
    Book() : BookID(0), Title("unknown"), Author("UNKNOWN") {}
    Book(int id, string t, string a) : BookID(id), Title(t), Author(a) {}
    // DESTRUCTOR
    virtual ~Book() {}
    // GETTER
    int getID() const { return BookID; }
    string getTitle() const { return Title; }
    string getAuthor() const { return Author; }
    // FUNCTION
    virtual void display() const = 0;
};

class Ebook : public Book
{
private:
    double filesize;
    string downloadlink;

public:
    // CONSTRUCTOR
    Ebook() : Book(), filesize(0), downloadlink("UNKNOWN") {}
    Ebook(int id, string t, string a, double s, string dl) : Book(id, t, a), filesize(s), downloadlink(dl) {}
    // GETTERS
    double getFileSize() const { return filesize; }
    string getDownloadLink() const { return downloadlink; }
    // SETTERS
    void setFileSize(double s) { filesize = s; }
    void setDownloadLink(string dl) { downloadlink = dl; }
    // FUNCTIONS
    void display() const override
    {
        cout << "E-ID:" << BookID << "\nTITLE:" << Title << "\nAUTHOR:" << Author
             << "\nFILE SIZE:" << filesize << "\nDOWNLOAD LINK:" << downloadlink << endl;
    }
};

class PhysicalBook : public Book
{
private:
    string rackLocation;
    int availableCopies = 0;

public:
    // CONSTRUCTOR
    PhysicalBook() : Book(), rackLocation("unknown"), availableCopies(0) {}
    PhysicalBook(int id, string t, string a, string rl, int copies) : Book(id, t, a), rackLocation(rl), availableCopies(copies) {}
    // GETTERS
    string getRackLocation() const { return rackLocation; }
    int getAvailableCopies() const { return availableCopies; }
    // SETTERS
    void setRackLocation(string rl) { rackLocation = rl; }
    void setAvailableCopies(int a) { availableCopies = a; }
    // FUNCTIONS
    void display() const override
    {
        cout << "ID:" << BookID << "\nTITLE:" << Title << "\nAUTHOR:" << Author
             << "\nRACK LOCATION:" << rackLocation << "\nAVAILABLE COPIES:" << availableCopies << endl;
    }
};

// BookCatalog
class BookCatalog
{
private:
    PhysicalBook *PB[100];
    int physicalbookcount = 0;

    Ebook *EB[100];
    int Ebookcount = 0;

public:
    // CONSTRUCTORS
    BookCatalog() : physicalbookcount(0), Ebookcount(0) {}
    // FUNCTIONS

    // ================================================
    // -----------------PHYSICAL BOOK------------------
    // ================================================
    void addphysicalbook(PhysicalBook *p)
    {
        if (physicalbookcount < 100)
        {
            PB[physicalbookcount++] = p;
            cout << "PHYSICAL BOOK ADDED SUCCESSFULLY!" << endl;
        }
        else
            cout << "PHYSICAL LIBRARY FULL!" << endl;
    }

    void removePhysicalBook(string t)
    {
        int pos = -1;
        if (physicalbookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return;
        }
        for (int i = 0; i < physicalbookcount; i++)
        {
            if (PB[i]->getTitle() == t)
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
        for (int i = pos; i < physicalbookcount - 1; i++)
            PB[i] = PB[i + 1];
        delete PB[pos];
        physicalbookcount--;
        cout << "PHYSICAL BOOK REMOVED!" << endl;
    }

    PhysicalBook *SearchPBbytitle(string t)
    {
        for (int i = 0; i < physicalbookcount; i++)
            if (PB[i]->getTitle() == t)
                return PB[i];
        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    PhysicalBook *SearchPBbyID(int id)
    {
        if (physicalbookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return nullptr;
        }
        for (int i = 0; i < physicalbookcount; i++)
        {
            if (PB[i]->getID() == id)
            {
                return PB[i];
            }
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    PhysicalBook *SearchPBbyAuthor(string a)
    {
        if (physicalbookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return nullptr;
        }
        for (int i = 0; i < physicalbookcount; i++)
        {
            if (PB[i]->getAuthor() == a)
            {
                return PB[i];
            }
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    void ViewPhysicalBookCatalog() const
    {
        if (physicalbookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return;
        }
        cout << "===PHYSICAL BOOKS===" << endl;
        for (int i = 0; i < physicalbookcount; i++)
        {
            PB[i]->display();
        }
    }

    // ================================================
    // --------------------E-BOOK----------------------
    // ================================================
    void addEbook(Ebook *e)
    {
        if (Ebookcount < 100)
        {
            EB[Ebookcount++] = e;
            cout << "E-BOOK ADDED SUCCESSFULLY!" << endl;
        }
        else
            cout << "E-BOOK CAPACITY FULL!" << endl;
    }

    void removeEBook(string t)
    {
        int pos = -1;
        if (Ebookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return;
        }
        for (int i = 0; i < Ebookcount; i++)
        {
            if (EB[i]->getTitle() == t)
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
        for (int i = pos; i < Ebookcount - 1; i++)
            EB[i] = EB[i + 1];
        delete EB[pos];
        Ebookcount--;
        cout << "E-BOOK REMOVED!" << endl;
    }

    Ebook *SearchEBbyID(int id)
    {
        if (Ebookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return nullptr;
        }
        for (int i = 0; i < Ebookcount; i++)
        {
            if (EB[i]->getID() == id)
            {
                return EB[i];
            }
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Ebook *SearchEBbyTitle(string t)
    {
        if (Ebookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return nullptr;
        }
        for (int i = 0; i < Ebookcount; i++)
        {
            if (EB[i]->getTitle() == t)
            {
                return EB[i];
            }
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    Ebook *SearchEBbyAuthor(string a)
    {
        if (Ebookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return nullptr;
        }
        for (int i = 0; i < Ebookcount; i++)
        {
            if (EB[i]->getAuthor() == a)
            {
                return EB[i];
            }
        }

        cout << "BOOK NOT FOUND!" << endl;
        return nullptr;
    }

    void ViewEBookCatalog() const
    {
        if (Ebookcount == 0)
        {
            cout << "NO BOOKS IN CATALOG!" << endl;
            return;
        }
        cout << "===E-BOOK BOOKS===" << endl;
        for (int i = 0; i < Ebookcount; i++)
        {
            EB[i]->display();
        }
    }
};

// Librarian
class Librarian : public user
{
public:
    // CONSTRUCTOR
    Librarian() : user(0, "unknown", "unknown") {}
    Librarian(string n, string e, int id) : user(id, n, e) {}
    // GETTERS
    int getBorrowedlimit() override { return 0; }
    string getUserType() override { return "Librarian"; }
    // FUNCTIONS
    void addPhysicalBook(BookCatalog &catalog, PhysicalBook *book)
    {
        catalog.addphysicalbook(book);
    }

    void removePhysicalBook(BookCatalog &catalog, string title)
    {
        catalog.removePhysicalBook(title);
    }

    void addEbook(BookCatalog &catalog, Ebook *book)
    {
        catalog.addEbook(book);
    }

    void removeEbook(BookCatalog &catalog, string title)
    {
        catalog.removeEBook(title);
    }
};

// Transaction
class Transaction
{
private:
    int UserID, BookID;
    string IssueDate, DueDate;
    bool returned;

public:
    // CONSTRUCTORS
    Transaction() : UserID(0), BookID(0), IssueDate("UNKNOWN"), DueDate("UNKNOWN"), returned(false) {}
    Transaction(int u, int b, string i, string d) : UserID(u), BookID(b), IssueDate(i), DueDate(d), returned(false) {}
    // GETTERS
    int getuserid() { return UserID; }
    int getbookid() { return BookID; }
    string getissuedate() { return IssueDate; }
    string getduedate() { return DueDate; }
    bool getReturned() { return returned; }
    // SETTERS
    void setIssueDate(string i) { IssueDate = i; }
    void setDueDate(string d) { DueDate = d; }
    void markreturned() { returned = true; }
};

// Fine
class Fine
{
private:
    int userID, bookID, dayslate;
    double amount;

public:
    // CONSTRUCTORS
    Fine() : userID(0), bookID(0), dayslate(0), amount(0) {}
    Fine(int u, int b, int d, string userType) : userID(u), bookID(b), dayslate(d)
    {
        double rate = 0;
        if (userType == "Student")
            rate = 10;
        else if (userType == "Faculty")
            rate = 5;
        else if (userType == "Guest")
            rate = 15;
        amount = dayslate * rate;
    }
    // FUNCTIONS
    void displayFine()
    {
        cout << "USER-ID:" << userID << "\nBOOK-ID:" << bookID << "\nDAYS-LATE:" << dayslate << "\nFINE:" << amount << " Rs" << endl;
    }
    // GETTERS
    double getAmount() { return amount; }
    int getUserID() { return userID; }
};

// Library
class library
{
private:
    BookCatalog catalog;

    Transaction *transaction[100];
    int transaction_count = 0;

    Fine *fines[100];
    int fine_count = 0;

    user *users[100];
    int user_count = 0;

public:
    // FUNCTIONS

    // ===============================================
    // -----------BOOKS MANAGEMENT--------------------
    // ===============================================
    bool borrowPhysicalBook(user *u, string title, string issue, string due)
    {
        PhysicalBook *book = catalog.SearchPBbytitle(title);
        if (book == nullptr)
        {
            cout << "BOOK NOT FOUND!" << endl;
            return false;
        }
        if (book->getAvailableCopies() == 0)
        {
            cout << "NO COPIES AVAILABLE!" << endl;
            return false;
        }
        if (u->borrowBook(book->getID()))
        {
            book->setAvailableCopies(book->getAvailableCopies() - 1);
            transaction[transaction_count++] = new Transaction(u->getid(), book->getID(), issue, due);
            cout << "BOOK BORROWED & TRANSACTION CREATED!" << endl;
            return true;
        }
        return false;
    }

    bool returnPhysicalBook(user *u, string title, int daysLate = 0)
    {
        PhysicalBook *book = catalog.SearchPBbytitle(title);
        if (book == nullptr)
        {
            cout << "BOOK NOT FOUND!" << endl;
            return false;
        }
        if (u->returnbook(book->getID()))
        {
            book->setAvailableCopies(book->getAvailableCopies() + 1);
            if (daysLate > 0)
            {
                fines[fine_count] = new Fine(u->getid(), book->getID(), daysLate, u->getUserType());
                fine_count++;
                cout << "Fine generated for late return!" << endl;
            }

            for (int i = 0; i < transaction_count; i++)
            {
                if (transaction[i]->getbookid() == book->getID() && transaction[i]->getuserid() == u->getid())
                {
                    transaction[i]->markreturned();
                    break;
                }
            }
            cout << "BOOK RETURNED SUCCESSFULLY!" << endl;
            return true;
        }
        return false;
    }

    // =============================================
    // -----------TRANSACTION AND FINES-------------
    // =============================================
    void viewAllTransactions()
    {
        if (transaction_count == 0)
        {
            cout << "NO TRANSACTIONS FOUND!" << endl;
            return;
        }

        cout << "===ALL TRANSACTIONS===" << endl;
        for (int i = 0; i < transaction_count; i++)
        {
            cout << "USER ID: " << transaction[i]->getuserid() << endl;
            cout << "BOOK ID: " << transaction[i]->getbookid() << endl;
            cout << "ISSUE DATE: " << transaction[i]->getissuedate() << endl;
            cout << "DUE DATE: " << transaction[i]->getduedate() << endl;
            cout << "RETURNED: " << (transaction[i]->getReturned() ? "YES" : "NO") << endl;
            cout << "----------------------" << endl;
        }
    }

    void viewAllFines()
    {
        if (fine_count == 0)
        {
            cout << "NO FINES GENERATED!" << endl;
            return;
        }

        cout << "===ALL FINES===" << endl;
        for (int i = 0; i < fine_count; i++)
        {
            fines[i]->displayFine();
            cout << "-----------------" << endl;
        }
    }

    // ======================================
    // ----------USER MANAGEMENT-------------
    // ======================================
    void registerUser()
    {
        if (user_count >= 100)
        {
            cout << "USER CAPACITY FULL! CANNOT REGISTER MORE USERS.\n";
            return;
        }

        int choice;
        cout << "REGISTER USER TYPE:\n";
        cout << "1. STUDENT\n2. FACULTY\n3. GUEST\nENTER CHOICE: ";
        cin >> choice;
        cin.ignore(); // To clear newline after choice

        string name, email, extra;
        int id;

        cout << "ENTER ID: ";
        cin >> id;
        cin.ignore();
        cout << "ENTER NAME: ";
        getline(cin, name);
        cout << "ENTER EMAIL: ";
        getline(cin, email);

        switch (choice)
        {
        case 1: // Student
            users[user_count++] = new Student(name, email, id);
            cout << "STUDENT REGISTERED SUCCESSFULLY!\n";
            break;

        case 2: // Faculty
            cout << "ENTER DEPARTMENT: ";
            getline(cin, extra);
            users[user_count++] = new Faculty(name, email, id, extra);
            cout << "FACULTY REGISTERED SUCCESSFULLY!\n";
            break;

        case 3: // Guest
            cout << "ENTER PURPOSE: ";
            getline(cin, extra);
            users[user_count++] = new Guest(name, email, id, extra);
            cout << "GUEST REGISTERED SUCCESSFULLY!\n";
            break;

        default:
            cout << "INVALID CHOICE! REGISTRATION FAILED.\n";
            break;
        }
    }

    user *getUserByIndex(int i)
    {
        if (i < 0 || i >= user_count)
            return nullptr;
        return users[i];
    }

    void viewAllUsers()
    {
        if (user_count == 0)
        {
            cout << "No users registered!\n";
            return;
        }
        for (int i = 0; i < user_count; i++)
        {
            cout << "ID: " << users[i]->getid()
                 << " | NAME: " << users[i]->getname()
                 << " | EMAIL: " << users[i]->getemail()
                 << " | TYPE: " << users[i]->getUserType() << endl;
        }
    }

    void viewUsersByType(string type)
    {
        bool found = false;
        for (int i = 0; i < user_count; i++)
        {
            if (users[i]->getUserType() == type)
            {
                cout << "ID: " << users[i]->getid()
                     << " | NAME: " << users[i]->getname()
                     << " | EMAIL: " << users[i]->getemail() << endl;
                found = true;
            }
        }
        if (!found)
            cout << "No users of type " << type << " found.\n";
    }

    //DESTRUCTOR
    ~library() {
    for (int i = 0; i < transaction_count; i++) delete transaction[i];
    for (int i = 0; i < fine_count; i++) delete fines[i];
    for (int i = 0; i < user_count; i++) delete users[i];
}
};

class AuthManager
{
private:
    library *lib; // pointer to library to access users

public:
    AuthManager(library *l) : lib(l) {}

    user *login(int id, string email)
    {
        // check all users in the library
        for (int i = 0; i < 100; i++)
        {
            user *u = lib->getUserByIndex(i); // you may need to add this getter in library
            if (u && u->getid() == id && u->getemail() == email)
            {
                cout << "LOGIN SUCCESSFUL! WELCOME" << u->getname() << " (" << u->getUserType() << ")\n";
                return u;
            }
        }
        cout << "INVALID CREDENTIALS!\n";
        return nullptr;
    }
};

int main()
{
}




