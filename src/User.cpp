#include "../Headers/User.h"
#include <iostream>
#include <algorithm>

using namespace std;

// Constructors
User::User()
    : id(0), name(""), passwordHash(""), type(""), bookings() {}

User::User(int id, const string& name, const string& passwordHash, string type)
    : id(id), name(name), passwordHash(passwordHash), type(""), bookings() {}

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
void User::addBooking(const Resource& booking) {
    bookings.push_back(booking);
}

void User::removeBooking(int itemID) {
    auto it = std::remove_if(bookings.begin(), bookings.end(), [itemID](const Resource& r) {
        return r.getId() == itemID;
    });
    if (it != bookings.end()) bookings.erase(it, bookings.end());
}

void User::viewMyBookings() const {
    std::cout << "Bookings for user '" << name << "' (id=" << id << "):\n";
    if (bookings.empty()) {
        std::cout << "  (no bookings)\n";
        return;
    }
    for (const auto& b : bookings) {
        std::cout << "  Resource id=" << b.getId() << " name='" << b.getName() << "' type='" << b.getType() << "'\n";
    }
}
