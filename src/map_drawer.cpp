#include "map_drawer.h"

// TODO fstream
#include <fstream>

using namespace std;

namespace MapDrawer {

bool IsAdjacentStops(const StopInfo &lhs, const StopInfo &rhs,
                     const descriptions::StopsDict &stops_dict) {
  return stops_dict.at(lhs.name)->distances.count(rhs.name) != 0 ||
         stops_dict.at(rhs.name)->distances.count(lhs.name) != 0;
}

Projector::Projector(const descriptions::StopsDict &stops_dict,
                     double max_width, double max_height, double padding)
    : width_(max_width), height_(max_height), padding_(padding) {
  unordered_map<string, PointIds> stop_ids;

  vector<StopInfo> lon_list = [&stops_dict]() {
    vector<StopInfo> list;
    list.reserve(stops_dict.size());
    for (const auto &[_, stop] : stops_dict) {
      list.push_back({stop->name, stop->position});
    }
    return list;
  }();
  vector<StopInfo> lat_list = lon_list;

  sort(lon_list.begin(), lon_list.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.point.lon < rhs.point.lon;
  });

  sort(lat_list.begin(), lat_list.end(), [](const auto &lhs, const auto &rhs) {
    return lhs.point.lat < rhs.point.lat;
  });

  vector<vector<StopInfo>> stops_lon;
  vector<vector<StopInfo>> stops_lat;

  PointId lon_id = 0;
  PointId lat_id = 0;
  for (size_t i = 1; i <= stops_dict.size(); ++i) {

    stop_ids[lon_list[i - 1].name].lon_id = lon_id;
    stop_ids[lat_list[i - 1].name].lat_id = lat_id;

    if (i != stops_dict.size()) {

      if (stops_lon.size() <= lon_id) {
        stops_lon.push_back({lon_list[i - 1]});
      } else {
        stops_lon[lon_id].push_back(lon_list[i - 1]);
      }

      if (stops_lat.size() <= lat_id) {
        stops_lat.push_back({lat_list[i - 1]});
      } else {
        stops_lat[lat_id].push_back(lat_list[i - 1]);
      }

      if (lon_list[i].point.lon != lon_list[i - 1].point.lon &&
          any_of(stops_lon[lon_id].begin(), stops_lon[lon_id].end(),
                 [&stops_dict, &lon_list, &i](const auto &stop) {
                   return IsAdjacentStops(stop, lon_list[i], stops_dict);
                 })) {
        ++lon_id;
      }
      if (lat_list[i].point.lat != lat_list[i - 1].point.lat &&
          any_of(stops_lat[lat_id].begin(), stops_lat[lat_id].end(),
                 [&stops_dict, &lat_list, &i](const auto &stop) {
                   return IsAdjacentStops(stop, lat_list[i], stops_dict);
                 })) {
        ++lat_id;
      }
    }

    // if (i != stops_dict.size()) {
    //   if (lon_list[i].point.lon != lon_list[i - 1].point.lon) {
    //     ++lon_id;
    //   }
    //   if (lat_list[i].point.lat != lat_list[i - 1].point.lat) {
    //     ++lat_id;
    //   }
    // }
  }

  // unordered_set<PointId> united_lon;
  // unordered_set<PointId> united_lat;

  // for (size_t i = 1; i < stops_lon.size(); ++i) {
  //   for (const auto &current_stop : stops_lon[i]) {
  //     if (all_of(stops_lon[i - 1].begin(), stops_lon[i - 1].end(),
  //                [&current_stop, &stops_dict](const StopInfo &info) {
  //                  return !IsAdjacentStops(info, current_stop, stops_dict);
  //                })) {
  //       united_lon.insert(i - 1);
  //     }
  //   }
  // }

  // for (size_t i = 1; i < stops_lat.size(); ++i) {
  //   for (const auto &current_stop : stops_lat[i]) {
  //     if (all_of(stops_lat[i - 1].begin(), stops_lat[i - 1].end(),
  //                [&current_stop, &stops_dict](const StopInfo &info) {
  //                  return !IsAdjacentStops(info, current_stop, stops_dict);
  //                })) {
  //       united_lat.insert(i - 1);
  //     }
  //   }
  // }

  // size_t lon_count = stops_lon.size() - united_lon.size() - 1;
  // size_t lat_count = stops_lat.size() - united_lat.size() - 1;

  x_step_ = (width_ - 2 * padding_) / (lon_id == 0 ? 1 : lon_id);
  y_step_ = (height_ - 2 * padding_) / (lat_id == 0 ? 1 : lat_id);

  // lon_id = 0;
  // lat_id = 0;
  // for (size_t idx = 0; idx < stops_lon.size(); ++idx) {
  //   stop_points_[]
  //   if (united_lon.count(idx) == 0) {
  //     ++lon_id;
  //   }
  // }

  for (const auto &[_, stop] : stops_dict) {
    stop_points_[stop->name] = Svg::Point{
        .x = stop_ids[stop->name].lon_id * x_step_ + padding_,
        .y = height_ - padding_ - stop_ids[stop->name].lat_id * y_step_};
  }
}

Svg::Point Projector::GetPoint(const std::string &stop_name) const {
  return stop_points_.at(stop_name);
}

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
Drawer::ParseStopPoints(const descriptions::StopsDict &stops_dict) {
  // vector<sphere::Point> stops_points;
  // stops_points.reserve(stops_dict.size());

  // for (const auto &[_, stop] : stops_dict) {
  //   stops_points.push_back(stop->position);
  // }

  auto projector = Projector(stops_dict, settings_.width, settings_.height,
                             settings_.padding);

  map<string, Svg::Point> result;

  for (const auto &[_, stop] : stops_dict) {
    result.insert({stop->name, projector.GetPoint(stop->name)});
  }

  return result;
  // return {};
}

Drawer::Drawer(const MapSettings &settings) : settings_(settings) {}
Drawer::Drawer(const descriptions::BusesDict &buses_dict,
               const descriptions::StopsDict &stops_dict,
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

void Drawer::PrepareBuses(const descriptions::BusesDict &buses_dict,
                          const map<string, Svg::Point> &stop_points) {

  vector<pair<string, const descriptions::Bus *>> buses_list(buses_dict.begin(),
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

void Drawer::ProcessLayers(const std::vector<std::string> &layers) {
  for (const auto &layer : layers) {
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
  ofstream out("out.svg");
  document_.Render(out);
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
