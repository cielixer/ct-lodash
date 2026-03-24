#pragma once

#include <algorithm>
#include <cstddef>
#include <functional>
#include <initializer_list>
#include <iterator>
#include <optional>
#include <type_traits>
#include <utility>
#include <vector>

#include "ctl/detail/container_traits.hpp"

namespace ctl {

// ============================================================================
// for_each — iterate elements; (key, value) for pair ranges, (elem) otherwise
// ============================================================================

template <typename C, typename Fn>
  requires detail::pair_value_range<const C>
constexpr void for_each(const C& container, Fn&& fn) {
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Fn, decltype(key), decltype(value)>) {
      fn(key, value);
    } else {
      fn(std::pair{key, value});
    }
  }
}

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
constexpr void for_each(const C& container, Fn&& fn) {
  for (const auto& elem : container) {
    fn(elem);
  }
}

// ============================================================================
// map — transform elements into std::vector
// ============================================================================

template <typename C, typename Fn>
  requires detail::pair_value_range<const C>
constexpr auto map(const C& container, Fn&& fn) {
  using result_type =
      std::invoke_result_t<Fn, const typename std::ranges::range_value_t<C>::first_type&,
                           const typename std::ranges::range_value_t<C>::second_type&>;
  std::vector<result_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  for (const auto& [key, value] : container) {
    result.push_back(fn(key, value));
  }
  return result;
}

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
constexpr auto map(const C& container, Fn&& fn) {
  using result_type = std::invoke_result_t<Fn, const std::ranges::range_value_t<C>&>;
  std::vector<result_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  for (const auto& elem : container) {
    result.push_back(fn(elem));
  }
  return result;
}

// ============================================================================
// filter — select elements matching predicate into std::vector
// ============================================================================

template <typename C, typename Pred>
  requires detail::pair_value_range<const C>
constexpr auto filter(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Pred, decltype(key), decltype(value)>) {
      if (pred(key, value)) {
        result.emplace_back(key, value);
      }
    } else {
      if (pred(std::pair{key, value})) {
        result.emplace_back(key, value);
      }
    }
  }
  return result;
}

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
constexpr auto filter(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  for (const auto& elem : container) {
    if (pred(elem)) {
      result.push_back(elem);
    }
  }
  return result;
}

// ============================================================================
// reduce — accumulate elements with binary operation
// ============================================================================

template <typename C, typename Init, typename Fn>
  requires detail::pair_value_range<const C>
constexpr auto reduce(const C& container, Init init, Fn&& fn) {
  auto acc = std::move(init);
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Fn, decltype(acc), decltype(key), decltype(value)>) {
      acc = fn(std::move(acc), key, value);
    } else {
      acc = fn(std::move(acc), std::pair{key, value});
    }
  }
  return acc;
}

template <typename C, typename Init, typename Fn>
  requires detail::non_pair_range<const C>
constexpr auto reduce(const C& container, Init init, Fn&& fn) {
  auto acc = std::move(init);
  for (const auto& elem : container) {
    acc = fn(std::move(acc), elem);
  }
  return acc;
}

// ============================================================================
// transform — reduce variant: mutates accumulator by reference, early exit
// ============================================================================

template <typename C, typename Acc, typename Fn>
  requires detail::pair_value_range<const C>
constexpr auto transform(const C& container, Acc acc, Fn&& fn) {
  for (const auto& [key, value] : container) {
    bool cont = true;
    if constexpr (std::is_invocable_r_v<bool, Fn, Acc&, decltype(key), decltype(value)>) {
      cont = fn(acc, key, value);
    } else if constexpr (std::is_invocable_v<Fn, Acc&, decltype(key), decltype(value)>) {
      fn(acc, key, value);
    } else if constexpr (std::is_invocable_r_v<bool, Fn, Acc&, std::pair<decltype(key), decltype(value)>>) {
      cont = fn(acc, std::pair{key, value});
    } else {
      fn(acc, std::pair{key, value});
    }
    if (!cont) break;
  }
  return acc;
}

template <typename C, typename Acc, typename Fn>
  requires detail::non_pair_range<const C>
constexpr auto transform(const C& container, Acc acc, Fn&& fn) {
  for (const auto& elem : container) {
    if constexpr (std::is_invocable_r_v<bool, Fn, Acc&, decltype(elem)>) {
      if (!fn(acc, elem)) break;
    } else {
      fn(acc, elem);
    }
  }
  return acc;
}

// ============================================================================
// find — first element matching predicate, or std::nullopt
// ============================================================================

template <typename C, typename Pred>
  requires detail::pair_value_range<const C>
