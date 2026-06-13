// ============================================================
// Transaction.cpp
// Contains: Transaction, Fines, Reservation, Table class
//           implementations, and all file I/O functions.
// ============================================================

#include "Transaction.h"

// ============================================================
//                    TRANSACTION CLASS
// ============================================================

int Transaction::next_id = 1;

Transaction::Transaction()
    : transaction_id(next_id++), user_id(0), book_id(0),
      borrow_time(0), due_time(0), return_time(0),
      return_date("NOT RETURNED"), returned(false) {}

Transaction::Transaction(int uid, int bid, int duration_minutes)
    : transaction_id(next_id++), user_id(uid), book_id(bid),
      return_date("NOT RETURNED"), returned(false), return_time(0)
{
    borrow_time = getCurrentTime();
    due_time    = addMinutes(borrow_time, duration_minutes);
}

Transaction::~Transaction() {}

// Returns 0 if book wasn't late or hasn't been returned yet
int Transaction::getOverdueMinutes()
{
    if (!returned || return_time == 0 || return_time <= due_time)
        return 0;
    return minutesDiff(due_time, return_time);
}

// Mark as returned, set return time, return overdue minutes for fine calculation
int Transaction::markreturned()
{
    returned    = true;
    return_time = getCurrentTime();
    return_date = timeToString(return_time);
    return getOverdueMinutes();
}

ostream &operator<<(ostream &os, Transaction &t)
{
    os << "=============== TRANSACTION DETAILS ===============" << endl
       << "| TRANSACTION ID : " << t.transaction_id << endl
       << "| USER ID        : " << t.user_id << endl
       << "| BOOK ID        : " << t.book_id << endl
       << "| BORROWED ON    : " << timeToString(t.borrow_time) << endl
       << "| DUE DATE       : " << timeToString(t.due_time) << endl
       << "| RETURN DATE    : " << t.return_date << endl
       << "| OVERDUE        : " << t.getOverdueMinutes() << endl
       << "| RETURNED       : " << (t.returned ? "YES" : "NO") << endl
       << "===================================================" << endl;
    return os;
}

// ============================================================
//                    FINES CLASS
// ============================================================

Fines::Fines()
    : user_id(0), book_id(0), minuteslate(0),
      rate(0), originalAmount(0), finalAmount(0), discountApplied(false) {}

Fines::Fines(int u, int b, int mins, User *user)
    : user_id(u), book_id(b)
{
    minuteslate    = (mins < 0) ? 0 : mins;
    rate           = user->getFineRate();
    originalAmount = rate * minuteslate;

    // Apply membership discount if the user has one
    if (user->hasMembership())
    {
        finalAmount     = user->applyMembershipDiscount(originalAmount);
        discountApplied = true;
    }
    else
    {
        finalAmount     = originalAmount;
        discountApplied = false;
    }
}

Fines::~Fines() {}

ostream &operator<<(ostream &os, const Fines &f)
{
    os << "================= FINE DETAILS =================" << endl
       << "| USER ID        : " << f.user_id << endl
       << "| BOOK ID        : " << f.book_id << endl
       << "| MINUTES LATE   : " << f.minuteslate << endl
       << "| RATE           : RS. " << f.rate << " /MIN" << endl
       << "| ORIGINAL AMT   : RS. " << f.originalAmount << endl;

    if (f.discountApplied)
    {
        os << "| DISCOUNT       : 15% (MEMBERSHIP)" << endl
           << "| FINAL AMT      : RS. " << f.finalAmount << endl;
    }
    else
    {
        os << "| FINAL AMT      : RS. " << f.finalAmount << endl;
    }

    os << "=================================================" << endl;
    return os;
}

// ============================================================
//                    RESERVATION CLASS
// ============================================================

int Reservation::next_id = 1;

Reservation::Reservation()
    : reservationid(next_id++), userid(0), bookid(0), tableid(0),
      date("UNKNOWN"), startTime("0"), endTime("0"), active(true) {}

