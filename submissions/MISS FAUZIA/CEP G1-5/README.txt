================================================================
         LIBRARY MANAGEMENT SYSTEM — CEP PROJECT
                      CIS  |  G1-5
================================================================

GROUP MEMBERS
--------------
  Name               Roll Number
  -----------------  -----------
  Abdur Rehman       CS-25032
  Hafsa Shakeel      CS-25011
  Ataifa Faisal      CS-25021
  Yazdaan Ali Mirza  CS-25030
  

Section: A

================================================================
STEPS TO RUN THE PROGRAM
================================================================

STEP 1 — COMPILE THE PROGRAM
-------------------------------
Open a terminal (Command Prompt or PowerShell) in the folder
that contains all the .cpp and .h files, then run:

    g++ *.cpp -o program

This compiles all source files and produces program.exe.

STEP 2 — RUN THE EXECUTABLE
-------------------------------
After compilation, run:

    .\program.exe

The system will automatically load saved data from files
(users, books, transactions, fines, tables, reservations).
If no data files exist yet, the system starts fresh with no
entries — this is normal on first run.

----------------------------------------------------------------

MAIN MENU
----------
You will see:

    ================ LIBRARY SYSTEM ================
    1. REGISTER USER
    2. LOGIN AS USER
    3. LOGIN AS LIBRARIAN
    4. EXIT

================================================================
OPTION 1 — REGISTER A NEW USER
================================================================
  - Choose option 1 from the main menu.
  - Select user type: Student, Faculty, or Guest.
  - Enter your details when prompted:
      Name, CNIC, Email, Phone Number
      Username (must be unique), Password
      Student: also enter Seat No. and Major
      Faculty: also enter Department and Designation
      Guest:   also enter Purpose of Visit
  - On success, your account is saved and you can now log in.

================================================================
OPTION 2 — LOGIN AS USER
================================================================
  - Choose option 2 from the main menu.
  - Enter your username and password.
  - On successful login, you reach the User Menu:

      USER MENU OPTIONS:
      1. View My Details
      2. Search Books
      3. Borrow / Return / Download / Read On-Site
      4. Reservation Menu
      5. Membership Menu
      6. View My Transactions
      7. View My Fines
      8. Logout

  BORROWING A BOOK:
    - Select option 3 (Book Actions).
    - Choose "Borrow a Book".
    - Enter the book title and desired borrow duration (minutes).
    - You must not have overdue books to borrow a new one.
    - Borrow limits: Student = 3, Faculty = 5, Guest = 1
      (Active membership adds +2 to the limit).

  RETURNING A BOOK:
    - Select option 3 (Book Actions) → "Return a Book".
    - Enter the title of the book you want to return.
    - If returned late, a fine is automatically calculated
      and saved.

  RESERVING A BOOK / TABLE:
    - Select option 4 (Reservation Menu).
    - You can reserve a specific book, reserve a specific
      table, or let the system auto-assign an available table.
    - Enter date (DD/MM/YYYY), start time and end time (HH:MM).

  MEMBERSHIP:
    - Select option 5 (Membership Menu).
    - Activate a 1-year membership to get +2 borrow limit
      and 15% discount on fines.
    - You can also view or cancel your membership here.

================================================================
OPTION 3 — LOGIN AS LIBRARIAN
================================================================
  Librarian Credentials:
      Username : admin
      Password : 123

  On login you reach the Librarian Main Menu:

      1. View Full Librarian Details
      2. Manage Books
      3. Manage Tables
      4. View All Users
      5. View Users by Type
      6. View All Transactions
      7. View All Fines
      8. View All Reservations
      9. View All Memberships
      10. Logout

  MANAGE BOOKS:
    - Add a new book (Title, Author, Category, ISBN,
      Publisher, Year of Publication, Number of Copies).
    - Remove a book by title.
    - View the full book catalog.

  MANAGE TABLES:
    - Add a new table (Location, Capacity).
    - Remove a table by ID.
    - View all tables.
    - View tables available for a given date and time slot.

================================================================
C++ CONCEPTS DEMONSTRATED
================================================================

CONCEPT                   | WHERE IT IS USED
--------------------------|-----------------------------------------------------
Classes & Objects         | All files — Book, User, library, Transaction, etc.
                          | every entity in the system is a class.

Constructors /            | Book.h/cpp, User.h/cpp, Transaction.h/cpp,
Destructors               | Membership.h/cpp — default, parameterized, and
                          | restore constructors are all used. Destructors
                          | handle cleanup throughout.

