#pragma once

#include <cstddef>
#include <string_view>

namespace ctl {

template <std::size_t N>
struct fixed_string {
  char data[N]{};  // NOLINT(modernize-avoid-c-arrays) — structural type requires C array

  constexpr fixed_string(const char (&str)[N]) {  // NOLINT(modernize-avoid-c-arrays)
    for (std::size_t i = 0; i < N; ++i) {
      data[i] = str[i];
    }
  }

  [[nodiscard]] constexpr std::string_view view() const { return {data, N - 1}; }
  constexpr auto operator<=>(const fixed_string&) const = default;
  constexpr bool operator==(const fixed_string&) const = default;
};

template <std::size_t N>
fixed_string(const char (&)[N]) -> fixed_string<N>;  // NOLINT(modernize-avoid-c-arrays)

namespace detail {

constexpr bool names_equal(const char* a, std::string_view b) {
  for (std::size_t i = 0; i < b.size(); ++i) {
    if (a[i] == '\0' || a[i] != b[i]) {
      return false;
    }
  }
  return a[b.size()] == '\0';
}

}  // namespace detail

}  // namespace ctl