constexpr auto find(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Pred, decltype(key), decltype(value)>) {
      if (pred(key, value)) {
        return std::optional<value_type>{value_type{key, value}};
      }
    } else {
      auto p = std::pair{key, value};
      if (pred(p)) {
        return std::optional<value_type>{value_type{key, value}};
      }
    }
  }
  return std::optional<value_type>{std::nullopt};
}

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
constexpr auto find(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  for (const auto& elem : container) {
    if (pred(elem)) {
      return std::optional<value_type>{elem};
    }
  }
  return std::optional<value_type>{std::nullopt};
}

// ============================================================================
// find_index — index of first matching element, or std::nullopt
// ============================================================================

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
constexpr auto find_index(const C& container, Pred&& pred) -> std::optional<std::size_t> {
  std::size_t idx = 0;
  for (const auto& elem : container) {
    if (pred(elem)) {
      return idx;
    }
    ++idx;
  }
  return std::nullopt;
}

// ============================================================================
// every — true if all elements satisfy predicate
// ============================================================================

template <typename C, typename Pred>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr bool every(const C& container, Pred&& pred) {
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Pred, decltype(key), decltype(value)>) {
      if (!pred(key, value)) return false;
    } else {
      if (!pred(std::pair{key, value})) return false;
    }
  }
  return true;
}

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr bool every(const C& container, Pred&& pred) {
  for (const auto& elem : container) {
    if (!pred(elem)) return false;
  }
  return true;
}

// ============================================================================
// some — true if any element satisfies predicate
// ============================================================================

template <typename C, typename Pred>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr bool some(const C& container, Pred&& pred) {
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Pred, decltype(key), decltype(value)>) {
      if (pred(key, value)) return true;
    } else {
      if (pred(std::pair{key, value})) return true;
    }
  }
  return false;
}

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr bool some(const C& container, Pred&& pred) {
  for (const auto& elem : container) {
    if (pred(elem)) return true;
  }
  return false;
}

// ============================================================================
// count_by — group elements by key function, count occurrences
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
constexpr auto count_by(const C& container, Fn&& fn) {
  using key_type = std::invoke_result_t<Fn, const std::ranges::range_value_t<C>&>;
  detail::auto_map<key_type, std::size_t> result;
  for (const auto& elem : container) {
    ++result[fn(elem)];
  }
  return result;
}

template <typename C, typename Fn>
  requires detail::pair_value_range<const C>
constexpr auto count_by(const C& container, Fn&& fn) {
  using pair_t = std::ranges::range_value_t<C>;
  using key_type = std::invoke_result_t<Fn, const typename pair_t::first_type&,
                                        const typename pair_t::second_type&>;
  detail::auto_map<key_type, std::size_t> result;
  for (const auto& [key, value] : container) {
    ++result[fn(key, value)];
  }
  return result;
}

// ============================================================================
// group_by — group elements by key function into map of vectors
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto group_by(const C& container, Fn&& fn) {
  using value_type = std::ranges::range_value_t<C>;
  using key_type = std::invoke_result_t<Fn, const value_type&>;
  detail::auto_map<key_type, std::vector<value_type>> result;
  for (const auto& elem : container) {
    result[fn(elem)].push_back(elem);
  }
  return result;
}

template <typename C, typename Fn>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr auto group_by(const C& container, Fn&& fn) {
  using pair_t = std::ranges::range_value_t<C>;
  using key_type = std::invoke_result_t<Fn, const typename pair_t::first_type&,
                                        const typename pair_t::second_type&>;
  detail::auto_map<key_type, std::vector<pair_t>> result;
  for (const auto& [key, value] : container) {
    result[fn(key, value)].emplace_back(key, value);
  }
  return result;
}

// ============================================================================
// key_by — index elements by key function into map (last wins on duplicates)
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto key_by(const C& container, Fn&& fn) {
  using value_type = std::ranges::range_value_t<C>;
  using key_type = std::invoke_result_t<Fn, const value_type&>;
  detail::auto_map<key_type, value_type> result;
  for (const auto& elem : container) {
    result.insert_or_assign(fn(elem), elem);
  }
  return result;
}

template <typename C, typename Fn>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr auto key_by(const C& container, Fn&& fn) {
  using raw_pair = std::ranges::range_value_t<C>;
  using stored_pair = std::pair<std::remove_const_t<typename raw_pair::first_type>,
                                typename raw_pair::second_type>;
  using key_type = std::invoke_result_t<Fn, const typename raw_pair::first_type&,
                                        const typename raw_pair::second_type&>;
  detail::auto_map<key_type, stored_pair> result;
  for (const auto& [key, value] : container) {
    result.insert_or_assign(fn(key, value), stored_pair{key, value});
  }
  return result;
}

// ============================================================================
// intersection — elements present in all given ranges (first range order)
// ============================================================================

