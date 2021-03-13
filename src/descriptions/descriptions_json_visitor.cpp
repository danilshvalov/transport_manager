#include "descriptions_json_visitor.h"

namespace descriptions::visitors {

using DistancesContainer = StopDescription::DistancesContainer;
using StopListContainer = BusDescription::StopListContainer;

sphere::Point ParsePosition(const Json::Dict &dict) {
  static const std::string kLatitudeKey = "latitude";
  static const std::string kLongitudeKey = "longitude";
  return {.lat = dict.at(kLatitudeKey).AsDouble(), .lon = dict.at(kLongitudeKey).AsDouble()};
}

DistancesContainer ParseRoadDistances(const Json::Dict &dict) {
  static const std::string kRoadDistancesKey = "road_distances";
  const auto &distances = dict.at(kRoadDistancesKey).AsMap();
  return {distances.begin(), distances.end()};
}

StopListContainer ParseStopList(const Json::Dict &dict) {
  static const std::string kStopListKey = "stops";
  const auto &json_array = dict.at(kStopListKey).AsArray();

  StopListContainer stop_list;
  stop_list.reserve(json_array.size());

  for (const auto &stop : json_array) {
    stop_list.push_back(stop.AsString());
  }
  return stop_list;
}

std::string ParseName(const Json::Dict &dict) {
  const std::string kNameKey = "name";
  return dict.at(kNameKey).AsString();
}

bool ParseIsRoundTrip(const Json::Dict &dict) {
  const std::string kIsRoundTripKey = "is_roundtrip";
  return dict.at(kIsRoundTripKey).AsBool();
}

void JSONDeserializeVisitor::operator()(StopDescription &desc) const {
  desc.name = ParseName(data_);
  desc.position = ParsePosition(data_);
  desc.distances = ParseRoadDistances(data_);
}
void JSONDeserializeVisitor::operator()(BusDescription &desc) const {
  desc.name = ParseName(data_);
  desc.stop_list = ParseStopList(data_);
  desc.is_roundtrip = ParseIsRoundTrip(data_);
}
JSONDeserializeVisitor::JSONDeserializeVisitor(const Json::Dict &dict) : data_(dict) {}

}