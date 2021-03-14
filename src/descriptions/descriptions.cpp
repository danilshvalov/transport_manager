#include "descriptions.h"
#include "descriptions_json_visitor.h"
#include "descriptions_errors.h"
#include "utils.h"
#include <functional>

namespace descriptions {


template<typename DataType, typename KeysType>
using ParseDescriptionFunction = DescriptionsHolder(*)(const DataType&, const KeysType&);

template<typename DataType>
DescriptionsHolder ParseDescription(const json::Dict& dict, const JsonDescriptionsKeys& keys);

template<typename DataType, typename KeysType>
auto ConvertTypeToFunction(std::string_view type);


template<typename DataType>
DescriptionsHolder ParseDescription(const json::Dict& dict, const JsonDescriptionsKeys& keys) {
  visitors::JSONDeserializeVisitor visitor(dict, keys);
  DataType description;
  Visit(visitor, description);
  return description;
}

template<typename DataType, typename KeysType>
auto ConvertTypeToFunction(std::string_view type) {
  static const unordered_map<std::string_view, ParseDescriptionFunction<DataType, KeysType>> kTypeToParseFunction = {
      {"Stop", ParseDescription<StopDescription>},
      {"Bus", ParseDescription<BusDescription>},
  };
  if (auto it = kTypeToParseFunction.find(type); it != kTypeToParseFunction.end()) {
    return it->second;
  }

  throw std::system_error(DescriptionError::kUnknownDescriptionType);
}




double ComputeStopDistance(const Stop& lhs, const Stop& rhs) {
  if (auto it = lhs.distances.find(rhs.name); it != end(lhs.distances)) {
    return it->second;
  } else {
    return rhs.distances.at(lhs.name);
  }
}



std::vector<std::variant<StopDescription, BusDescription>> ReadDescriptions(const json::Array& data,
                                                                            const JsonDescriptionsKeys& keys) {
  std::vector<DescriptionsHolder> input_query;
  input_query.reserve(data.size());

  for (const auto &node_dict : data) {
    const auto& dict = node_dict.AsMap();
    const auto& func = ConvertTypeToFunction<decltype(dict), decltype(keys)>(dict.at(keys.type).AsString());
    input_query.push_back(func(dict, keys));
  }

  return input_query;
}

} // namespace descriptions