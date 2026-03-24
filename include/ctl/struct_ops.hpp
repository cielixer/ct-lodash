#pragma once

#include <any>
#include <optional>
#include <string_view>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

#include <boost/mp11.hpp>

#include "ctl/concepts.hpp"
#include "ctl/detail/reflection.hpp"

namespace ctl {

// ============================================================================
// is_equal — deep field-wise equality for described structs
// ============================================================================

template <typename T>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] constexpr bool is_equal(const T& lhs, const T& rhs) {
  using record = std::remove_cvref_t<T>;
  using descriptors = detail::reflection::member_descriptors_t<record>;
  bool equal = true;
  boost::mp11::mp_for_each<descriptors>([&]<typename Desc>(Desc) {
    if (!equal) return;
    const auto& l = lhs.*Desc::pointer;
    const auto& r = rhs.*Desc::pointer;
    using member_type = std::remove_cvref_t<decltype(l)>;
    if constexpr (described_record<member_type>) {
      if (!is_equal(l, r)) equal = false;
    } else {
      if (!(l == r)) equal = false;
    }
  });
  return equal;
}

// ============================================================================
// clone — deep copy of a described struct
// ============================================================================

template <typename T>
  requires(described_record<std::remove_cvref_t<T>> &&
           std::is_copy_constructible_v<std::remove_cvref_t<T>>)
[[nodiscard]] constexpr auto clone(const T& obj) -> std::remove_cvref_t<T> {
  return obj;
}

// ============================================================================
// to_tuple — convert described struct to std::tuple of field values
// ============================================================================

namespace detail {

template <typename T, typename Descriptors, std::size_t... Is>
constexpr auto to_tuple_impl(const T& obj, std::index_sequence<Is...>) {
  return std::tuple{(obj.*boost::mp11::mp_at_c<Descriptors, Is>::pointer)...};
}

template <typename T, typename Descriptors, typename Fn, std::size_t... Is>
constexpr auto map_values_impl(const T& obj, Fn& fn, std::index_sequence<Is...>) {
  return std::tuple{fn(std::string_view{boost::mp11::mp_at_c<Descriptors, Is>::name},
                       obj.*boost::mp11::mp_at_c<Descriptors, Is>::pointer)...};
}

}  // namespace detail

template <typename T>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto to_tuple(const T& obj) {
  using record = std::remove_cvref_t<T>;
  using descriptors = detail::reflection::member_descriptors_t<record>;
  constexpr auto count = boost::mp11::mp_size<descriptors>::value;
  return detail::to_tuple_impl<record, descriptors>(obj, std::make_index_sequence<count>{});
}

// ============================================================================
// from_tuple — construct described struct from std::tuple
// ============================================================================

namespace detail {

template <typename T, std::size_t... Is, typename Tuple>
constexpr T from_tuple_impl(Tuple&& tup, std::index_sequence<Is...>) {
  return T{std::get<Is>(std::forward<Tuple>(tup))...};
}

}  // namespace detail

template <typename T, typename Tuple>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto from_tuple(Tuple&& tup) -> std::remove_cvref_t<T> {
  using record = std::remove_cvref_t<T>;
  constexpr auto count =
      std::tuple_size_v<std::remove_cvref_t<Tuple>>;
  return detail::from_tuple_impl<record>(std::forward<Tuple>(tup),
                                         std::make_index_sequence<count>{});
}

// ============================================================================
// map_values — transform all field values of a described struct
// ============================================================================

template <typename T, typename Fn>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto map_values(const T& obj, Fn&& fn) {
  using record = std::remove_cvref_t<T>;
  using descriptors = detail::reflection::member_descriptors_t<record>;
  constexpr auto count = boost::mp11::mp_size<descriptors>::value;
  auto&& fn_ref = fn;
  return detail::map_values_impl<record, descriptors>(
      obj, fn_ref, std::make_index_sequence<count>{});
}

// ============================================================================
// to_pairs — convert described struct to vector of (name, value) pairs
// ============================================================================

template <typename T>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] auto to_pairs(const T& obj) {
  using record = std::remove_cvref_t<T>;
  using descriptors = detail::reflection::member_descriptors_t<record>;
  std::vector<std::pair<std::string_view, std::any>> pairs;
  pairs.reserve(boost::mp11::mp_size<descriptors>::value);
  boost::mp11::mp_for_each<descriptors>([&]<typename Desc>(Desc) {
    pairs.push_back(
        std::pair{std::string_view{Desc::name}, std::any{obj.*Desc::pointer}});
  });
  return pairs;
}

// ============================================================================
// find_key — find name of first field matching predicate over value
// ============================================================================

template <typename T, typename Pred>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto find_key(const T& obj, Pred&& pred)
    -> std::optional<std::string_view> {
  using record = std::remove_cvref_t<T>;
  using descriptors = detail::reflection::member_descriptors_t<record>;
  std::optional<std::string_view> found = std::nullopt;
  auto&& pred_ref = pred;
  boost::mp11::mp_for_each<descriptors>([&]<typename Desc>(Desc) {
    if (found.has_value()) return;
    if constexpr (std::is_invocable_v<decltype(pred_ref)&,
                                      decltype((obj.*Desc::pointer))>) {
      if (pred_ref(obj.*Desc::pointer)) {
        found = std::string_view{Desc::name};
      }
    }
  });
  return found;
}

// ============================================================================
// matches — create predicate matching all fields against a pattern
// ============================================================================