Inheritance               | User.h — Student, Faculty, and Guest all inherit
                          | from the abstract base class User.
                          | librarian inherits no base but manages library.

Polymorphism              | User.h/cpp — virtual functions getborrowlimit(),
(Runtime)                 | getFineRate(), get_usertype(), printTo() are
                          | overridden in Student, Faculty, and Guest.

Abstract Classes /        | User is an abstract class with pure virtual
Pure Virtual Functions    | functions (= 0). It cannot be instantiated directly.

Operator Overloading      | Book.h — operator== overloaded for int, string,
                          | and Book comparisons.
                          | User.h, library.h, Transaction.h, Membership.h —
                          | operator<< overloaded for formatted output.

Static Members            | User.h — static int next_id and static setNextID()
                          | Book.h — static int next_id and getNextID()
                          | Membership.h — static constants FINE_DISCOUNT and
                          | DEFAULT_DURATION_DAYS
                          | RuntimeLogger in Globals.h — static flag and methods

Custom Exception Handling | Globals.h — six custom exception classes:
                          | FileException, BookException, UserException,
                          | AuthException, ReservationException,
                          | TransactionException. All caught in main.cpp and
                          | library.cpp using try-catch blocks.

Vectors (STL)             | library.h — vectors store users, transactions,
                          | fines, reservations, and tables dynamically.
                          | BookCatalog.h — vector<Book*> holds the catalog.

Pointers                  | User.h — membership stored as Membership*
                          | library.h — users stored as vector<User*>
                          | Authentication.cpp — loginUser returns User*

File I/O (Binary)         | Globals.h defines flat structs (UserRecord,
                          | BookRecord, etc.) for binary file reading/writing.
                          | Every class has save/load/rewrite functions in its
                          | respective .cpp file.

Friend Functions          | User.h, Book.h, Transaction.h, Membership.h,
                          | library.h — friend ostream& operator<< lets
                          | cout print private members cleanly.

Encapsulation             | Every class uses private data members with
                          | public getters and setters throughout all files.

Input Validation          | Globals.h/cpp — getValidDate(), getValidTime(),
                          | getValidEmail(), getValidCNIC(), getValidPhone(),
                          | getValidPassword(), getValidIntInRange(), etc.

Time Handling (ctime)     | Globals.cpp, Transaction.cpp, Membership.cpp —
                          | time_t, getCurrentTime(), timeToString(),
                          | addMinutes(), minutesDiff(), and
                          | reservationToTimeT() handle real-time logic.

Composition               | library class contains a BookCatalog object,
                          | vector of Tables, Transactions, Fines, etc.
                          | User class contains a Membership pointer.

Separation of Concerns    | Project is split into header (.h) and
                          | implementation (.cpp) files for each module:
                          | Book, User, library, Transaction, Membership,
                          | Globals, and main.

================================================================
WHAT WORKS / WHAT DOES NOT
================================================================

WORKS:
  - User registration (Student, Faculty, Guest)
  - Login and logout for both users and librarian
  - Borrowing, returning, downloading, and reading books on-site
  - Overdue detection and automatic fine calculation
  - Membership activation, cancellation, borrow limit boost,
    and 15% fine discount
  - Book reservations and table reservations (manual and auto)
  - Auto-expiry of past reservations on program start
  - Librarian adding and removing books and tables
  - All data persists between runs via binary file I/O
  - Input validation on all fields (CNIC, email, phone, date,
    time, password, integers, strings)
  - Runtime logger suppresses stray messages during file loading
  - All six custom exceptions are thrown and caught correctly

DOES NOT WORK / KNOWN LIMITATIONS:
  - Nothing significant is broken. All core features are
    functional and tested.
  - The program currently supports only one librarian account
    (hardcoded admin). Multiple librarian accounts are not
    supported.
  - There is no search by ISBN in the user search menu
    (search covers title, author, category, publisher, year).

================================================================
REFERENCES / RESOURCES USED
================================================================

  1. cplusplus.com          — C++ standard library reference,
                              file I/O, time functions, vectors,
                              and exception handling:
                              https://cplusplus.com

  2. Code With Harry         — OOP in C++ tutorials, inheritance,
                              polymorphism, and virtual functions:
                              https://www.codewithharry.com

  3. Coder Army              — Data structures, STL vectors,
                              and file handling in C++:
                              https://www.coderarmy.in

================================================================
                        END OF README
================================================================
