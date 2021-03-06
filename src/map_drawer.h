#pragma once

#include <algorithm>
#include <map>
#include <numeric>
#include <unordered_map>
#include <unordered_set>
#include <vector>


#include "descriptions/descriptions.h"
#include "json.h"
#include "svg/svg.h"

namespace MapDrawer {
struct StopInfo {
  std::string name;
  sphere::Point point;
};

bool IsAdjacentStops(const StopInfo &lhs, const StopInfo &rhs,
                     const descriptions::StopsDict &stops_dict);

class Projector {
private:
  std::unordered_map<std::string, Svg::Point> stop_points_;

  using PointId = size_t;

  struct PointIds {
    PointId lon_id = 1;
    PointId lat_id = 1;
  };

  double x_step_;
  double y_step_;

  double width_;
  double height_;
  double padding_;

public:
  Projector(const descriptions::StopsDict &stops_dict, double max_width,
            double max_height, double padding);
  Svg::Point GetPoint(const std::string &stop_name) const;
};

struct LabelSettings {
  uint32_t font_size;
  string font_family;
  Svg::Color color;
  Svg::Point offset;
  bool is_bold;
};

struct CircleSettings {
  double radius;
  Svg::Color color;
};
struct UnderlayerSettings {
  double width;
  Svg::Color color;
  string line_join;
  string line_cap;
};

struct LineSettings {
  double line_width;
  string line_join;
  string line_cap;
};

struct MapSettings {
  double width;
  double height;
  double padding;
  LabelSettings stop_label_settings;
  LabelSettings bus_label_settings;
  CircleSettings circle_settings;
  UnderlayerSettings underlayer_settings;
  LineSettings line_settings;
  std::vector<Svg::Color> color_palette;
  std::vector<std::string> layers;
};

std::vector<std::string> ParseLayers(const json::Array &array);
Svg::Color ParseColor(const json::Node &node);
std::vector<Svg::Color> ParseColorPalette(const json::Array &dict);
Svg::Point ParseOffset(const json::Array &array);

class Drawer {
public:
private:
  std::vector<Svg::Circle> stop_circles_;
  std::vector<Svg::Text> stop_names_;
  std::vector<Svg::Polyline> bus_lines_;
  std::vector<Svg::Text> bus_names_;
  MapSettings settings_;
  Svg::Document document_;

  void InitSettings(const json::Dict &settings);
  void PrepareStops(const map<string, Svg::Point> &stop_points);
  void PrepareBuses(const descriptions::BusesDict &buses_dict,
                    const map<string, Svg::Point> &stop_points);
  void PrepareLabel(Svg::Text label, vector<Svg::Text> &container,
                    const LabelSettings &label_settings);
  void PrepareBusLine(Svg::Polyline line);
  void ConfigureLabel(Svg::Text &label, const LabelSettings &label_settings);
  void ConfigureTextUnderlayer(Svg::Text &underlayer) const;
  void ConfigureStopCircle(Svg::Circle &circle) const;
  void ConfigureBusLine(Svg::Polyline &polyline) const;
  void ProcessLayers(const std::vector<std::string> &layers);

  std::map<std::string, Svg::Point>
  ParseStopPoints(const descriptions::StopsDict &stops_dict);

public:
  explicit Drawer(const MapSettings &settings);
  explicit Drawer(const descriptions::BusesDict &buses_dict,
                  const descriptions::StopsDict &stops_dict,
                  const json::Dict &settings);
  const Svg::Document &GetMap() const;
  Drawer &AddStopNames();
  Drawer &AddStopCircles();
  Drawer &AddBusLines();
  Drawer &AddBusNames();
};

std::map<std::string, Svg::Point>
ParseStopPoints(const descriptions::StopsDict &stops_dict);
} // namespace MapDrawer
