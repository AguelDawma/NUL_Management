#ifndef LECTUREHALL_H
#define LECTUREHALL_H

#include <string>
#include "Lab.h"

using namespace std;

class LectureHall: protected Lab{
    public:
        LectureHall();
    LectureHall(int, string, string, Location, bool);
};

#endif //LECTUREHALL_H