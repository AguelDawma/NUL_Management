#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits> // Used for cin.ignore if needed

#include "Resource.h"
#include "Lab.h" // Required for dynamic_cast and waitlist functionality

using namespace std;


class User {
    private:
        int id;
        string name;
        string passwordHash; // Store the hashed password
        string type;
        vector<const Resource*> bookings; // Vector to store pointers to booked resources

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
        
        // ? NEW: Function to add user to a resource's waitlist
        void addToResourceWaitlist(Resource* resource);
};

// =======================================================================
//                        USER IMPLEMENTATION
// =======================================================================

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

/**
 * @brief Adds a resource pointer to the user's list of bookings.
 * @param booking A constant pointer to the booked Resource object.
 */
void User::addBooking(const Resource* booking) {
    bookings.push_back(booking);
    cout << "  Booking confirmed: Resource ID " << booking->getId() << " added to your list.\n";
}

/**
 * @brief Removes a booking from the user's list based on the Resource ID.
 * @param itemID The ID of the resource to remove.
 */
void User::removeBooking(int itemID) {
    // Use std::remove_if to logically move elements to be removed to the end
    auto it = remove_if(bookings.begin(), bookings.end(), [itemID](const Resource* r) {
        return r->getId() == itemID;
    });

    // Erase the elements at the end
    if (it != bookings.end()) {
        bookings.erase(it, bookings.end());
        cout << "  Booking for Resource ID " << itemID << " successfully removed.\n";
    } else {
        cout << "  Booking for Resource ID " << itemID << " not found in your list.\n";
    }
}

/**
 * @brief Displays all current bookings for the user.
 */
void User::viewMyBookings() const {
    cout << "\n=== Bookings for user '" << name << "' (id=" << id << ") ===\n";
    
    if (bookings.empty()) {
        cout << "  (no current bookings)\n";
    } else {
        for (const Resource* resource : bookings) {
            cout << "  - Resource ID: " << resource->getId()
                 << ", Name: " << resource->getName()
                 << ", Type: " << resource->getType()
                 << ", Location: " << resource->getLocation().getName() << "\n";
            
            // Optionally show slot details if it's a Lab/LectureHall
            const Lab* lab_resource = dynamic_cast<const Lab*>(resource);
            if (lab_resource) {
                // Since this is a user booking, we should probably know *which* slot was booked.
                // For simplicity here, we'll just list all the slots it contains.
                cout << "    (Contains " << lab_resource->getSlots().size() << " time slots.)\n";
            }
        }
    }
    cout << "=======================================\n";
}

/**
 * @brief Attempts to add the current user to a resource's waitlist (if supported).
 * @param resource A pointer to the Resource object.
 */
void User::addToResourceWaitlist(Resource* resource) {
    // Check if the resource is a Lab or LectureHall (which inherits from Lab)
    // and thus has the waitlist functionality.
    Lab* lab_resource = dynamic_cast<Lab*>(resource);
    
    if (lab_resource) {
        // Lab::addToWaitlist handles the actual queue push and confirmation message.
        lab_resource->addToWaitlist(this->id);
    } else {
        cout << "? Error: Resource type '" << resource->getType() << "' does not support a waitlist.\n";
    }
}


#endif // USER_H

