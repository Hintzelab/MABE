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
    void turn(std::string);
    int move();
};
