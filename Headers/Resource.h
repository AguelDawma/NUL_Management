#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
#include "Location"

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
        void setAvailability(bool availability);

};

#endif // RESOURCE_H