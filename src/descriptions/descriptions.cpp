#include "descriptions.h"
#include "descriptions_json_visitor.h"
#include "descriptions_errors.h"
#include "utils.h"
#include <functional>

namespace descriptions {

DescriptionsHolder ParseStopDescription(const Json::Dict& dict);
DescriptionsHolder ParseBusDescription(const Json::Dict& dict);

template<typename T>
auto ConvertTypeToFunction(const T& input, const std::string& type) {
  static const unordered_map<std::string, function<DescriptionsHolder(const T&)>> kTypeToParseFunction = {
      {"Stop", ParseStopDescription},
      {"Bus", ParseBusDescription},
  };
  if (auto it = kTypeToParseFunction.find(type); it != kTypeToParseFunction.end()) {
    return it->second;
  }

  throw std::system_error(DescriptionError::kUnknownDescriptionType);
}

DescriptionsHolder ParseStopDescription(const Json::Dict& dict) {
  visitors::JSONDeserializeVisitor visitor(dict);
  StopDescription description;
  Visit(visitor, description);
  return description;
}

DescriptionsHolder ParseBusDescription(const Json::Dict& dict) {
  visitors::JSONDeserializeVisitor visitor(dict);
  BusDescription description;
  Visit(visitor, description);
  return description;
}



//std::unordered_map<std::string, double>
//ParseRoadDistances(const Json::Dict &distances) {
//  return {distances.begin(), distances.end()};
//}

Stop Stop::ParseFrom(const Json::Dict& input) {
  return Stop{
      .name = input.at("name").AsString(),
      .position =
      sphere::Point{
          sphere::ConvertToRadians(input.at("latitude").AsDouble()),
          sphere::ConvertToRadians(input.at("longitude").AsDouble())}};
//      .distances = ParseRoadDistances(input.at("road_distances").AsMap())};
}

Bus Bus::ParseFrom(const Json::Dict& input) {
  return Bus{.name = input.at("name").AsString(),
      .stop_list = ParseStops(input.at("stops").AsArray(),
                              input.at("is_roundtrip").AsBool()),
      .is_roundtrip = input.at("is_roundtrip").AsBool()};
}
DescriptionsHolder ParseDescription(const Json::Dict& input, const configuration::json::DescriptionsKeys& keys) {
  const auto& type = input.at(keys.type).AsString();

  auto func = ConvertTypeToFunction(input, type);
  func(input);

  throw std::runtime_error("unknown type");
}

std::vector<DescriptionsHolder> ReadDescriptions(const std::vector<Json::Node>& input) {
  std::vector<DescriptionsHolder> input_query;
  input_query.reserve(input.size());

  for (const auto& item : input) {
//    input_query.emplace_back(ParseDescription(item.AsMap()));
  }

  return input_query;
}
double ComputeStopDistance(const Stop& lhs, const Stop& rhs) {
  if (auto it = lhs.distances.find(rhs.name); it != end(lhs.distances)) {
    return it->second;
  } else {
    return rhs.distances.at(lhs.name);
  }
}

std::vector<std::string> ParseStops(const std::vector<Json::Node>& stop_list,
                                    bool is_roundtrip) {
  std::vector<std::string> result;
  result.reserve((is_roundtrip ? stop_list.size() : stop_list.size() * 2 - 1));

  for (const auto& stop_node : stop_list) {
    result.push_back(stop_node.AsString());
  }

  if (!is_roundtrip) {
    for (size_t i = stop_list.size() - 1; i > 0; --i) {
      result.push_back(stop_list[i - 1].AsString());
    }
  }

  return result;
}

} // namespace descriptions