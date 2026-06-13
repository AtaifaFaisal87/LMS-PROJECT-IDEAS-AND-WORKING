#ifndef MEMBERSHIP_H
#define MEMBERSHIP_H

// ============================================================
// Membership.h
// Contains: Membership class definition
// ============================================================

#include "Globals.h"

class Membership
{
private:
    int membershipID;
    static int next_id;

    int userID;
    string userType;
    string userName;

    time_t activationTime;
    time_t expiryTime;
    bool active;

    // ---- CONSTANTS ----
    static const double FINE_DISCOUNT;       // 15%
    static const int DEFAULT_DURATION_DAYS;  // 365 days

public:
    // DEFAULT CONSTRUCTOR
    Membership();

    // PARAMETERIZED CONSTRUCTOR (creates a new membership)
    Membership(int uid, string utype, string uname, int durationDays = 365);

    // RESTORE CONSTRUCTOR (for loading from file — does not reset times)
    Membership(int uid, string utype, string uname, time_t activation, time_t expiry);

    // DESTRUCTOR
    ~Membership();

    // ---- GETTERS ----
    int    getMembershipID()  const { return membershipID; }
    int    getUserID()        const { return userID; }
    string getUserType()      const { return userType; }
    string getUserName()      const { return userName; }
    bool   isActive()         const { return active; }
    time_t getActivationTime()      { return activationTime; }
    time_t getExpiryTime()          { return expiryTime; }

    // ---- SETTERS ----
    void setActivationTime(time_t t) { activationTime = t; }
    void setExpiryTime(time_t t)     { expiryTime = t; }

    // Check if membership is still valid
    bool isValid() const;

    // ---- FINE DISCOUNT ----
    double applyDiscount(double originalFine) const;

    // ---- BORROW LIMIT BOOST ----
    int getBorrowBoost() const;

    // ---- CANCEL ----
    void cancel();

    // ---- DISPLAY ----
    friend ostream &operator<<(ostream &os, const Membership &m);
};

#endif
