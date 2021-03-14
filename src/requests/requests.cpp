#include "requests.h"
#include "requests_errors.h"
#include "requests_json_visitor.h"

using namespace std;

namespace requests {





/* FOR DELETION */
json::Dict Stop::Process(const TransportManager& manager) const {
  if (const auto& stop = manager.GetStop(name); stop) {
    return json::Dict{{"buses", [&]() {
      vector<json::Node> buses_nodes;
      buses_nodes.reserve(stop->buses.size());

      for (const auto& bus : stop->buses) {
        buses_nodes.emplace_back(bus);
      }

      return buses_nodes;
    }()}};
  }

  return json::Dict{{"error_message", "not found"s}};
}
json::Dict Bus::Process(const TransportManager& manager) const {
  if (const auto& bus = manager.GetBus(name); bus) {
    return json::Dict{
        {"route_length", json::Node{bus->route_length}},
        {"curvature", json::Node{bus->curvature}},
        {"stop_count", json::Node{static_cast<int>(bus->stop_count)}},
        {"unique_stop_count",
         json::Node{static_cast<int>(bus->unique_stop_count)}}};
  }
  return json::Dict{{"error_message", "not found"s}};
}

/* ------------------------------------ */

json::Dict Route::Process(const TransportManager& manager) const {
  const auto& route = manager.FindRoute(from, to);

  if (route) {
    return json::Dict{{"total_time", json::Node(route->total_time)},
                      {"items", ParseItems(route->items)}};
  }

  return json::Dict{{"error_message", json::Node("not found"s)}};
}

json::Dict Map::Process(const TransportManager& managaer) const {
  stringstream dict;
  managaer.GetMap().Render(dict);

  return {{"dict", dict.str()}};
}
/* ------------------------------------ */



json::Array
Route::ParseItems(const std::vector<TransportRouter::RouteInfo::Item>& items) {
  json::Array result;
  for (const auto& item : items) {
    if (std::holds_alternative<TransportRouter::RouteInfo::BusItem>(item)) {
      const auto& bus_item =
          std::get<TransportRouter::RouteInfo::BusItem>(item);
      result.emplace_back(json::Dict{
          {"type", json::Node("Bus"s)},
          {"bus", json::Node(bus_item.bus_name)},
          {"span_count", json::Node(static_cast<int>(bus_item.span_count))},
          {"time", json::Node(bus_item.time)}});
    } else if (std::holds_alternative<TransportRouter::RouteInfo::StopItem>(
        item)) {
      const auto& stop_item =
          std::get<TransportRouter::RouteInfo::StopItem>(item);
      result.emplace_back(
          json::Dict{{"type", json::Node("Wait"s)},
                     {"stop_name", json::Node(stop_item.stop_name)},
                     {"time", json::Node(stop_item.time)}});
    }
  }

  return result;
}

template<typename DataType, typename KeysType>
using ParseFunction = RequestsHolder(*)(const DataType&, const KeysType&);

template<typename RequestType>
RequestsHolder ParseRequest(const json::Dict& dict, const configuration::json::RequestsKeys& keys);

template<typename DataType, typename KeysType>
auto ConvertTypeToFunction(std::string_view type) {
  static const std::unordered_map<std::string_view, ParseFunction<DataType, KeysType>>
      kTypeToParseFunction = {
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


template<typename RequestType>
RequestsHolder ParseRequest(const json::Dict& dict, const configuration::json::RequestsKeys& keys) {
  visitors::JSONDeserializeVisitor visitor(dict, keys);
  RequestType request;
  Visit(visitor, request);
  return request;
}



std::vector<RequestsHolder> ReadRequests(const json::Array& data, const JsonRequestsKeys& keys) {
  std::vector<RequestsHolder> result;
  result.reserve(data.size());

  for (const auto& node_dict : data) {
    const auto& dict = node_dict.AsMap();
    const auto& func =
        ConvertTypeToFunction<decltype(dict), decltype(keys)>(dict.at(keys.type).AsString());
    result.push_back(func(dict, keys));
  }

  return result;
}
} // namespace requests
