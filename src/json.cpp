#include "json.h"

using namespace std;

namespace Json {

Document::Document(Node root) : root(move(root)) {
}

const Node &Document::GetRoot() const {
  return root;
}

Node LoadNode(istream &input);

Node LoadArray(istream &input) {
  vector<Node> result;

  for (char c; input >> c && c != ']';) {
    if (c != ',') {
      input.putback(c);
    }
    result.push_back(LoadNode(input));
  }

  return Node(move(result));
}

Node LoadNumber(istream &input) {
  bool is_negative = false;
  if (input.peek() == '-') {
    is_negative = true;
    input.get();
  }
  int int_part = 0;
  while (isdigit(input.peek())) {
    int_part *= 10;
    int_part += input.get() - '0';
  }
  if (input.peek() != '.') {
    return Node(int_part * (is_negative ? -1 : 1));
  }
  input.get();  // '.'
  double result = int_part;
  double frac_mult = 0.1;
  while (isdigit(input.peek())) {
    result += frac_mult * (input.get() - '0');
    frac_mult /= 10;
  }
  return Node(result * (is_negative ? -1 : 1));
}

Node LoadBool(istream &input) {
  string s;
  while (isalpha(input.peek())) {
    s.push_back(input.get());
  }
  return Node(s == "true");
}

Node LoadString(istream &input) {
  string line;
  getline(input, line, '"');
  return Node(move(line));
}

Node LoadDict(istream &input) {
  map<string, Node> result;

  for (char c; input >> c && c != '}';) {
    if (c == ',') {
      input >> c;
    }

    string key = LoadString(input).AsString();
    input >> c;
    result.emplace(move(key), LoadNode(input));
  }

  return Node(move(result));
}

Node LoadNode(istream &input) {
  char c;
  input >> c;

  if (c == '[') {
    return LoadArray(input);
  } else if (c == '{') {
    return LoadDict(input);
  } else if (c == '"') {
    return LoadString(input);
  } else if (c == 'f' || c == 't') {
    input.putback(c);
    return LoadBool(input);
  } else {
    input.putback(c);
    return LoadNumber(input);
  }
}

Document Load(istream &input) {
  return Document{LoadNode(input)};
}

std::ostream &operator<<(ostream &os, const Node &nodes) {
  if (std::holds_alternative<double>(nodes)) {
    return os << nodes.AsDouble();
  } else if (std::holds_alternative<int>(nodes)) {
    return os << nodes.AsInt();
  } else if (std::holds_alternative<std::map<std::string, Node>>(nodes)) {
    return os << nodes.AsMap();
  } else if (std::holds_alternative<std::string>(nodes)) {
    return os << quoted(nodes.AsString());
  } else if (std::holds_alternative<std::vector<Json::Node>>(nodes)) {
    return os << nodes.AsArray();
  }
  return os;
}
std::ostream &operator<<(ostream &os, const std::map<std::string, Json::Node> &nodes) {
  os << "{";
  bool first = true;
  for (const auto &node : nodes) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    os << "\"" << node.first << "\"" << ": " << node.second;
  }
  return os << "}";
}

std::ostream &operator<<(std::ostream &os, const std::vector<Json::Node> &nodes) {
  os << '[';
  bool first = true;
  for (const auto &node : nodes) {
    if (first) {
      first = false;
    } else {
      os << ", ";
    }
    os << node;
  }
  return os << "]";
}
}