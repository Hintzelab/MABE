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

#include <vector>
#include <iostream>
#include <iomanip>

#include "PointNd.h"

// Vector2d is wraps a vector<T> and provides x,y style access
// no error checking is provided for out of range errors
// internally this class uses R(ow) and C(olumn) (i.e. how the data is stored in
// the data vector)
// the user sees x,y where x = column, y = row

template <typename T> class Vector2d {
  std::vector<T> data;
  int R, C;

  // get index into data vector for a given x,y
  inline int getIndex(int r, int c) { return (r * C) + c; }

public:
  Vector2d() {
    R = 0;
    C = 0;
  }
  // construct a vector of size x * y
  Vector2d(int x, int y) : R(y), C(x) { data.resize(R * C); }

  Vector2d(int x, int y, T value) : R(y), C(x) { data.resize(R * C, value); }

  void reset(int x, int y) {
    R = y;
    C = x;
    data.clear();
    data.resize(R * C);
  }

  void reset(int x, int y, T value) {
    R = y;
    C = x;
    data.clear();
    data.resize(R * C, value);
  }

  // overwrite this classes data (vector<T>) with data coppied from newData
  void assign(std::vector<T> newData) {
    if ((int)newData.size() != R * C) {
      std::cout << "  ERROR :: in Vector2d::assign() vector provided does not "
                   "fit. provided vector is size "
                << newData.size() << " but Rows(" << R << ") * Columns(" << C
                << ") == " << R * C << ". Exitting." << std::endl;
      exit(1);
    }
    data = newData;
  }

  // provides access to value x,y can be l-value or r-value (i.e. used for
  // lookup of assignment)
  T &operator()(int x, int y) { return data[getIndex(y, x)]; }

  T &operator()(double x, double y) {
    return data[getIndex((int)(y), (int)(x))];
  }

  T &operator()(std::pair<int, int> loc) {
    return data[getIndex(loc.second, loc.first)];
  }

  T &operator()(std::pair<double, double> loc) {
    return data[getIndex((int)(loc.second), (int)(loc.first))];
  }

  T &operator()(Point2d loc) { return data[getIndex((int)loc.y, (int)loc.x)]; }

  // show the contents of this Vector2d with index values, and x,y values
  void show() {
    for (int r = 0; r < R; r++) {
      for (int c = 0; c < C; c++) {
        std::cout << getIndex(r, c) << " : " << c << "," << r << " : "
                  << data[getIndex(r, c)] << "\n";
      }
    }
  }

  // show the contents of this Vector2d in a grid
  void showGrid(int precision = -1) {
    if (precision < 0) {
      for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
          std::cout << data[getIndex(r, c)] << " ";
        }
        std::cout << "\n";
      }
    } else {
      for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
          if (data[getIndex(r, c)] == 0) {
            std::cout << std::setfill(' ') << std::setw((precision * 2) + 2)
                      << " ";
          } else {
            std::cout << std::setfill(' ') << std::setw((precision * 2) + 1)
                      << std::fixed << std::setprecision(precision)
                      << data[getIndex(r, c)] << " ";
          }
        }
        std::cout << "\n";
      }
    }
  }
  int x() { return C; }

  int y() { return R; }
};

// Vector3d is wraps a vector<T> and provides x,y,z style access
// no error checking is provided for out of range errors
// internally this class uses R(ow), C(olumn) and B(in) (i.e. how the data is
// stored in the data vector)
// the user sees x,y,z where x = column, y = row and z = bin

template <typename T> class Vector3d {
  std::vector<T> data;
  int R, C, B;

  // get index into data vector for a given x,y,z
  inline int getIndex(int r, int c, int b) { return (r * C * B) + (c * B) + b; }

public:
  // construct a vector of size x * y * z
  Vector3d(int x, int y, int z) : R(y), C(x), B(z) { data.resize(C * R * B); }

  // overwrite this classes data (vector<T>) with data coppied from newData
  void assign(std::vector<T> newData, bool byBin = true) {
    if ((int)newData.size() != R * C * B) {
      std::cout << "  ERROR :: in Vector3d::assign() vector provided does not "
                   "fit. provided vector is size "
                << newData.size() << " but Rows(" << R << ") * Columns(" << C
                << ") * Bins(" << B << ") == " << R * C * B << ". Exitting."
                << std::endl;
      exit(1);
    }
    if (byBin) {
      int i = 0;
      for (int b = 0; b < B; b++) {
        for (int r = 0; r < R; r++) {
          for (int c = 0; c < C; c++) {
            std::cout << i << " " << newData[i] << "\n";
            data[getIndex(r, c, b)] = newData[i++];
          }
        }
      }
    } else {
      data = newData;
    }
  }

  // provides access to value x,y,z can be l-value or r-value (i.e. used for
  // lookup of assignment)
  T &operator()(int x, int y, int z) {
    return data[getIndex(y, x, z)]; // i.e. getIndex(r,c,b)
  }

  // returns vector of values for all z at x,y read only!
  std::vector<T> operator()(int x, int y) {
    std::vector<T> sub;
    for (int i = getIndex(y, x, 0); i < getIndex(y, x, B); i++) {
      sub.push_back(data[i]);
    }
    return sub;
  }

  // show the contents of this Vector3d with index values, and x,y,z values -
  // used for debuging
  void show() {
    for (int r = 0; r < R; r++) {
      for (int c = 0; c < C; c++) {
        for (int b = 0; b < B; b++) {
          std::cout << getIndex(r, c, b) << " : " << c << "," << r << "," << b
                    << " : " << data[getIndex(r, c, b)] << "\n";
        }
      }
    }
  }

  // show the contents of one x,y for this Vector3d with index z values - used
  // for debuging
  void show(int x, int y) {
    std::vector<T> sub = (*this)(x, y);
    for (int b = 0; b < B; b++) {
      std::cout << b << " : " << sub[b] << "\n";
    }
  }

  // show the contents for z of this Vector3d in a grid (default z = 0)
  void showGrid(int z = -1) {
    if (z == -1) { // show all
      for (int b = 0; b < B; b++) {
        std::cout << "\n"
                  << " bin " << b << " :"
                  << "\n";
        showGrid(b);
      }
    } else {
      for (int r = 0; r < R; r++) {
        for (int c = 0; c < C; c++) {
          std::cout << data[getIndex(r, c, z)] << " ";
        }
        std::cout << "\n";
      }
    }
  }

  int x() { return C; }

  int y() { return R; }

  int z() { return B; }
};
