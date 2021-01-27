#pragma once

#include <algorithm>
#include <map>
#include <numeric>
#include <vector>

#include "description.h"
#include "json.h"
#include "svg.h"

namespace MapDrawer {

struct TextSettings {
  uint32_t font_size;
  string font_family;
  Svg::Color color;
  Svg::Point offset;
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
  TextSettings text_settings;
  CircleSettings circle_settings;
  UnderlayerSettings underlayer_settings;
  LineSettings line_settings;
  std::vector<Svg::Color> color_palette;
};

Svg::Color ParseColor(const Json::Node &node);
std::vector<Svg::Color> ParseColorPalette(const Json::Array &dict);
Svg::Point ParseOffset(const Json::Array &array);

class Drawer {
public:
private:
  std::vector<Svg::Circle> circles_;
  std::vector<Svg::Text> texts_;
  std::vector<Svg::Polyline> polylines_;
  MapSettings settings_;
  Svg::Document document_;

  void InitSettings(const Json::Dict &settings);
  void PrepareStops(const map<string, Svg::Point> &stop_points);
  void PrepareBusLines(const map<string, Svg::Polyline> &bus_lines);
  void ConfigureText(Svg::Text& text) const;
  void ConfigureTextUnderlayer(Svg::Text& underlayer) const;
  void ConfigureCircle(Svg::Circle& circle) const;
  void ConfigurePolyline(Svg::Polyline& polyline) const;

  std::map<std::string, Svg::Point>
  ParseStopPoints(const Descriptions::StopsDict &stops_dict);
  std::map<std::string, Svg::Polyline>
  ParseBuses(const Descriptions::BusesDict &buses_dict,
             const map<string, Svg::Point> &stop_points);

public:
  explicit Drawer(const MapSettings &settings);
  explicit Drawer(const Descriptions::BusesDict &buses_dict,
                  const Descriptions::StopsDict &stops_dict,
                  const Json::Dict &settings);
  const Svg::Document& GetMap() const;
  Drawer &AddStopNames();
  Drawer &AddStopCircles();
  Drawer &AddBusLines();
};

std::map<std::string, Svg::Point>
ParseStopPoints(const Descriptions::StopsDict &stops_dict);
} // namespace MapDrawer
