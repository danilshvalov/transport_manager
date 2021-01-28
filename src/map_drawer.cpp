#include "map_drawer.h"

// TODO fstream
#include <fstream>

using namespace std;

namespace MapDrawer {

vector<string> ParseLayers(const Json::Array &array) {
  vector<string> result;
  result.reserve(array.size());

  for (const auto &layer : array) {
    result.push_back(layer.AsString());
  }
  return result;
}

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

Drawer::Drawer(const MapSettings &settings) : settings_(settings) {}
Drawer::Drawer(const Descriptions::BusesDict &buses_dict,
               const Descriptions::StopsDict &stops_dict,
               const Json::Dict &settings) {
  InitSettings(settings);

  auto stop_points = ParseStopPoints(stops_dict);

  PrepareStops(stop_points);
  PrepareBuses(buses_dict, stop_points);

  ProcessLayers(settings_.layers);
}

void Drawer::InitSettings(const Json::Dict &settings) {
  settings_ = MapSettings{
      .width = settings.at("width").AsDouble(),
      .height = settings.at("height").AsDouble(),
      .padding = settings.at("padding").AsDouble(),
      .stop_label_settings = {.font_size = static_cast<uint32_t>(
                                  settings.at("stop_label_font_size").AsInt()),
                              .font_family = "Verdana",
                              .color = "black",
                              .offset = ParseOffset(
                                  settings.at("stop_label_offset").AsArray()),
                              .is_bold = false},
      .bus_label_settings = {.font_size = static_cast<uint32_t>(
                                 settings.at("bus_label_font_size").AsInt()),
                             .font_family = "Verdana",
                             .offset = ParseOffset(
                                 settings.at("bus_label_offset").AsArray()),
                             .is_bold = true},
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
          ParseColorPalette(settings.at("color_palette").AsArray()),
      .layers = ParseLayers(settings.at("layers").AsArray())};
}

void Drawer::PrepareStops(const map<string, Svg::Point> &stop_points) {
  const auto &stop_label_settings = settings_.stop_label_settings;

  for (const auto &[stop_name, stop_point] : stop_points) {
    Svg::Text text;
    text.SetPoint(stop_point).SetData(stop_name);
    PrepareLabel(move(text), stop_names_, stop_label_settings);

    Svg::Circle circle;
    circle.SetCenter(stop_point);
    ConfigureStopCircle(circle);
    stop_circles_.push_back(move(circle));
  }
}

void Drawer::PrepareBuses(const Descriptions::BusesDict &buses_dict,
                          const map<string, Svg::Point> &stop_points) {

  vector<pair<string, const Descriptions::Bus *>> buses_list(buses_dict.begin(),
                                                             buses_dict.end());
  sort(buses_list.begin(), buses_list.end());

  const auto &bus_label_settings = settings_.bus_label_settings;

  size_t color_idx = 0;

  for (const auto &[_, bus] : buses_list) {
    Svg::Polyline line;
    for (const auto &stop : bus->stop_list) {
      line.AddPoint(stop_points.at(stop));
    }

    Svg::Text name;
    name.SetData(bus->name);

    line.SetStrokeColor(settings_.color_palette[color_idx]);
    name.SetFillColor(settings_.color_palette[color_idx]);
    color_idx = (color_idx + 1) % (settings_.color_palette.size());

    PrepareBusLine(move(line));

    const auto &stop_list = bus->stop_list;

    if (!bus->is_roundtrip && stop_list.size() > 1 &&
        stop_list.front() != stop_list[stop_list.size() / 2]) {
      name.SetPoint(stop_points.at(stop_list.front()));
      PrepareLabel(name, bus_names_, bus_label_settings);
      name.SetPoint(stop_points.at(stop_list[stop_list.size() / 2]));
      PrepareLabel(move(name), bus_names_, bus_label_settings);
    } else {
      name.SetPoint(stop_points.at(stop_list.front()));
      PrepareLabel(move(name), bus_names_, bus_label_settings);
    }
  }
}

void Drawer::PrepareLabel(Svg::Text label, vector<Svg::Text> &container,
                          const LabelSettings &label_settings) {
  ConfigureLabel(label, label_settings);
  Svg::Text underlayer = label;
  ConfigureTextUnderlayer(underlayer);
  container.push_back(move(underlayer));
  container.push_back(move(label));
}

void Drawer::PrepareBusLine(Svg::Polyline line) {
  ConfigureBusLine(line);
  bus_lines_.push_back(move(line));
}

void Drawer::ConfigureLabel(Svg::Text &label,
                            const LabelSettings &label_settings) {
  label.SetFontFamily(label_settings.font_family)
      .SetFontSize(label_settings.font_size)
      .SetOffset(label_settings.offset)
      .SetBold(label_settings.is_bold);

  if (!holds_alternative<monostate>(label_settings.color)) {
    label.SetFillColor(label_settings.color);
  }
}

void Drawer::ConfigureTextUnderlayer(Svg::Text &underlayer) const {
  const auto &underlayer_settings = settings_.underlayer_settings;

  underlayer.SetFillColor(underlayer_settings.color)
      .SetStrokeColor(underlayer_settings.color)
      .SetStrokeWidth(underlayer_settings.width)
      .SetStrokeLineCap(underlayer_settings.line_cap)
      .SetStrokeLineJoin(underlayer_settings.line_join);
}

void Drawer::ConfigureStopCircle(Svg::Circle &circle) const {
  const auto &circle_settings = settings_.circle_settings;

  circle.SetRadius(circle_settings.radius).SetFillColor(circle_settings.color);
}

void Drawer::ConfigureBusLine(Svg::Polyline &polyline) const {
  const auto &line_settings = settings_.line_settings;

  polyline.SetStrokeLineCap(line_settings.line_cap)
      .SetStrokeLineJoin(line_settings.line_join)
      .SetStrokeWidth(line_settings.line_width);
}

void Drawer::ProcessLayers(const std::vector<std::string> &layers) 
{
  for (const auto& layer : layers) {
    if (layer == "bus_lines") {
      AddBusLines();
    } else if (layer == "bus_labels") {
      AddBusNames();
    } else if (layer == "stop_points") {
      AddStopCircles();
    } else if (layer == "stop_labels") {
      AddStopNames();
    }
  }
}

const Svg::Document &Drawer::GetMap() const {
  // ofstream out("out.svg");
  // document_.Render(out);
  return document_;
}

Drawer &Drawer::AddBusLines() {
  for (const auto &line : bus_lines_) {
    document_.Add(line);
  }
  return *this;
}

Drawer &Drawer::AddStopCircles() {
  for (const auto &circle : stop_circles_) {
    document_.Add(circle);
  }
  return *this;
}

Drawer &Drawer::AddBusNames() {
  for (const auto &text : bus_names_) {
    document_.Add(text);
  }
  return *this;
}

Drawer &Drawer::AddStopNames() {
  for (const auto &text : stop_names_) {
    document_.Add(text);
  }
  return *this;
}
} // namespace MapDrawer
