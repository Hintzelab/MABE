//  MABE is a product of The Hintze Lab @ MSU
//     for general research information:
//         hintzelab.msu.edu
//     for MABE documentation:
//         github.com/Hintzelab/MABE/wiki
//
//  Copyright (c) 2015 Michigan State University. All rights reserved.
//     to view the full license, visit:
//         github.com/Hintzelab/MABE/wiki/License

#pragma once

#include <cmath>
#include <vector>
#include <iostream>


// point2d defines a 2d vector with addtion, subtraction, dot/scalar product(*)
// and cross product
// also included are distance functions and functions which return the signed
// angle between 2 point2ds (relitive to 0,0)

class Point2d { // a point class, useful for 2d worlds
public:
  double x;
  double y;

  const double pi = atan(1.0) * 4;

  Point2d() {
    x = 0.0;
    y = 0.0;
  }
  Point2d(double _x, double _y) : x(_x), y(_y) {} // construct with x,y

  void set(double _x, double _y) {
    x = _x;
    y = _y;
  }

  void show() { // print x and y of this
    std::cout << x << "," << y << "\n";
  }

  Point2d operator=(Point2d other) { // scalar/dot product of this and other
    this->x = other.x;
    this->y = other.y;
    return *this;
  }

  double operator*(Point2d other) { // scalar/dot product of this and other
    return x * other.x + y * other.y;
  }

  Point2d scale(double mag) { // vector * scalar
    Point2d newVect(x * mag, y * mag);
    return newVect;
  }

  double cross_prod(Point2d other) // cross product of this and other
  {
    return x * other.y - y * other.x;
  }

  Point2d operator+(Point2d other) { // add this point and other
    Point2d newVect(x + other.x, y + other.y);
    return newVect;
  }

  Point2d operator-(Point2d other) { // subtract other from this point
    Point2d newVect;
    newVect.x = x - other.x;
    newVect.y = y - other.y;
    return newVect;
  }

  bool operator==(Point2d other) { // scalar/dot product of this and other
    if (x == other.x && y == other.y) {
      return true;
    }
    return false;
  }

  double dist() { // length between this point and 0,0
    return sqrt(x * x + y * y);
  }

  double dist(Point2d other) { // length between this point and other
    return (*this - other).dist();
  }

  double angle_between_radian(Point2d other) // return angle in radians between
                                             // this point and other relative to
                                             // origin (0,0)
  {
    if (abs(x - other.x) < .000001 &&
        abs(y - other.y) < .000001) { // vectors are effecvily the same
      return (0);
    }
    if (abs(x / other.x - y / other.y) <
        .000001) { // vectors are effecvily parallel
      if (((x > 0) == (other.x > 0)) &&
          ((y > 0) == (other.y > 0))) { // and are pointing the same way
        return (0);
      }
    }
    return (cross_prod(other) < 0 ? 1 : -1) *
           acos((*this) * other / (dist() * other.dist()));
  }

  double angle_between_deg(Point2d other) // return angle in degrees between
                                          // this point and other relative to
                                          // origin (0,0)
  {
    return angle_between_radian(other) / pi * 180;
  }
};

// point3d defines a 3d vector with addtion, subtraction, dot/scalar product(*)
// and cross product
// also included are distance functions and functions which return the unsigned
// angle between 2 point3ds (relitive to 0,0,0)

class Point3d { // a point class, useful for 2d worlds
public:
  double x;
  double y;
  double z;

  const double pi = std::atan(1.0) * 4;

  Point3d() {
    x = 0.0;
    y = 0.0;
    z = 0.0;
  }
  Point3d(double _x, double _y, double _z = 0.0)
      : x(_x), y(_y), z(_z) {} // construct with x,y,z

  void set(double _x, double _y, double _z = 0.0) {
    x = _x;
    y = _y;
    z = _z;
  }

  void show() { // print x and y of this
    std::cout << x << "," << y << "," << z << "\n";
  }

  Point3d operator=(Point3d other) { // scalar/dot product of this and other
    this->x = other.x;
    this->y = other.y;
    this->z = other.z;
    return *this;
  }

