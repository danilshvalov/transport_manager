#pragma once

#include <algorithm>
#include <cmath>
#include <iostream>

#include "svg.h"
#include "utils.h"

namespace Sphere {

struct Point {
  double lat = 0;
  double lon = 0;
  friend std::ostream &operator<<(std::ostream &os, const Point &p);
  friend bool operator==(const Point &lhs, const Point &rhs) {
    return tie(lhs.lat, lhs.lon) == tie(rhs.lat, rhs.lon);
  }
};

template <typename PointIt>
class Projector {
 private:
  double min_lon_ = 0;
  double max_lat_ = 0;
  double padding_;
  double zoom_coef_ = 0;

 public:
  Projector(const Range<PointIt> &points_range, double max_width,
            double max_height, double padding)
      : padding_(padding) {
    const auto [right_it, left_it] = std::minmax_element(
        std::begin(points_range), std::end(points_range),
        [](const auto &lhs, const auto &rhs) { return lhs.lat < rhs.lat; });

    const auto [bottom_it, top_it] = std::minmax_element(
        std::begin(points_range), std::end(points_range),
        [](const auto &lhs, const auto &rhs) { return lhs.lon < rhs.lon; });

    const double max_lon = top_it->lon;
    min_lon_ = bottom_it->lon;

    max_lat_ = left_it->lat;
    const double min_lat = right_it->lat;

    std::optional<double> width_zoom_coef;
    if (!IsZero(max_lon - min_lon_)) {
      width_zoom_coef = (max_width - 2 * padding) / (max_lon - min_lon_);
    }

    std::optional<double> height_zoom_coef;

    if (!IsZero(max_lat_ - min_lat)) {
      height_zoom_coef = (max_height - 2 * padding) / (max_lat_ - min_lat);
    }

    if (width_zoom_coef && height_zoom_coef) {
      zoom_coef_ = std::min(*width_zoom_coef, *height_zoom_coef);
    } else if (width_zoom_coef) {
      zoom_coef_ = *width_zoom_coef;
    } else if (height_zoom_coef) {
      zoom_coef_ = *height_zoom_coef;
    }
    
  }

  Svg::Point GetPoint(const Point &point) {
    return {
        .x = (point.lon - min_lon_) * zoom_coef_ + padding_,
        .y = (max_lat_ - point.lat) * zoom_coef_ + padding_,
    };
  }
};

double CalcDistance(const Point &lhs, const Point &rhs);

double ConvertToRadians(double degree);

Point Parse(std::istream &is);

}  // namespace Sphere
