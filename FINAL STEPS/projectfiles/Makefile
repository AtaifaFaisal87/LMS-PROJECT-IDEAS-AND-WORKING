# Makefile for Library Management System

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall

# Target executable name
TARGET = library_system

# Source files
SOURCES = main.cpp

# Header files
HEADERS = Library.h

# Default target
all: $(TARGET)

# Build the executable
$(TARGET): $(SOURCES) $(HEADERS)
	@echo "🔨 Compiling Library Management System..."
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCES)
	@echo "✅ Compilation successful!"
	@echo "📦 Executable created: $(TARGET)"
	@echo ""
	@echo "To run the program, type: ./$(TARGET)"

# Clean compiled files
clean:
	@echo "🧹 Cleaning up..."
	rm -f $(TARGET) *.o *.gch
	@echo "✅ Clean complete!"

# Run the program
run: $(TARGET)
	@echo "🚀 Running Library Management System..."
	@echo ""
	./$(TARGET)

# Help target
help:
	@echo "Library Management System - Makefile Help"
	@echo "=========================================="
	@echo "Available targets:"
	@echo "  make          - Compile the project"
	@echo "  make all      - Same as 'make'"
	@echo "  make clean    - Remove compiled files"
	@echo "  make run      - Compile and run the program"
	@echo "  make help     - Show this help message"
	@echo ""
	@echo "Default login credentials:"
	@echo "  Librarian - Username: admin, Password: 123"

.PHONY: all clean run help
