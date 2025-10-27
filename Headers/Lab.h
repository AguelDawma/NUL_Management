#ifndef LAB_H
#define LAB_H
#include <iostream>
#include <string>
#include <vector>
#include "Resource.h"
#include "Slot.h"

using namespace std;

class Lab: public Resource {
    protected:
        vector<Slot> slots;

    public:
        // Setters
        void addSlot(const Slot& slot);

        // Getters / actions
        void viewAvailableSlots() const;
        bool bookSlot(int slotId); // returns true if booked
        bool cancelSlotBooking(int slotId); // returns true if canceled
        void addLabSlots();
        Slot getSlot(int id) const;
        vector<Slot> getSlots() const { return slots; }

        Lab();
        Lab(int id, const string& name, const string& type, Location location, bool available);
        ~Lab();
};

Lab::Lab() {
    // default Resource members are uninitialized — set defaults
    setId(0);
    setName("");
    setType("");
    setAvailability(true);
    addLabSlots();
}

Lab::Lab(int id, const string& name, const string& type, Location location, bool available) {
    setId(id);
    setName(name);
    setType(type);
    setLocation(location);
    setAvailability(available);
    addLabSlots();
}

Lab::~Lab() {
    // vector will clean up automatically
}

Slot Lab::getSlot(int id) const {
    for(auto& slot : slots){
        if(slot.id == id){
            return slot;
        }
    }

    cout << "\nSlot not found."<<endl;
}

void Lab::addSlot(const Slot& slot) {
    slots.push_back(slot);
}

void Lab::viewAvailableSlots() const {
    cout << "Available slots for Lab '" << getName() << "' (id=" << getId() << "):\n";
    bool any = false;
    for (const auto& s : slots) {
        if (!s.isBooked) {
            any = true;
            cout << "  Slot id=" << s.id << " " << s.startTime << " - " << s.endTime << "\n";
        }
    }
    if (!any) cout << "  (no available slots)\n";
}

bool Lab::bookSlot(int slotId) {
    for (auto& s : slots) {
        if (s.id == slotId && !s.isBooked) {
            s.isBooked = true;
            return true;
        }
    }
    cout<<"\nSlot is booked!"<<endl;
    return false;
}

bool Lab::cancelSlotBooking(int slotId) {
    for (auto& s : slots) {
        if (s.id == slotId && s.isBooked) {
            s.isBooked = false;
            return true;
        }
    }
    return false;
}

void Lab::addLabSlots(){
    Slot monday1(000,"Monday","08:00","10:00");
    Slot monday2(001,"Monday","10:00","12:00");
    Slot monday3(002,"Monday","12:00","14:00");
    Slot monday4(003,"Monday","14:00","16:00");
    Slot Tuesday1(010,"Tuesday","08:00","10:00");
    Slot Tuesday2(011,"Tuesday","10:00","12:00");
    Slot Tuesday3(012,"Tuesday","12:00","14:00");
    Slot Tuesday4(013,"Tuesday","14:00","16:00");

    addSlot(monday1);
    addSlot(monday2);
    addSlot(monday3);
    addSlot(monday4);
    addSlot(Tuesday1);
    addSlot(Tuesday2);
    addSlot(Tuesday3);
    addSlot(Tuesday4);
    
}

#endif // LAB_H

