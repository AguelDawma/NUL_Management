#ifndef LECTUREHALL_H
#define LECTUREHALL_H

#include <string>
#include "Lab.h"

using namespace std;

class LectureHall: public Lab{
    public:
        LectureHall();
        LectureHall(int, string, string, Location, bool);
};

LectureHall::LectureHall() : Lab() {}

LectureHall::LectureHall(int id, string name, string type, Location location, bool available)
    : Lab(id, name, type, location, available) {}

#endif //LECTUREHALL_H