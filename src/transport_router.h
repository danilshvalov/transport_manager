#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <iostream>
#include <variant>

#include "router.h"
#include "graph.h"
#include "description.h"

class TransportRouter {
 public:
  struct RoutingSettings {
    double velocity;
    double bus_wait_time;
  };
 private:
  struct StopVertexIds {
    Graph::VertexId in;
    Graph::VertexId out;
  };

  struct VertexInfo {
    std::string stop_name;
  };

  struct BusEdgeInfo {
    std::string bus_name;
    size_t span_count;
  };

  struct WaitEdgeInfo {};

  using EdgeInfo = std::variant<BusEdgeInfo, WaitEdgeInfo>;
  using BusEdge = Graph::Edge<double>;

  RoutingSettings routing_settings_;

  Graph::DirectedWeightedGraph<double> graph_;
  std::unique_ptr<Graph::Router<double>> router_;

  std::unordered_map<std::string, StopVertexIds> stops_vertex_ids_;
  std::vector<VertexInfo> vertices_info_;
  std::vector<EdgeInfo> edges_info_;

  void FillGraphWithStops(const std::unordered_map<std::string, const Descriptions::Stop *> &stops_dict);

  void FillGraphWithBuses(const Descriptions::BusesDict &buses_dict, const Descriptions::StopsDict &stops_dict);

 public:
  TransportRouter(std::unordered_map<std::string, const Descriptions::Bus *> buses_dict,
                  std::unordered_map<std::string, const Descriptions::Stop *> stops_dict,
                  RoutingSettings routing_settings);

  struct RouteInfo {
    double total_time;

    struct BusItem {
      std::string bus_name;
      double time;
      size_t span_count;
    };

    struct StopItem {
      std::string stop_name;
      double time;
    };

    using Item = std::variant<BusItem, StopItem>;

    std::vector<Item> items;
  };

  std::optional<RouteInfo> FindRoute(const std::string &from, const std::string &to) const;
};



