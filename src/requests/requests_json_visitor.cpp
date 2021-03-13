#include "requests_json_visitor.h"

namespace requests::visitors {

JSONDeserializeVisitor::JSONDeserializeVisitor(const Json::Dict &dict, const Keys &keys)
    : data_(dict), keys_(keys) {}

void JSONDeserializeVisitor::operator()(StopRequestData &req_data) const {
  req_data.name = data_.at(keys_.name).AsString();
}
void JSONDeserializeVisitor::operator()(BusRequestData &req_data) const {
  req_data.name = data_.at(keys_.name).AsString();
}
void JSONDeserializeVisitor::operator()(RouteRequestData &req_data) const {
  req_data.from = data_.at(keys_.from).AsString();
  req_data.to = data_.at(keys_.to).AsString();
}

}
