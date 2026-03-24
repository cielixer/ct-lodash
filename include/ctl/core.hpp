#pragma once

#include <string_view>
#include <type_traits>
#include <utility>

#include "ctl/detail/reflection.hpp"

namespace ctl {

template <std::size_t I, typename T>
constexpr decltype(auto) get_by_index(T&& obj) {
  return detail::reflection::get_by_index<I>(std::forward<T>(obj));
}

template <typename T>
constexpr std::size_t field_count_v = detail::reflection::field_count_v<T>;

template <typename T, typename Visitor>
bool visit_member_by_name(T& obj, std::string_view name, Visitor&& visitor) {
  bool found = false;

  detail::reflection::for_each_member(obj, [&](std::string_view member_name, auto& member) {
    if (!found && member_name == name) {
      found = true;
      visitor(member);
    }
  });

  return found;
}

template <typename T, typename Visitor>
bool visit_member_by_name(const T& obj, std::string_view name, Visitor&& visitor) {
  bool found = false;

  detail::reflection::for_each_member(obj, [&](std::string_view member_name, const auto& member) {
    if (!found && member_name == name) {
      found = true;
      visitor(member);
    }
  });

  return found;
}

template <typename Expected, typename T>
Expected* get_ptr(T& obj, std::string_view name) {
  Expected* result = nullptr;

  visit_member_by_name(obj, name, [&](auto& member) {
    using member_t = std::remove_cvref_t<decltype(member)>;
    if constexpr (std::is_same_v<member_t, Expected>) {
      result = &member;
    }
  });

  return result;
}

template <typename Expected, typename T>
const Expected* get_ptr(const T& obj, std::string_view name) {
  const Expected* result = nullptr;

  visit_member_by_name(obj, name, [&](const auto& member) {
    using member_t = std::remove_cvref_t<decltype(member)>;
    if constexpr (std::is_same_v<member_t, Expected>) {
      result = &member;
    }
  });

  return result;
}

}  // namespace ctl
