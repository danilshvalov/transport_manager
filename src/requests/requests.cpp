#include "requests.h"
#include "requests_errors.h"
#include "requests_json_visitor.h"

using namespace std;

namespace requests {

/* FOR DELETION */
Json::Dict Stop::Process(const TransportManager& manager) const {
  if (const auto& stop = manager.GetStop(name); stop) {
    return Json::Dict{{"buses", [&]() {
      vector<Json::Node> buses_nodes;
      buses_nodes.reserve(stop->buses.size());

      for (const auto& bus : stop->buses) {
        buses_nodes.emplace_back(bus);
      }

      return buses_nodes;
    }()}};
  }

  return Json::Dict{{"error_message", "not found"s}};
}
Json::Dict Bus::Process(const TransportManager& manager) const {
  if (const auto& bus = manager.GetBus(name); bus) {
    return Json::Dict{
        {"route_length", Json::Node{bus->route_length}},
        {"curvature", Json::Node{bus->curvature}},
        {"stop_count", Json::Node{static_cast<int>(bus->stop_count)}},
        {"unique_stop_count",
         Json::Node{static_cast<int>(bus->unique_stop_count)}}};
  }
  return Json::Dict{{"error_message", "not found"s}};
}

/* ------------------------------------ */


/* FOR DELETION */
//std::vector<Json::Node> ProcessAll(const std::vector<Json::Node>& attrs,
//                                   const DataType& transport_manager) {
//  std::vector<Json::Node> result;
//  result.reserve(attrs.size());
//
//  for (const auto& node : attrs) {
//    auto response = std::visit(
//        [&transport_manager](const auto& request) {
//          return request.Process(transport_manager);
//        },
//        ReadRequests(node.AsMap()));
//    response["request_id"] = node.AsMap().at("id");
//    result.emplace_back(std::move(response));
//  }
//
//  return result;
//}
Json::Dict Route::Process(const TransportManager& manager) const {
  const auto& route = manager.FindRoute(from, to);

  if (route) {
    return Json::Dict{{"total_time", Json::Node(route->total_time)},
                      {"items", ParseItems(route->items)}};
  }

  return Json::Dict{{"error_message", Json::Node("not found"s)}};
}

Json::Dict Map::Process(const TransportManager& managaer) const {
  stringstream dict;
  managaer.GetMap().Render(dict);

  return {{"dict", dict.str()}};
}
/* ------------------------------------ */



Json::Array
Route::ParseItems(const std::vector<TransportRouter::RouteInfo::Item>& items) {
  Json::Array result;
  for (const auto& item : items) {
    if (std::holds_alternative<TransportRouter::RouteInfo::BusItem>(item)) {
      const auto& bus_item =
          std::get<TransportRouter::RouteInfo::BusItem>(item);
      result.emplace_back(Json::Dict{
          {"type", Json::Node("Bus"s)},
          {"bus", Json::Node(bus_item.bus_name)},
          {"span_count", Json::Node(static_cast<int>(bus_item.span_count))},
          {"time", Json::Node(bus_item.time)}});
    } else if (std::holds_alternative<TransportRouter::RouteInfo::StopItem>(
        item)) {
      const auto& stop_item =
          std::get<TransportRouter::RouteInfo::StopItem>(item);
      result.emplace_back(
          Json::Dict{{"type", Json::Node("Wait"s)},
                     {"stop_name", Json::Node(stop_item.stop_name)},
                     {"time", Json::Node(stop_item.time)}});
    }
  }

  return result;
}
std::vector<RequestsHolder> ReadRequests(const Json::Array& array, const JsonRequestsKeys& keys) {
  std::vector<RequestsHolder> result;
  result.reserve(array.size());

  for (const auto& node_dict : array) {
    const auto& dict = node_dict.AsMap();
    const auto& func =
        ConvertTypeToFunction<decltype(dict), decltype(keys)>(dict.at(keys.type).AsString());
    result.push_back(func(dict, keys));
  }

  return result;
}
} // namespace requests
