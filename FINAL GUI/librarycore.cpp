#include "librarycore.h"
#include <QFile>
#include <QDataStream>

int Membership::next_id = 1;
const double Membership::FINE_DISCOUNT = 0.15;
const int Membership::DEFAULT_DURATION_DAYS = 365;
int User::next_id = 1;
const double User::MAX_UNPAID_FINE_LIMIT = 500.0;
int Book::next_id = 1;
int Transaction::next_id = 1;
int Reservation::next_id = 1;
int Table::next_id = 1;

Membership::Membership()
    : membershipID(next_id++), userID(0), userType("NONE"), userName("NONE"), active(false) {}

Membership::Membership(int uid, const QString& utype, const QString& uname, int durationDays)
    : membershipID(next_id++), userID(uid), userType(utype), userName(uname), active(true)
{
    activationTime = QDateTime::currentDateTime();
    expiryTime = activationTime.addDays(durationDays);
}

Membership::Membership(int uid, const QString& utype, const QString& uname,
                       const QDateTime& activation, const QDateTime& expiry)
    : membershipID(next_id++), userID(uid), userType(utype), userName(uname),
      active(true), activationTime(activation), expiryTime(expiry) {}

bool Membership::isValid() const { return active && (QDateTime::currentDateTime() <= expiryTime); }
double Membership::applyDiscount(double originalFine) const { return isValid() ? originalFine * 0.85 : originalFine; }
int Membership::getBorrowBoost() const {
    if (!isValid()) return 0;
    if (userType == "STUDENT") return 2;
    if (userType == "FACULTY") return 2;
    if (userType == "GUEST") return 1;
    return 0;
}
void Membership::cancel() { active = false; }

User::User(const QString& n, const QString& c, const QString& e, const QString& p)
    : userid(next_id++), name(n), CNIC(c), email(e), phonenumber(p),
      membership(nullptr), daily_borrows(0), last_borrow_date(""),
      balance(0.0), total_fines_paid(0.0), isDeleted(false) {}

User::~User() { delete membership; }
bool User::hasMembership() const { return membership != nullptr && membership->isValid(); }
void User::activateMembership(int durationDays) {
    if (hasMembership()) return;
    delete membership;
    membership = new Membership(userid, get_usertype(), name, durationDays);
}
void User::cancelMembership() { if (membership) membership->cancel(); }
void User::viewMembership() {}
void User::restoreMembership(const QDateTime& a, const QDateTime& e) {
    delete membership;
    membership = new Membership(userid, get_usertype(), name, a, e);
}
int User::getEffectiveBorrowLimit() const { return getborrowlimit() + (hasMembership() ? membership->getBorrowBoost() : 0); }
bool User::canBorrowToday() {
    QString today = getTodayDateString();
    if (last_borrow_date != today) { daily_borrows = 0; last_borrow_date = today; }
    return daily_borrows < getEffectiveBorrowLimit();
}
void User::recordDailyBorrow() {
    QString today = getTodayDateString();
    if (last_borrow_date != today) { daily_borrows = 0; last_borrow_date = today; }
    daily_borrows++;
}
double User::applyMembershipDiscount(double fine) { return hasMembership() ? membership->applyDiscount(fine) : fine; }
bool User::borrowbook(int id) {
    if (borrowedbook.size() < getEffectiveBorrowLimit()) { borrowedbook.push_back(id); return true; }
    return false;
}
bool User::returnbook(int id) {
    for (int i = 0; i < borrowedbook.size(); i++) {
        if (borrowedbook[i] == id) { borrowedbook.erase(borrowedbook.begin() + i); return true; }
    }
    return false;
}
bool User::deductBalance(double amount) {
    total_fines_paid += amount;
    if (balance >= amount) { balance -= amount; return true; }
    balance -= amount; return false;
}
void User::addBalance(double amount) { balance += amount; }

double User::calculateTotalUnpaidFines(Library* lib) {
    double total = 0.0;
    for (int i = 0; i < lib->getTransactionsVector().size(); i++) {
        Transaction& t = lib->getTransactionsVector()[i];
        if (t.getuserid() == userid && !t.isReturned()) total += lib->calculateLiveFine(t, this);
    }
    for (int i = 0; i < lib->getFinesVector().size(); i++) {
        if (lib->getFinesVector()[i].getUserID() == userid) total += lib->getFinesVector()[i].getAmount();
    }
    return total;
}

