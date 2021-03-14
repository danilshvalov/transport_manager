#include "descriptions_parsers.h"


namespace descriptions::parsers {
sphere::Point ParsePosition(const json::Dict& dict, const JsonDescriptionsKeys& keys) {
  return {.lat = dict.at(keys.lat).AsDouble(), .lon = dict.at(keys.lon).AsDouble()};
}

DistancesContainer ParseRoadDistances(const json::Dict& dict, const JsonDescriptionsKeys& keys) {
  const auto& distances = dict.at(keys.distances).AsMap();
  return {distances.begin(), distances.end()};
}

StopListContainer ParseStopList(const json::Dict& dict, const JsonDescriptionsKeys& keys, bool is_roundtrip) {
  const auto& json_array = dict.at(keys.stop_list).AsArray();

  StopListContainer stop_list;
  stop_list.reserve((is_roundtrip ? json_array.size() : json_array.size() * 2 - 1));

  for (const auto& stop : json_array) {
    stop_list.push_back(stop.AsString());
  }

  if (!is_roundtrip) {
    for (size_t i = json_array.size() - 1; i > 0; --i) {
      stop_list.push_back(json_array[i - 1].AsString());
    }
  }

  return stop_list;
}

std::string ParseName(const json::Dict& dict, const JsonDescriptionsKeys& keys) {
  return dict.at(keys.name).AsString();
}

bool ParseIsRoundTrip(const json::Dict& dict, const JsonDescriptionsKeys& keys) {
  return dict.at(keys.is_roundtrip).AsBool();
}
}