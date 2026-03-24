#pragma once

// ctl/json.hpp — Opt-in JSON serialization for described structs.
// Requires nlohmann/json (>= 3.11). Users must find_package(nlohmann_json)
// and link nlohmann_json::nlohmann_json in their CMakeLists.txt.
// ctl itself does NOT depend on nlohmann/json.

#include <nlohmann/json.hpp>
#include <optional>
#include <ranges>
#include <string>
#include <string_view>
#include <type_traits>
#include <utility>

#include "ctl/concepts.hpp"
#include "ctl/detail/reflection.hpp"

namespace ctl {

namespace detail::json {

template <typename T>
struct is_optional : std::false_type {};

template <typename T>
struct is_optional<std::optional<T>> : std::true_type {};

template <typename T>
inline constexpr bool is_optional_v = is_optional<std::remove_cvref_t<T>>::value;

template <typename T>
struct is_pair : std::false_type {};

template <typename K, typename V>
struct is_pair<std::pair<K, V>> : std::true_type {};

template <typename T>
inline constexpr bool is_pair_v = is_pair<std::remove_cvref_t<T>>::value;

template <typename T>
concept json_native = requires(const T& v) {
  { nlohmann::json(v) };
};

template <typename T>
concept iterable_range = std::ranges::range<T> && !std::is_same_v<std::remove_cvref_t<T>, std::string>;

template <typename T>
concept map_like_range = iterable_range<T> && is_pair_v<std::ranges::range_value_t<T>>;

template <typename T>
concept seq_like_range = iterable_range<T> && !is_pair_v<std::ranges::range_value_t<T>>;

template <typename T>
nlohmann::json value_to_json(const T& val) {
  using clean = std::remove_cvref_t<T>;

  if constexpr (described_record<clean>) {
    nlohmann::json obj = nlohmann::json::object();
    reflection::for_each_member(val, [&](std::string_view name, const auto& member) {
      obj[std::string(name)] = value_to_json(member);
    });
    return obj;
  } else if constexpr (is_optional_v<clean>) {
    if (val.has_value()) {
      return value_to_json(*val);
    }
    return nlohmann::json(nullptr);
  } else if constexpr (map_like_range<clean>) {
    nlohmann::json obj = nlohmann::json::object();
    for (const auto& [k, v] : val) {
      obj[nlohmann::json(k).template get<std::string>()] = value_to_json(v);
    }
    return obj;
  } else if constexpr (seq_like_range<clean>) {
    nlohmann::json arr = nlohmann::json::array();
    for (const auto& elem : val) {
      arr.push_back(value_to_json(elem));
    }
    return arr;
  } else {
    return nlohmann::json(val);
  }
}

template <typename T>
T value_from_json(const nlohmann::json& j);

template <typename T>
  requires described_record<T>
T record_from_json(const nlohmann::json& j) {
  using descriptors = reflection::member_descriptors_t<T>;

  T result{};
  boost::mp11::mp_for_each<descriptors>([&]<typename Desc>(Desc) {
    using member_type = std::remove_cvref_t<decltype(std::declval<T>().*Desc::pointer)>;
    const std::string key{Desc::name};
    if (j.contains(key)) {
      result.*Desc::pointer = value_from_json<member_type>(j.at(key));
    }
  });
  return result;
}

template <typename C>
  requires iterable_range<C>
C range_from_json(const nlohmann::json& j) {
  C result{};
  if constexpr (map_like_range<C>) {
    using value_type = std::ranges::range_value_t<C>;
    using K = typename value_type::first_type;
    using V = typename value_type::second_type;
    for (auto& [key, val] : j.items()) {
      if constexpr (std::is_same_v<std::remove_cvref_t<K>, std::string>) {
        result.emplace(key, value_from_json<V>(val));
      } else {
        result.emplace(value_from_json<K>(nlohmann::json(key)), value_from_json<V>(val));
      }
    }
  } else {
    using value_type = std::ranges::range_value_t<C>;
    for (const auto& elem : j) {
      if constexpr (requires { result.push_back(std::declval<value_type>()); }) {
        result.push_back(value_from_json<value_type>(elem));
      } else if constexpr (requires { result.insert(std::declval<value_type>()); }) {
        result.insert(value_from_json<value_type>(elem));
      }
    }
  }
  return result;
}

template <typename T>
T value_from_json(const nlohmann::json& j) {
  using clean = std::remove_cvref_t<T>;

  if constexpr (described_record<clean>) {
    return record_from_json<clean>(j);
  } else if constexpr (is_optional_v<clean>) {
    if (j.is_null()) {
      return std::nullopt;
    }
    using inner = typename clean::value_type;
    return value_from_json<inner>(j);
  } else if constexpr (iterable_range<clean>) {
    return range_from_json<clean>(j);
  } else {
    return j.get<clean>();
  }
}

}  // namespace detail::json

template <typename T>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] nlohmann::json to_json(const T& obj) {
  return detail::json::value_to_json(obj);
}

template <typename T>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] T from_json(const nlohmann::json& j) {
  return detail::json::value_from_json<std::remove_cvref_t<T>>(j);
}

}  // namespace ctl
