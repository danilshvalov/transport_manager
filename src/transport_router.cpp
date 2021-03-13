#include "transport_router.h"
void TransportRouter::FillGraphWithStops(const std::unordered_map<std::string, const descriptions::Stop *> &stops_dict) {
  Graph::VertexId vertex_id = 0;

  for (const auto&[stop_name, _] : stops_dict) {
    auto &vertex_ids = stops_vertex_ids_[stop_name];
    vertex_ids.in = vertex_id++;
    vertex_ids.out = vertex_id++;
    vertices_info_[vertex_ids.in] = {.stop_name = stop_name};
    vertices_info_[vertex_ids.out] = {.stop_name = stop_name};

    edges_info_.emplace_back(WaitEdgeInfo{});
    const auto &edge_id = graph_.AddEdge(BusEdge{
        .from = vertex_ids.out,
        .to = vertex_ids.in,
        .weight = routing_settings_.bus_wait_time
    });

    assert(edge_id == edges_info_.size() - 1);
  }

  assert(vertex_id == graph_.GetVertexCount());
}
void TransportRouter::FillGraphWithBuses(const descriptions::BusesDict &buses_dict,
                                         const descriptions::StopsDict &stops_dict) {
  for (const auto&[_, bus_item] : buses_dict) {
    const auto &bus = *bus_item;
    const size_t stop_count = bus.stop_list.size();

    if (stop_count <= 1) continue;

    auto compute_distance_from = [&bus, &stops_dict](size_t lhs_idx) {
      return descriptions::ComputeStopDistance(*stops_dict.at(bus.stop_list[lhs_idx]),
                                               *stops_dict.at(bus.stop_list[lhs_idx + 1]));
    };

    for (size_t start_stop_idx = 0; start_stop_idx + 1 < stop_count; ++start_stop_idx) {
      const Graph::VertexId start_vertex = stops_vertex_ids_[bus.stop_list[start_stop_idx]].in;
      double total = 0;

      for (size_t finish_stop_idx = start_stop_idx + 1; finish_stop_idx < stop_count; ++finish_stop_idx) {
        const Graph::VertexId finish_vertex = stops_vertex_ids_[bus.stop_list[finish_stop_idx]].out;
        total += compute_distance_from(finish_stop_idx - 1);

        edges_info_.emplace_back(BusEdgeInfo{
            .bus_name = bus.name,
            .span_count = finish_stop_idx - start_stop_idx
        });

        const auto &edge_id = graph_.AddEdge(BusEdge{
            .from = start_vertex,
            .to = finish_vertex,
            .weight = total / (routing_settings_.velocity * 1000 / 60)
        });

        assert(edge_id == edges_info_.size() - 1);
      }
    }
  }
}
TransportRouter::TransportRouter(std::unordered_map<std::string, const descriptions::Bus *> buses_dict,
                                 std::unordered_map<std::string, const descriptions::Stop *> stops_dict,
                                 TransportRouter::RoutingSettings routing_settings) :
    routing_settings_(routing_settings),
    graph_(stops_dict.size() * 2),
    vertices_info_(stops_dict.size() * 2) {

  FillGraphWithStops(stops_dict);
  FillGraphWithBuses(buses_dict, stops_dict);

  router_ = std::make_unique<Graph::Router<double>>(graph_);
}
std::optional<TransportRouter::RouteInfo> TransportRouter::FindRoute(const std::string &from, const std::string &to) const {
  const Graph::VertexId vertex_from = stops_vertex_ids_.at(from).out;
  const Graph::VertexId vertex_to = stops_vertex_ids_.at(to).out;

  const auto &route = router_->BuildRoute(vertex_from, vertex_to);

  if (!route) {
    return std::nullopt;
  }

  RouteInfo route_info = {.total_time = route->weight};

  for (Graph::EdgeId edge_idx = 0; edge_idx < route->edge_count; ++edge_idx) {
    const auto &edge_id = router_->GetRouteEdge(route->id, edge_idx);

    const auto &edge = graph_.GetEdge(edge_id);
    const auto &edge_info = edges_info_[edge_id];

    if (std::holds_alternative<BusEdgeInfo>(edge_info)) {
      const auto &bus_edge_info = std::get<BusEdgeInfo>(edge_info);
      route_info.items.emplace_back(RouteInfo::BusItem{
          .bus_name = bus_edge_info.bus_name,
          .time = edge.weight,
          .span_count = bus_edge_info.span_count,
      });
    } else {
      const Graph::VertexId &vertex_id = edge.from;
      route_info.items.emplace_back(RouteInfo::StopItem{
          .stop_name = vertices_info_[vertex_id].stop_name,
          .time = edge.weight,
      });
    }
  }

  router_->ReleaseRoute(route->id);

  return route_info;
}
