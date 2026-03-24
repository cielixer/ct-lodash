#pragma once

#include <boost/describe.hpp>
#include <boost/mp11.hpp>
#include <cstddef>
#include <type_traits>

#include "ctl/concepts.hpp"
#include "ctl/detail/fixed_string.hpp"
#include "ctl/path.hpp"

namespace ctl::detail {

template <typename T, fixed_string Name>
struct name_matches {
  template <typename Desc>
  using fn = std::bool_constant<names_equal(Desc::name, Name.view())>;
};

template <typename T, fixed_string Name>
struct member_lookup {
  using record = std::remove_cvref_t<T>;
  using descriptors =
      boost::describe::describe_members<record, boost::describe::mod_public>;

  using index = boost::mp11::mp_find_if_q<descriptors, name_matches<record, Name>>;

  static_assert(index::value < boost::mp11::mp_size<descriptors>::value,
                "ctl: member name not found in described struct");

  using descriptor = boost::mp11::mp_at<descriptors, index>;
  static constexpr auto pointer = descriptor::pointer;
};

template <typename T, fixed_string Name>
inline constexpr auto member_ptr_v = member_lookup<T, Name>::pointer;

template <auto HeadPtr, typename Tail>
struct concat_paths;

template <auto HeadPtr, auto... TailPtrs>
struct concat_paths<HeadPtr, ctl::path<TailPtrs...>> {
  using type = ctl::path<HeadPtr, TailPtrs...>;
};

template <typename T, fixed_string... Names>
struct string_to_path;

template <typename T, fixed_string Name>
struct string_to_path<T, Name> {
  static constexpr auto ptr = member_ptr_v<T, Name>;
  using type = ctl::path<ptr>;
};

template <typename T, fixed_string First, fixed_string... Rest>
struct string_to_path<T, First, Rest...> {
  static constexpr auto ptr = member_ptr_v<T, First>;
  using member_type =
      std::remove_cvref_t<decltype(std::declval<std::remove_cvref_t<T>>().*ptr)>;
  using tail = typename string_to_path<member_type, Rest...>::type;
  using type = typename concat_paths<ptr, tail>::type;
};

template <typename T, fixed_string... Names>
using string_path_t = typename string_to_path<T, Names...>::type;

template <typename T>
struct is_fixed_string : std::false_type {};

template <std::size_t N>
struct is_fixed_string<fixed_string<N>> : std::true_type {};

template <typename T>
inline constexpr bool is_fixed_string_v = is_fixed_string<T>::value;

template <typename T, fixed_string Name>
struct has_member {
  using record = std::remove_cvref_t<T>;
  using descriptors =
      boost::describe::describe_members<record, boost::describe::mod_public>;
  using index = boost::mp11::mp_find_if_q<descriptors, name_matches<record, Name>>;
  static constexpr bool value = index::value < boost::mp11::mp_size<descriptors>::value;
};

template <typename T, fixed_string Name>
inline constexpr bool has_member_v = has_member<T, Name>::value;

template <typename T, fixed_string First, fixed_string... Rest>
struct has_member_path {
  static constexpr bool value = []() {
    if constexpr (!has_member_v<T, First>) {
      return false;
    } else {
      using member_type = std::remove_cvref_t<
          decltype(std::declval<std::remove_cvref_t<T>>().*member_ptr_v<T, First>)>;
      if constexpr (sizeof...(Rest) == 0) {
        return true;
      } else {
        return has_member_path<member_type, Rest...>::value;
      }
    }
  }();
};

template <typename T, fixed_string Name>
struct has_member_path<T, Name> {
  static constexpr bool value = has_member_v<T, Name>;
};

template <typename T, fixed_string... Names>
inline constexpr bool has_member_path_v = has_member_path<T, Names...>::value;

}  // namespace ctl::detail
