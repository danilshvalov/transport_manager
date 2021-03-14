#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>
#include <sstream>
#include <unordered_map>
#include <functional>

#include "json.h"
#include "transport_manager.h"
#include "responses.h"
#include "requests_types.h"
#include "requests_json_visitor.h"
#include "requests_errors.h"

namespace requests {

/* FOR DELETION */

struct Stop {
  std::string name;
  json::Dict Process(const TransportManager& manager) const;
};

struct Bus {
  std::string name;
  json::Dict Process(const TransportManager& manager) const;
};

struct Route {
  static json::Array
  ParseItems(const std::vector<TransportRouter::RouteInfo::Item>& items);
  std::string from;
  std::string to;
  json::Dict Process(const TransportManager& manager) const;
};

struct Map {
  json::Dict Process(const TransportManager& managaer) const;
};

/* ------------------------------------ */

using JsonRequestsKeys = configuration::json::RequestsKeys;

std::vector<RequestsHolder> ReadRequests(const json::Array& data, const JsonRequestsKeys& keys);

std::vector<json::Node> ProcessAll(const std::vector<json::Node>& attrs,
                                   const TransportManager& transport_manager);

} // namespace requests