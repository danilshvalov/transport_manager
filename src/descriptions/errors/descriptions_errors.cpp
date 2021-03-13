#include "descriptions_errors.h"

namespace descriptions {

class DescriptionErrorCategoryImpl : public std::error_category {
  [[nodiscard]] const char* name() const noexcept override;
  [[nodiscard]] std::string message(int ev) const override;
};
const char* DescriptionErrorCategoryImpl::name() const noexcept {
  return "descriptions error";
}

std::string DescriptionErrorCategoryImpl::message(int ev) const {
  const auto condition = static_cast<DescriptionError>(ev);

  switch (condition) {
    case DescriptionError::kUnknownDescriptionType:
      return "Unknown description type";
    default:
      return "Unknown DescriptionError";
  }
}

const std::error_category& DescriptionErrorCategory() {
  static DescriptionErrorCategoryImpl instance;
  return instance;
}

std::error_code make_error_code(DescriptionError e) {
  return {static_cast<int>(e), DescriptionErrorCategory()};
}

std::error_condition make_error_condition(DescriptionError e) {
  return {static_cast<int>(e), DescriptionErrorCategory()};
}

}