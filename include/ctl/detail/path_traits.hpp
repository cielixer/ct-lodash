#pragma once

#include <type_traits>
#include <utility>

namespace ctl {
template <auto... Ptrs>
struct path;
}

namespace ctl::detail {

template <typename T>
struct is_path : std::false_type {};

template <auto... Ptrs>
struct is_path<ctl::path<Ptrs...>> : std::true_type {};

template <typename T>
inline constexpr bool is_path_v = is_path<std::remove_cvref_t<T>>::value;

template <auto Ptr>
inline constexpr bool is_member_object_ptr_constant_v =
    std::is_member_object_pointer_v<decltype(Ptr)>;

template <typename Obj, auto Ptr>
using apply_member_ptr_t = decltype((std::declval<Obj>().*Ptr));

template <typename Obj, auto... Ptrs>
struct path_value_impl;

template <typename Obj, auto Ptr>
struct path_value_impl<Obj, Ptr> {
  using type = apply_member_ptr_t<Obj, Ptr>;
};

template <typename Obj, auto Ptr, auto... Rest>
struct path_value_impl<Obj, Ptr, Rest...> {
  using next_t = apply_member_ptr_t<Obj, Ptr>;
  using type = typename path_value_impl<next_t, Rest...>::type;
};

template <typename Obj>
constexpr bool path_chain_applicable() {
  return false;
}

template <typename Obj, auto Ptr>
constexpr bool path_chain_applicable() {
  if constexpr (!is_member_object_ptr_constant_v<Ptr>) {
    return false;
  } else {
    return requires { std::declval<Obj>().*Ptr; };
  }
}

template <typename Obj, auto Ptr, auto... Rest>
  requires(sizeof...(Rest) > 0)
constexpr bool path_chain_applicable() {
  if constexpr (!is_member_object_ptr_constant_v<Ptr>) {
    return false;
  } else if constexpr (requires { std::declval<Obj>().*Ptr; }) {
    using next_t = decltype((std::declval<Obj>().*Ptr));
    return path_chain_applicable<next_t, Rest...>();
  } else {
    return false;
  }
}

template <typename Obj, typename Path>
struct path_applicable : std::false_type {};

template <typename Obj, auto... Ptrs>
struct path_applicable<Obj, ctl::path<Ptrs...>>
    : std::bool_constant<path_chain_applicable<Obj, Ptrs...>()> {};

template <typename Obj, typename Path>
inline constexpr bool path_applicable_v = path_applicable<Obj, Path>::value;

template <typename Obj, typename Path, bool = path_applicable_v<Obj, Path>>
struct path_value;

template <typename Obj, typename Path>
struct path_value<Obj, Path, false> {};

template <typename Obj, auto... Ptrs>
struct path_value<Obj, ctl::path<Ptrs...>, true> {
  using type = typename path_value_impl<Obj, Ptrs...>::type;
};

}  // namespace ctl::detail
