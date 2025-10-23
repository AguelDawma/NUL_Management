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

#endif //BUS_H