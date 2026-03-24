# Map Operations

Functions that operate on pair-value ranges (maps, vector of pairs).

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `pick_by(m, pred)` | Keep entries where pred is true | `unordered_map` or `map` † |
| `omit_by(m, pred)` | Remove entries where pred is true | `unordered_map` or `map` † |
| `map_keys(m, fn)` | Transform keys | `unordered_map` or `map` † |
| `invert(m)` | Swap keys and values | `unordered_map` or `map` † |

> † **auto_map**: Returns `std::unordered_map` when the key is hashable (`int`, `std::string`, etc.), `std::map` otherwise. Just use `auto`.

> These work on **pair-value ranges only** (e.g., `std::map`). For struct-level pick/omit, see [Struct Ops](2-struct-ops.md).

## Setup

```cpp
#include <ctl/ctl.hpp>
std::map<std::string, int> scores = {{"alice", 90}, {"bob", 60}, {"charlie", 85}};
```

## pick_by

Keep entries matching a predicate. Callback receives `(key, value)`.

```cpp
auto high = ctl::pick_by(scores, [](const std::string&, int v) { return v >= 80; });
// {"alice": 90, "charlie": 85}
```

## omit_by

Remove entries matching a predicate.

```cpp
auto low = ctl::omit_by(scores, [](const std::string&, int v) { return v >= 80; });
// {"bob": 60}
```

## map_keys

Transform keys while keeping values. Callback receives `(key, value)`, returns new key.

```cpp
auto upper = ctl::map_keys(scores, [](const std::string& k, int) {
    std::string u = k;
    std::transform(u.begin(), u.end(), u.begin(), ::toupper);
    return u;
});
// {"ALICE": 90, "BOB": 60, "CHARLIE": 85}
```

## invert

Swap keys and values. Last wins on duplicate values.

```cpp
std::map<std::string, int> m = {{"a", 1}, {"b", 2}, {"c", 3}};
auto inv = ctl::invert(m);
// {1: "a", 2: "b", 3: "c"}
```

## Next

- [Utility](10-utility.md)
- [Collections](0-collections.md)
