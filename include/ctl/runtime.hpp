#pragma once

#include <string_view>
#include <type_traits>
#include <utility>

#include "ctl/concepts.hpp"
#include "ctl/detail/reflection.hpp"

namespace ctl::runtime {

// ============================================================================
// visit - Apply visitor to top-level member by name
// ============================================================================

template <typename T, typename Visitor>
bool visit(T&& obj, std::string_view name, Visitor&& visitor) {
  using record = std::remove_cvref_t<T>;
  static_assert(described_record<record>,
                "ctl::runtime::visit: T must satisfy ctl::described_record (register with "
                "CTL_DESCRIBE_STRUCT)");

  bool found = false;
  detail::reflection::for_each_member(std::forward<T>(obj),
                                      [&](std::string_view member_name, auto& member_ref) {
                                        if (member_name == name && !found) {
                                          std::forward<Visitor>(visitor)(member_ref);
                                          found = true;
                                        }
                                      });
  return found;
}

// ============================================================================
// try_get - Get pointer to member if name and type match
// ============================================================================

template <typename Expected, typename T>
auto try_get(T&& obj, std::string_view name) {
  using record = std::remove_cvref_t<T>;
  static_assert(described_record<record>,
                "ctl::runtime::try_get: T must satisfy ctl::described_record (register with "
                "CTL_DESCRIBE_STRUCT)");

  using result_type =
      std::conditional_t<std::is_const_v<std::remove_reference_t<T>>, const Expected*, Expected*>;

  result_type result = nullptr;
  detail::reflection::for_each_member(
      std::forward<T>(obj), [&](std::string_view member_name, auto& member_ref) {
        if (member_name == name && !result) {
          using member_type = std::remove_cvref_t<decltype(member_ref)>;
          if constexpr (std::is_same_v<member_type, Expected>) {
            result = &member_ref;
          }
        }
      });
  return result;
}

// ============================================================================
// try_update - Update member if name and type match (mutable objects only)
// ============================================================================

template <typename Expected, typename T, typename Value>
  requires(!std::is_const_v<std::remove_reference_t<T>>)
bool try_update(T& obj, std::string_view name, Value&& value) {
  using record = std::remove_cvref_t<T>;
  static_assert(described_record<record>,
                "ctl::runtime::try_update: T must satisfy ctl::described_record (register with "
                "CTL_DESCRIBE_STRUCT)");

  bool updated = false;
  detail::reflection::for_each_member(obj, [&](std::string_view member_name, auto& member_ref) {
    if (member_name == name && !updated) {
      using member_type = std::remove_cvref_t<decltype(member_ref)>;
      if constexpr (std::is_same_v<member_type, Expected> &&
                    std::is_assignable_v<member_type&, Value>) {
        member_ref = std::forward<Value>(value);
        updated = true;
      }
    }
  });
  return updated;
}

// Diagnostic for const objects
template <typename Expected, typename T, typename Value>
  requires std::is_const_v<std::remove_reference_t<T>>
bool try_update(T&, std::string_view, Value&&) {
  using record = std::remove_cvref_t<T>;
  static_assert(described_record<record>,
                "ctl::runtime::try_update: T must satisfy ctl::described_record (register with "
                "CTL_DESCRIBE_STRUCT)");
  static_assert(!std::is_const_v<std::remove_reference_t<T>>,
                "ctl::runtime::try_update: cannot mutate const object");
  return false;
}

}  // namespace ctl::runtime
