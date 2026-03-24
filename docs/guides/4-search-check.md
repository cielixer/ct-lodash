# Search & Check

Functions that search for elements or check conditions across a collection.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `find(c, pred)` | First match | `optional<T>` |
| `find_index(c, pred)` | Index of first match | `optional<size_t>` |
| `find_last(c, pred)` | Last match | `optional<T>` |
| `find_last_index(c, pred)` | Index of last match | `optional<size_t>` |
| `every(c, pred)` | All match? | `bool` |
| `some(c, pred)` | Any match? | `bool` |
| `includes(c, val)` | Contains value? | `bool` |
| `without(c, {vals})` | Remove listed values | `vector<T>` |
| `for_each(c, fn)` | Side-effect iteration | `void` |

## Setup

```cpp
#include <ctl/ctl.hpp>
struct Person { std::string name; int age; };
CTL_DESCRIBE_STRUCT(Person, (name, age))
std::vector<Person> users = {{"Alice", 25}, {"Bob", 17}, {"Charlie", 30}};
```

## find / find_index

```cpp
auto alice = ctl::find(users, [](const Person& p) { return p.name == "Alice"; });
// alice.has_value() == true, alice->age == 25

auto idx = ctl::find_index(users, [](const Person& p) { return p.age > 20; });
// idx == 0
```

## find_last / find_last_index

```cpp
auto last_adult = ctl::find_last(users, [](const Person& p) { return p.age > 20; });
// last_adult->name == "Charlie"

auto last_idx = ctl::find_last_index(users, [](const Person& p) { return p.age > 20; });
// last_idx == 2
```

## every / some

```cpp
bool all_adults = ctl::every(users, [](const Person& p) { return p.age >= 18; });
// false (Bob is 17)

bool any_adult = ctl::some(users, [](const Person& p) { return p.age >= 18; });
// true
```

Works on maps too — callback receives `(key, value)`:

```cpp
std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
bool all_positive = ctl::every(m, [](const auto&, int v) { return v > 0; });
```

## includes

Value equality check.

```cpp
std::vector<int> nums = {1, 2, 3, 4};
ctl::includes(nums, 3);  // true
ctl::includes(nums, 9);  // false
```

## without

Returns a new vector excluding listed values.

```cpp
std::vector<int> nums = {1, 2, 3, 4, 5};
auto result = ctl::without(nums, {2, 4});
// {1, 3, 5}
```

## for_each

Side-effect iteration. Returns `void`.

```cpp
ctl::for_each(users, [](const Person& p) {
    std::cout << p.name << "\n";
});

// On maps
std::map<std::string, int> scores = {{"a", 1}};
ctl::for_each(scores, [](const std::string& k, int v) {
    std::cout << k << "=" << v << "\n";
});
```

## Next

- [Transform & Filter](3-transform-filter.md)
- [Aggregation](5-aggregation.md)
