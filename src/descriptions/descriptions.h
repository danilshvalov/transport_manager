#pragma once

#include "json.h"
#include "sphere.h"
#include "configuration.h"

#include <vector>
#include <variant>
#include <unordered_map>

namespace descriptions {

template<typename Object>
using Dict = std::unordered_map<std::string, const Object *>;

struct Description {
  template<typename Visitable, typename Visitor>
  static void Accept(Visitable &visitable, Visitor &visitor) {
    visitor(visitable);
  }
};

struct StopDescription : VisitableType {
  using DistancesContainer = std::unordered_map<std::string, double>;

  std::string name;
  sphere::Point position;
  DistancesContainer distances;
};

struct BusDescription : VisitableType {
  using StopListContainer = std::vector<std::string>;

  std::string name;
  StopListContainer stop_list;
  bool is_roundtrip;
};

// FOR DELETION
/* ------------------------------------------------ */
struct Stop {
  using DistancesContainer = std::unordered_map<std::string, double>;

  std::string name;
  sphere::Point position;
  DistancesContainer distances;

  static Stop ParseFrom(const Json::Dict &input);
};

struct Bus {
  std::string name;
  std::vector<std::string> stop_list;
  bool is_roundtrip;

  static Bus ParseFrom(const Json::Dict &input);
};
/* ------------------------------------------------ */


using InputQuery = std::variant<Stop, Bus>;
using DescriptionsHolder = std::variant<StopDescription, BusDescription>;
using StopsDict = Dict<Stop>;
using BusesDict = Dict<Bus>;

double ComputeStopDistance(const Stop &lhs, const Stop &rhs);

std::vector<std::string> ParseStops(const std::vector<Json::Node> &stop_list, bool is_roundtrip);
std::unordered_map<std::string, double> ParseRoadDistances(const Json::Dict &distances);
DescriptionsHolder ParseDescription(const Json::Dict &input, const configuration::json::DescriptionsKeys& keys);

std::vector<DescriptionsHolder> ReadDescriptions(const std::vector<Json::Node> &input);


template<typename DataType, typename ConfigType>
std::vector<DescriptionsHolder> ReadDescriptionsNew(const std::vector<DataType> &input, const ConfigType &cfg) {
  std::vector<DescriptionsHolder> input_query;
  input_query.reserve(input.size());

  for (const auto &item : input) {
    input_query.emplace_back(ParseDescription(item.AsMap(), cfg));
  }

  return input_query;
}

}



