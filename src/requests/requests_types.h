#pragma once

#include <string>
#include <variant>

namespace requests {

struct EmptyRequestData {};

struct StopRequestData {
  std::string name;
};

struct BusRequestData {
  std::string name;
};

struct RouteRequestData {
  std::string from;
  std::string to;
};

template<typename DataType>
struct Request {
  size_t id;
  DataType data;

  template<typename This, typename Visitor>
  static void Accept(This& t, Visitor& visitor) {
    visitor(t);
    visitor(t.data);
  }
};

template<>
struct Request<EmptyRequestData> {
  size_t id;

  template<typename This, typename Visitor>
  static void Accept(This& t, Visitor& visitor) {
    visitor(t);
  }
};

struct StopRequest : Request<StopRequestData> {};
struct BusRequest : Request<BusRequestData> {};
struct RouteRequest : Request<RouteRequestData> {};
struct MapRequest : Request<EmptyRequestData> {};

using RequestsHolder = std::variant<StopRequest, BusRequest, RouteRequest, MapRequest>;

}