#ifndef BUS_H
#define BUS_H

#include "Resource.h"

using namespace std;

class Bus: protected Resource{
    private:
        string fromDate;
        string toDate;

    public:
        Bus();

        void setFromDate(string);
        void setToDate(string);

        string getFromDate();
        string getToDate();
};


Bus::Bus() : Resource(), fromDate(""), toDate("") {}

void Bus::setFromDate(string d) { fromDate = d; }
void Bus::setToDate(string d) { toDate = d; }

string Bus::getFromDate() { return fromDate; }
string Bus::getToDate() { return toDate; }


#endif //BUS_H