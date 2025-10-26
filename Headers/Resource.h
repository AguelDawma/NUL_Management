#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include "Location.h"

using namespace std;

class Resource{
    protected:
        int id;
        string name;
        string type;
        Location location;
        bool isAvailable;

    public:
        //setters
        void setId(int id);
        void setName(const string& name);
        void setType(const string& type);
        void setLocation(const Location& location);

        //getters
        int getId() const;
        string getName() const;
        string getType() const;
        Location getLocation() const;

        //availability
        bool getAvailability() const;
        void setAvailability(bool availability);\

        virtual ~Resource() {}

};

// Setters
void Resource::setId(int id) { this->id = id; }
void Resource::setName(const string& name) { this->name = name; }
void Resource::setType(const string& type) { this->type = type; }
void Resource::setLocation(const Location& location) { this->location = location; }

// Getters
int Resource::getId() const { return id; }
string Resource::getName() const { return name; }
string Resource::getType() const { return type; }
Location Resource::getLocation() const { return location; }

// Availability
bool Resource::getAvailability() const { return isAvailable; }
void Resource::setAvailability(bool availability) { isAvailable = availability; }


#endif // RESOURCE_H