#pragma once

#include <istream>
#include <map>
#include <string>
#include <variant>
#include <vector>
#include <iomanip>

namespace Json {

class Node : std::variant<std::vector<Node>,
                          std::map<std::string, Node>,
                          int,
                          std::string, double, bool> {
 public:
  using variant::variant;

  const auto &AsArray() const {
    return std::get<std::vector<Node>>(*this);
  }
  const auto &AsMap() const {
    return std::get<std::map<std::string, Node>>(*this);
  }
  int AsInt() const {
    return std::get<int>(*this);
  }
  const auto &AsString() const {
    return std::get<std::string>(*this);
  }
  double AsDouble() const {
    return IsDouble() ? std::get<double>(*this) : std::get<int>(*this);
  }
  bool AsBool() const {
    return std::get<bool>(*this);
  }

  friend std::ostream &operator<<(std::ostream &os, const Json::Node &nodes);
  friend std::ostream &operator<<(std::ostream &os, const std::vector<Json::Node> &nodes);
  friend std::ostream &operator<<(std::ostream &os, const std::map<std::string, Json::Node> &nodes);

  explicit operator double() const {
    return AsDouble();
  }

  explicit operator int() const {
    return AsInt();
  }

  explicit operator std::string() const {
    return AsString();
  }

  explicit operator bool() const {
    return AsBool();
  }
  bool IsString() const {
    return std::holds_alternative<std::string>(*this);
  }
  bool IsArray() const {
    return std::holds_alternative<std::vector<Node>>(*this);
  }
  bool IsMap() const {
    return std::holds_alternative<std::map<std::string, Node>>(*this);
  }
  bool IsBool() const {
    return std::holds_alternative<bool>(*this);
  }
  bool IsInt() const {
    return std::holds_alternative<int>(*this);
  }
  bool IsDouble() const {
    return std::holds_alternative<double>(*this);
  }
};

using Dict = std::map<std::string, Node>;
using Array = std::vector<Node>;

class Document {
 public:
  explicit Document(Node root);

  const Node &GetRoot() const;

 private:
  Node root;
};

Document Load(std::istream &input);

}