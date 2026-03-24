#pragma once

#include <type_traits>

#include "ctl/detail/path_traits.hpp"

namespace ctl {

template <auto... Ptrs>
struct path {
  static_assert(sizeof...(Ptrs) > 0, "ctl::path requires at least one member pointer");
  static_assert((std::is_member_object_pointer_v<decltype(Ptrs)> && ...),
                "ctl::path requires member object pointers");
};

template <auto Ptr>
using field = path<Ptr>;

template <typename Obj, typename Path>
concept path_applicable = detail::path_applicable_v<Obj, std::remove_cvref_t<Path>>;

template <typename Obj, typename Path>
struct path_value_checked {
  using path_type = std::remove_cvref_t<Path>;

  static_assert(detail::is_path_v<path_type>,
                "ctl::path_value_t<Obj, Path>: Path must be ctl::path<...> or ctl::field<...>");
  static_assert(path_applicable<Obj, path_type>,
                "ctl::path_value_t<Obj, Path>: path is not applicable to object type");

  using type = typename detail::path_value<Obj, path_type>::type;
};

template <typename Obj, typename Path>
using path_value_t = typename path_value_checked<Obj, Path>::type;

}  // namespace ctl
