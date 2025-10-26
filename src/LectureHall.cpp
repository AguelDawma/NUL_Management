#include "../Headers/LectureHall.h"

LectureHall::LectureHall() : Lab() {}

LectureHall::LectureHall(int id, string name, string type, Location location, bool available)
    : Lab(id, name, type, location, available) {}
