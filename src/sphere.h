#pragma once

#define _USE_MATH_DEFINES

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <unordered_map>

#include "svg/svg.h"
#include "utils.h"


namespace sphere {

struct Point {
  double lat = 0;
  double lon = 0;
  friend std::ostream &operator<<(std::ostream &os, const Point &p);
  friend bool operator==(const Point &lhs, const Point &rhs);
};


double CalcDistance(const Point &lhs, const Point &rhs);

double ConvertToRadians(double degree);

Point Parse(std::istream &is);

} // namespace sphere
