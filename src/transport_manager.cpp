#include "transport_manager.h"

using namespace std;

const TransportManager::Stop *
TransportManager::GetStop(const std::string &name) const {
  return GetValuePointer(stops_, name);
}

const TransportManager::Bus *
TransportManager::GetBus(const std::string &name) const {
  return GetValuePointer(buses_, name);
}
double TransportManager::ComputeRouteLength(
    const std::vector<std::string> &stop_list,
    std::unordered_map<std::string, const descriptions::Stop *> &stops_info) {
  double result = 0;
  for (size_t i = 1; i < stop_list.size(); ++i) {
    result += descriptions::ComputeStopDistance(
        *stops_info.at(stop_list[i - 1]), *stops_info.at(stop_list[i]));
  }
  return result;
}
double TransportManager::ComputeRouteDistance(
    const std::vector<std::string> &stop_list,
    std::unordered_map<std::string, const descriptions::Stop *> &stops_info) {
  double result = 0;
  for (size_t i = 1; i < stop_list.size(); ++i) {
    result += sphere::CalcDistance(stops_info[stop_list[i]]->position,
                                   stops_info[stop_list[i - 1]]->position);
  }
  return result;
}

TransportManager::TransportManager(std::vector<descriptions::InputQuery> data,
                                   const Json::Dict &routing_settings,
                                   const Json::Dict &drawer_settings) {
  auto stops_end =
      std::partition(std::begin(data), std::end(data), [](const auto &item) {
        return std::holds_alternative<descriptions::Stop>(item);
      });

  descriptions::StopsDict stops_data;

  for (const auto &desc : Range(std::begin(data), stops_end)) {
    const auto &stop = std::get<descriptions::Stop>(desc);
    stops_data[stop.name] = &stop;
    stops_.insert({stop.name, {}});
  }

  descriptions::BusesDict buses_data;

  for (const auto &desc : Range(stops_end, std::end(data))) {
    const auto &bus = std::get<descriptions::Bus>(desc);
    buses_data[bus.name] = &bus;
    buses_.insert(
        {bus.name,
         Bus{.route_length = ComputeRouteLength(bus.stop_list, stops_data),
             .stop_count = bus.stop_list.size(),
             .unique_stop_count =
                 ComputeUniqueItemsCount(AsRange(bus.stop_list))}});
    buses_[bus.name].curvature = buses_[bus.name].route_length * 1.0 /
                                ComputeRouteDistance(bus.stop_list, stops_data);

    for (const auto &stop : bus.stop_list) {
      stops_[stop].buses.insert(bus.name);
    }
  }

  transport_router_ = std::make_unique<TransportRouter>(
      buses_data, stops_data,
      TransportRouter::RoutingSettings{
          .velocity = routing_settings.at("bus_velocity").AsDouble(),
          .bus_wait_time = routing_settings.at("bus_wait_time").AsDouble()});

  map_drawer_ = std::make_unique<MapDrawer::Drawer>(buses_data, stops_data,
                                                    drawer_settings);
} 
std::optional<TransportRouter::RouteInfo>
TransportManager::FindRoute(const std::string &from,
                            const std::string &to) const {
  return transport_router_->FindRoute(from, to);
}

const Svg::Document &TransportManager::GetMap() const {
  return map_drawer_->GetMap();
}
