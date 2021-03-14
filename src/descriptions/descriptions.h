#pragma once

#include "descriptions_types.h"

#include <vector>
#include <variant>
#include <unordered_map>

namespace descriptions {



// FOR DELETION
/* ------------------------------------------------ */
struct Stop {
  using DistancesContainer = std::unordered_map<std::string, double>;

  std::string name;
  sphere::Point position;
  DistancesContainer distances;

  static Stop ParseFrom(const json::Dict &input);
};

struct Bus {
  std::string name;
  std::vector<std::string> stop_list;
  bool is_roundtrip;

  static Bus ParseFrom(const json::Dict &input);
};

using StopsDict = Dict<Stop>;
using BusesDict = Dict<Bus>;
using InputQuery = std::variant<Stop, Bus>;
/* ------------------------------------------------ */


double ComputeStopDistance(const Stop &lhs, const Stop &rhs);


std::vector<DescriptionsHolder> ReadDescriptions(const json::Array &data, const JsonDescriptionsKeys &keys);

}



