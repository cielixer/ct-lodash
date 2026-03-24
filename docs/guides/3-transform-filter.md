# Transform & Filter

Functions that transform or select elements from collections.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `map(c, fn)` | Transform each element | `vector<R>` |
| `filter(c, pred)` | Keep matching elements | `vector<T>` |
| `reject(c, pred)` | Remove matching elements | `vector<T>` |
| `flat_map(c, fn)` | Map then flatten one level | `vector<R>` |
| `transform(c, acc, fn)` | Mutable accumulator, early exit | Accumulator type |
| `compact(c)` | Remove falsy values | `vector<T>` |
| `partition(c, pred)` | Split into match/no-match | `pair<vector<T>, vector<T>>` |

## Setup

```cpp
#include <ctl/ctl.hpp>
struct Person { std::string name; int age; };
CTL_DESCRIBE_STRUCT(Person, (name, age))
std::vector<Person> users = {{"Alice", 25}, {"Bob", 17}, {"Charlie", 30}};
```

## map

```cpp
auto names = ctl::map(users, [](const Person& p) { return p.name; });
// {"Alice", "Bob", "Charlie"}

// On maps: callback receives (key, value)
std::map<std::string, int> scores = {{"a", 1}, {"b", 2}};
auto doubled = ctl::map(scores, [](const auto& k, int v) { return v * 2; });
// {2, 4}
```

## filter / reject

```cpp
auto adults = ctl::filter(users, [](const Person& p) { return p.age >= 18; });
// Alice, Charlie

auto minors = ctl::reject(users, [](const Person& p) { return p.age >= 18; });
// Bob
```

## flat_map

Map each element to a range, then flatten one level.

```cpp
std::vector<std::string> words = {"hello world", "foo bar"};
auto chars = ctl::flat_map(words, [](const std::string& w) {
    std::vector<char> v(w.begin(), w.end());
    return v;
});
```

## transform

Like `reduce`, but mutates the accumulator by reference. Return `false` from callback to stop early.

```cpp
auto result = ctl::transform(users, std::vector<std::string>{},
    [](std::vector<std::string>& acc, const Person& p) -> bool {
        acc.push_back(p.name);
        return p.name != "Bob";  // stop after Bob
    });
// {"Alice", "Bob"}
```

If the callback returns `void`, iteration continues to the end.

## compact

Removes values that are falsy (`0`, `false`, `nullptr`, `""`).

```cpp
std::vector<int> nums = {0, 1, 2, 0, 3};
auto nonzero = ctl::compact(nums);  // {1, 2, 3}
```

## partition

Splits into two vectors: matching and non-matching.

```cpp
auto [adults, minors] = ctl::partition(users,
    [](const Person& p) { return p.age >= 18; });
// adults: Alice, Charlie
// minors: Bob
```

## Next

- [Search & Check](4-search-check.md)
- [Aggregation](5-aggregation.md)
