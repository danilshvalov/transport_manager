#include "requests_errors.h"

namespace requests {

class RequestErrorCategoryImpl :  public std::error_category {
  [[nodiscard]] const char *name() const noexcept override;
  [[nodiscard]] std::string message(int ev) const override;
};
const char* RequestErrorCategoryImpl::name() const noexcept {
  return "request error";
}

std::string RequestErrorCategoryImpl::message(int ev) const {
  const auto condition = static_cast<RequestError>(ev);

  switch (condition) {
    case RequestError::kUnknownRequestType:
      return "Unknown request type";
    default:
      return "Unknown RequestError";
  }
}

const std::error_category& RequestErrorCategory() {
  static RequestErrorCategoryImpl instance;
  return instance;
}
std::error_code make_error_code(RequestError e) {
  return {static_cast<int>(e), RequestErrorCategory()};
}
std::error_condition make_error_condition(RequestError e) {
  return {static_cast<int>(e), RequestErrorCategory()};
}
}