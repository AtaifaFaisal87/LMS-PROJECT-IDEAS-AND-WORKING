# 🚀 QUICK START GUIDE

## 📥 You Have 5 Files:

1. **Library.h** - All classes and functions (DON'T compile this directly!)
2. **main.cpp** - Main program entry point
3. **Makefile** - Build automation
4. **README.md** - Complete documentation
5. **COMPILATION.md** - Detailed compilation guide

---

## ⚡ Compile & Run (Choose ONE method):

### ✅ METHOD 1: Using Make (EASIEST)
```bash
make
./library_system
```

### ✅ METHOD 2: Manual Compilation
```bash
g++ -o library_system main.cpp -std=c++11
./library_system
```

### ✅ METHOD 3: Windows (MinGW)
```cmd
g++ -o library_system.exe main.cpp -std=c++11
library_system.exe
```

---

## 🔑 Default Login

**Librarian:**
- Username: `admin`
- Password: `123`

---

## ❌ Common Mistakes

**DON'T compile Library.h:**
```bash
g++ -o library Library.h main.cpp  # ❌ WRONG
```

**DO compile main.cpp:**
```bash
g++ -o library_system main.cpp -std=c++11  # ✅ CORRECT
```

---

## 🆘 Having Errors?

### Error: "g++ command not found"
```bash
# Install g++:
sudo apt-get install g++  # Linux
xcode-select --install     # macOS
# Download MinGW for Windows
```

### Error: "undefined reference"
Make sure you're compiling `main.cpp`, NOT `Library.h`!

### Error: "No such file"
Make sure both `Library.h` and `main.cpp` are in the same folder!

---

## 📚 What Each File Does:

- **Library.h** → Contains ALL the code (classes, functions, etc.)
- **main.cpp** → Says "#include Library.h" and runs main()
- **Makefile** → Automates the compilation process
- **README.md** → Full documentation
- **COMPILATION.md** → Detailed help for compilation issues

---

## 💡 Why Only Compile main.cpp?

Because `main.cpp` includes `Library.h` at the top:
```cpp
#include "Library.h"  // ← This brings in ALL the code
```

So when you compile `main.cpp`, the compiler automatically:
1. Opens `Library.h`
2. Copies all its content
3. Compiles everything together

That's why you only need: `g++ main.cpp`

---

## 🎯 That's It!

Just run:
```bash
g++ -o library_system main.cpp -std=c++11
./library_system
```

For more help, read **COMPILATION.md** or **README.md**

---

**Happy Coding! 📖✨**
