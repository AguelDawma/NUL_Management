#ifndef USER_H
#define USER_H

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <algorithm>
#include <limits> 

#include "Resource.h"
#include "Lab.h" 

using namespace std;


class User {
    private:
        int id;
        string name;
        string passwordHash; 
        string type;
        queue<pair<const Resource*, int>> bookings;

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
        void addBooking(const Resource* booking, int sid);
        void removeBooking(int itemID);
        void viewMyBookings() const;
        queue<pair<const Resource*, int>> getBookings() const;
        void loadBooking(int resourceId, int slotId);

        void addToResourceWaitlist(Resource* resource);
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

/**
 * @brief Adds a resource pointer to the user's list of bookings.
 * @param booking A constant pointer to the booked Resource object.
 */
void User::addBooking(const Resource* booking, int slotId = -1) {
    bookings.push(make_pair(booking, slotId));
    cout << "  Booking confirmed: Resource ID " << booking->getId() << " added to your list.\n";
}

queue<pair<const Resource*, int>> User::getBookings() const {
    return bookings;
}

/**
 * @brief Removes a booking from the user's list based on the Resource ID.
 * @param itemID The ID of the resource to remove.
 */
void User::removeBooking(int itemID) {
    queue<pair<const Resource*, int>> temp_queue;
    bool found = false;

    while (!bookings.empty()) {
        const pair<const Resource*, int>& r = bookings.front();
        bookings.pop();

        if (r.first->getId() == itemID) {
            found = true;
            cout << "  Booking for Resource ID " << itemID << " successfully removed.\n";
            continue; // Skip adding this resource to the temp queue
        }
        temp_queue.push(r);
    }

    bookings = temp_queue; // Restore the bookings queue without the removed item

    if (!found) {
        cout << "  Booking for Resource ID " << itemID << " not found in your list.\n";
    }
}

void User::viewMyBookings() const {
    std::cout << "\nBookings for user '" << name << "' (id=" << id << ")\n";
    
    // Check the size of the original queue
    if (bookings.empty()) { 
        std::cout << "  (no current bookings)\n";
    } else {
        // 1. Create a temporary COPY of the bookings queue to iterate through.
        queue<pair<const Resource*, int>> temp_bookings = bookings;

        // 2. Iterate using a while loop until the temporary queue is empty.
        while (!temp_bookings.empty()) {
            
            // Get the element at the front
            const Resource* resource = temp_bookings.front().first; 
            
            // Print the booking details
            std::cout << "  - Resource ID: " << resource->getId()
                      << ", Name: " << resource->getName()
                      << ", Type: " << resource->getType()
                      << ", Location: " << resource->getLocation().getName() << "\n";
            
            // Optionally show slot details if it's a Lab/LectureHall
            const Lab* lab_resource = dynamic_cast<const Lab*>(resource);
            if (lab_resource) {
                std::cout << "    (Contains " << lab_resource->getSlots().size() << " time slots.)\n";
            }

            temp_bookings.pop(); 
        }
    }
    std::cout << "=======================================\n";
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

void User::loadBooking(int resourceId, int slotId = -1) {
    // Create a temporary Resource object with the given ID
    pair<Resource*, int> booking = make_pair(new Resource(), -1);
    booking.first->setId(resourceId);
    if(slotId != -1) {
        // If slotId is valid, we assume it's a Lab or LectureHall
        Lab* lab_booking = new Lab();
        lab_booking->setId(resourceId);
        lab_booking->bookSlot(slotId);
        // Here we could load the specific slot details if needed
        booking.first = lab_booking;
    }
    // Add to bookings queue
    bookings.push(booking);
}


#endif // USER_H

