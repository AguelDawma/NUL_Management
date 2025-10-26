#ifndef SLOT_H
#define SLOT_H

#include <string>

using namespace std;

struct Slot {
    int id = 0;
    string day;         // e.g., "Monday"
    string startTime;   // e.g., "09:00"
    string endTime;     // e.g., "10:30"
    bool isBooked = false;

    Slot() = default;
    Slot(int id, const string& day, const string& startTime, const string& endTime)
        : id(id), day(day), startTime(startTime), endTime(endTime), isBooked(false) {}
};

#endif // SLOT_H
