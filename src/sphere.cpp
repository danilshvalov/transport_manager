#include "sphere.h"

namespace Sphere {
double CalcDistance(const Point &lhs, const Point &rhs) {
  return std::acos(std::sin(lhs.lat) * std::sin(rhs.lat) +
                   std::cos(lhs.lat) * std::cos(rhs.lat) *
                       std::cos(std::abs(lhs.lon - rhs.lon))) *
         6371000;
}
std::ostream &operator<<(std::ostream &os, const Point &p) {
  return os << "(" << p.lat << ", " << p.lon << ")";
}
double ConvertToRadians(double degree) { return degree * M_PI / 180; }
Point Parse(std::istream &is) {
  double lat_degrees, lon_degrees;
  is >> lat_degrees;
  is.ignore(1);
  is >> lon_degrees;
  return Point{ConvertToRadians(lat_degrees), ConvertToRadians(lon_degrees)};
}

bool operator==(const Point &lhs, const Point &rhs) {
  return tie(lhs.lat, lhs.lon) == tie(rhs.lat, rhs.lon);
}
} // namespace Sphere
