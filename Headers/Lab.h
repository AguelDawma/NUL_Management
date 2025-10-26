#ifndef LAB_H
#define LAB_H

#include <string>
#include <vector>
#include "Resource.h"
#include "Slot.h"

using namespace std;

class Lab: protected Resource {
    protected:
        std::vector<Slot> slots;

    public:
        // Setters
        void addSlot(const Slot& slot);

        // Getters / actions
        void viewAvailableSlots() const;
        bool bookSlot(int slotId); // returns true if booked
        bool cancelSlotBooking(int slotId); // returns true if canceled

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
}

Lab::Lab(int id, const string& name, const string& type, Location location, bool available) {
    setId(id);
    setName(name);
    setType(type);
    setLocation(location);
    setAvailability(available);
}

Lab::~Lab() {
    // vector will clean up automatically
}

void Lab::addSlot(const Slot& slot) {
    slots.push_back(slot);
}

void Lab::viewAvailableSlots() const {
    std::cout << "Available slots for Lab '" << getName() << "' (id=" << getId() << "):\n";
    bool any = false;
    for (const auto& s : slots) {
        if (!s.isBooked) {
            any = true;
            std::cout << "  Slot id=" << s.id << " " << s.startTime << " - " << s.endTime << "\n";
        }
    }
    if (!any) std::cout << "  (no available slots)\n";
}

bool Lab::bookSlot(int slotId) {
    for (auto& s : slots) {
        if (s.id == slotId && !s.isBooked) {
            s.isBooked = true;
            return true;
        }
    }
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

#endif // LAB_H

