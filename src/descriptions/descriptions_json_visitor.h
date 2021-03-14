#pragma once

#include "descriptions.h"
#include "sphere.h"

namespace descriptions::visitors {
struct JSONDeserializeVisitor {
 public:
  explicit JSONDeserializeVisitor(const json::Dict & dict, const JsonDescriptionsKeys& keys);
  void operator()(StopDescription &desc) const;
  void operator()(BusDescription & desc) const;

 private:
  const json::Dict& data_;
  const JsonDescriptionsKeys& keys_;
};
}
