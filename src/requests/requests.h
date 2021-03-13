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
  Json::Dict Process(const TransportManager& manager) const;
};

struct Bus {
  std::string name;
  Json::Dict Process(const TransportManager& manager) const;
};

struct Route {
  static Json::Array
  ParseItems(const std::vector<TransportRouter::RouteInfo::Item>& items);
  std::string from;
  std::string to;
  Json::Dict Process(const TransportManager& manager) const;
};

struct Map {
  Json::Dict Process(const TransportManager& managaer) const;
};

/* ------------------------------------ */

using JsonRequestsKeys = configuration::json::RequestsKeys;

template<typename DataType, typename KeysType>
using ParseFunction = RequestsHolder(*)(const DataType&, const KeysType&);


template<typename RequestType>
RequestsHolder ParseRequest(const Json::Dict& dict, const JsonRequestsKeys& keys) {
  visitors::JSONDeserializeVisitor visitor(dict, keys);
  RequestType request;
  Visit(visitor, request);
  return request;
}

template<typename DataType, typename KeysType>
auto ConvertTypeToFunction(const std::string& type) {
  static const std::unordered_map<string_view, ParseFunction<DataType, KeysType>> kTypeToParseFunction = {
      {"Bus", ParseRequest<BusRequest>},
      {"Stop", ParseRequest<StopRequest>},
      {"Route", ParseRequest<RouteRequest>},
      {"Map", ParseRequest<MapRequest>}
  };

  if (auto it = kTypeToParseFunction.find(type); it != kTypeToParseFunction.end()) {
    return it->second;
  }

  throw std::system_error(RequestError::kUnknownRequestType);
}

std::vector<RequestsHolder> ReadRequests(const Json::Array& array, const JsonRequestsKeys& keys);

std::vector<Json::Node> ProcessAll(const std::vector<Json::Node>& attrs,
                                   const TransportManager& transport_manager);

} // namespace requests