Reservation::Reservation(int uid, int bid, int tid,
                          string d, string start, string end)
{
    reservationid = next_id++;
    userid        = uid;
    bookid        = bid;
    tableid       = tid;
    date          = d;
    startTime     = start;
    endTime       = end;
    active        = true;
}

Reservation::~Reservation() {}

bool Reservation::isActive()
{
    return active;
}

bool Reservation::isExpired()
{
    time_t endT = reservationToTimeT(date, endTime);
    return getCurrentTime() > endT;
}

// String comparison works here because times are stored as HH:MM
// (lexicographic order = chronological order)
bool Reservation::overlap(string s1, string e1, string s2, string e2)
{
    return !(e1 <= s2 || e2 <= s1);
}

// Display (only prints if active)
ostream &operator<<(ostream &os, const Reservation &r)
{
    if (!r.active)
        return os;

    os << "=============== RESERVATION DETAILS ===============" << endl
       << "| RESERVATION ID : " << r.reservationid << endl
       << "| USER ID        : " << r.userid << endl;

    if (r.bookid)
        os << "| BOOK ID        : " << r.bookid << endl;
    else
        os << "| TABLE ID       : " << r.tableid << endl;

    os << "| DATE           : " << r.date << endl
       << "| START TIME     : " << to12Hour(r.startTime) << endl
       << "| END TIME       : " << to12Hour(r.endTime) << endl
       << "===================================================" << endl;

    return os;
}

// Print including status (for expired/cancelled reservations)
void Reservation::printInactive(ostream &os) const
{
    os << "=============== RESERVATION DETAILS ===============" << endl
       << "| RESERVATION ID : " << reservationid << endl
       << "| USER ID        : " << userid << endl;

    if (bookid)
        os << "| BOOK ID        : " << bookid << endl;
    else
        os << "| TABLE ID       : " << tableid << endl;

    os << "| DATE           : " << date << endl
       << "| START TIME     : " << to12Hour(startTime) << endl
       << "| END TIME       : " << to12Hour(endTime) << endl
       << "| STATUS         : " << (active ? "ACTIVE" : "EXPIRED/CANCELLED") << endl
       << "===================================================" << endl;
}

// ============================================================
//                    TABLE CLASS
// ============================================================

int Table::next_id = 1;

Table::Table()
{
    tableid  = next_id++;
    location = "UNKNOWN";
    capacity = 1;
}

Table::Table(string loc, int cap)
{
    tableid  = next_id++;
    location = loc;
    capacity = cap;
}

ostream &operator<<(ostream &os, const Table &t)
{
    os << "================= TABLE DETAILS =================" << endl
       << "| TABLE ID   : " << t.tableid << endl
       << "| LOCATION   : " << t.location << endl
       << "| CAPACITY   : " << t.capacity << endl
       << "=================================================" << endl;
    return os;
}

// ============================================================
//              TRANSACTION FILE I/O
// ============================================================

void saveTransactionToFile(Transaction &t)
{
    ofstream file("transactions.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN transactions.dat FOR WRITING!");

    TransactionRecord tr = {};

    tr.tid         = t.getTransactionID();
    tr.uid         = t.getuserid();
    tr.bid         = t.getbookid();
    tr.borrow_time = t.getBorrowTime();
    tr.due_time    = t.getDueTime();
    tr.return_time = t.getReturnTime();
    tr.returned    = t.isReturned();

    strncpy(tr.return_date, t.getReturnDate().c_str(), sizeof(tr.return_date) - 1);
    tr.return_date[sizeof(tr.return_date) - 1] = '\0';

    file.write((char *)&tr, sizeof(tr));
    file.close();
}

