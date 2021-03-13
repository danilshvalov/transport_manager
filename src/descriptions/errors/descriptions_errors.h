#pragma once

#include <system_error>
#include <type_traits>

namespace descriptions {

enum class DescriptionError {
  kUnknownDescriptionType = 1
};

const std::error_category& DescriptionErrorCategory();

std::error_code make_error_code(DescriptionError e);
std::error_condition make_error_condition(DescriptionError e);

}

template<>
struct std::is_error_code_enum<descriptions::DescriptionError> : std::true_type {};

template<>
struct std::is_error_condition_enum<descriptions::DescriptionError> : std::true_type {};

