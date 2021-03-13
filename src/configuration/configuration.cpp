#include "configuration.h"

namespace configuration {

json::JSONConfig &GeneralConfig::GetJSONConfig() {
  return json_config_;
}

const json::JSONConfig &GeneralConfig::GetJSONConfig() const {
  return json_config_;
}

GeneralConfig &GeneralConfig::SetJSONConfig(const json::JSONConfig &cfg) {
  json_config_ = cfg;
  return *this;
}

GeneralConfig &GeneralConfig::SetJSONConfig(json::JSONConfig &&cfg) {
  json_config_ = std::move(cfg);
  return *this;
}

}