namespace detail {

template <typename V, typename C>
constexpr bool contains_value(const C& container, const V& value) {
  for (const auto& elem : container) {
    if (elem == value) return true;
  }
  return false;
}

}  // namespace detail

template <typename First, typename... Rest>
  requires(detail::non_pair_range<const First> && (detail::non_pair_range<const Rest> && ...))
[[nodiscard]] constexpr auto intersection(const First& first, const Rest&... rest) {
  using value_type = std::ranges::range_value_t<First>;
  std::vector<value_type> result;
  for (const auto& elem : first) {
    if ((detail::contains_value(rest, elem) && ...)) {
      if (!detail::contains_value(result, elem)) {
        result.push_back(elem);
      }
    }
  }
  return result;
}

// ============================================================================
// difference — elements in first range not present in any other range
// ============================================================================

template <typename First, typename... Rest>
  requires(detail::non_pair_range<const First> && (detail::non_pair_range<const Rest> && ...))
[[nodiscard]] constexpr auto difference(const First& first, const Rest&... rest) {
  using value_type = std::ranges::range_value_t<First>;
  std::vector<value_type> result;
  for (const auto& elem : first) {
    if (!(detail::contains_value(rest, elem) || ...)) {
      result.push_back(elem);
    }
  }
  return result;
}

// ============================================================================
// set_union — unique elements from all given ranges (first-seen order)
// ============================================================================

template <typename First, typename... Rest>
  requires(detail::non_pair_range<const First> && (detail::non_pair_range<const Rest> && ...))
[[nodiscard]] constexpr auto set_union(const First& first, const Rest&... rest) {
  using value_type = std::ranges::range_value_t<First>;
  std::vector<value_type> result;
  auto add_unique = [&](const auto& container) {
    for (const auto& elem : container) {
      if (!detail::contains_value(result, elem)) {
        result.push_back(elem);
      }
    }
  };
  add_unique(first);
  (add_unique(rest), ...);
  return result;
}

// ============================================================================
// sort_by — return sorted vector using comparison key
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
constexpr auto sort_by(const C& container, Fn&& fn) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result(std::ranges::begin(container), std::ranges::end(container));
  std::sort(result.begin(), result.end(), [&fn](const auto& a, const auto& b) {
    return fn(a) < fn(b);
  });
  return result;
}

// ============================================================================
// order_by — multi-key sort with per-key ascending/descending direction
// ============================================================================

namespace detail {

template <typename Fn>
struct asc_order {
  Fn fn;
};

template <typename Fn>
struct desc_order {
  Fn fn;
};

template <typename T>
struct is_order : std::false_type {};

template <typename Fn>
struct is_order<asc_order<Fn>> : std::true_type {};

template <typename Fn>
struct is_order<desc_order<Fn>> : std::true_type {};

template <typename T>
inline constexpr bool is_order_v = is_order<std::remove_cvref_t<T>>::value;

template <typename T>
struct is_desc : std::false_type {};

template <typename Fn>
struct is_desc<desc_order<Fn>> : std::true_type {};

template <typename T>
inline constexpr bool is_desc_v = is_desc<std::remove_cvref_t<T>>::value;

}  // namespace detail

template <typename Fn>
[[nodiscard]] constexpr auto asc(Fn&& fn) {
  return detail::asc_order<std::decay_t<Fn>>{std::forward<Fn>(fn)};
}

template <typename Fn>
[[nodiscard]] constexpr auto desc(Fn&& fn) {
  return detail::desc_order<std::decay_t<Fn>>{std::forward<Fn>(fn)};
}

template <typename C, typename... Orders>
  requires(detail::non_pair_range<const C> && (detail::is_order_v<Orders> && ...))
[[nodiscard]] constexpr auto order_by(const C& container, Orders&&... orders) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result(std::ranges::begin(container), std::ranges::end(container));

  auto compare = [&](const auto& a, const auto& b) -> bool {
    int cmp = 0;
    auto try_order = [&](const auto& order) {
      if (cmp != 0) return;
      auto ka = order.fn(a);
      auto kb = order.fn(b);
      if (ka < kb) cmp = 1;
      else if (kb < ka) cmp = -1;
      if constexpr (detail::is_desc_v<decltype(order)>) {
        cmp = -cmp;
      }
    };
    (try_order(orders), ...);
    return cmp > 0;
  };

  std::sort(result.begin(), result.end(), compare);
  return result;
}

// ============================================================================
// zip — combine two ranges into vector of pairs
// ============================================================================

template <typename A, typename B>
  requires(detail::range<const A> && detail::range<const B>)