void loadTransactionsFromFile(vector<Transaction> &transactions, vector<User *> &users)
{
    ifstream file("transactions.dat", ios::binary);
    if (!file)
        return;

    TransactionRecord tr;

    while (file.read((char *)&tr, sizeof(tr)))
    {
        Transaction t;
        t.setTransactionID(tr.tid);
        t.setUserID(tr.uid);
        t.setBookID(tr.bid);
        t.setBorrowTime(tr.borrow_time);
        t.setDueTime(tr.due_time);
        t.setReturnTime(tr.return_time);
        t.setReturnDate(tr.return_date);
        t.setReturned(tr.returned);

        transactions.push_back(t);

        // Restore borrowed books into the user object
        if (!tr.returned)
        {
            for (int i = 0; i < (int)users.size(); i++)
            {
                if (users[i]->get_userid() == tr.uid)
                {
                    users[i]->borrowbook(tr.bid);
                    break;
                }
            }
        }
    }

    file.close();
}

void rewriteTransactionsFile(vector<Transaction> &transactions)
{
    ofstream file("transactions.dat", ios::binary);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN transactions.dat FOR REWRITING!");

    for (int i = 0; i < (int)transactions.size(); i++)
    {
        TransactionRecord tr = {};

        tr.tid         = transactions[i].getTransactionID();
        tr.uid         = transactions[i].getuserid();
        tr.bid         = transactions[i].getbookid();
        tr.borrow_time = transactions[i].getBorrowTime();
        tr.due_time    = transactions[i].getDueTime();
        tr.return_time = transactions[i].getReturnTime();
        tr.returned    = transactions[i].isReturned();

        strncpy(tr.return_date, transactions[i].getReturnDate().c_str(), sizeof(tr.return_date) - 1);
        tr.return_date[sizeof(tr.return_date) - 1] = '\0';

        file.write((char *)&tr, sizeof(tr));
    }

    file.close();
}

// ============================================================
//              FINES FILE I/O
// ============================================================

void saveFineToFile(Fines &f)
{
    ofstream file("fines.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN fines.dat FOR WRITING!");

    FineRecord fr = {};

    fr.user_id        = f.getUserID();
    fr.book_id        = f.getBookID();
    fr.minuteslate    = f.getMinutesLate();
    fr.rate           = f.getRate();
    fr.originalAmount = f.getOriginal();
    fr.finalAmount    = f.getAmount();
    fr.discountApplied = f.wasDiscounted();

    file.write((char *)&fr, sizeof(fr));
    file.close();
}

void loadFinesFromFile(vector<Fines> &fines)
{
    ifstream file("fines.dat", ios::binary);
    if (!file)
        return;

    FineRecord fr;

    while (file.read((char *)&fr, sizeof(fr)))
    {
        Fines f;
        f.setUserID(fr.user_id);
        f.setBookID(fr.book_id);
        f.setMinutesLate(fr.minuteslate);
        f.setRate(fr.rate);
        f.setOriginalAmount(fr.originalAmount);
        f.setFinalAmount(fr.finalAmount);
        f.setDiscountApplied(fr.discountApplied);

        fines.push_back(f);
    }

    file.close();
}

void rewriteFinesFile(vector<Fines> &fines)
{
    ofstream file("fines.dat", ios::binary);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN fines.dat FOR REWRITING!");

    for (int i = 0; i < (int)fines.size(); i++)
    {
        FineRecord fr = {};

        fr.user_id         = fines[i].getUserID();
        fr.book_id         = fines[i].getBookID();
        fr.minuteslate     = fines[i].getMinutesLate();
        fr.rate            = fines[i].getRate();
        fr.originalAmount  = fines[i].getOriginal();
        fr.finalAmount     = fines[i].getAmount();
        fr.discountApplied = fines[i].wasDiscounted();

        file.write((char *)&fr, sizeof(fr));
    }

    file.close();
}

// ============================================================
//              RESERVATION FILE I/O
// ============================================================

