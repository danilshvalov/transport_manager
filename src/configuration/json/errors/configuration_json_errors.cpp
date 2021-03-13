#include "configuration_json_errors.h"

namespace configuration::json {

class JsonErrorCategoryImpl : public std::error_category {
  [[nodiscard]] const char *name() const noexcept override;
  [[nodiscard]] std::string message(int ev) const override;
};

const char *JsonErrorCategoryImpl::name() const noexcept {
  return "json configuration error";
}

std::string JsonErrorCategoryImpl::message(int ev) const {
  const auto condition = static_cast<JsonError>(ev);

  switch (condition) {

    case JsonError::kDescriptionKeysUndefined:return "Description keys undefined";
    case JsonError::kRequestsKeysUndefined:return "Request keys undefined";
    default:return "Unknown JsonError";
  }
}

const std::error_category &JsonErrorCategory() {
  static JsonErrorCategoryImpl instance;
  return instance;
}

std::error_code make_error_code(JsonError e) {
  return {static_cast<int>(e), JsonErrorCategory()};
}
std::error_condition make_error_condition(JsonError e) {
  return {static_cast<int>(e), JsonErrorCategory()};
}

}