constexpr auto zip(const A& first, const B& second) {
  using a_value = std::ranges::range_value_t<A>;
  using b_value = std::ranges::range_value_t<B>;
  std::vector<std::pair<a_value, b_value>> result;
  auto it_a = std::ranges::begin(first);
  auto end_a = std::ranges::end(first);
  auto it_b = std::ranges::begin(second);
  auto end_b = std::ranges::end(second);
  while (it_a != end_a && it_b != end_b) {
    result.emplace_back(*it_a, *it_b);
    ++it_a;
    ++it_b;
  }
  return result;
}

// ============================================================================
// chunk — split range into vector of vectors of size n
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
constexpr auto chunk(const C& container, std::size_t size) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<std::vector<value_type>> result;
  if (size == 0) return result;
  auto it = std::ranges::begin(container);
  auto end = std::ranges::end(container);
  while (it != end) {
    std::vector<value_type> current;
    for (std::size_t i = 0; i < size && it != end; ++i, ++it) {
      current.push_back(*it);
    }
    result.push_back(std::move(current));
  }
  return result;
}

// ============================================================================
// take — first n elements as vector
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
constexpr auto take(const C& container, std::size_t count) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  std::size_t i = 0;
  for (const auto& elem : container) {
    if (i++ >= count) break;
    result.push_back(elem);
  }
  return result;
}

// ============================================================================
// drop — skip first n elements, return rest as vector
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
constexpr auto drop(const C& container, std::size_t count) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  std::size_t i = 0;
  for (const auto& elem : container) {
    if (i++ >= count) {
      result.push_back(elem);
    }
  }
  return result;
}

// ============================================================================
// head — first element as optional, or std::nullopt
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto head(const C& container) {
  using value_type = std::ranges::range_value_t<C>;
  auto it = std::ranges::begin(container);
  if (it == std::ranges::end(container)) {
    return std::optional<value_type>{std::nullopt};
  }
  return std::optional<value_type>{*it};
}

// ============================================================================
// last — last element as optional, or std::nullopt
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto last(const C& container) {
  using value_type = std::ranges::range_value_t<C>;
  std::optional<value_type> result{std::nullopt};
  for (const auto& elem : container) {
    result = elem;
  }
  return result;
}

// ============================================================================
// tail — all elements except the first
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto tail(const C& container) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    auto n = std::ranges::size(container);
    if (n > 0) {
      result.reserve(n - 1);
    }
  }
  bool first = true;
  for (const auto& elem : container) {
    if (first) {
      first = false;
      continue;
    }
    result.push_back(elem);
  }
  return result;
}

// ============================================================================
// initial — all elements except the last
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto initial(const C& container) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  for (const auto& elem : container) {
    result.push_back(elem);
  }
  if (!result.empty()) {
    result.pop_back();
  }
  return result;
}

// ============================================================================
// size — number of elements in range
// ============================================================================

template <typename C>
  requires detail::range<const C>
[[nodiscard]] constexpr auto size(const C& container) -> std::size_t {
  return static_cast<std::size_t>(std::ranges::distance(container));
}

// ============================================================================
// reverse — elements in reverse order
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto reverse(const C& container) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result(std::ranges::begin(container), std::ranges::end(container));
  std::reverse(result.begin(), result.end());
  return result;
}

// ============================================================================
// includes — true if container has a value
// ============================================================================

template <typename C, typename V>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr bool includes(const C& container, const V& value) {
  for (const auto& elem : container) {
    if (elem == value) {
      return true;
    }
  }
  return false;
}

// ============================================================================
// compact — remove falsy values
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto compact(const C& container) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  for (const auto& elem : container) {
    if (static_cast<bool>(elem)) {
      result.push_back(elem);
    }
  }
  return result;
}

// ============================================================================
// flat_map — map then flatten one level
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto flat_map(const C& container, Fn&& fn) {
  using mapped_range_type = std::invoke_result_t<Fn, const std::ranges::range_value_t<C>&>;
  using value_type = std::ranges::range_value_t<mapped_range_type>;
  std::vector<value_type> result;
  for (const auto& elem : container) {
    auto mapped = fn(elem);
    for (const auto& inner : mapped) {
      result.push_back(inner);
    }
  }
  return result;
}

// ============================================================================
// reject — select elements NOT matching predicate
// ============================================================================

template <typename C, typename Pred>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr auto reject(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Pred, decltype(key), decltype(value)>) {
      if (!pred(key, value)) {
        result.emplace_back(key, value);
      }
    } else {
      if (!pred(std::pair{key, value})) {
        result.emplace_back(key, value);
      }
    }
  }
  return result;
}

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto reject(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  for (const auto& elem : container) {
    if (!pred(elem)) {
      result.push_back(elem);
    }
  }
  return result;
}

// ============================================================================
// take_while — take from front while predicate is true
// ============================================================================

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto take_while(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  for (const auto& elem : container) {
    if (!pred(elem)) {
      break;
    }
    result.push_back(elem);
  }
  return result;
}

