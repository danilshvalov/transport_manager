#pragma once

#include "sphere.h"
#include "configuration.h"
#include "json.h"

#include <vector>
#include <variant>
#include <unordered_map>

namespace descriptions {

struct Description {
  template<typename Visitable, typename Visitor>
  static void Accept(Visitable &visitable, Visitor &visitor) {
    visitor(visitable);
  }
};

struct StopDescription : Description {
  using DistancesContainer = std::unordered_map<std::string, double>;

  std::string name;
  sphere::Point position;
  DistancesContainer distances;
};

struct BusDescription : Description {
  using StopListContainer = std::vector<std::string>;

  std::string name;
  StopListContainer stop_list;
  bool is_roundtrip;
};


template<typename Object>
using Dict = std::unordered_map<std::string, const Object *>;
using DescriptionsHolder = std::variant<StopDescription, BusDescription>;
using JsonDescriptionsKeys = configuration::json::DescriptionsKeys;
using StopDescriptionsDict = Dict<StopDescription>;
using BusDescriptionsDict = Dict<BusDescription>;
}