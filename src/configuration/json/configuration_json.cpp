#include "configuration_json.h"
#include "configuration_json_errors.h"


namespace configuration::json {

JSONConfig &JSONConfig::SetDescriptionsKeys(const DescriptionsKeys &keys) {
  descriptions_keys_ = keys;
  return *this;
}
JSONConfig &JSONConfig::SetRequestsKeys(const RequestsKeys &keys) {
  requests_keys_ = keys;
  return *this;
}
const DescriptionsKeys &JSONConfig::GetDescriptionsKeys() const {
  if (!descriptions_keys_) throw std::system_error(JsonError::kDescriptionKeysUndefined);
  return *descriptions_keys_;
}
DescriptionsKeys &JSONConfig::GetDescriptionsKeys() {
  if (!descriptions_keys_) throw std::system_error(JsonError::kDescriptionKeysUndefined);
  return *descriptions_keys_;
}
const RequestsKeys &JSONConfig::GetRequestsKeys() const {
  if (!requests_keys_) throw std::system_error(JsonError::kRequestsKeysUndefined);
  return *requests_keys_;
}
RequestsKeys &JSONConfig::GetRequestsKeys() {
  if (!requests_keys_) throw std::system_error(JsonError::kRequestsKeysUndefined);
  return *requests_keys_;
}

}