# 📚 Library Management System

A comprehensive C++ Library Management System with user authentication, book management, borrowing system, fines, memberships, and table reservations.

## 📁 File Structure

```
LibraryManagementSystem/
├── Library.h          # All class declarations and implementations
├── main.cpp           # Main program entry point
├── Makefile           # Build automation (optional)
├── README.md          # This file
└── COMPILATION.md     # Detailed compilation guide
```

## ⚡ Quick Start

### Option 1: Using Make (Recommended)
```bash
make
./library_system
```

### Option 2: Manual Compilation
```bash
g++ -o library_system main.cpp -std=c++11
./library_system
```

### Option 3: Windows (MinGW/MSYS2)
```bash
g++ -o library_system.exe main.cpp -std=c++11
library_system.exe
```

## 🔑 Default Credentials

**Librarian Login:**
- Username: `admin`
- Password: `123`

## 🎯 Features

### User Management
- ✅ User Registration (Student, Faculty, Guest)
- ✅ User Authentication
- ✅ Profile Management
- ✅ Membership System

### Book Management  
- ✅ Add/Remove Books
- ✅ Search Books (by Title, Author, Category, ISBN)
- ✅ View Complete Catalog
- ✅ Track Book Availability

### Borrowing System
- ✅ Borrow Books with Real-time Due Dates
- ✅ Return Books
- ✅ View Borrowing History
- ✅ User-specific Borrow Limits

### Fine System
- ✅ Automatic Fine Calculation
- ✅ Membership Discounts (15% off)
- ✅ Fine Payment Processing
- ✅ Overdue Book Tracking

### Table Reservation
- ✅ Reserve Study Tables
- ✅ View Available Tables
- ✅ Time-slot Management
- ✅ Cancellation Support

### Data Persistence
- ✅ All data saved to binary files
- ✅ Automatic load on startup
- ✅ Crash-safe file operations

## 📊 System Limits

| User Type | Borrow Limit | With Membership |
|-----------|--------------|-----------------|
| Student   | 3 books      | 5 books         |
| Faculty   | 5 books      | 7 books         |
| Guest     | 1 book       | 2 books         |

## 🗂️ Data Files Created

The system automatically creates and manages these files:

- `users.dat` - User account information
- `books.dat` - Book catalog
- `transactions.dat` - Borrowing records
- `fines.dat` - Fine records
- `tables.dat` - Table information

**Note:** On first run, you may see "File not found" warnings - this is normal. The files will be created automatically.

## 🔧 System Requirements

- **Compiler:** g++ with C++11 support or later
- **OS:** Linux, macOS, or Windows (with MinGW/Cygwin)
- **RAM:** Minimum 512MB
- **Disk Space:** 10MB for program and data files

## 📖 Usage Guide

### 1. Starting the System
```bash
./library_system
```

You'll see the main menu:
```
================ LIBRARY SYSTEM ================
1. REGISTER USER
2. LOGIN AS USER
3. LOGIN AS LIBRARIAN
4. EXIT
```

### 2. Register a New User
- Choose option 1
- Select user type (Student/Faculty/Guest)
- Enter required information
- Set username and password

### 3. Login as User
- Choose option 2
- Enter your username and password
- Access user menu to:
  - Borrow books
  - Return books
  - Purchase membership
  - Reserve tables
  - View your account

### 4. Login as Librarian
- Choose option 3
- Use credentials: `admin` / `123`
- Access librarian menu to:
  - Add/remove books
  - Manage users
  - View all transactions
  - Manage tables
  - View system statistics

## 🐛 Troubleshooting

### Compilation Errors

**Error: `g++: command not found`**
```bash
# Ubuntu/Debian
sudo apt-get install g++

# macOS
xcode-select --install

# Windows
# Install MinGW from mingw-w64.org
```

**Error: `undefined reference to...`**
```bash
# Make sure you're compiling main.cpp (NOT Library.h)
g++ -o library_system main.cpp -std=c++11
```

**Error: `No such file or directory`**
```bash
# Make sure both files are in the same directory
ls -l Library.h main.cpp
```

### Runtime Errors

**File Permission Error**
```bash
# Make sure you have write permissions in the current directory
chmod +w .
```

**Segmentation Fault**
```bash
# This usually means corrupted data files
# Delete all .dat files and restart
rm -f *.dat
./library_system
```

## 🔒 Security Notes

- Passwords are stored in plain text (for educational purposes only)
- For production use, implement proper password hashing
- File operations have basic error handling

## 📝 Development

### Code Organization

**Library.h** contains:
- All exception classes
- All data structures
- All utility functions
- All class declarations and implementations

**main.cpp** contains:
- Main program entry point
- System initialization
- Main menu loop

### Why This Structure?

This project was originally a single large file. It has been split into:
- **Library.h:** All reusable code (classes, functions, utilities)
- **main.cpp:** The application entry point

This makes it easier to:
- Understand the code organization
- Compile the project correctly
- Maintain and modify the code

## 📚 Additional Features

### Membership Benefits
- 15% discount on fines
- Increased borrow limits
- 365-day validity
- Automatic expiry checking

### Real-time Features
- Live due date calculation
- Automatic fine computation
- Overdue book detection
- Availability tracking

## 🎓 Educational Purpose

This system demonstrates:
- Object-Oriented Programming (OOP) in C++
- File I/O operations
- Exception handling
- Data structures (vectors, arrays)
- User authentication
- Business logic implementation

## 🤝 Support

If you encounter issues:

1. **Check the error message** - Most errors are self-explanatory
2. **Verify file permissions** - Make sure you can write to the directory
3. **Check compiler version** - Ensure C++11 support
4. **Delete data files** - If you suspect corruption
5. **Recompile** - Sometimes a clean compile fixes issues

## 📄 License

This is an educational project. Feel free to use and modify for learning purposes.

---

**Enjoy your Library Management System! 📖✨**

For detailed compilation instructions, see [COMPILATION.md](COMPILATION.md)
