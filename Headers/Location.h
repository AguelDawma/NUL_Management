#ifndef LOCATION_H
#define LOCATION_H

#include <string>

using namespace std;

class Location {
public:
    Location();
    Location(int id, const string& building, const string& room);

    void setId(int id);
    void setBuilding(const string& building);
    void setRoom(const string& room);

    int getId() const;
    string getBuilding() const;
    string getRoom() const;

private:
    int id = 0;
    string building;
    string room;
};

Location::Location() : id(0), building(""), room("") {}

Location::Location(int id, const string& building, const string& room)
    : id(id), building(building), room(room) {}

void Location::setId(int id) { this->id = id; }
void Location::setBuilding(const string& building) { this->building = building; }
void Location::setRoom(const string& room) { this->room = room; }

int Location::getId() const { return id; }
string Location::getBuilding() const { return building; }
string Location::getRoom() const { return room; }


#endif // LOCATION_H
