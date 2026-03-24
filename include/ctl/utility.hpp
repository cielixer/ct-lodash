#pragma once

#include <cstddef>
#include <functional>
#include <optional>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace ctl {

// ============================================================================
// identity — returns its argument unchanged
// ============================================================================

struct identity_fn {
  template <typename T>
  [[nodiscard]] constexpr auto operator()(T&& val) const -> std::remove_cvref_t<T> {
    return std::forward<T>(val);
  }
};

inline constexpr identity_fn identity{};

// ============================================================================
// constant — returns a function that always yields the captured value
// ============================================================================

template <typename V>
[[nodiscard]] constexpr auto constant(V value) {
  return [v = std::move(value)](auto&&...) -> const V& { return v; };
}

// ============================================================================
// tap — call fn(obj) for side effects, then return obj
// ============================================================================

template <typename T, typename Fn>
constexpr T& tap(T& obj, Fn&& fn) {
  std::forward<Fn>(fn)(obj);
  return obj;
}

template <typename T, typename Fn>
constexpr const T& tap(const T& obj, Fn&& fn) {
  std::forward<Fn>(fn)(obj);
  return obj;
}

// ============================================================================
// negate — invert predicate result
// ============================================================================

template <typename Fn>
[[nodiscard]] constexpr auto negate(Fn fn) {
  return [fn = std::move(fn)](auto&&... args) {
    return !fn(std::forward<decltype(args)>(args)...);
  };
}

// ============================================================================
// noop — callable that does nothing
// ============================================================================

struct noop_fn {
  constexpr void operator()(auto&&...) const noexcept {}
};

inline constexpr noop_fn noop{};

// ============================================================================
// times — call function n times with index and collect results
// ============================================================================

template <typename Fn>
[[nodiscard]] constexpr auto times(std::size_t n, Fn&& fn) {
  using result_type = std::invoke_result_t<Fn&, std::size_t>;
  static_assert(!std::is_void_v<result_type>, "times requires non-void result type");

  std::vector<result_type> result;
  result.reserve(n);
  auto&& fn_ref = fn;
  for (std::size_t i = 0; i < n; ++i) {
    result.push_back(std::invoke(fn_ref, i));
  }
  return result;
}

// ============================================================================
// range — generate integer ranges
// ============================================================================

[[nodiscard]] constexpr auto range(int end) -> std::vector<int>;
[[nodiscard]] constexpr auto range(int start, int end) -> std::vector<int>;
[[nodiscard]] constexpr auto range(int start, int end, int step) -> std::vector<int>;

[[nodiscard]] constexpr auto range(int end) -> std::vector<int> {
  return range(0, end, 1);
}

[[nodiscard]] constexpr auto range(int start, int end) -> std::vector<int> {
  return range(start, end, start <= end ? 1 : -1);
}

[[nodiscard]] constexpr auto range(int start, int end, int step) -> std::vector<int> {
  std::vector<int> result;
  if (step == 0) {
    return result;
  }
  if (step > 0 && start >= end) {
    return result;
  }
  if (step < 0 && start <= end) {
    return result;
  }

  for (int i = start; (step > 0) ? (i < end) : (i > end); i += step) {
    result.push_back(i);
  }
  return result;
}

namespace detail {

template <std::size_t I, typename Tuple, typename... Args>
constexpr decltype(auto) pipe_invoke(Tuple& funcs, Args&&... args) {
  if constexpr (I + 1 == std::tuple_size_v<std::remove_reference_t<Tuple>>) {
    return std::invoke(std::get<I>(funcs), std::forward<Args>(args)...);
  } else {
    return pipe_invoke<I + 1>(funcs,
                              std::invoke(std::get<I>(funcs), std::forward<Args>(args)...));
  }
}

template <std::size_t I, typename Tuple, typename... Args>
constexpr decltype(auto) compose_invoke(Tuple& funcs, Args&&... args) {
  if constexpr (I + 1 == std::tuple_size_v<std::remove_reference_t<Tuple>>) {
    return std::invoke(std::get<I>(funcs), std::forward<Args>(args)...);
  } else {
    return std::invoke(std::get<I>(funcs),
                       compose_invoke<I + 1>(funcs, std::forward<Args>(args)...));
  }
}

}  // namespace detail

// ============================================================================
// pipe — compose functions left-to-right
// ============================================================================

template <typename... Fns>
  requires(sizeof...(Fns) > 0)
[[nodiscard]] constexpr auto pipe(Fns... fns) {
  return [funcs = std::tuple{std::move(fns)...}](auto&&... args) mutable -> decltype(auto) {
    return detail::pipe_invoke<0>(funcs, std::forward<decltype(args)>(args)...);
  };
}

// ============================================================================
// compose — compose functions right-to-left
// ============================================================================

template <typename... Fns>
  requires(sizeof...(Fns) > 0)
[[nodiscard]] constexpr auto compose(Fns... fns) {
  return [funcs = std::tuple{std::move(fns)...}](auto&&... args) mutable -> decltype(auto) {
    return detail::compose_invoke<0>(funcs, std::forward<decltype(args)>(args)...);
  };
}

// ============================================================================
// cond — evaluate predicate/transform pairs in order
// ============================================================================

template <typename... Pairs>
[[nodiscard]] constexpr auto cond(Pairs... pairs) {
  return [pairs_tuple = std::tuple{std::move(pairs)...}](auto&&... args) mutable {
    using first_pair_t =
        std::tuple_element_t<0, std::remove_reference_t<decltype(pairs_tuple)>>;
    using result_type = std::invoke_result_t<decltype(std::declval<first_pair_t&>().second)&,
                                             decltype(args)...>;

    std::optional<result_type> result = std::nullopt;
    auto try_pair = [&](auto& pair) {
      if (result.has_value()) {
        return;
      }
      if (std::invoke(pair.first, args...)) {
        result = std::optional<result_type>{std::invoke(pair.second, args...)};
      }
    };
    std::apply([&](auto&... pair) { (try_pair(pair), ...); }, pairs_tuple);

    return result;
  };
}

// ============================================================================
// thru — pass value through function and return function result
// ============================================================================

template <typename T, typename Fn>
[[nodiscard]] constexpr auto thru(T&& val, Fn&& fn) {
  return std::forward<Fn>(fn)(std::forward<T>(val));
}

}  // namespace ctl
