# Ordering & Slicing

Sort, reorder, and slice collections.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `sort_by(c, fn)` | Sort by single key (ascending) | `vector<T>` |
| `order_by(c, orders...)` | Multi-key sort with direction | `vector<T>` |
| `asc(fn)` | Ascending order spec | Order wrapper |
| `desc(fn)` | Descending order spec | Order wrapper |
| `reverse(c)` | Reverse order | `vector<T>` |
| `take(c, n)` | First n elements | `vector<T>` |
| `drop(c, n)` | Skip first n elements | `vector<T>` |
| `take_while(c, pred)` | Take from front while true | `vector<T>` |
| `drop_while(c, pred)` | Skip from front while true | `vector<T>` |
| `head(c)` | First element | `optional<T>` |
| `last(c)` | Last element | `optional<T>` |
| `tail(c)` | All except first | `vector<T>` |
| `initial(c)` | All except last | `vector<T>` |
| `chunk(c, n)` | Split into groups of n | `vector<vector<T>>` |

## Setup

```cpp
#include <ctl/ctl.hpp>
struct Person { std::string name; int age; std::string dept; };
CTL_DESCRIBE_STRUCT(Person, (name, age, dept))

std::vector<Person> users = {
    {"Charlie", 30, "sales"}, {"Alice", 25, "eng"}, {"Bob", 25, "eng"}
};
```

## sort_by

Single-key ascending sort.

```cpp
auto by_age = ctl::sort_by(users, [](const Person& p) { return p.age; });
// Alice(25), Bob(25), Charlie(30)
```

## order_by / asc / desc

Multi-key sort with per-key direction.

```cpp
auto sorted = ctl::order_by(users,
    ctl::asc([](const Person& p) { return p.age; }),
    ctl::desc([](const Person& p) { return p.name; })
);
// age asc, then name desc: Bob(25), Alice(25), Charlie(30)
```

## reverse

```cpp
std::vector<int> nums = {1, 2, 3};
auto rev = ctl::reverse(nums);  // {3, 2, 1}
```

## take / drop

```cpp
std::vector<int> nums = {1, 2, 3, 4, 5};
auto first3 = ctl::take(nums, 3);   // {1, 2, 3}
auto rest   = ctl::drop(nums, 3);   // {4, 5}
```

## take_while / drop_while

```cpp
std::vector<int> nums = {1, 2, 3, 4, 1};
auto tw = ctl::take_while(nums, [](int x) { return x < 3; });  // {1, 2}
auto dw = ctl::drop_while(nums, [](int x) { return x < 3; });  // {3, 4, 1}
```

## head / last

Return `std::nullopt` for empty input.

```cpp
std::vector<int> nums = {10, 20, 30};
auto h = ctl::head(nums);  // 10
auto l = ctl::last(nums);  // 30

std::vector<int> empty = {};
auto e = ctl::head(empty);  // std::nullopt
```

## tail / initial

```cpp
std::vector<int> nums = {1, 2, 3, 4};
auto t = ctl::tail(nums);     // {2, 3, 4}
auto i = ctl::initial(nums);  // {1, 2, 3}
```

## chunk

```cpp
std::vector<int> nums = {1, 2, 3, 4, 5};
auto chunks = ctl::chunk(nums, 2);
// {{1, 2}, {3, 4}, {5}}
```

## Next

- [Set Operations](7-set-ops.md)
- [Reshape](8-reshape.md)
