# Aggregation

Functions that reduce collections into summary values.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `reduce(c, init, fn)` | Fold into single value | Init type |
| `count_by(c, fn)` | Count per group | `unordered_map` or `map` † |
| `group_by(c, fn)` | Group into map of vectors | `unordered_map` or `map` † |
| `key_by(c, fn)` | Index by key (last wins) | `unordered_map` or `map` † |
| `min_by(c, fn)` | Min element by key | `optional<T>` |
| `max_by(c, fn)` | Max element by key | `optional<T>` |
| `sum_by(c, fn)` | Sum of iteratee results | Numeric type |
| `mean(c)` | Arithmetic average | `double` |
| `mean_by(c, fn)` | Average of iteratee results | `double` |
| `size(c)` | Element count | `size_t` |

> † **auto_map**: Returns `std::unordered_map` when the key is hashable (`int`, `std::string`, etc.), `std::map` otherwise. Just use `auto`.

## Setup

```cpp
#include <ctl/ctl.hpp>
struct Person { std::string name; int age; std::string dept; };
CTL_DESCRIBE_STRUCT(Person, (name, age, dept))

std::vector<Person> users = {
    {"Alice", 25, "eng"}, {"Bob", 30, "eng"}, {"Charlie", 35, "sales"}
};
```

## reduce

```cpp
int total_age = ctl::reduce(users, 0,
    [](int acc, const Person& p) { return acc + p.age; });
// 90

// On maps: callback receives (acc, key, value)
std::map<std::string, int> scores = {{"a", 1}, {"b", 2}};
int sum = ctl::reduce(scores, 0, [](int acc, const auto&, int v) { return acc + v; });
// 3
```

## count_by

```cpp
auto counts = ctl::count_by(users, [](const Person& p) { return p.dept; });
// {"eng": 2, "sales": 1}
```

## group_by

```cpp
auto groups = ctl::group_by(users, [](const Person& p) { return p.dept; });
// {"eng": [Alice, Bob], "sales": [Charlie]}
```

## key_by

Last element wins on duplicate keys.

```cpp
auto by_name = ctl::key_by(users, [](const Person& p) { return p.name; });
// {"Alice": Person{...}, "Bob": Person{...}, "Charlie": Person{...}}
```

## min_by / max_by

Returns `std::nullopt` for empty input.

```cpp
auto youngest = ctl::min_by(users, [](const Person& p) { return p.age; });
// youngest->name == "Alice"

auto oldest = ctl::max_by(users, [](const Person& p) { return p.age; });
// oldest->name == "Charlie"
```

## sum_by

```cpp
int total = ctl::sum_by(users, [](const Person& p) { return p.age; });
// 90
```

## mean / mean_by

Returns `0.0` for empty input.

```cpp
std::vector<int> nums = {10, 20, 30};
double avg = ctl::mean(nums);  // 20.0

double avg_age = ctl::mean_by(users, [](const Person& p) { return p.age; });
// 30.0
```

## size

```cpp
auto n = ctl::size(users);  // 3
```

## Next

- [Ordering & Slicing](6-ordering-slicing.md)
- [Set Operations](7-set-ops.md)
