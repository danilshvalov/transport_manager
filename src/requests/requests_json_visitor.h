#pragma once

#include "json.h"
#include "requests_types.h"
#include "transport_manager.h"
#include "configuration.h"

namespace requests::visitors {

using Keys = configuration::json::RequestsKeys;

class JSONDeserializeVisitor {
 public:
  JSONDeserializeVisitor(const Json::Dict &dict, const Keys &keys);

  template<typename T>
  void operator()(Request<T> &req) const {
    req.id = data_.at(keys_.id).AsInt();
  }
  void operator()(StopRequestData &req_data) const;
  void operator()(BusRequestData &req_data) const;
  void operator()(RouteRequestData &req_data) const;

 private:
  const Json::Dict &data_;
  const Keys &keys_;
};

}