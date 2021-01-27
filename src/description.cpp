#include "description.h"

namespace Descriptions {
std::unordered_map<std::string, double> ParseRoadDistances(const Json::Dict &distances) {
  return {distances.begin(), distances.end()};
}

Stop Stop::ParseFrom(const Json::Dict &input) {
  return Stop{
      .name = input.at("name").AsString(),
      .position = Sphere::Point{Sphere::ConvertToRadians(input.at("latitude").AsDouble()),
                                Sphere::ConvertToRadians(input.at("longitude").AsDouble())},
      .distances = ParseRoadDistances(input.at("road_distances").AsMap())
  };
}

Bus Bus::ParseFrom(const Json::Dict &input) {
  return Bus{.name = input.at("name").AsString(),
      .stop_list = ParseStops(input.at("stops").AsArray(),
                              input.at("is_roundtrip").AsBool())};
}
InputQuery ParseDescription(const Json::Dict &input) {
  if (auto type = input.at("type").AsString(); type == "Bus") {
    return Bus::ParseFrom(input);
  }
  return Stop::ParseFrom(input);
}

std::vector<InputQuery> ReadDescriptions(const std::vector<Json::Node> &input) {
  std::vector<InputQuery> input_query;
  input_query.reserve(input.size());

  for (const auto &item : input) {
    input_query.emplace_back(ParseDescription(item.AsMap()));
  }

  return input_query;
}
double ComputeStopDistance(const Stop &lhs, const Stop &rhs) {
  if (auto it = lhs.distances.find(rhs.name); it != end(lhs.distances)) {
    return it->second;
  } else {
    return rhs.distances.at(lhs.name);
  }
}
std::vector<std::string> ParseStops(const std::vector<Json::Node> &stop_list, bool is_roundtrip) {
  std::vector<std::string> result;
  result.reserve((is_roundtrip ? stop_list.size() : stop_list.size() * 2 - 1));

  for (const auto &stop_node : stop_list) {
    result.push_back(stop_node.AsString());
  }

  if (!is_roundtrip) {
    for (size_t i = stop_list.size() - 1; i > 0; --i) {
      result.push_back(stop_list[i - 1].AsString());
    }
  }

  return result;
}
}