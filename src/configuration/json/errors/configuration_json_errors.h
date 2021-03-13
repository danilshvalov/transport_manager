#pragma once

#include <system_error>
#include <type_traits>

namespace configuration::json {

enum class JsonError {
  kDescriptionKeysUndefined = 1,
  kRequestsKeysUndefined,
};

const std::error_category& JsonErrorCategory();

std::error_code make_error_code(JsonError e);
std::error_condition make_error_condition(JsonError e);

}

template<>
struct std::is_error_code_enum<configuration::json::JsonError> : std::true_type {};

template<>
struct std::is_error_condition_enum<configuration::json::JsonError> : std::true_type {};