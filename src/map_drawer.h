#pragma once

#include <algorithm>
#include <map>
#include <numeric>
#include <vector>

#include "description.h"
#include "json.h"
#include "svg.h"

namespace MapDrawer {

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

std::vector<std::string> ParseLayers(const Json::Array &array);
Svg::Color ParseColor(const Json::Node &node);
std::vector<Svg::Color> ParseColorPalette(const Json::Array &dict);
Svg::Point ParseOffset(const Json::Array &array);

class Drawer {
public:
private:
  std::vector<Svg::Circle> stop_circles_;
  std::vector<Svg::Text> stop_names_;
  std::vector<Svg::Polyline> bus_lines_;
  std::vector<Svg::Text> bus_names_;
  MapSettings settings_;
  Svg::Document document_;

  void InitSettings(const Json::Dict &settings);
  void PrepareStops(const map<string, Svg::Point> &stop_points);
  void PrepareBuses(const Descriptions::BusesDict &buses_dict,
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
  ParseStopPoints(const Descriptions::StopsDict &stops_dict);

public:
  explicit Drawer(const MapSettings &settings);
  explicit Drawer(const Descriptions::BusesDict &buses_dict,
                  const Descriptions::StopsDict &stops_dict,
                  const Json::Dict &settings);
  const Svg::Document &GetMap() const;
  Drawer &AddStopNames();
  Drawer &AddStopCircles();
  Drawer &AddBusLines();
  Drawer &AddBusNames();
};

std::map<std::string, Svg::Point>
ParseStopPoints(const Descriptions::StopsDict &stops_dict);
} // namespace MapDrawer
