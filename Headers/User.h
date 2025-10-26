#ifndef USER_H
#define USER_H
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include "Resource.h"

using namespace std;


class User {
    private:
        int id;
        string name;
        string passwordHash; // Store the hashed password
        string type;
        vector<const Resource*> bookings;

    public:
        // Constructors
        User();
        User(int id, const string& name, const string& passwordHash, string);

        // Getters
        int getId() const;
        string getName() const;
        string getPasswordHash() const;
        string getType() const;

        // Setters
        void setId(int id);
        void setName(const string& name);
        void setPasswordHash(const string& passwordHash);
        void setType(const string& type);

        // Utility Functions
        void addBooking(const Resource* booking);
        void removeBooking(int itemID);
        void viewMyBookings() const;
};

// Constructors
User::User()
    : id(0), name(""), passwordHash(""), type(""), bookings() {}

User::User(int id, const string& name, const string& passwordHash, string type)
    : id(id), name(name), passwordHash(passwordHash), type(type), bookings() {}

// Getters
int User::getId() const { return id; }
string User::getName() const { return name; }
string User::getPasswordHash() const { return passwordHash; }
string User::getType() const { return type; }

// Setters
void User::setId(int id) { this->id = id; }
void User::setName(const string& name) { this->name = name; }
void User::setPasswordHash(const string& passwordHash) { this->passwordHash = passwordHash; }
void User::setType(const string& type) { this->type = type; }

// Utility Functions
void User::addBooking(const Resource* booking) {
    bookings.push_back(booking);
}

void User::removeBooking(int itemID) {
    auto it = remove_if(bookings.begin(), bookings.end(), [itemID](const Resource* r) {
        return r->getId() == itemID;
    });
    if (it != bookings.end()) bookings.erase(it, bookings.end());
}

void User::viewMyBookings() const {
    cout << "Bookings for user '" << name << "' (id=" << id << "):\n";
    if (bookings.empty()) {
        cout << "  (no bookings)\n";
        return;
    }
    for (const auto& b : bookings) {
        cout << "  Resource id=" << b->getId() << " name='" << b->getName() << "' type='" << b->getType() << "'\n";
    }
}


#endif // USER_H