Student::Student(const QString& n, const QString& c, const QString& e,
                 const QString& p, const QString& s, const QString& m)
    : User(n, c, e, p), seatno(s), major(m) {}

Faculty::Faculty(const QString& n, const QString& c, const QString& e,
                 const QString& p, const QString& dept, const QString& des)
    : User(n, c, e, p), department(dept), designation(des) {}

Guest::Guest(const QString& n, const QString& c, const QString& e,
             const QString& p, const QString& ps)
    : User(n, c, e, p), purpose(ps) {}

Book::Book()
    : bookid(next_id++), title("UNKNOWN"), author("UNKNOWN"), category("UNKNOWN"),
      ISBN("UNKNOWN"), publisher("UNKNOWN"), year_of_publication(0),
      available_copies(0), availability_status(false),
      download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

Book::Book(const QString& t, const QString& a, const QString& c,
           const QString& isbn, const QString& p, int yop)
    : bookid(next_id++), title(t), author(a), category(c),
      ISBN(isbn), publisher(p), year_of_publication(yop),
      available_copies(0), availability_status(false),
      download_count(0), read_onsite_count(0), timesborrowed_count(0) {}

void Book::setAvailableCopies(int ac) { available_copies = ac; availability_status = (available_copies > 0); }
void Book::incrementBorrowCount() { if (available_copies >= 0) timesborrowed_count++; }
void Book::incrementDownloadCount() { download_count++; }
void Book::incrementReadOnSiteCount() { read_onsite_count++; }

BookCatalog::~BookCatalog() { for (Book* b : BK) delete b; BK.clear(); }
void BookCatalog::addbook(Book* b) { BK.push_back(b); }
void BookCatalog::removeBook(const QString& t) {
    for (int i = 0; i < BK.size(); i++) {
        if (BK[i]->get_title() == t) { delete BK[i]; BK.erase(BK.begin() + i); return; }
    }
}
Book* BookCatalog::SearchbyID(int id) { for (Book* b : BK) if (b->get_bookid() == id) return b; return nullptr; }
Book* BookCatalog::SearchbyTitle(const QString& t) { for (Book* b : BK) if (b->get_title() == t) return b; return nullptr; }
QVector<Book*> BookCatalog::SearchbyAuthor(const QString& a) { QVector<Book*> r; for (Book* b : BK) if (b->get_author() == a) r.push_back(b); return r; }
QVector<Book*> BookCatalog::SearchbyCategory(const QString& c) { QVector<Book*> r; for (Book* b : BK) if (b->get_category() == c) r.push_back(b); return r; }
QVector<Book*> BookCatalog::SearchbyPublisher(const QString& p) { QVector<Book*> r; for (Book* b : BK) if (b->get_publisher() == p) r.push_back(b); return r; }
QVector<Book*> BookCatalog::SearchbyYOP(int yop) { QVector<Book*> r; for (Book* b : BK) if (b->get_YOP() == yop) r.push_back(b); return r; }
void BookCatalog::ViewBookCatalog() const {}
Book* BookCatalog::getBookAtIndex(int index) { if (index >= 0 && index < BK.size()) return BK[index]; return nullptr; }

Transaction::Transaction()
    : transaction_id(next_id++), user_id(0), book_id(0), return_date("NOT RETURNED"), returned(false) {}

Transaction::Transaction(int uid, int bid, int duration_minutes)
    : transaction_id(next_id++), user_id(uid), book_id(bid),
      return_date("NOT RETURNED"), returned(false)
{
    borrow_time = QDateTime::currentDateTime();
    due_time = borrow_time.addSecs(duration_minutes * 60);
}

int Transaction::getOverdueMinutes() const {
    if (!returned || !return_time.isValid() || return_time <= due_time) return 0;
    return minutesDiff(due_time, return_time);
}
int Transaction::markreturned() {
    returned = true;
    return_time = QDateTime::currentDateTime();
    return_date = return_time.toString("yyyy-MM-dd hh:mm:ss");
    return getOverdueMinutes();
}

Fines::Fines() : user_id(0), book_id(0), minuteslate(0), rate(0), originalAmount(0), finalAmount(0), discountApplied(false) {}
Fines::Fines(int u, int b, int mins, User* user) : user_id(u), book_id(b) {
    minuteslate = (mins < 0) ? 0 : mins;
    rate = user->getFineRate();
    originalAmount = rate * minuteslate;
    if (user->hasMembership()) { finalAmount = user->applyMembershipDiscount(originalAmount); discountApplied = true; }
    else { finalAmount = originalAmount; discountApplied = false; }
}

