# Library Management System — CEP Project
## CIS | G1-5 | Section: A

---

## Group Members

| Name | Roll Number |
|------|-------------|
| Abdur Rehman | CS-25032 |
| Hafsa Shakeel | CS-25011 |
| Ataifa Faisal | CS-25021 |
| Yazdaan Ali Mirza | CS-25030 |

---

## Steps to Run the Program

### Step 1 — Compile the Program

Open a terminal (Command Prompt or PowerShell) in the folder that contains all the `.cpp` and `.h` files, then run:

```bash
g++ -o library_system main.cpp Utils.cpp Library.cpp User.cpp Librarian.cpp Authentication.cpp FileIO.cpp Fines.cpp Statics.cpp
```

This compiles all source files and produces `library.exe`.

### Step 2 — Run the Executable

After compilation, run:

```bash
./library_system.exe
```

The system will automatically load saved data from files (users, books, transactions, fines, tables, reservations). If no data files exist yet, the system starts fresh with no entries — this is normal on first run.

---

## Main Menu

```
================ LIBRARY SYSTEM ================
1. REGISTER USER
2. LOGIN AS USER
3. LOGIN AS LIBRARIAN
4. EXIT
```

---

## Option 1 — Register a New User

- Choose option 1 from the main menu.
- Select user type: **Student**, **Faculty**, or **Guest**.
- Enter your details when prompted:
  - Name, CNIC, Email, Phone Number
  - Username (must be unique), Password
  - **Student:** also enter Seat No. and Major
  - **Faculty:** also enter Department and Designation
  - **Guest:** also enter Purpose of Visit
- On success, your account is saved and you can now log in.

---

## Option 2 — Login as User

- Choose option 2 from the main menu.
- Enter your username and password.
- On successful login, you reach the **User Menu**:

```
USER MENU OPTIONS:
1. View My Details
2. Search Books
3. Borrow / Return / Download / Read On-Site
4. Reservation Menu
5. Membership Menu
6. View My Transactions
7. View My Fines
8. Logout
```

### Borrowing a Book
- Select option 3 (Book Actions).
- Choose "Borrow a Book".
- Enter the book title and desired borrow duration (minutes).
- You must not have overdue books to borrow a new one.
- Borrow limits: Student = 3, Faculty = 5, Guest = 1
  - Active membership adds +2 to the limit.

### Returning a Book
- Select option 3 (Book Actions) → "Return a Book".
- Enter the title of the book you want to return.
- If returned late, a fine is automatically calculated and saved.

### Reserving a Book / Table
- Select option 4 (Reservation Menu).
- You can reserve a specific book, reserve a specific table, or let the system auto-assign an available table.
- Enter date (DD/MM/YYYY), start time and end time (HH:MM).

### Membership
- Select option 5 (Membership Menu).
- Activate a 1-year membership to get +2 borrow limit and 15% discount on fines.
- You can also view or cancel your membership here.

---

## Option 3 — Login as Librarian

**Librarian Credentials:**
- **Username:** `admin`
- **Password:** `123`

On login you reach the **Librarian Main Menu**:

```
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
```

### Manage Books
- Add a new book (Title, Author, Category, ISBN, Publisher, Year of Publication, Number of Copies).
- Remove a book by title.
- View the full book catalog.

### Manage Tables
- Add a new table (Location, Capacity).
- Remove a table by ID.
- View all tables.
- View tables available for a given date and time slot.

### Manage Users
- View all users in the system.
- View users filtered by type (Student, Faculty, Guest).
- Delete a user by ID (soft delete — user data is preserved but marked as deleted).
- A user with unreturned books cannot be deleted until all books are returned.

---

## C++ Concepts Demonstrated
1. Classes & Objects
   All files — Book, User, library, Transaction, etc. Every entity in the
   system is a class.

2. Constructors / Destructors
   Book.h/cpp, User.h/cpp, Transaction.h/cpp, Membership.h/cpp — default,
   parameterized, and restore constructors are all used. Destructors handle
   cleanup throughout.

3. Inheritance
   User.h — Student, Faculty, and Guest all inherit from the abstract base
   class User.