  double operator*(Point3d other) { // scalar/dot product of this and other
    return x * other.x + y * other.y + z * other.z;
  }

  Point3d scale(double mag) { // vector * scalar
    Point3d newVect(x * mag, y * mag, z * mag);
    return newVect;
  }

  Point3d cross_prod(Point3d other) // cross product of this and other
  {
    Point3d newPoint(y * other.z - other.y * z, other.x * z - x * other.z,
                     x * other.y - other.x * y);
    return (newPoint);
  }

  Point3d operator+(Point3d other) { // add this point and other
    Point3d newVect(x + other.x, y + other.y, z + other.z);
    return newVect;
  }

  Point3d operator-(Point3d other) { // subtract other from this point
    Point3d newVect(x - other.x, y - other.y, z - other.z);
    return newVect;
  }

  double dist() { // length between this point and 0,0,0
    return sqrt(x * x + y * y + z * z);
  }

  double dist(Point3d other) { // length between this point and other
    return (*this - other).dist();
  }

  double angle_between_radian(Point3d other) // return angle (positive) in
                                             // radians between this point and
                                             // other relative to origin (0,0)
  {
    if (x == other.x && y == other.y && z == other.z) {
      return (0);
    }
    if (x / other.x == y / other.y &&
        x / other.x == z / other.z) { // vectors are parallel
      return (0);
    }

    return acos((*this) * other / (dist() * other.dist()));
  }

  double angle_between_deg(Point3d other) // return (positive) angle in degrees
                                          // between this point and other
                                          // relative to origin (0,0)
  {
    return angle_between_radian(other) / pi * 180;
  }
};

//// test code
// cout << "in test mode!!!!" << endl;
//
// Point2d p1(2, 10);
// Point2d p2(10, 10);
//
// cout << "p1: ";
// p1.show();
// cout << "\np2: ";
// p2.show();
//
// cout << "\n   diff = ";
//(p1 - p2).show();
// cout << endl;
//
// cout << "\n   sum = ";
//(p1 + p2).show();
// cout << endl;
//
// cout << "\n   dot = " << (p1 * p2) << endl;
//
// cout << "\n   cross = " << p1.cross_prod(p2) << endl << endl;
//
// cout << "p1.dist() = " << p1.dist() << endl;
// cout << "p2.dist() = " << p2.dist() << endl;
//
// cout << "dist between = " << (p1 - p2).dist() << endl;
// cout << "dist between(with func) = " << p1.dist(p2) << endl;
//
// cout << "p1.angle_between_deg(p2) = " << p1.angle_between_deg(p2) << endl;
// cout << "p2.angle_between_deg(p1) = " << p2.angle_between_deg(p1) << endl;
// cout << "p1.angle_between_deg(p1) = " << p1.angle_between_deg(p1) << endl;
//
// cout << "2d test done!" << endl;
//
//
// Point3d pa(0, -10, 10);
// Point3d pb(0, -10, 10);
//
// cout << "pa: ";
// pa.show();
// cout << "\npb: ";
// pb.show();
//
// cout << "\n   diff = ";
//(pa - pb).show();
// cout << endl;
//
// cout << "\n   sum = ";
//(pa + pb).show();
// cout << endl;
//
// cout << "\n   dot = " << (pa * pb) << endl;
//
// cout << "\n   cross = ";
//(pa.cross_prod(pb)).show();
// cout << endl << endl;
//
// cout << "pa.dist() = " << pa.dist() << endl;
// cout << "pb.dist() = " << pb.dist() << endl;
//
// cout << "dist between = " << (pa - pb).dist() << endl;
// cout << "dist between(with func) = " << pa.dist(pb) << endl;
//
// cout << "pa.angle_between_deg(pb) = " << pa.angle_between_deg(pb) << endl;
// cout << "pb.angle_between_deg(pa) = " << pb.angle_between_deg(pa) << endl;
// cout << "pa.angle_between_deg(pa) = " << pa.angle_between_deg(pa) << endl;
//
// cout << "3d test done!" << endl;
//
// exit(1);