Reservation::Reservation()
    : reservationid(next_id++), userid(0), bookid(0), tableid(0),
      date("UNKNOWN"), startTime("0"), endTime("0"), active(true) {}
Reservation::Reservation(int uid, int bid, int tid, const QString& d,
                         const QString& start, const QString& end)
    : reservationid(next_id++), userid(uid), bookid(bid), tableid(tid),
      date(d), startTime(start), endTime(end), active(true) {}
bool Reservation::isExpired() const { return QDateTime::currentDateTime() > reservationToDateTime(date, endTime); }
bool Reservation::overlap(const QString& s1, const QString& e1, const QString& s2, const QString& e2) { return !(e1 <= s2 || e2 <= s1); }

Table::Table() : tableid(next_id++), location("UNKNOWN"), capacity(1) {}
Table::Table(const QString& loc, int cap) : tableid(next_id++), location(loc), capacity(cap) {}

librarian::librarian() : librarian_id(0), name("UNKNOWN"), email("UNKNOWN"), phone_no("UNKNOWN") {}
librarian::librarian(int id, const QString& n, const QString& e, const QString& p) : librarian_id(id), name(n), email(e), phone_no(p) {}
void librarian::setCredentials(const QString& u, const QString& p) { username = u; password = p; }

Library::~Library() { for (User* u : users) delete u; users.clear(); }

User* Library::getUserByIndex(int index) {
    if (index >= 0 && index < users.size()) return users[index];
    return nullptr;
}

int Library::getActiveUserCount() const {
    int count = 0;
    for (User* u : users) if (!u->getIsDeleted()) count++;
    return count;
}

void Library::registerUserInLibrary(User* newUser) {
    for (User* u : users) {
        if (u->getIsDeleted()) continue;
        if (u->get_cnic() == newUser->get_cnic()) throw UserException("CNIC ALREADY EXISTS.");
        if (u->get_email() == newUser->get_email()) throw UserException("EMAIL ALREADY EXISTS.");
        if (u->get_phone() == newUser->get_phone()) throw UserException("PHONE ALREADY EXISTS.");
        if (u->getUsername() == newUser->getUsername()) throw UserException("USERNAME ALREADY EXISTS.");
    }
    users.push_back(newUser);
}

bool Library::deleteUser(int uid) {
    int idx = -1;
    for (int i = 0; i < users.size(); i++) { if (users[i]->get_userid() == uid) { idx = i; break; } }
    if (idx == -1) return false;
    User* u = users[idx];
    if (u->getIsDeleted()) return false;
    if (u->getBorrowedCount() > 0) return false;
    u->setIsDeleted(true);
    for (int i = 0; i < reservations.size(); i++)
        if (reservations[i].get_userid() == uid && reservations[i].isActive()) reservations[i].cancel();
    return true;
}

bool Library::readOnSite(User* u, const QString& title) {
    Book* book = catalog.SearchbyTitle(title);
    if (!book) throw BookException("BOOK NOT FOUND: " + title);
    book->incrementReadOnSiteCount(); return true;
}

bool Library::downloadBook(User* u, const QString& title) {
    Book* book = catalog.SearchbyTitle(title);
    if (!book) throw BookException("BOOK NOT FOUND: " + title);
    book->incrementDownloadCount(); return true;
}

bool Library::hasOverdueBooks(User* u) const {
    QDateTime now = QDateTime::currentDateTime();
    for (const Transaction& t : transactions) {
        if (t.getuserid() == u->get_userid() && !t.isReturned() && now > t.getDueTime()) return true;
    }
    return false;
}

bool Library::borrowBook(User* u, const QString& title, int duration_minutes) {
    if (hasOverdueBooks(u)) throw TransactionException("YOU HAVE OVERDUE BOOKS! RETURN THEM FIRST.");
    if (!u->canBorrowBasedOnBalance()) throw UserException("YOUR BALANCE IS TOO LOW!");
    if (!u->canBorrowToday()) throw UserException("DAILY BORROW LIMIT REACHED!");
    Book* book = catalog.SearchbyTitle(title);
    if (!book) throw BookException("BOOK NOT FOUND: " + title);
    if (book->getAvailableCopies() == 0) throw BookException("NO COPIES AVAILABLE.");
    if (!u->borrowbook(book->get_bookid())) throw UserException("BORROW LIMIT REACHED!");
    book->setAvailableCopies(book->getAvailableCopies() - 1);
    book->incrementBorrowCount(); u->recordDailyBorrow();
    transactions.append(Transaction(u->get_userid(), book->get_bookid(), duration_minutes));
    return true;
}

