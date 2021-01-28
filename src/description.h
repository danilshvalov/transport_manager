#pragma once

#include "json.h"
#include "sphere.h"

#include <vector>
#include <variant>
#include <unordered_map>

namespace Descriptions {

template<typename Object>
using Dict = std::unordered_map<std::string, const Object *>;

struct Stop {
  std::string name;
  Sphere::Point position;
  std::unordered_map<std::string, double> distances;

  static Stop ParseFrom(const Json::Dict &input);
};

struct Bus {
  std::string name;
  std::vector<std::string> stop_list;
  bool is_roundtrip;

  static Bus ParseFrom(const Json::Dict &input);
};

using InputQuery = std::variant<Stop, Bus>;
using StopsDict = Dict<Stop>;
using BusesDict = Dict<Bus>;

double ComputeStopDistance(const Stop &lhs, const Stop &rhs);


std::vector<std::string> ParseStops(const std::vector<Json::Node> &stop_list, bool is_roundtrip);
std::unordered_map<std::string, double> ParseRoadDistances(const Json::Dict &distances);
InputQuery ParseDescription(const Json::Dict &input);

std::vector<InputQuery> ReadDescriptions(const std::vector<Json::Node> &input);

}



