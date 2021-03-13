#pragma once

#include "descriptions.h"
#include "sphere.h"

namespace descriptions::visitors {
struct JSONDeserializeVisitor {
 public:
  explicit JSONDeserializeVisitor(const Json::Dict & dict);
  void operator()(StopDescription &desc) const;
  void operator()(BusDescription & desc) const;

 private:
  const Json::Dict& data_;
};
}