bool Library::returnBook(User* u, const QString& title) {
    Book* book = catalog.SearchbyTitle(title);
    if (!book) throw BookException("BOOK NOT FOUND: " + title);
    if (!u->returnbook(book->get_bookid())) throw TransactionException("YOU HAVE NOT BORROWED THIS BOOK.");
    book->setAvailableCopies(book->getAvailableCopies() + 1);
    for (int i = 0; i < transactions.size(); i++) {
        if (transactions[i].getbookid() == book->get_bookid() &&
            transactions[i].getuserid() == u->get_userid() && !transactions[i].isReturned()) {
            int overdue_mins = transactions[i].markreturned();
            if (overdue_mins > 0) {
                fines.append(Fines(u->get_userid(), book->get_bookid(), overdue_mins, u));
                double originalFine = u->getFineRate() * overdue_mins;
                double finalFine = u->hasMembership() ? u->applyMembershipDiscount(originalFine) : originalFine;
                u->deductBalance(finalFine);
            }
            break;
        }
    }
    return true;
}

int Library::calculateLiveFine(Transaction& t, User* u) const {
    if (t.isReturned()) return 0;
    QDateTime now = QDateTime::currentDateTime();
    if (now <= t.getDueTime()) return 0;
    return minutesDiff(t.getDueTime(), now) * u->getFineRate();
}

void Library::viewTransactions() {}
void Library::viewFines() {}
void Library::viewUserTransactions(int) {}
void Library::viewUserFines(int) {}
void Library::viewUserReservations(int uid) { autoExpireReservations(); }
void Library::viewAllReservations() { autoExpireReservations(); }
void Library::viewTables() {}
void Library::viewUsers() {}
void Library::viewUsersByType(const QString&) {}
void Library::viewAllMemberships() {}

void Library::reserveBook(int uid, int bkid, const QString& date, const QString& start, const QString& end) {
    Book* book = catalog.SearchbyID(bkid);
    if (!book) throw BookException("BOOK NOT FOUND WITH ID: " + QString::number(bkid));
    if (book->getAvailableCopies() <= 0) throw BookException("NO COPIES AVAILABLE TO RESERVE.");
    reservations.append(Reservation(uid, bkid, 0, date, start, end));
    book->decreaseCopy();
}

void Library::reserveTableManual(int uid, int tid, const QString& date, const QString& start, const QString& end) {
    bool tableExists = false;
    for (const Table& t : tables) if (t.get_id() == tid) { tableExists = true; break; }
    if (!tableExists) throw ReservationException("TABLE ID DOES NOT EXIST!");
    for (const Reservation& r : reservations) {
        if (r.get_tableid() == tid && r.get_date() == date && r.isActive()) {
            if (Reservation::overlap(r.get_start(), r.get_end(), start, end))
                throw ReservationException("TABLE IS ALREADY BOOKED FOR THAT TIME SLOT!");
        }
    }
    reservations.append(Reservation(uid, 0, tid, date, start, end));
}

void Library::reserveTableAuto(int uid, const QString& date, const QString& start, const QString& end) {
    if (tables.isEmpty()) throw ReservationException("NO TABLES EXIST IN THE LIBRARY!");
    for (const Table& table : tables) {
        int tid = table.get_id(); bool conflict = false;
        for (const Reservation& r : reservations) {
            if (r.get_tableid() == tid && r.get_date() == date && r.isActive()) {
                if (Reservation::overlap(r.get_start(), r.get_end(), start, end)) { conflict = true; break; }
            }
        }
        if (!conflict) { reservations.append(Reservation(uid, 0, tid, date, start, end)); return; }
    }
    throw ReservationException("NO TABLES AVAILABLE FOR THE SELECTED TIME SLOT!");
}

