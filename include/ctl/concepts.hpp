#pragma once

#include "ctl/detail/describe_traits.hpp"

namespace ctl {

template <typename T>
concept described_record = detail::is_described_record_v<T>;

}
