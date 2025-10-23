#ifndef LAB_H
#define LAB_H

#include <string>
#include "Resource.h"

using namespace std;

class Lab: protected Resource {
    protected:
        Slot* slots;

    public:
        // Setters
        void addSlot(Slot slot);

        // Getters
        void viewAvailableSlots() const;
        void bookSlot();
        void cancelSlotBooking();

        Lab();
        Lab(int, string, string, Location, bool);
        ~Lab();
};

#endif // LAB_H