// ============================================================================
// drop_while — drop from front while predicate is true
// ============================================================================

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto drop_while(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  bool dropping = true;
  for (const auto& elem : container) {
    if (dropping && pred(elem)) {
      continue;
    }
    dropping = false;
    result.push_back(elem);
  }
  return result;
}

// ============================================================================
// find_last — last element matching predicate, or std::nullopt
// ============================================================================

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto find_last(const C& container, Pred&& pred) {
  using value_type = std::ranges::range_value_t<C>;
  std::optional<value_type> result{std::nullopt};
  for (const auto& elem : container) {
    if (pred(elem)) {
      result = elem;
    }
  }
  return result;
}

// ============================================================================
// find_last_index — index of last matching element, or std::nullopt
// ============================================================================

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto find_last_index(const C& container,
                                             Pred&& pred) -> std::optional<std::size_t> {
  std::optional<std::size_t> result{std::nullopt};
  std::size_t idx = 0;
  for (const auto& elem : container) {
    if (pred(elem)) {
      result = idx;
    }
    ++idx;
  }
  return result;
}

// ============================================================================
// without — remove all values listed in excludes
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto without(const C& container,
                                     std::initializer_list<std::ranges::range_value_t<C>> excludes) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  for (const auto& elem : container) {
    bool excluded = false;
    for (const auto& ex : excludes) {
      if (elem == ex) {
        excluded = true;
        break;
      }
    }
    if (!excluded) {
      result.push_back(elem);
    }
  }
  return result;
}

// ============================================================================
// partition — split elements by predicate into matching/non-matching
// ============================================================================

template <typename C, typename Pred>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto partition(const C& container, Pred&& pred)
    -> std::pair<std::vector<std::ranges::range_value_t<C>>, std::vector<std::ranges::range_value_t<C>>> {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> matching;
  std::vector<value_type> non_matching;
  if constexpr (detail::sized_range<const C>) {
    auto n = std::ranges::size(container);
    matching.reserve(n);
    non_matching.reserve(n);
  }
  for (const auto& elem : container) {
    if (pred(elem)) {
      matching.push_back(elem);
    } else {
      non_matching.push_back(elem);
    }
  }
  return {std::move(matching), std::move(non_matching)};
}

// ============================================================================
// uniq — remove consecutive duplicate values
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto uniq(const C& container) {
  using value_type = std::ranges::range_value_t<C>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  for (const auto& elem : container) {
    if (result.empty() || !(result.back() == elem)) {
      result.push_back(elem);
    }
  }
  return result;
}

// ============================================================================
// uniq_by — remove consecutive duplicates by key function
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto uniq_by(const C& container, Fn&& fn) {
  using value_type = std::ranges::range_value_t<C>;
  using key_type = std::invoke_result_t<Fn, const value_type&>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C>) {
    result.reserve(std::ranges::size(container));
  }
  std::optional<key_type> previous_key{std::nullopt};
  bool has_previous = false;
  for (const auto& elem : container) {
    auto key = fn(elem);
    if (!has_previous || !(*previous_key == key)) {
      result.push_back(elem);
      previous_key = key;
      has_previous = true;
    }
  }
  return result;
}

// ============================================================================
// flatten — flatten one level from range-of-ranges
// ============================================================================

template <typename C>
  requires(detail::non_pair_range<const C> && detail::range<const std::ranges::range_value_t<C>>)
[[nodiscard]] constexpr auto flatten(const C& container) {
  using inner_range_type = std::ranges::range_value_t<C>;
  using value_type = std::ranges::range_value_t<inner_range_type>;
  std::vector<value_type> result;
  if constexpr (detail::sized_range<const C> && detail::sized_range<const inner_range_type>) {
    std::size_t total = 0;
    for (const auto& inner : container) {
      total += std::ranges::size(inner);
    }
    result.reserve(total);
  }
  for (const auto& inner : container) {
    for (const auto& elem : inner) {
      result.push_back(elem);
    }
  }
  return result;
}

