#pragma once

#include <string>
#include <optional>

namespace configuration::json {

struct DescriptionsKeys {
  std::string type;
  std::string name;
  std::string lat;
  std::string lon;
  std::string distances;
  std::string stop_list;
};

struct RequestsKeys {
  std::string id;
  std::string type;
  std::string name;
  std::string from;
  std::string to;
};

class JSONConfig {
 private:
  std::optional<DescriptionsKeys> descriptions_keys_;
  std::optional<RequestsKeys> requests_keys_;
 public:
  JSONConfig& SetDescriptionsKeys(const DescriptionsKeys& keys);
  JSONConfig& SetRequestsKeys(const RequestsKeys& keys);
  const DescriptionsKeys& GetDescriptionsKeys() const;
  DescriptionsKeys& GetDescriptionsKeys();
  const RequestsKeys& GetRequestsKeys() const;
  RequestsKeys& GetRequestsKeys();


};
}