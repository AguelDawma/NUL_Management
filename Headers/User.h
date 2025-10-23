#ifndef USER_H
#define USER_H

#include <string>
#include "Resource.h"

using namespace std;


class User {
    private:
        int id;
        string name;
        string passwordHash; // Store the hashed password
        string type;
        Resource* bookings;

    public:
        // Constructors
        User();
        User(int id, const string& name, const string& passwordHash);

        // Getters
        int getId() const;
        string getName() const;
        string getPasswordHash() const;

        // Setters
        void setId(int id);
        void setName(const string& name);
        void setPasswordHash(const string& passwordHash);

        // Utility Functions
        void addBooking(const Type& booking);
        void removeBooking(int itemID);
        void viewMyBookings();
};

#endif // USER_H