namespace detail {

template <typename T>
concept nested_range = non_pair_range<T> && range<const std::ranges::range_value_t<T>>;

template <typename T>
struct deep_value_type {
  using type = std::ranges::range_value_t<T>;
};

template <typename T>
  requires range<const std::ranges::range_value_t<T>>
struct deep_value_type<T> {
  using type = typename deep_value_type<std::ranges::range_value_t<T>>::type;
};

template <typename T>
using deep_value_type_t = typename deep_value_type<T>::type;

template <std::size_t Depth, typename T>
struct flatten_depth_value_type {
  using type = std::ranges::range_value_t<T>;
};

template <std::size_t Depth, typename T>
  requires(Depth > 0 && range<const std::ranges::range_value_t<T>>)
struct flatten_depth_value_type<Depth, T> {
  using type = typename flatten_depth_value_type<Depth - 1, std::ranges::range_value_t<T>>::type;
};

template <std::size_t Depth, typename T>
using flatten_depth_value_type_t = typename flatten_depth_value_type<Depth, T>::type;

template <typename C, typename K, typename Fn>
constexpr bool contains_mapped_value(const C& container, const K& key, Fn&& fn) {
  for (const auto& elem : container) {
    if (fn(elem) == key) {
      return true;
    }
  }
  return false;
}

template <typename Input, typename Output>
constexpr void flatten_deep_append(const Input& input, Output& output) {
  for (const auto& elem : input) {
    if constexpr (range<const std::remove_cvref_t<decltype(elem)>>) {
      flatten_deep_append(elem, output);
    } else {
      output.push_back(elem);
    }
  }
}

template <std::size_t Depth, typename Input, typename Output>
constexpr void flatten_depth_append(const Input& input, Output& output) {
  for (const auto& elem : input) {
    if constexpr (Depth > 0 && range<const std::remove_cvref_t<decltype(elem)>>) {
      flatten_depth_append<Depth - 1>(elem, output);
    } else {
      output.push_back(elem);
    }
  }
}

}  // namespace detail

// ============================================================================
// difference_by — difference by iteratee key (iteratee first; C++ variadic rule)
// ============================================================================

template <typename Fn, typename First, typename... Rest>
  requires(detail::non_pair_range<const First> && (detail::non_pair_range<const Rest> && ...))
[[nodiscard]] constexpr auto difference_by(Fn&& fn, const First& first, const Rest&... rest) {
  using value_type = std::ranges::range_value_t<First>;
  std::vector<value_type> result;
  for (const auto& elem : first) {
    auto key = fn(elem);
    if (!(detail::contains_mapped_value(rest, key, fn) || ...)) {
      result.push_back(elem);
    }
  }
  return result;
}

// ============================================================================
// intersection_by — intersection by iteratee key (iteratee first; C++ variadic rule)
// ============================================================================

template <typename Fn, typename First, typename... Rest>
  requires(detail::non_pair_range<const First> && (detail::non_pair_range<const Rest> && ...))
[[nodiscard]] constexpr auto intersection_by(Fn&& fn, const First& first, const Rest&... rest) {
  using value_type = std::ranges::range_value_t<First>;
  using key_type = std::invoke_result_t<Fn, const value_type&>;
  std::vector<value_type> result;
  std::vector<key_type> seen_keys;
  for (const auto& elem : first) {
    auto key = fn(elem);
    if ((detail::contains_mapped_value(rest, key, fn) && ...)) {
      if (!detail::contains_value(seen_keys, key)) {
        result.push_back(elem);
        seen_keys.push_back(key);
      }
    }
  }
  return result;
}

// ============================================================================
// union_by — unique union by iteratee key (iteratee first; C++ variadic rule)
// ============================================================================

template <typename Fn, typename First, typename... Rest>
  requires(detail::non_pair_range<const First> && (detail::non_pair_range<const Rest> && ...))
[[nodiscard]] constexpr auto union_by(Fn&& fn, const First& first, const Rest&... rest) {
  using value_type = std::ranges::range_value_t<First>;
  using key_type = std::invoke_result_t<Fn, const value_type&>;
  std::vector<value_type> result;
  std::vector<key_type> seen_keys;
  auto add_unique = [&](const auto& container) {
    for (const auto& elem : container) {
      auto key = fn(elem);
      if (!detail::contains_value(seen_keys, key)) {
        result.push_back(elem);
        seen_keys.push_back(key);
      }
    }
  };
  add_unique(first);
  (add_unique(rest), ...);
  return result;
}

// ============================================================================
// xor_with — symmetric difference across ranges (present in exactly one range)
// ============================================================================

template <typename First, typename... Rest>
  requires(detail::non_pair_range<const First> && (detail::non_pair_range<const Rest> && ...))
[[nodiscard]] constexpr auto xor_with(const First& first, const Rest&... rest) {
  using value_type = std::ranges::range_value_t<First>;
  std::vector<value_type> result;
  auto add_if_unique_to_one_range = [&](const auto& container) {
    for (const auto& elem : container) {
      if (detail::contains_value(result, elem)) {
        continue;
      }
      std::size_t occurrences = detail::contains_value(first, elem) ? 1U : 0U;
      occurrences += ((detail::contains_value(rest, elem) ? 1U : 0U) + ...);
      if (occurrences == 1U) {
        result.push_back(elem);
      }
    }
  };
  add_if_unique_to_one_range(first);
  (add_if_unique_to_one_range(rest), ...);
  return result;
}

// ============================================================================
// xor_by — symmetric difference by iteratee key (iteratee first; C++ variadic rule)
// ============================================================================

