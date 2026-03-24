#pragma once

#include <boost/describe.hpp>
#include <type_traits>

namespace ctl::detail {

template <typename T>
inline constexpr bool is_described_record_v =
    boost::describe::has_describe_members<std::remove_cvref_t<T>>::value;

}
