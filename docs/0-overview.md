# ctl — Lodash for C++ structs

ctl is a header-only C++23 library that brings lodash-style utilities to C++ structs and collections. Register your structs with `CTL_DESCRIBE_STRUCT`, then use familiar functional operations — `get`, `set`, `map`, `filter`, `merge`, `pick`, and 80+ more.

## Features
- Struct reflection via Boost.Describe (field access by name, index, or path)
- Collection operations on vectors, maps, arrays (map, filter, reduce, group_by...)
- Functional composition (pipe, compose, cond)
- Runtime escape hatch (visit, try_get when field name is only known at runtime)
- JSON serialization (opt-in `to_json`/`from_json` via `<ctl/json.hpp>`, requires nlohmann/json)
- Struct hashing (opt-in `CTL_MAKE_HASHABLE` — auto-generates `std::hash` + `operator==`)

## Documentation
| Topic | Link |
|---|---|
| Installation | [1-install.md](1-install.md) |
| Basic Usage | [2-basic-usage.md](2-basic-usage.md) |
| Limitations & Lodash Comparison | [3-limitations.md](3-limitations.md) |

### Guides
- [Collections](guides/0-collections.md)
- [Access & Update](guides/1-access-update.md)
- [Struct Operations](guides/2-struct-ops.md)
- [Transform & Filter](guides/3-transform-filter.md)
- [Search & Check](guides/4-search-check.md)
- [Aggregation](guides/5-aggregation.md)
- [Ordering & Slicing](guides/6-ordering-slicing.md)
- [Set Operations](guides/7-set-ops.md)
- [Reshape](guides/8-reshape.md)
- [Map Operations](guides/9-map-ops.md)
- [Utility](guides/10-utility.md)
- [Runtime Access](guides/11-runtime.md)
- [JSON Serialization](guides/12-json.md)

## Quick Example
```cpp
#include <ctl/ctl.hpp>
#include <string>
#include <vector>

struct Person {
    std::string name;
    int age;
};
CTL_DESCRIBE_STRUCT(Person, (name, age))

int main() {
    std::vector<Person> users = {{"Alice", 25}, {"Bob", 30}, {"Charlie", 35}};
    
    auto names = ctl::pipe(
        ctl::filter([](const Person& p) { return p.age > 28; }),
        ctl::map([](const Person& p) { return ctl::get<"name">(p); })
    )(users);
    
    // names contains {"Bob", "Charlie"}
    return 0;
}
```
