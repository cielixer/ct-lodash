#pragma once

#include <array>
#include <cstddef>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>

#include <boost/mp11.hpp>

#include "ctl/concepts.hpp"
#include "ctl/core.hpp"
#include "ctl/detail/fixed_string.hpp"
#include "ctl/detail/path_traits.hpp"
#include "ctl/detail/reflection.hpp"
#include "ctl/detail/string_lookup.hpp"
#include "ctl/path.hpp"

namespace ctl {

// ============================================================================
// Positional Access - get<I>(obj) returns I-th field
// ============================================================================

template <std::size_t I, typename T>
constexpr decltype(auto) get(T&& obj) {
  using record = std::remove_cvref_t<T>;
  static_assert(
      described_record<record>,
      "ctl::get<I>: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");
  static_assert(I < field_count_v<record>, "ctl::get<I>: index out of range");
  return detail::reflection::get_by_index<I>(std::forward<T>(obj));
}

// ============================================================================
// Path Access - get<Path>(obj) returns member via compile-time path
// ============================================================================

namespace detail {

// Helper to resolve path recursively while preserving cv/ref qualifiers
template <typename Obj, auto Ptr>
constexpr decltype(auto) apply_member_ptr(Obj&& obj) {
  return std::forward<Obj>(obj).*Ptr;
}

template <typename Obj, auto Ptr, auto... Rest>
constexpr decltype(auto) resolve_path_chain(Obj&& obj) {
  if constexpr (sizeof...(Rest) == 0) {
    return apply_member_ptr<Obj, Ptr>(std::forward<Obj>(obj));
  } else {
    return resolve_path_chain<decltype(apply_member_ptr<Obj, Ptr>(std::forward<Obj>(obj))),
                              Rest...>(apply_member_ptr<Obj, Ptr>(std::forward<Obj>(obj)));
  }
}

}  // namespace detail

template <typename Path, typename T>
constexpr decltype(auto) get(T&& obj) {
  using record = std::remove_cvref_t<T>;
  using path_type = std::remove_cvref_t<Path>;

  static_assert(
      described_record<record>,
      "ctl::get<Path>: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");
  static_assert(detail::is_path_v<path_type>,
                "ctl::get<Path>: Path must be ctl::path<...> or ctl::field<...>");
  static_assert(path_applicable<T, path_type>,
                "ctl::get<Path>: invalid member pointer chain for object type");

  // Extract member pointers from path<Ptr1, Ptr2, ...>
  return []<auto... Ptrs>(T&& o, ctl::path<Ptrs...>) -> decltype(auto) {
    static_assert(sizeof...(Ptrs) > 0, "ctl::get<Path>: invalid member pointer chain");
    return detail::resolve_path_chain<T, Ptrs...>(std::forward<T>(o));
  }(std::forward<T>(obj), path_type{});
}

// ============================================================================
// String Access - get<"field">(obj) returns member by compile-time name
// ============================================================================

template <fixed_string... Names, typename T>
  requires(sizeof...(Names) > 0)
constexpr decltype(auto) get(T&& obj) {
  using record = std::remove_cvref_t<T>;

  static_assert(
      described_record<record>,
      "ctl::get<\"name\">: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");

  using path_type = detail::string_path_t<record, Names...>;
  return get<path_type>(std::forward<T>(obj));
}

// ============================================================================
// Field Iteration - for_each_field(obj, fn) iterates described members
// ============================================================================

template <typename T, typename Visitor>
constexpr void for_each_field(T&& obj, Visitor&& visitor) {
  using record = std::remove_cvref_t<T>;
  static_assert(described_record<record>,
                "ctl::for_each_field: T must satisfy ctl::described_record (register with "
                "CTL_DESCRIBE_STRUCT)");
  detail::reflection::for_each_member(std::forward<T>(obj), std::forward<Visitor>(visitor));
}

// ============================================================================
// Membership Test - has<"field">(obj) / has<Path>(obj)
// ============================================================================

template <fixed_string... Names, typename T>
  requires(sizeof...(Names) > 0)
[[nodiscard]] constexpr bool has(T&&) {
  using record = std::remove_cvref_t<T>;
  static_assert(
      described_record<record>,
      "ctl::has<\"name\">: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");
  return detail::has_member_path_v<record, Names...>;
}

template <typename Path, typename T>
[[nodiscard]] constexpr bool has(T&&) {
  using record = std::remove_cvref_t<T>;
  using path_type = std::remove_cvref_t<Path>;
  static_assert(
      described_record<record>,
      "ctl::has<Path>: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");
  static_assert(detail::is_path_v<path_type>,
                "ctl::has<Path>: Path must be ctl::path<...> or ctl::field<...>");
  return path_applicable<record&, path_type>;
}

// ============================================================================
// Keys - keys<T>() returns std::array<string_view, N> of field names
// ============================================================================

template <typename T>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] consteval auto keys() {
  using record = std::remove_cvref_t<T>;
  using descriptors = detail::reflection::member_descriptors_t<record>;
  constexpr auto count = boost::mp11::mp_size<descriptors>::value;
  std::array<std::string_view, count> result{};
  std::size_t idx = 0;
  boost::mp11::mp_for_each<descriptors>([&]<typename Desc>(Desc) {
    result[idx++] = Desc::name;
  });
  return result;
}

// ============================================================================
// Values - values(obj) returns std::tuple of all field values
// ============================================================================

namespace detail {

template <typename T, typename Descriptors, std::size_t... Is>
constexpr auto values_impl(T&& obj, std::index_sequence<Is...>) {
  return std::tuple{
      (std::forward<T>(obj).*boost::mp11::mp_at_c<Descriptors, Is>::pointer)...};
}

}  // namespace detail

template <typename T>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto values(T&& obj) {
  using record = std::remove_cvref_t<T>;
  using descriptors = detail::reflection::member_descriptors_t<record>;
  constexpr auto count = boost::mp11::mp_size<descriptors>::value;
  return detail::values_impl<T, descriptors>(std::forward<T>(obj),
                                             std::make_index_sequence<count>{});
}

}  // namespace ctl
