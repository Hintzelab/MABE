#pragma once

#include "Object.h"

#include <cstdlib>
#include <thread>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

class cObstacle : public cObject{
private:
    double mVelocity = 0.5;

public:
    // constructor
    cObstacle() = delete;
    cObstacle(int serialNum, double vel) {mSerialNumber = serialNum; mVelocity = vel;};

    int turn() { return int(1/mVelocity); }
};
