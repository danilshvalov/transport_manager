#pragma once

#include <unordered_map>
#include <iterator>
#include <unordered_set>
#include <stdexcept>

template<typename K, typename V>
const V *GetValuePointer(const std::unordered_map<K, V> &dict, const K &key) {
  if (auto it = dict.find(key); it != end(dict)) {
    return &it->second;
  }
  return nullptr;
}

template<typename It>
class Range {
 private:
  It begin_;
  It end_;
 public:
  using ValueType = typename std::iterator_traits<It>::value_type;

  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const {
    return begin_;
  }
  It end() const {
    return end_;
  }
};

template<typename C>
auto AsRange(const C &container) {
  return Range(std::begin(container), std::end(container));
}

template<typename It>
size_t ComputeUniqueItemsCount(Range<It> range) {
  return std::unordered_set<typename Range<It>::ValueType>{
      range.begin(), range.end()
  }.size();
}

template<typename T>
bool IsZero(T number) {
  if (!std::is_arithmetic_v<T>) {
    throw std::logic_error("Not arithmetic type");
  }
  return number == 0;
}

template<typename Visitor, typename Visitable>
void Visit(Visitor &visitor, Visitable &visitable) {
  Visitable::Accept(visitable, visitor);
}

template<typename Visitor, typename V1, typename ...V>
void Visit(Visitor &visitor, V1 &v1, V &...v) {
  Visit(visitor, v1);
  Visit(visitor, v...);
}

struct VisitableType {
  template<typename Visitable, typename Visitor>
  static void Accept(Visitable &visitable, Visitor &visitor) {
    visitor(visitable);
  }
};
