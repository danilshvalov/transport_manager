#pragma once

#include <system_error>
#include <type_traits>

namespace requests {

enum class RequestError {
  kUnknownRequestType = 1
};

const std::error_category& RequestErrorCategory();

std::error_code make_error_code(RequestError e);
std::error_condition make_error_condition(RequestError e);

}

template<>
struct std::is_error_code_enum<requests::RequestError> : std::true_type {};

template<>
struct std::is_error_condition_enum<requests::RequestError> : std::true_type {};