#ifndef BUS_H
#define BUS_H

#include "Resource.h"

using namespace std;

class Bus: public Resource{
    private:
        string fromDate;
        string toDate;

    public:
        Bus();
        Bus(int, string, string, Location, bool);

        void setFromDate(string);
        void setToDate(string);

        string getFromDate();
        string getToDate();

        ~Bus();
};


Bus::Bus() : Resource(), fromDate(""), toDate("") {}

Bus::Bus(int Id, string Name, string Type, Location loc, bool isAv){
	id=Id;
	name = Name;
	type = Type;
	isAvailable = isAv;
	location = loc;
}

void Bus::setFromDate(string d) { fromDate = d; }
void Bus::setToDate(string d) { toDate = d; }

string Bus::getFromDate() { return fromDate;}
string Bus::getToDate() { return toDate; }

Bus::~Bus(){
    
}


#endif //BUS_H