...
---

## What Works / What Does Not

### Works
- User registration (Student, Faculty, Guest)
- Login and logout for both users and librarian
- Borrowing, returning, downloading, and reading books on-site
- Overdue detection and automatic fine calculation
- Membership activation, cancellation, borrow limit boost, and 15% fine discount
- Book reservations and table reservations (manual and auto)
- Auto-expiry of past reservations on program start
- Librarian adding and removing books and tables
- All data persists between runs via binary file I/O
- Input validation on all fields (CNIC, email, phone, date, time, password, integers, strings)
- Runtime logger suppresses stray messages during file loading
- All six custom exceptions are thrown and caught correctly

### Known Limitations
- The program currently supports only one librarian account (hardcoded admin). Multiple librarian accounts are not supported.
- There is no search by ISBN in the user search menu (search covers title, author, category, publisher, year).

---

## References / Resources Used

1. **cplusplus.com** — C++ standard library reference, file I/O, time functions, vectors, and exception handling: https://cplusplus.com
2. **Code With Harry** — OOP in C++ tutorials, inheritance, polymorphism, and virtual functions: https://www.codewithharry.com
3. **Coder Army** — Data structures, STL vectors, and file handling in C++: https://www.coderarmy.in

---

========================================================
                HEADER FILES (.h)
========================================================

1. Constants.h
   Global constants:
   - MAX_TABLES
   - Fine limits
   - Membership defaults

2. Exceptions.h
   Custom exception classes:
   - FileException
   - BookException
   - UserException
   - AuthException
   - ReservationException
   - TransactionException

3. Structs.h
   Binary file I/O structs:
   - UserRecord
   - BookRecord
   - TransactionRecord
   - FineRecord
   - TableRecord
   - ReservationRecord

4. Utils.h
   Utility functions:
   - Input validation
   - RuntimeLogger
   - Time helper functions

5. Membership.h
   Membership class:
   - 15% fine discount
   - Borrow limit boosts

6. Book.h
   Book class:
   - Download counter
   - Read-onsite counter
   - Borrow counter
   - Availability tracking

7. BookCatalog.h
   Book catalog container:
   - Search by ID
   - Search by title
   - Search by author
   - Search by category
   - Search by publisher
   - Search by year

8. Transaction.h
   Transaction class:
   - Borrow time
   - Due time
   - Return time
   - Overdue minute tracking

9. Fines.h
   Fines class:
   - Overdue fine calculation
   - Membership discount support

10. Reservation.h
    Reservation class:
    - Table reservations
    - Book reservations
    - Time overlap detection

11. Table.h
    Table management class for library seating.

12. User.h
    Abstract User base class with derived classes:
    - Student
    - Faculty
    - Guest

13. Librarian.h
    Librarian class:
    - Admin credentials
    - Menu forward declarations

14. Authentication.h
    Login and logout authentication system.

15. Library.h
    Main Library class connecting all components.

16. FileIO.h
    Binary file save/load function declarations.


========================================================
             IMPLEMENTATION FILES (.cpp)
========================================================

1. main.cpp
   - Program entry point
   - Main menu loop
   - File loading
   - Librarian setup

2. Utils.cpp
   - Input validation functions
   - Time conversion utilities

3. Library.cpp
   Library class methods:
   - Borrow books
   - Return books
   - Reservations
   - Table management
   - User management
   - System views

4. User.cpp
   User menu implementations:
   - Dashboard
   - Search
   - Borrow/Return
   - Reservations
   - Membership
   - Profile update

5. Librarian.cpp
   Librarian menus:
   - View system
   - Manage books
   - Manage tables
   - Manage users

6. Authentication.cpp
   - User login
   - Librarian login

7. FileIO.cpp
   Binary file operations:
   - Users
   - Books
   - Transactions
   - Fines
   - Reservations
   - Tables

8. Fines.cpp
   Fines constructor:
   - Membership-aware discounts

9. Statics.cpp
   Static member initializations:
   - Book::next_id
   - Transaction::next_id
   - Reservation::next_id
   - Table::next_id
   - Membership::next_id

========================================================
> **End of README**