template <typename Fn, typename First, typename... Rest>
  requires(detail::non_pair_range<const First> && (detail::non_pair_range<const Rest> && ...))
[[nodiscard]] constexpr auto xor_by(Fn&& fn, const First& first, const Rest&... rest) {
  using value_type = std::ranges::range_value_t<First>;
  using key_type = std::invoke_result_t<Fn, const value_type&>;
  std::vector<value_type> result;
  std::vector<key_type> seen_keys;
  auto add_if_unique_to_one_range = [&](const auto& container) {
    for (const auto& elem : container) {
      auto key = fn(elem);
      if (detail::contains_value(seen_keys, key)) {
        continue;
      }
      seen_keys.push_back(key);
      std::size_t occurrences = detail::contains_mapped_value(first, key, fn) ? 1U : 0U;
      occurrences += ((detail::contains_mapped_value(rest, key, fn) ? 1U : 0U) + ...);
      if (occurrences == 1U) {
        result.push_back(elem);
      }
    }
  };
  add_if_unique_to_one_range(first);
  (add_if_unique_to_one_range(rest), ...);
  return result;
}

// ============================================================================
// flatten_deep — recursively flatten homogeneous nested ranges to leaf values
// ============================================================================

template <typename C>
  requires detail::nested_range<const C>
[[nodiscard]] constexpr auto flatten_deep(const C& container) {
  using value_type = detail::deep_value_type_t<C>;
  std::vector<value_type> result;
  detail::flatten_deep_append(container, result);
  return result;
}

// ============================================================================
// flatten_depth — flatten N levels; depth is compile-time template parameter
// ============================================================================

template <std::size_t Depth, typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto flatten_depth(const C& container) {
  using value_type = detail::flatten_depth_value_type_t<Depth, C>;
  std::vector<value_type> result;
  detail::flatten_depth_append<Depth>(container, result);
  return result;
}

// ============================================================================
// unzip — split range of pairs into pair of vectors
// ============================================================================

template <typename C>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr auto unzip(const C& container) {
  using pair_type = std::ranges::range_value_t<C>;
  using first_type = std::remove_const_t<typename pair_type::first_type>;
  using second_type = typename pair_type::second_type;
  std::vector<first_type> firsts;
  std::vector<second_type> seconds;
  if constexpr (detail::sized_range<const C>) {
    auto n = std::ranges::size(container);
    firsts.reserve(n);
    seconds.reserve(n);
  }
  for (const auto& [key, value] : container) {
    firsts.push_back(key);
    seconds.push_back(value);
  }
  return std::pair{std::move(firsts), std::move(seconds)};
}

// ============================================================================
// zip_with — zip two ranges and map each pair through a function
// ============================================================================

template <typename A, typename B, typename Fn>
  requires(detail::range<const A> && detail::range<const B>)
[[nodiscard]] constexpr auto zip_with(const A& first, const B& second, Fn&& fn) {
  using a_value = std::ranges::range_value_t<A>;
  using b_value = std::ranges::range_value_t<B>;
  using result_type = std::invoke_result_t<Fn, const a_value&, const b_value&>;
  std::vector<result_type> result;
  auto it_a = std::ranges::begin(first);
  auto end_a = std::ranges::end(first);
  auto it_b = std::ranges::begin(second);
  auto end_b = std::ranges::end(second);
  while (it_a != end_a && it_b != end_b) {
    result.push_back(fn(*it_a, *it_b));
    ++it_a;
    ++it_b;
  }
  return result;
}

// ============================================================================
// zip_object — pair keys and values into std::map (stops at shorter input)
// ============================================================================

template <typename K, typename V>
  requires(detail::non_pair_range<const K> && detail::non_pair_range<const V>)
[[nodiscard]] constexpr auto zip_object(const K& keys, const V& values) {
  using key_type = std::remove_const_t<std::ranges::range_value_t<K>>;
  using value_type = std::ranges::range_value_t<V>;
  detail::auto_map<key_type, value_type> result;
  auto it_key = std::ranges::begin(keys);
  auto end_key = std::ranges::end(keys);
  auto it_value = std::ranges::begin(values);
  auto end_value = std::ranges::end(values);
  while (it_key != end_key && it_value != end_value) {
    result.insert_or_assign(*it_key, *it_value);
    ++it_key;
    ++it_value;
  }
  return result;
}

// ============================================================================
// pick_by — select map entries where predicate is true
// ============================================================================

template <typename C, typename Pred>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr auto pick_by(const C& container, Pred&& pred) {
  using pair_type = std::ranges::range_value_t<C>;
  using key_type = std::remove_const_t<typename pair_type::first_type>;
  using value_type = typename pair_type::second_type;
  detail::auto_map<key_type, value_type> result;
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Pred, decltype(key), decltype(value)>) {
      if (pred(key, value)) {
        result.insert_or_assign(key, value);
      }
    } else {
      if (pred(std::pair{key, value})) {
        result.insert_or_assign(key, value);
      }
    }
  }
  return result;
}

