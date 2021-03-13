#pragma once

#include "json/configuration_json.h"

namespace configuration {

class GeneralConfig {
 private:
  json::JSONConfig json_config_;
 public:
  json::JSONConfig& GetJSONConfig();
  [[nodiscard]] const json::JSONConfig& GetJSONConfig() const;
  GeneralConfig& SetJSONConfig(const json::JSONConfig& cfg);
  GeneralConfig& SetJSONConfig(json::JSONConfig&& cfg);
};

}