template <typename T>
  requires described_record<std::remove_cvref_t<T>>
[[nodiscard]] constexpr auto matches(T pattern) {
  using record = std::remove_cvref_t<T>;
  return [pattern = std::move(pattern)](const record& obj) constexpr {
    return is_equal(pattern, obj);
  };
}

// ============================================================================
// pick — copy fields from source to a user-defined target struct by name
//
// C++23 cannot synthesize new struct types at compile time.
// The caller must define the target struct and register it with
// CTL_DESCRIBE_STRUCT. pick then copies every field in Target whose name
// matches a field in Source.
//
//   auto result = ctl::pick<SmallPerson>(person);
//   result.name;  // copied from person.name
// ============================================================================

namespace detail {

template <typename SrcDescs, typename DstDesc>
struct find_matching_src_field {
  static constexpr std::string_view dst_name{DstDesc::name};

  template <typename SrcDesc>
  using name_matches = std::bool_constant<
      std::string_view{SrcDesc::name} == dst_name>;

  using found = boost::mp11::mp_filter<name_matches, SrcDescs>;
  static constexpr bool value = boost::mp11::mp_size<found>::value > 0;
  using type = boost::mp11::mp_eval_if_c<!value, void, boost::mp11::mp_front, found>;
};

}  // namespace detail

template <typename Target, typename Source>
  requires(described_record<std::remove_cvref_t<Target>> &&
           described_record<std::remove_cvref_t<Source>>)
[[nodiscard]] constexpr auto pick(const Source& source) -> Target {
  using src_record = std::remove_cvref_t<Source>;
  using dst_record = std::remove_cvref_t<Target>;
  using src_descs = detail::reflection::member_descriptors_t<src_record>;
  using dst_descs = detail::reflection::member_descriptors_t<dst_record>;

  dst_record result{};
  boost::mp11::mp_for_each<dst_descs>([&]<typename DstDesc>(DstDesc) {
    using finder = detail::find_matching_src_field<src_descs, DstDesc>;
    static_assert(finder::value,
                  "ctl::pick: target field has no matching field in source");
    using src_desc = typename finder::type;
    result.*DstDesc::pointer = source.*src_desc::pointer;
  });
  return result;
}

// ============================================================================
// omit — inverse of pick: copy source fields into a target that excludes some
//
// Identical mechanism to pick. The caller defines a target struct containing
// only the fields they want to keep (i.e., omitting the unwanted ones).
//
//   auto result = ctl::omit<PersonWithoutAge>(person);
// ============================================================================

template <typename Target, typename Source>
  requires(described_record<std::remove_cvref_t<Target>> &&
           described_record<std::remove_cvref_t<Source>>)
[[nodiscard]] constexpr auto omit(const Source& source) -> Target {
  return pick<Target>(source);
}

// ============================================================================
// merge — deep field-wise merge of same-type described structs
//
// Rightmost source wins for each field. Nested described_record fields
// are merged recursively rather than overwritten wholesale.
//
//   auto result = ctl::merge(base, override1, override2);
// ============================================================================

namespace detail {

template <typename T>
constexpr void merge_into(T& target, const T& source) {
  using record = std::remove_cvref_t<T>;
  using descriptors = reflection::member_descriptors_t<record>;
  boost::mp11::mp_for_each<descriptors>([&]<typename Desc>(Desc) {
    using member_type = std::remove_cvref_t<decltype(target.*Desc::pointer)>;
    if constexpr (described_record<member_type>) {
      merge_into(target.*Desc::pointer, source.*Desc::pointer);
    } else {
      target.*Desc::pointer = source.*Desc::pointer;
    }
  });
}

}  // namespace detail

template <typename T, typename... Sources>
  requires(described_record<std::remove_cvref_t<T>> &&
           (std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<Sources>> && ...))
[[nodiscard]] constexpr auto merge(const T& base, const Sources&... sources) -> std::remove_cvref_t<T> {
  auto result = base;
  (detail::merge_into(result, sources), ...);
  return result;
}

// ============================================================================
// defaults — fill fields from fallbacks, first value wins (leftmost priority)
//
// Opposite of merge: the base value is kept for every field, and fallback
// values are only applied for nested described_record fields recursively.
// For flat same-type structs this is equivalent to returning the base.
// The real utility is with nested structs where inner fields may differ.
//
//   auto result = ctl::defaults(partial, fallback1, fallback2);
// ============================================================================

namespace detail {

template <typename T>
constexpr void defaults_into(T& target, const T& fallback) {
  using record = std::remove_cvref_t<T>;
  using descriptors = reflection::member_descriptors_t<record>;
  boost::mp11::mp_for_each<descriptors>([&]<typename Desc>(Desc) {
    using member_type = std::remove_cvref_t<decltype(target.*Desc::pointer)>;
    if constexpr (described_record<member_type>) {
      defaults_into(target.*Desc::pointer, fallback.*Desc::pointer);
    }
  });
}

}  // namespace detail

template <typename T, typename... Fallbacks>
  requires(described_record<std::remove_cvref_t<T>> &&
           (std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<Fallbacks>> && ...))
[[nodiscard]] constexpr auto defaults(const T& base, const Fallbacks&... fallbacks) -> std::remove_cvref_t<T> {
  auto result = base;
  (detail::defaults_into(result, fallbacks), ...);
  return result;
}

}  // namespace ctl
