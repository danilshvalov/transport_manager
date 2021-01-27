#include "transport_manager.h"

// TODO remove fstream
#include <fstream>

using namespace std;

const TransportManager::Stop *
TransportManager::GetStop(const std::string &name) const {
  return GetValuePointer(stops, name);
}

const TransportManager::Bus *
TransportManager::GetBus(const std::string &name) const {
  return GetValuePointer(buses, name);
}
double TransportManager::ComputeRouteLength(
    const std::vector<std::string> &stop_list,
    std::unordered_map<std::string, const Descriptions::Stop *> &stops_info) {
  double result = 0;
  for (size_t i = 1; i < stop_list.size(); ++i) {
    result += Descriptions::ComputeStopDistance(
        *stops_info.at(stop_list[i - 1]), *stops_info.at(stop_list[i]));
  }
  return result;
}
double TransportManager::ComputeRouteDistance(
    const std::vector<std::string> &stop_list,
    std::unordered_map<std::string, const Descriptions::Stop *> &stops_info) {
  double result = 0;
  for (size_t i = 1; i < stop_list.size(); ++i) {
    result += Sphere::CalcDistance(stops_info[stop_list[i]]->position,
                                   stops_info[stop_list[i - 1]]->position);
  }
  return result;
}

TransportManager::TransportManager(std::vector<Descriptions::InputQuery> data,
                                   const Json::Dict &routing_settings,
                                   const Json::Dict &drawer_settings) {
  auto stops_end =
      std::partition(std::begin(data), std::end(data), [](const auto &item) {
        return std::holds_alternative<Descriptions::Stop>(item);
      });

  Descriptions::StopsDict stops_data;

  for (const auto &desc : Range(std::begin(data), stops_end)) {
    const auto &stop = std::get<Descriptions::Stop>(desc);
    stops_data[stop.name] = &stop;
    stops.insert({stop.name, {}});
  }

  Descriptions::BusesDict buses_data;

  for (const auto &desc : Range(stops_end, std::end(data))) {
    const auto &bus = std::get<Descriptions::Bus>(desc);
    buses_data[bus.name] = &bus;
    buses.insert(
        {bus.name,
         Bus{.route_length = ComputeRouteLength(bus.stop_list, stops_data),
             .stop_count = bus.stop_list.size(),
             .unique_stop_count =
                 ComputeUniqueItemsCount(AsRange(bus.stop_list))}});
    buses[bus.name].curvature = buses[bus.name].route_length * 1.0 /
                                ComputeRouteDistance(bus.stop_list, stops_data);

    for (const auto &stop : bus.stop_list) {
      stops[stop].buses.insert(bus.name);
    }
  }

  transport_router_ = std::make_unique<TransportRouter>(
      buses_data, stops_data,
      TransportRouter::RoutingSettings{
          .velocity = routing_settings.at("bus_velocity").AsDouble(),
          .bus_wait_time = routing_settings.at("bus_wait_time").AsDouble()});

  map_drawer_ = std::make_unique<MapDrawer::Drawer>(buses_data, stops_data,
                                                    drawer_settings);
  // TODO remove output
  ofstream output("out.svg");
  map_drawer_->AddBusLines().AddStopCircles().AddStopNames().DrawMap(output);
}
std::optional<TransportRouter::RouteInfo>
TransportManager::FindRoute(const std::string &from,
                            const std::string &to) const {
  return transport_router_->FindRoute(from, to);
}
