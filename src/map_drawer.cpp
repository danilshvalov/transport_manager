#include "map_drawer.h"

// TODO remove test_runner.h
#include "test_runner.h"
#include <fstream>

using namespace std;

namespace MapDrawer {

Svg::Color ParseColor(const Json::Node &node) {
  if (node.IsString()) {
    return {node.AsString()};
  } else if (node.IsArray()) {
    const auto &array = node.AsArray();

    if (array.size() == 3 || array.size() == 4) {
      auto rgb = Svg::Rgb{Svg::Rgb{
          .red = static_cast<uint8_t>(array[0].AsInt()),
          .green = static_cast<uint8_t>(array[1].AsInt()),
          .blue = static_cast<uint8_t>(array[2].AsInt()),
      }};

      if (array.size() == 3) {
        return {rgb};
      } else {
        return {Svg::Rgba{rgb, array[3].AsDouble()}};
      }
    }
  }

  return Svg::NoneColor;
}

std::vector<Svg::Color> ParseColorPalette(const Json::Array &array) {
  std::vector<Svg::Color> result;
  for (const auto &color : array) {
    result.push_back(ParseColor(color));
  }
  return result;
}

Svg::Point ParseOffset(const Json::Array &array) {
  return {array[0].AsDouble(), array[1].AsDouble()};
}

map<string, Svg::Point>
Drawer::ParseStopPoints(const Descriptions::StopsDict &stops_dict) {
  vector<Sphere::Point> stops_points;
  stops_points.reserve(stops_dict.size());

  for (const auto &[_, stop] : stops_dict) {
    stops_points.push_back(stop->position);
  }

  auto projector =
      Sphere::Projector(Range(stops_points.begin(), stops_points.end()),
                        settings_.width, settings_.height, settings_.padding);

  map<string, Svg::Point> result;

  for (const auto &[_, stop] : stops_dict) {
    result.insert({stop->name, projector.GetPoint(stop->position)});
  }

  return result;
}

map<string, Svg::Polyline>
Drawer::ParseBuses(const Descriptions::BusesDict &buses_dict,
                   const map<string, Svg::Point> &stop_points) {
  map<string, Svg::Polyline> result;

  for (const auto &[_, bus] : buses_dict) {
    Svg::Polyline line;
    for (const auto &stop : bus->stop_list) {
      line.AddPoint(stop_points.at(stop));
    }

    result.insert({bus->name, line});
  }

  size_t color_idx = 0;

  for (auto &[_, bus_line] : result) {
    bus_line.SetStrokeColor(settings_.color_palette[color_idx]);
    color_idx = (color_idx + 1) % (settings_.color_palette.size());
  }

  return result;
}

Drawer::Drawer(const MapSettings &settings) : settings_(settings) {}
Drawer::Drawer(const Descriptions::BusesDict &buses_dict,
               const Descriptions::StopsDict &stops_dict,
               const Json::Dict &settings) {
  InitSettings(settings);

  auto stop_points = ParseStopPoints(stops_dict);
  auto bus_lines = ParseBuses(buses_dict, stop_points);

  PrepareStops(stop_points);
  PrepareBusLines(bus_lines);
}

void Drawer::InitSettings(const Json::Dict &settings) {
  settings_ = MapSettings{
      .width = settings.at("width").AsDouble(),
      .height = settings.at("height").AsDouble(),
      .padding = settings.at("padding").AsDouble(),
      .text_settings =
          {
              .font_size = static_cast<uint32_t>(
                  settings.at("stop_label_font_size").AsInt()),
              .font_family = "Verdana",
              .color = "black",
              .offset = ParseOffset(settings.at("stop_label_offset").AsArray()),
          },
      .circle_settings = {.radius = settings.at("stop_radius").AsDouble(),
                          .color = "white"},
      .underlayer_settings = {.width =
                                  settings.at("underlayer_width").AsDouble(),
                              .color =
                                  ParseColor(settings.at("underlayer_color")),
                              .line_join = "round",
                              .line_cap = "round"},
      .line_settings = {.line_width = settings.at("line_width").AsDouble(),
                        .line_join = "round",
                        .line_cap = "round"},
      .color_palette =
          ParseColorPalette(settings.at("color_palette").AsArray())};
}

void Drawer::PrepareStops(const map<string, Svg::Point> &stop_points) {
  for (const auto &[stop_name, stop_point] : stop_points) {
    Svg::Text text_blank;
    text_blank.SetPoint(stop_point).SetData(stop_name);
    ConfigureText(text_blank);

    Svg::Text text_underlayer = text_blank;
    ConfigureTextUnderlayer(text_underlayer);

    texts_.push_back(move(text_underlayer));
    texts_.push_back(move(text_blank));

    Svg::Circle circle_blank;
    circle_blank.SetCenter(stop_point);
    ConfigureCircle(circle_blank);
    circles_.push_back(move(circle_blank));
  }
}

void Drawer::PrepareBusLines(const map<string, Svg::Polyline> &bus_lines) {
  for (auto [_, bus_line] : bus_lines) {
    ConfigurePolyline(bus_line);
    polylines_.push_back(move(bus_line));
  }
}

void Drawer::ConfigureText(Svg::Text &text) const {
  const auto &text_settings = settings_.text_settings;

  text.SetFontFamily(text_settings.font_family)
      .SetFontSize(text_settings.font_size)
      .SetFillColor(text_settings.color)
      .SetOffset(text_settings.offset);
}

void Drawer::ConfigureTextUnderlayer(Svg::Text &underlayer) const {
  const auto &underlayer_settings = settings_.underlayer_settings;

  underlayer.SetFillColor(underlayer_settings.color)
      .SetStrokeColor(underlayer_settings.color)
      .SetStrokeWidth(underlayer_settings.width)
      .SetStrokeLineCap(underlayer_settings.line_cap)
      .SetStrokeLineJoin(underlayer_settings.line_join);
}

void Drawer::ConfigureCircle(Svg::Circle &circle) const {
  const auto &circle_settings = settings_.circle_settings;

  circle.SetRadius(circle_settings.radius).SetFillColor(circle_settings.color);
}

void Drawer::ConfigurePolyline(Svg::Polyline &polyline) const {
  const auto &line_settings = settings_.line_settings;

  polyline.SetStrokeLineCap(line_settings.line_cap)
      .SetStrokeLineJoin(line_settings.line_join)
      .SetStrokeWidth(line_settings.line_width);
}

const Svg::Document &Drawer::GetMap() const {
  // ofstream out("out.svg");
  // document_.Render(out);
  return document_;
}

Drawer &Drawer::AddBusLines() {
  for (const auto &line : polylines_) {
    document_.Add(line);
  }
  return *this;
}

Drawer &Drawer::AddStopCircles() {
  for (const auto &circle : circles_) {
    document_.Add(circle);
  }

  return *this;
}

Drawer &Drawer::AddStopNames() {
  for (const auto &text : texts_) {
    document_.Add(text);
  }
  return *this;
}
} // namespace MapDrawer
