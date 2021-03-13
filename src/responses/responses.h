#pragma once
#include <variant>
#include <set>

#include "utils.h"

namespace responses {

struct StopResponse : VisitableType {
  std::set<std::string> buses;
};

struct BusResponse : VisitableType {
  double route_length;
  double curvature;
  size_t stop_count;
  size_t unique_stop_count;
};

using ResponsesHolder = std::variant<StopResponse, BusResponse>;

}