#include "descriptions_json_visitor.h"
#include "descriptions_parsers.h"

namespace descriptions::visitors {

void JSONDeserializeVisitor::operator()(StopDescription& desc) const {
  desc.name = parsers::ParseName(data_, keys_);
  desc.position = parsers::ParsePosition(data_, keys_);
  desc.distances = parsers::ParseRoadDistances(data_, keys_);
}
void JSONDeserializeVisitor::operator()(BusDescription& desc) const {
  desc.name = parsers::ParseName(data_, keys_);
  desc.is_roundtrip = parsers::ParseIsRoundTrip(data_, keys_);
  desc.stop_list = parsers::ParseStopList(data_, keys_, desc.is_roundtrip);
}
JSONDeserializeVisitor::JSONDeserializeVisitor(const json::Dict& dict, const JsonDescriptionsKeys& keys)
    : data_(dict), keys_(keys) {}

}