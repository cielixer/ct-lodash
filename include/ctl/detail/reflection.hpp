#pragma once

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <boost/pfr.hpp>
#include <cstddef>
#include <string_view>
#include <type_traits>
#include <utility>

#include "ctl/concepts.hpp"

namespace ctl::detail::reflection {

template <typename T>
using record_t = std::remove_cvref_t<T>;

template <typename T>
using member_descriptors_t =
    boost::describe::describe_members<record_t<T>, boost::describe::mod_public>;

template <std::size_t I, typename T>
constexpr decltype(auto) get_by_index(T&& obj) {
  return boost::pfr::get<I>(std::forward<T>(obj));
}

template <typename T>
inline constexpr std::size_t field_count_v = boost::pfr::tuple_size_v<record_t<T>>;

template <typename T>
inline constexpr std::size_t member_count_v = boost::mp11::mp_size<member_descriptors_t<T>>::value;

template <typename T, typename Visitor>
constexpr void for_each_member(T&& obj, Visitor&& visitor) {
  using record = record_t<T>;

  if constexpr (!ctl::described_record<record>) {
    static_assert(
        ctl::described_record<record>,
        "expected described record; register with CTL_DESCRIBE_STRUCT(Type, Bases, Members)");
  } else {
    using descriptors = member_descriptors_t<record>;
    boost::mp11::mp_for_each<descriptors>([&]<typename Desc>(Desc) {
      visitor(std::string_view{Desc::name}, std::forward<T>(obj).*Desc::pointer);
    });
  }
}

}  // namespace ctl::detail::reflection
