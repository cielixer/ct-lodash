#pragma once

#include <cstddef>
#include <functional>
#include <iterator>
#include <map>
#include <ranges>
#include <type_traits>
#include <unordered_map>
#include <utility>

namespace ctl::detail {

template <typename T>
struct is_pair : std::false_type {};

template <typename K, typename V>
struct is_pair<std::pair<K, V>> : std::true_type {};

template <typename T>
inline constexpr bool is_pair_v = is_pair<std::remove_cvref_t<T>>::value;

template <typename C>
concept range = std::ranges::range<C>;

template <typename C>
concept sized_range = std::ranges::sized_range<C>;

template <typename C>
concept pair_value_range = range<C> && is_pair_v<std::ranges::range_value_t<C>>;

template <typename C>
concept non_pair_range = range<C> && !is_pair_v<std::ranges::range_value_t<C>>;

template <typename T>
struct is_bounded_array : std::false_type {};

template <typename T, std::size_t N>
struct is_bounded_array<T[N]> : std::true_type {};  // NOLINT(modernize-avoid-c-arrays)

template <typename T>
inline constexpr bool is_bounded_array_v = is_bounded_array<std::remove_cvref_t<T>>::value;

// hashable — true when std::hash<K> is valid and key supports equality comparison
template <typename K>
concept hashable = requires(K k) {
  { std::hash<K>{}(k) } -> std::convertible_to<std::size_t>;
} && std::equality_comparable<K>;

// auto_map — unordered_map when key is hashable, std::map otherwise
template <typename K, typename V>
using auto_map = std::conditional_t<hashable<K>, std::unordered_map<K, V>, std::map<K, V>>;

}  // namespace ctl::detail
