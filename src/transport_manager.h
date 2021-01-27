#pragma once

#include <unordered_map>
#include <string>
#include <vector>
#include <set>
#include <memory>
#include <algorithm>
#include <optional>

#include "utils.h"
#include "description.h"
#include "transport_router.h"
#include "svg.h"
#include "map_drawer.h"

namespace Responses {
struct Bus {
  double route_length;
  double curvature;
  size_t stop_count;
  size_t unique_stop_count;
};

struct Stop {
  std::set<std::string> buses;
};
}

class TransportManager {
 private:
  using Bus = Responses::Bus;
  using Stop = Responses::Stop;

  static double ComputeRouteLength(const std::vector<std::string> &stop_list,
                                   std::unordered_map<std::string, const Descriptions::Stop *> &stops_info);

  static double ComputeRouteDistance(const std::vector<std::string> &stop_list,
                                      std::unordered_map<std::string, const Descriptions::Stop *> &stops_info);

  std::unordered_map<std::string, Bus> buses;
  std::unordered_map<std::string, Stop> stops;
  std::unique_ptr<TransportRouter> transport_router_;
  std::unique_ptr<MapDrawer::Drawer> map_drawer_;
 public:
  explicit TransportManager(std::vector<Descriptions::InputQuery>,
                            const Json::Dict &routing_settings,
                            const Json::Dict &drawer_settings);
  const Stop *GetStop(const std::string &name) const;
  const Bus *GetBus(const std::string &name) const;
  std::optional<TransportRouter::RouteInfo> FindRoute(const std::string &from, const std::string &to) const;
};