void Library::cancelReservation(int rid, int uid) {
    for (int i = 0; i < reservations.size(); i++) {
        if (reservations[i].get_reservationid() == rid) {
            if (reservations[i].get_userid() != uid) throw ReservationException("YOU CAN ONLY CANCEL YOUR OWN RESERVATIONS!");
            reservations[i].cancel(); return;
        }
    }
    throw ReservationException("RESERVATION ID NOT FOUND!");
}

void Library::autoExpireReservations() {
    for (int i = 0; i < reservations.size(); i++) {
        if (reservations[i].isActive() && reservations[i].isExpired()) reservations[i].cancel();
    }
}

void Library::addTable(const QString& loc, int cap) {
    if (tables.size() >= MAX_TABLES) throw ReservationException("TABLE LIMIT REACHED!");
    tables.append(Table(loc, cap));
}

void Library::removeTable(int tid) {
    for (int i = 0; i < tables.size(); i++) {
        if (tables[i].get_id() == tid) { tables.erase(tables.begin() + i); return; }
    }
    throw ReservationException("TABLE ID NOT FOUND!");
}

void Library::viewAvailableTables(const QString&, const QString&, const QString&) {}

bool Library::hasAvailableTable(const QString& date, const QString& start, const QString& end) const {
    for (const Table& table : tables) {
        int tid = table.get_id(); bool conflict = false;
        for (const Reservation& r : reservations) {
            if (r.get_tableid() == tid && r.get_date() == date && r.isActive()) {
                if (Reservation::overlap(r.get_start(), r.get_end(), start, end)) { conflict = true; break; }
            }
        }
        if (!conflict) return true;
    }
    return false;
}

Table Library::getTable(int i) const {
    if (i < 0 || i >= tables.size()) throw ReservationException("TABLE INDEX OUT OF BOUNDS");
    return tables[i];
}

