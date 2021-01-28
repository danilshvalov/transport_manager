#pragma once

#include <algorithm>
#include <cassert>
#include <cmath>
#include <iostream>
#include <unordered_map>

#include "svg.h"
#include "utils.h"

namespace Sphere {

struct Point {
  double lat = 0;
  double lon = 0;
  friend std::ostream &operator<<(std::ostream &os, const Point &p);
  friend bool operator==(const Point &lhs, const Point &rhs);
  struct Hasher {
    size_t operator()(const Point &point) const {
      return std::hash<double>()(point.lon) * std::hash<double>()(point.lat);
    }
  };
};

template <typename PointIt> class Projector {
private:
  double padding_;
  double height_;
  double x_step_;
  double y_step_;

  using PointId = size_t;

  struct PointInfo {
    PointId lon_id = 0;
    PointId lat_id = 0;
  };

  std::unordered_map<typename PointIt::value_type, PointInfo,
                     typename PointIt::value_type::Hasher>
      points_;

public:
  Projector(const Range<PointIt> &points_range, double max_width,
            double max_height, double padding)
      : padding_(padding), height_(max_height) {
    vector<typename PointIt::value_type> lon_list(points_range.begin(),
                                                  points_range.end());
    sort(lon_list.begin(), lon_list.end(),
         [](const auto &lhs, const auto &rhs) { return lhs.lon < rhs.lon; });

    vector<typename PointIt::value_type> lat_list(points_range.begin(),
                                                  points_range.end());
    sort(lat_list.begin(), lat_list.end(),
         [](const auto &lhs, const auto &rhs) { return lhs.lat < rhs.lat; });

    PointId lon_id = 0;
    PointId lat_id = 0;

    for (size_t i = 1; i <= lon_list.size(); ++i) {
      points_[lon_list[i - 1]].lon_id = lon_id;
      points_[lat_list[i - 1]].lat_id = lat_id;
      if (i != lon_list.size()) {
        if (lon_list[i].lon != lon_list[i - 1].lon) {
          ++lon_id;
        }
        if (lat_list[i].lat != lat_list[i - 1].lat) {
          ++lat_id;
        }
      }
    }

    assert(lon_list.size() == points_.size());

    x_step_ = (max_width - 2 * padding_) / (lon_id == 0 ? 1 : lon_id) ;
    y_step_ = (max_height - 2 * padding_) / (lat_id == 0 ? 1 : lat_id);
  }

  Svg::Point GetPoint(const Point &point) {
    const auto &point_ids = points_[point];
    return {.x = point_ids.lon_id * x_step_ + padding_,
            .y = height_ - padding_ - point_ids.lat_id * y_step_};
  }
};

double CalcDistance(const Point &lhs, const Point &rhs);

double ConvertToRadians(double degree);

Point Parse(std::istream &is);

} // namespace Sphere
