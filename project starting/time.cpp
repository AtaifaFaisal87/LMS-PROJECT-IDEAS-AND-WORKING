#include <iostream>
#include <ctime>
using namespace std;

class LibraryUser {
private:
    bool hasBook;
    time_t borrowTime;

public:
    LibraryUser() {
        hasBook = false;
        borrowTime = 0;
    }

    void borrowBook() {
        if (hasBook) {
            time_t currentTime = time(nullptr);
            double secondsPassed = difftime(currentTime, borrowTime);

            if (secondsPassed > 30) {
                double fine = calculateFine(secondsPassed);
                cout << "WARNING! You kept the book for " << secondsPassed << " seconds.\n";
                cout << "Your fine is: " << fine << " PKR\n";
            } else {
                cout << "You already have a book! Return it first.\n";
            }
        } else {
            borrowTime = time(nullptr);
            hasBook = true;
            cout << "Book borrowed at: " << ctime(&borrowTime);
        }
    }

    void returnBook() {
        if (!hasBook) {
            cout << "No book to return!\n";
            return;
        }

        time_t currentTime = time(nullptr);
        double secondsPassed = difftime(currentTime, borrowTime);

        if (secondsPassed > 30) {
            double fine = calculateFine(secondsPassed);
            cout << "Book returned.\n";
            cout << "Late by " << secondsPassed << " seconds.\n";
            cout << "Fine: " << fine << " PKR\n";
        } else {
            cout << "Book returned on time. No fine!\n";
        }

        hasBook = false;
    }

    double calculateFine(double seconds) {
        int intervals = seconds / 30; // how many 30-sec blocks

        double fine = 10; // first fine

        for (int i = 1; i < intervals; i++) {
            fine *= 2; // doubling
        }

        return fine;
    }
};

int main() {
    LibraryUser user;
    int choice;

    while (true) {
        cout << "\n--- Library System ---\n";
        cout << "1. Borrow\n2. Return\n3. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        if (choice == 1) {
            user.borrowBook();
        }
        else if (choice == 2) {
            user.returnBook();
        }
        else {
            break;
        }
    }

    return 0;
}