void Library::loadAllData() {
    QFile userFile("users.dat");
    if (userFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&userFile);
        while (!in.atEnd()) {
            int id, borrowed_count, daily_borrows, membershipDurationDays;
            QString name, email, phone, cnic, type, username, password;
            QString seatno, major, department, designation, purpose;
            QString last_borrow_date;
            double balance, total_fines_paid;
            bool isDeleted, hasMembership;
            QDateTime membershipActivation, membershipExpiry;
            QVector<int> borrowed_ids;
            in >> id >> name >> email >> phone >> cnic >> type >> username >> password
               >> isDeleted >> borrowed_count;
            for (int i = 0; i < borrowed_count && i < 10; i++) { int bid; in >> bid; borrowed_ids.append(bid); }
            in >> daily_borrows >> last_borrow_date >> balance >> total_fines_paid >> hasMembership;
            if (hasMembership) { in >> membershipActivation >> membershipExpiry >> membershipDurationDays; }
            if (type == "STUDENT") {
                in >> seatno >> major;
                Student* s = new Student(name, cnic, email, phone, seatno, major);
                s->setid(id); s->setUsername(username); s->setPassword(password);
                s->setDailyBorrows(daily_borrows); s->setLastBorrowDate(last_borrow_date);
                s->setBalance(balance); s->setTotalFinesPaid(total_fines_paid); s->setIsDeleted(isDeleted);
                if (hasMembership) s->restoreMembership(membershipActivation, membershipExpiry);
                for (int bid : borrowed_ids) s->borrowbook(bid);
                users.append(s);
            } else if (type == "FACULTY") {
                in >> department >> designation;
                Faculty* f = new Faculty(name, cnic, email, phone, department, designation);
                f->setid(id); f->setUsername(username); f->setPassword(password);
                f->setDailyBorrows(daily_borrows); f->setLastBorrowDate(last_borrow_date);
                f->setBalance(balance); f->setTotalFinesPaid(total_fines_paid); f->setIsDeleted(isDeleted);
                if (hasMembership) f->restoreMembership(membershipActivation, membershipExpiry);
                for (int bid : borrowed_ids) f->borrowbook(bid);
                users.append(f);
            } else {
                in >> purpose;
                Guest* g = new Guest(name, cnic, email, phone, purpose);
                g->setid(id); g->setUsername(username); g->setPassword(password);
                g->setDailyBorrows(daily_borrows); g->setLastBorrowDate(last_borrow_date);
                g->setBalance(balance); g->setTotalFinesPaid(total_fines_paid); g->setIsDeleted(isDeleted);
                if (hasMembership) g->restoreMembership(membershipActivation, membershipExpiry);
                for (int bid : borrowed_ids) g->borrowbook(bid);
                users.append(g);
            }
        }
        userFile.close();
    }
    int maxID = 0;
    for (User* u : users) if (u->get_userid() > maxID) maxID = u->get_userid();
    User::setNextID(maxID + 1);

    QFile bookFile("books.dat");
    if (bookFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&bookFile);
        while (!in.atEnd()) {
            int id, year, copies, download_count, read_onsite_count, times_borrowed;
            QString title, author, category, isbn, publisher;
            in >> id >> title >> author >> category >> isbn >> publisher
               >> year >> copies >> download_count >> read_onsite_count >> times_borrowed;
            Book* b = new Book(title, author, category, isbn, publisher, year);
            if (id >= Book::getNextID()) Book::setNextID(id + 1);
            b->set_bookid(id); b->setAvailableCopies(copies);
            for (int i = 0; i < download_count; i++) b->incrementDownloadCount();
            for (int i = 0; i < read_onsite_count; i++) b->incrementReadOnSiteCount();
            for (int i = 0; i < times_borrowed; i++) b->incrementBorrowCount();
            catalog.addbook(b);
        }
        bookFile.close();
    }

    QFile txnFile("transactions.dat");
    if (txnFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&txnFile);
        while (!in.atEnd()) {
            int tid, uid, bid;
            QDateTime borrow_time, due_time, return_time;
            QString return_date;
            bool returned;
            in >> tid >> uid >> bid >> borrow_time >> due_time >> return_time >> return_date >> returned;
            Transaction t;
            t.setTransactionID(tid); t.setUserID(uid); t.setBookID(bid);
            t.setBorrowTime(borrow_time); t.setDueTime(due_time); t.setReturnTime(return_time);
            t.setReturnDate(return_date); t.setReturned(returned);
            transactions.append(t);
            if (!returned) {
                for (User* u : users) { if (u->get_userid() == uid) { u->borrowbook(bid); break; } }
            }
        }
        txnFile.close();
    }

    QFile fineFile("fines.dat");
    if (fineFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&fineFile);
        while (!in.atEnd()) {
            int user_id, book_id, minuteslate;
            double rate, originalAmount, finalAmount;
            bool discountApplied;
            in >> user_id >> book_id >> minuteslate >> rate >> originalAmount >> finalAmount >> discountApplied;
            Fines f;
            f.setUserID(user_id); f.setBookID(book_id); f.setMinutesLate(minuteslate);
            f.setRate(rate); f.setOriginalAmount(originalAmount); f.setFinalAmount(finalAmount);
            f.setDiscountApplied(discountApplied);
            fines.append(f);
        }
        fineFile.close();
    }

    QFile tableFile("tables.dat");
    if (tableFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&tableFile);
        while (!in.atEnd()) {
            int id, capacity; QString location;
            in >> id >> location >> capacity;
            Table t; t.setID(id); t.setLocation(location); t.setCapacity(capacity);
            tables.append(t);
        }
        tableFile.close();
    }

    QFile resFile("reservations.dat");
    if (resFile.open(QIODevice::ReadOnly)) {
        QDataStream in(&resFile);
        while (!in.atEnd()) {
            int resid, uid, bid, tid;
            QString date, startTime, endTime;
            bool active;
            in >> resid >> uid >> bid >> tid >> date >> startTime >> endTime >> active;
            Reservation r(uid, bid, tid, date, startTime, endTime);
            r.setReservationID(resid);
            if (!active) r.cancel();
            reservations.append(r);
        }
        resFile.close();
    }
    autoExpireReservations();
}

