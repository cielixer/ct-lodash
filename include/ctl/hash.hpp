#pragma once

#include <cstddef>
#include <functional>

#include "ctl/concepts.hpp"
#include "ctl/detail/reflection.hpp"

namespace ctl::detail {

// boost::hash_combine equivalent — 0x9e3779b9 is the golden ratio constant
inline constexpr std::size_t hash_combine(std::size_t seed, std::size_t value) noexcept {
  return seed ^ (value + 0x9e3779b9 + (seed << 6) + (seed >> 2));
}

template <described_record T>
std::size_t struct_hash(const T& obj) noexcept {
  std::size_t seed = 0;
  reflection::for_each_member(obj, [&](std::string_view /*name*/, const auto& field) {
    seed = hash_combine(seed, std::hash<std::remove_cvref_t<decltype(field)>>{}(field));
  });
  return seed;
}

template <described_record T>
bool struct_equal(const T& a, const T& b) noexcept {
  bool eq = true;
  [&]<std::size_t... Is>(std::index_sequence<Is...>) {
    ((eq = eq && (reflection::get_by_index<Is>(a) == reflection::get_by_index<Is>(b))), ...);
  }(std::make_index_sequence<reflection::field_count_v<T>>{});
  return eq;
}

}  // namespace ctl::detail

/// Opt-in macro: generates std::hash and operator== for a described struct.
/// All fields must be individually hashable (have std::hash specialization).
///
/// Usage:
///   struct Point { int x; int y; };
///   CTL_DESCRIBE_STRUCT(Point, (), (x, y))
///   CTL_MAKE_HASHABLE(Point)
///
///   // Now Point works as a key in std::unordered_map / std::unordered_set
///   std::unordered_map<Point, std::string> lookup;
///
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define CTL_MAKE_HASHABLE(Type)                                            \
  inline bool operator==(const Type& a, const Type& b) noexcept {          \
    return ::ctl::detail::struct_equal(a, b);                              \
  }                                                                        \
  template <>                                                              \
  struct std::hash<Type> {                                                 \
    static_assert(::ctl::described_record<Type>,                           \
                  "CTL_MAKE_HASHABLE requires CTL_DESCRIBE_STRUCT");       \
    std::size_t operator()(const Type& obj) const noexcept {               \
      return ::ctl::detail::struct_hash(obj);                              \
    }                                                                      \
  };
