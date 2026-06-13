// ============================================================
// Membership.cpp
// Contains: Membership class function implementations
// ============================================================

#include "Membership.h"

// ---- Static member definitions ----
int Membership::next_id = 1;
const double Membership::FINE_DISCOUNT = 0.15;
const int Membership::DEFAULT_DURATION_DAYS = 365;

// DEFAULT CONSTRUCTOR
Membership::Membership()
    : membershipID(next_id++), userID(0),
      userType("NONE"), userName("NONE"),
      activationTime(0), expiryTime(0), active(false) {}

// PARAMETERIZED CONSTRUCTOR
Membership::Membership(int uid, string utype, string uname, int durationDays)
    : membershipID(next_id++), userID(uid),
      userType(utype), userName(uname), active(true)
{
    activationTime = getCurrentTime();
    expiryTime = addMinutes(activationTime, durationDays * 24 * 60);
}

// RESTORE CONSTRUCTOR (for loading from file)
Membership::Membership(int uid, string utype, string uname, time_t activation, time_t expiry)
    : membershipID(next_id++), userID(uid),
      userType(utype), userName(uname), active(true),
      activationTime(activation), expiryTime(expiry) {}

// DESTRUCTOR
Membership::~Membership() {}

// Check if membership is still valid
bool Membership::isValid() const
{
    return active && (getCurrentTime() <= expiryTime);
}

// Apply 15% discount on the fine
double Membership::applyDiscount(double originalFine) const
{
    if (!isValid())
        return originalFine;
    return originalFine * (1.0 - FINE_DISCOUNT);
}

// Extra books the user can borrow with membership
int Membership::getBorrowBoost() const
{
    if (!isValid())
        return 0;
    if (userType == "STUDENT") return 2; // 3 -> 5
    if (userType == "FACULTY") return 2; // 5 -> 7
    if (userType == "GUEST")   return 1; // 1 -> 2
    return 0;
}

// Cancel the membership
void Membership::cancel()
{
    active = false;
    cout << "MEMBERSHIP CANCELLED FOR " << userName << "." << endl;
}

// Display membership details
ostream &operator<<(ostream &os, const Membership &m)
{
    os << "=========== MEMBERSHIP DETAILS ===========" << endl
       << "| MEMBERSHIP ID  : " << m.membershipID << endl
       << "| MEMBER NAME    : " << m.userName << endl
       << "| USER TYPE      : " << m.userType << endl
       << "| STATUS         : " << (m.isValid() ? "ACTIVE" : "EXPIRED/INACTIVE") << endl
       << "| FINE DISCOUNT  : 15%" << endl
       << "| BORROW BOOST   : +" << m.getBorrowBoost() << " BOOKS" << endl
       << "| ACTIVATED ON   : " << timeToString(m.activationTime)
       << "| EXPIRES ON     : " << timeToString(m.expiryTime)
       << "==========================================" << endl;
    return os;
}
