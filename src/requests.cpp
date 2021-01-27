#include "requests.h"

using namespace std;

namespace Requests {

Json::Dict Stop::Process(const TransportManager &manager) const {
  if (const auto &stop = manager.GetStop(name); stop) {
    return Json::Dict{
        {"buses", [&]() {
          vector<Json::Node> buses_nodes;
          buses_nodes.reserve(stop->buses.size());

          for (const auto &bus : stop->buses) {
            buses_nodes.emplace_back(bus);
          }

          return buses_nodes;
        }()}
    };
  }

  return Json::Dict{{"error_message", "not found"s}};
}
Json::Dict Bus::Process(const TransportManager &manager) const {
  if (const auto &bus = manager.GetBus(name); bus) {
    return Json::Dict{
        {"route_length", Json::Node{bus->route_length}},
        {"curvature", Json::Node{bus->curvature}},
        {"stop_count", Json::Node{static_cast<int>(bus->stop_count)}},
        {"unique_stop_count", Json::Node{static_cast<int>(bus->unique_stop_count)}}
    };
  }
  return Json::Dict{{"error_message", "not found"s}};
}

std::variant<Bus, Stop, Route> ReadRequest(const Json::Dict &attrs) {
  if (const auto &type = attrs.at("type").AsString(); type == "Bus") {
    return Bus{.name = attrs.at("name").AsString()};
  } else if (type == "Route") {
    return Route{.from = attrs.at("from").AsString(), .to = attrs.at("to").AsString()};
  } else if (type == "Stop") {
    return Stop{.name = attrs.at("name").AsString()};
  }
  throw std::logic_error("Unexpected request type");
}
std::vector<Json::Node> ProcessAll(const std::vector<Json::Node> &attrs, const TransportManager &transport_manager) {
  std::vector<Json::Node> result;
  result.reserve(attrs.size());

  for (const auto &node :attrs) {
    auto response = std::visit([&transport_manager](const auto &request) {
      return request.Process(transport_manager);
    }, ReadRequest(node.AsMap()));
    response["request_id"] = node.AsMap().at("id");
    result.emplace_back(std::move(response));
  }

  return result;
}
Json::Dict Route::Process(const TransportManager &manager) const {
  const auto &route = manager.FindRoute(from, to);

  if (route) {
    return Json::Dict{
        {"total_time", Json::Node(route->total_time)},
        {"items", ParseItems(route->items)}
    };
  }

  return Json::Dict{{"error_message", Json::Node("not found"s)}};
}
Json::Array Route::ParseItems(const std::vector<TransportRouter::RouteInfo::Item> &items) {
  Json::Array result;
  for (const auto &item : items) {
    if (std::holds_alternative<TransportRouter::RouteInfo::BusItem>(item)) {
      const auto &bus_item = std::get<TransportRouter::RouteInfo::BusItem>(item);
      result.emplace_back(Json::Dict{
          {"type", Json::Node("Bus"s)},
          {"bus", Json::Node(bus_item.bus_name)},
          {"span_count", Json::Node(static_cast<int>(bus_item.span_count))},
          {"time", Json::Node(bus_item.time)}
      });
    } else if (std::holds_alternative<TransportRouter::RouteInfo::StopItem>(item)) {
      const auto &stop_item = std::get<TransportRouter::RouteInfo::StopItem>(item);
      result.emplace_back(Json::Dict{
          {"type", Json::Node("Wait"s)},
          {"stop_name", Json::Node(stop_item.stop_name)},
          {"time", Json::Node(stop_item.time)}
      });
    }
  }

  return result;
}
}