// ============================================================================
// omit_by — select map entries where predicate is false
// ============================================================================

template <typename C, typename Pred>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr auto omit_by(const C& container, Pred&& pred) {
  using pair_type = std::ranges::range_value_t<C>;
  using key_type = std::remove_const_t<typename pair_type::first_type>;
  using value_type = typename pair_type::second_type;
  detail::auto_map<key_type, value_type> result;
  for (const auto& [key, value] : container) {
    if constexpr (std::is_invocable_v<Pred, decltype(key), decltype(value)>) {
      if (!pred(key, value)) {
        result.insert_or_assign(key, value);
      }
    } else {
      if (!pred(std::pair{key, value})) {
        result.insert_or_assign(key, value);
      }
    }
  }
  return result;
}

// ============================================================================
// map_keys — transform keys of pair range into a new std::map
// ============================================================================

template <typename C, typename Fn>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr auto map_keys(const C& container, Fn&& fn) {
  using pair_type = std::ranges::range_value_t<C>;
  using mapped_key_type =
      std::invoke_result_t<Fn, const typename pair_type::first_type&, const typename pair_type::second_type&>;
  using value_type = typename pair_type::second_type;
  detail::auto_map<mapped_key_type, value_type> result;
  for (const auto& [key, value] : container) {
    result.insert_or_assign(fn(key, value), value);
  }
  return result;
}

// ============================================================================
// invert — swap keys and values from pair range into std::map
// ============================================================================

template <typename C>
  requires detail::pair_value_range<const C>
[[nodiscard]] constexpr auto invert(const C& container) {
  using pair_type = std::ranges::range_value_t<C>;
  using key_type = typename pair_type::second_type;
  using value_type = std::remove_const_t<typename pair_type::first_type>;
  detail::auto_map<key_type, value_type> result;
  for (const auto& [key, value] : container) {
    result.insert_or_assign(value, key);
  }
  return result;
}

// ============================================================================
// min_by — minimum element by iteratee key, or std::nullopt for empty input
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto min_by(const C& container, Fn&& fn) {
  using value_type = std::ranges::range_value_t<C>;
  auto it = std::ranges::begin(container);
  auto end = std::ranges::end(container);
  if (it == end) {
    return std::optional<value_type>{std::nullopt};
  }
  value_type best = *it;
  auto best_key = fn(best);
  ++it;
  for (; it != end; ++it) {
    auto key = fn(*it);
    if (key < best_key) {
      best = *it;
      best_key = key;
    }
  }
  return std::optional<value_type>{best};
}

// ============================================================================
// max_by — maximum element by iteratee key, or std::nullopt for empty input
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto max_by(const C& container, Fn&& fn) {
  using value_type = std::ranges::range_value_t<C>;
  auto it = std::ranges::begin(container);
  auto end = std::ranges::end(container);
  if (it == end) {
    return std::optional<value_type>{std::nullopt};
  }
  value_type best = *it;
  auto best_key = fn(best);
  ++it;
  for (; it != end; ++it) {
    auto key = fn(*it);
    if (best_key < key) {
      best = *it;
      best_key = key;
    }
  }
  return std::optional<value_type>{best};
}

// ============================================================================
// sum_by — sum iteratee outputs over container elements
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto sum_by(const C& container, Fn&& fn) {
  using result_type = std::invoke_result_t<Fn, const std::ranges::range_value_t<C>&>;
  result_type result{};
  for (const auto& elem : container) {
    result += fn(elem);
  }
  return result;
}

// ============================================================================
// mean — arithmetic average of elements, or 0.0 for empty input
// ============================================================================

template <typename C>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto mean(const C& container) -> double {
  double total = 0.0;
  std::size_t count = 0;
  for (const auto& elem : container) {
    total += static_cast<double>(elem);
    ++count;
  }
  if (count == 0U) {
    return 0.0;
  }
  return total / static_cast<double>(count);
}

// ============================================================================
// mean_by — arithmetic average of iteratee outputs, or 0.0 for empty input
// ============================================================================

template <typename C, typename Fn>
  requires detail::non_pair_range<const C>
[[nodiscard]] constexpr auto mean_by(const C& container, Fn&& fn) -> double {
  double total = 0.0;
  std::size_t count = 0;
  for (const auto& elem : container) {
    total += static_cast<double>(fn(elem));
    ++count;
  }
  if (count == 0U) {
    return 0.0;
  }
  return total / static_cast<double>(count);
}

}  // namespace ctl