void Library::saveAllData() {
    QFile userFile("users.dat");
    if (userFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&userFile);
        for (User* u : users) {
            out << u->get_userid() << u->get_name() << u->get_email()
                << u->get_phone() << u->get_cnic() << u->get_usertype()
                << u->getUsername() << u->getPassword() << u->getIsDeleted()
                << u->getBorrowedCount();
            for (int i = 0; i < u->getBorrowedCount() && i < 10; i++)
                out << u->getBorrowedBooks()[i];
            out << u->getDailyBorrows() << u->getLastBorrowDate()
                << u->getBalance() << u->getTotalFinesPaid() << u->hasMembership();
            if (u->hasMembership()) {
                out << u->getMembership()->getActivationTime()
                    << u->getMembership()->getExpiryTime() << 365;
            }
            if (u->get_usertype() == "STUDENT") {
                Student* s = dynamic_cast<Student*>(u);
                out << s->getSeatNo() << s->getMajor();
            } else if (u->get_usertype() == "FACULTY") {
                Faculty* f = dynamic_cast<Faculty*>(u);
                out << f->getDepartment() << f->getDesignation();
            } else {
                Guest* g = dynamic_cast<Guest*>(u);
                out << g->getPurpose();
            }
        }
        userFile.close();
    }

    QFile bookFile("books.dat");
    if (bookFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&bookFile);
        for (int i = 0; i < catalog.get_bookcount(); i++) {
            Book* b = catalog.getBookAtIndex(i);
            out << b->get_bookid() << b->get_title() << b->get_author()
                << b->get_category() << b->get_isbn() << b->get_publisher()
                << b->get_YOP() << b->getAvailableCopies()
                << b->get_downloadcount() << b->get_read_onsitecount()
                << b->get_timesborrowed_count();
        }
        bookFile.close();
    }

    QFile txnFile("transactions.dat");
    if (txnFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&txnFile);
        for (const Transaction& t : transactions) {
            out << t.getTransactionID() << t.getuserid() << t.getbookid()
                << t.getBorrowTime() << t.getDueTime() << t.getReturnTime()
                << t.getReturnDate() << t.isReturned();
        }
        txnFile.close();
    }

    QFile fineFile("fines.dat");
    if (fineFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&fineFile);
        for (const Fines& f : fines) {
            out << f.getUserID() << f.getBookID() << f.getMinutesLate()
                << f.getRate() << f.getOriginal() << f.getAmount() << f.wasDiscounted();
        }
        fineFile.close();
    }

    QFile tableFile("tables.dat");
    if (tableFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&tableFile);
        for (const Table& t : tables) {
            out << t.get_id() << t.get_location() << t.get_capacity();
        }
        tableFile.close();
    }

    QFile resFile("reservations.dat");
    if (resFile.open(QIODevice::WriteOnly)) {
        QDataStream out(&resFile);
        for (const Reservation& r : reservations) {
            out << r.get_reservationid() << r.get_userid() << r.get_bookid()
                << r.get_tableid() << r.get_date() << r.get_start()
                << r.get_end() << r.isActive();
        }
        resFile.close();
    }
}

Authentication::Authentication(librarian* l) : lib_admin(l) {}

User* Authentication::loginUser(Library* lib, const QString& username, const QString& password) {
    for (int i = 0; i < lib->getUserCount(); i++) {
        User* u = lib->getUserByIndex(i);
        if (u->getUsername() == username && u->getPassword() == password) {
            if (u->getIsDeleted()) throw AuthException("THIS ACCOUNT HAS BEEN DELETED.");
            return u;
        }
    }
    throw AuthException("INVALID USERNAME OR PASSWORD!");
}

librarian* Authentication::loginLibrarian(const QString& username, const QString& password) {
    if (!lib_admin) throw AuthException("NO LIBRARIAN REGISTERED!");
    if (lib_admin->getUsername() == username && lib_admin->getPassword() == password)
        return lib_admin;
    throw AuthException("INVALID LIBRARIAN CREDENTIALS!");
}

QString getTodayDateString() { return QDateTime::currentDateTime().toString("dd/MM/yyyy"); }
int minutesDiff(const QDateTime& start, const QDateTime& end) {
    qint64 diff = start.secsTo(end);
    return (diff > 0) ? (int)(diff / 60) : 0;
}
QDateTime reservationToDateTime(const QString& date, const QString& timeStr) {
    QDate d = QDate::fromString(date, "dd/MM/yyyy");
    QTime t = QTime::fromString(timeStr, "hh:mm");
    return QDateTime(d, t);
}
bool isValidReservationTime(const QString& date, const QString& start, const QString& end) {
    int startHour = start.left(2).toInt();
    int endHour = end.left(2).toInt();
    if (startHour >= 0 && startHour < 5) return false;
    if (endHour >= 0 && endHour < 5) return false;
    QDateTime startT = reservationToDateTime(date, start);
    QDateTime endT = reservationToDateTime(date, end);
    QDateTime now = QDateTime::currentDateTime();
    if (startT <= now) return false;
    if (endT <= startT) return false;
    return true;
}
