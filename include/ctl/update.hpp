#pragma once

#include <concepts>
#include <type_traits>
#include <utility>

#include "ctl/access.hpp"
#include "ctl/concepts.hpp"
#include "ctl/detail/fixed_string.hpp"
#include "ctl/detail/path_traits.hpp"
#include "ctl/detail/string_lookup.hpp"
#include "ctl/path.hpp"

namespace ctl {

// ============================================================================
// In-place mutation - set<Path>(obj, value) returns T&
// ============================================================================

template <typename Path, typename T, typename Value>
  requires(!std::is_const_v<std::remove_reference_t<T>>)
constexpr T& set(T& obj, Value&& value) {
  using record = std::remove_cvref_t<T>;
  using path_type = std::remove_cvref_t<Path>;

  static_assert(
      described_record<record>,
      "ctl::set<Path>: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");
  static_assert(detail::is_path_v<path_type>,
                "ctl::set<Path>: Path must be ctl::path<...> or ctl::field<...>");
  static_assert(path_applicable<T&, path_type>,
                "ctl::set<Path>: invalid member pointer chain for object type");

  using target_ref = path_value_t<T&, path_type>;

  static_assert(std::is_assignable_v<target_ref, Value&&>,
                "ctl::set<Path>: value type not assignable to target member type");

  // Use get<Path> to resolve the path and assign
  get<path_type>(obj) = std::forward<Value>(value);

  return obj;
}

// Diagnostic for const objects
template <typename Path, typename T, typename Value>
  requires std::is_const_v<std::remove_reference_t<T>>
constexpr T& set(T& obj, Value&&) {
  using record = std::remove_cvref_t<T>;
  using path_type = std::remove_cvref_t<Path>;

  static_assert(
      described_record<record>,
      "ctl::set<Path>: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");
  static_assert(detail::is_path_v<path_type>,
                "ctl::set<Path>: Path must be ctl::path<...> or ctl::field<...>");
  static_assert(!std::is_const_v<std::remove_reference_t<T>>,
                "ctl::set<Path>: cannot mutate const object");
  return obj;
}

// ============================================================================
// Copy-on-write - with<Path>(obj, value) returns modified copy
// ============================================================================

template <typename Path, typename T, typename Value>
  requires std::copy_constructible<std::remove_cvref_t<T>>
constexpr std::remove_cvref_t<T> with(T&& obj, Value&& value) {
  using obj_type = std::remove_cvref_t<T>;
  using path_type = std::remove_cvref_t<Path>;

  static_assert(
      described_record<obj_type>,
      "ctl::with<Path>: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");
  static_assert(detail::is_path_v<path_type>,
                "ctl::with<Path>: Path must be ctl::path<...> or ctl::field<...>");
  static_assert(path_applicable<obj_type&, path_type>,
                "ctl::with<Path>: invalid member pointer chain for object type");

  using target_ref = path_value_t<obj_type&, path_type>;

  static_assert(std::is_assignable_v<target_ref, Value&&>,
                "ctl::with<Path>: value type not assignable to target member type");

  // Create a copy
  obj_type copy = obj;

  // Mutate the copy
  get<path_type>(copy) = std::forward<Value>(value);

  return copy;
}

template <typename Path, typename T, typename Value>
  requires(!std::copy_constructible<std::remove_cvref_t<T>>)
constexpr std::remove_cvref_t<T> with(T&&, Value&&) {
  using obj_type = std::remove_cvref_t<T>;
  using path_type = std::remove_cvref_t<Path>;

  static_assert(
      described_record<obj_type>,
      "ctl::with<Path>: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");
  static_assert(detail::is_path_v<path_type>,
                "ctl::with<Path>: Path must be ctl::path<...> or ctl::field<...>");
  static_assert(path_applicable<obj_type&, path_type>,
                "ctl::with<Path>: invalid member pointer chain for object type");
  static_assert(std::copy_constructible<obj_type>,
                "ctl::with<Path>: object type must be copy constructible");

  return {};
}

// ============================================================================
// Functional in-place mutation - update<Path>(obj, fn) returns T&
// ============================================================================

template <typename Path, typename T, typename Fn>
  requires(!std::is_const_v<std::remove_reference_t<T>>)
constexpr T& update(T& obj, Fn&& fn) {
  using record = std::remove_cvref_t<T>;
  using path_type = std::remove_cvref_t<Path>;

  static_assert(described_record<record>,
                "ctl::update<Path>: T must satisfy ctl::described_record (register with "
                "CTL_DESCRIBE_STRUCT)");
  static_assert(detail::is_path_v<path_type>,
                "ctl::update<Path>: Path must be ctl::path<...> or ctl::field<...>");
  static_assert(path_applicable<T&, path_type>,
                "ctl::update<Path>: invalid member pointer chain for object type");

  using target_ref = path_value_t<T&, path_type>;
  static_assert(std::invocable<Fn&&, target_ref>,
                "ctl::update<Path>: fn must be invocable with target member reference");

  // Apply function to the member reference
  std::forward<Fn>(fn)(get<path_type>(obj));

  return obj;
}

// Diagnostic for const objects
template <typename Path, typename T, typename Fn>
  requires std::is_const_v<std::remove_reference_t<T>>
constexpr T& update(T& obj, Fn&&) {
  using record = std::remove_cvref_t<T>;
  using path_type = std::remove_cvref_t<Path>;

  static_assert(described_record<record>,
                "ctl::update<Path>: T must satisfy ctl::described_record (register with "
                "CTL_DESCRIBE_STRUCT)");
  static_assert(detail::is_path_v<path_type>,
                "ctl::update<Path>: Path must be ctl::path<...> or ctl::field<...>");
  static_assert(!std::is_const_v<std::remove_reference_t<T>>,
                "ctl::update<Path>: cannot mutate const object");
  return obj;
}

// ============================================================================
// String-based mutation - set/with/update<"field">(obj, ...)
// ============================================================================

template <fixed_string... Names, typename T, typename Value>
  requires(sizeof...(Names) > 0 && !std::is_const_v<std::remove_reference_t<T>>)
constexpr T& set(T& obj, Value&& value) {
  using record = std::remove_cvref_t<T>;
  using path_type = detail::string_path_t<record, Names...>;

  static_assert(
      described_record<record>,
      "ctl::set<\"name\">: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");

  return set<path_type>(obj, std::forward<Value>(value));
}

template <fixed_string... Names, typename T, typename Value>
  requires(sizeof...(Names) > 0 && std::copy_constructible<std::remove_cvref_t<T>>)
constexpr std::remove_cvref_t<T> with(T&& obj, Value&& value) {
  using record = std::remove_cvref_t<T>;
  using path_type = detail::string_path_t<record, Names...>;

  static_assert(
      described_record<record>,
      "ctl::with<\"name\">: T must satisfy ctl::described_record (register with CTL_DESCRIBE_STRUCT)");

  return with<path_type>(std::forward<T>(obj), std::forward<Value>(value));
}

template <fixed_string... Names, typename T, typename Fn>
  requires(sizeof...(Names) > 0 && !std::is_const_v<std::remove_reference_t<T>>)
constexpr T& update(T& obj, Fn&& fn) {
  using record = std::remove_cvref_t<T>;
  using path_type = detail::string_path_t<record, Names...>;

  static_assert(
      described_record<record>,
      "ctl::update<\"name\">: T must satisfy ctl::described_record (register with "
      "CTL_DESCRIBE_STRUCT)");

  return update<path_type>(obj, std::forward<Fn>(fn));
}

}  // namespace ctl