void saveReservationToFile(Reservation &r)
{
    ofstream file("reservations.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN reservations.dat FOR WRITING!");

    ReservationRecord rr = {};
    rr.reservationid = r.get_reservationid();
    rr.userid        = r.get_userid();
    rr.bookid        = r.get_bookid();
    rr.tableid       = r.get_tableid();
    rr.active        = r.isActive();
    strncpy(rr.date,      r.get_date().c_str(),  sizeof(rr.date) - 1);
    strncpy(rr.startTime, r.get_start().c_str(), sizeof(rr.startTime) - 1);
    strncpy(rr.endTime,   r.get_end().c_str(),   sizeof(rr.endTime) - 1);

    file.write((char *)&rr, sizeof(rr));
    file.close();
}

void loadReservationsFromFile(vector<Reservation> &reservations)
{
    ifstream file("reservations.dat", ios::binary);
    if (!file)
        return;

    ReservationRecord rr;
    while (file.read((char *)&rr, sizeof(rr)))
    {
        Reservation r(rr.userid, rr.bookid, rr.tableid, rr.date, rr.startTime, rr.endTime);
        r.setReservationID(rr.reservationid);
        if (!rr.active)
            r.cancel();
        reservations.push_back(r);
    }
    file.close();
}

void rewriteReservationsFile(vector<Reservation> &reservations)
{
    ofstream file("reservations.dat", ios::binary);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN reservations.dat FOR REWRITING!");

    for (int i = 0; i < (int)reservations.size(); i++)
    {
        ReservationRecord rr = {};
        rr.reservationid = reservations[i].get_reservationid();
        rr.userid        = reservations[i].get_userid();
        rr.bookid        = reservations[i].get_bookid();
        rr.tableid       = reservations[i].get_tableid();
        rr.active        = reservations[i].isActive();
        strncpy(rr.date,      reservations[i].get_date().c_str(),  sizeof(rr.date) - 1);
        strncpy(rr.startTime, reservations[i].get_start().c_str(), sizeof(rr.startTime) - 1);
        strncpy(rr.endTime,   reservations[i].get_end().c_str(),   sizeof(rr.endTime) - 1);

        file.write((char *)&rr, sizeof(rr));
    }
    file.close();
}

// ============================================================
//              TABLE FILE I/O
// ============================================================

void saveTableToFile(Table &t)
{
    ofstream file("tables.dat", ios::binary | ios::app);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN tables.dat FOR WRITING!");

    TableRecord tr = {};
    tr.id       = t.get_id();
    tr.capacity = t.get_capacity();
    strncpy(tr.location, t.get_location().c_str(), sizeof(tr.location) - 1);
    tr.location[sizeof(tr.location) - 1] = '\0';

    file.write((char *)&tr, sizeof(tr));
    file.close();
}

void loadTablesFromFile(vector<Table> &tables)
{
    ifstream file("tables.dat", ios::binary);
    if (!file)
        return;

    TableRecord tr;

    while (file.read((char *)&tr, sizeof(tr)))
    {
        Table t;
        t.setID(tr.id);
        t.setLocation(tr.location);
        t.setCapacity(tr.capacity);
        tables.push_back(t);
    }

    file.close();
}

void rewriteTablesFile(vector<Table> &tables)
{
    ofstream file("tables.dat", ios::binary);
    if (!file)
        throw FileException("ERROR: COULD NOT OPEN tables.dat FOR REWRITING!");

    for (int i = 0; i < (int)tables.size(); i++)
    {
        TableRecord tr = {};
        tr.id       = tables[i].get_id();
        tr.capacity = tables[i].get_capacity();
        strncpy(tr.location, tables[i].get_location().c_str(), sizeof(tr.location) - 1);
        tr.location[sizeof(tr.location) - 1] = '\0';

        file.write((char *)&tr, sizeof(tr));
        if (!file)
            throw FileException("ERROR: FAILED TO WRITE TABLE RECORD DURING REWRITE!");
    }

    file.close();
}
