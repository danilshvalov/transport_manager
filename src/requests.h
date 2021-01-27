#pragma once

#include <string>
#include <vector>
#include <variant>
#include <map>

#include "json.h"
#include "transport_manager.h"

namespace Requests {
struct Stop {
  std::string name;
  Json::Dict Process(const TransportManager &manager) const;
};

struct Bus {
  std::string name;
  Json::Dict Process(const TransportManager &manager) const;
};

struct Route {
  static Json::Array ParseItems(const std::vector<TransportRouter::RouteInfo::Item> &items);
  std::string from;
  std::string to;
  Json::Dict Process(const TransportManager &manager) const;
};

std::variant<Bus, Stop, Route> ReadRequest(const Json::Dict &attrs);

std::vector<Json::Node> ProcessAll(const std::vector<Json::Node> &attrs,
                                   const TransportManager &transport_manager);

}