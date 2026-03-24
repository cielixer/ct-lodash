# ctl: Lodash for C++ structs

`ctl` is a header-only C++23 library that brings lodash-style functional programming to C++ structs and collections. It provides **90+ functions** spanning struct reflection, collection algorithms, and functional utilities — all type-safe, all working with plain structs.

```cpp
#include <ctl/ctl.hpp>

struct Address { std::string city; int zip; };
struct Person  { std::string name; int age; Address address; };
CTL_DESCRIBE_STRUCT(Address, (), (city, zip))
CTL_DESCRIBE_STRUCT(Person,  (), (name, age, address))

using City = ctl::path<&Person::address, &Address::city>;

Person alice = { "Alice", 30, { "Seoul", 06100 } };

// Read nested field — pointer-to-member path or string literals
auto city  = ctl::get<City>(alice);                           // "Seoul"
auto city2 = ctl::get<"address", "city">(alice);              // same thing

// Functional update — returns a new copy, original untouched
auto moved = ctl::with<"address", "city">(alice, std::string("Tokyo"));

// In-place mutation on nested field
ctl::set<"address", "city">(alice, std::string("Berlin"));

// Transform nested field with a function
ctl::update<"address", "city">(alice, [](auto& c) { c += " (DE)"; });  // "Berlin (DE)"

// Check if nested path exists at compile time
constexpr bool ok = ctl::has<"address", "city">(alice);       // true
constexpr bool no = ctl::has<"address", "phone">(alice);      // false

// --- String-based runtime access (when field name is dynamic) ---

ctl::runtime::visit(alice, "name", [](auto& v) {
    std::cout << v << "\n";                                 // "Alice"
});

auto* age = ctl::runtime::try_get<int>(alice, "age");       // int* → 30
ctl::runtime::try_update<int>(alice, "age", 31);            // alice.age → 31
```

## What's Inside

| Module | Count | Examples |
|--------|------:|---------|
| **Collection** | 61 | `map`, `filter`, `reduce`, `group_by`, `sort_by`, `zip`, `flatten`, `partition`, … |
| **Struct Ops** | 12 | `is_equal`, `clone`, `pick`, `omit`, `merge`, `defaults`, `to_pairs`, `map_values`, … |
| **Utility** | 11 | `pipe`, `compose`, `cond`, `times`, `range`, `identity`, `negate`, … |
| **Access** | 5 | `get`, `has`, `keys`, `values`, `for_each_field` |
| **Update** | 3 | `set`, `with`, `update` |
| **Runtime** | 3 | `visit`, `try_get`, `try_update` |
| **JSON** *(opt-in)* | 2 | `to_json`, `from_json` |
| **Hash** | 1 | `CTL_MAKE_HASHABLE` — auto-generate `std::hash` + `operator==` |

### Design Choices

- **No `std::ranges` or concepts in user code** — ctl hides them internally. You just pass vectors and lambdas.
- **`pick`/`omit`/`merge`** require user-defined target structs (C++ cannot synthesize new struct types at compile time).
- **Strict type safety** — no `any`, no type erasure, no macros beyond `CTL_DESCRIBE_STRUCT` and `CTL_MAKE_HASHABLE`.

## Quick Start

### 1. Describe Your Structs

```cpp
#include <ctl/ctl.hpp>

struct Address {
    std::string city;
    int zip_code;
};

struct Person {
    std::string name;
    int age;
    Address address;
};

CTL_DESCRIBE_STRUCT(Address, (), (city, zip_code))
CTL_DESCRIBE_STRUCT(Person, (), (name, age, address))
```

### 2. Struct Operations

```cpp
Person p = { "Alice", 30, { "Seoul", 06100 } };

// Compile-time path access
auto city = ctl::get<ctl::path<&Person::address, &Address::city>>(p);  // "Seoul"

// Functional update (returns new copy)
auto older = ctl::with<ctl::field<&Person::age>>(p, 31);

// Reflection
auto ks = ctl::keys(p);        // ["name", "age", "address"]
bool eq = ctl::is_equal(p, older);  // false
```

### 3. Collections

```cpp
std::vector<Person> people = { /* ... */ };

auto names    = ctl::map(people, [](auto& p) { return p.name; });
auto adults   = ctl::filter(people, [](auto& p) { return p.age >= 18; });
auto by_city  = ctl::group_by(people, [](auto& p) { return p.address.city; });
auto sorted   = ctl::sort_by(people, [](auto& p) { return p.age; });
auto chunks   = ctl::chunk(people, 3);
auto youngest = ctl::min_by(people, [](auto& p) { return p.age; });
```

### 4. Utilities

```cpp
auto process = ctl::pipe(
    [](int x) { return x * 2; },
    [](int x) { return x + 1; },
    [](int x) { return std::to_string(x); }
);
std::string result = process(5);  // "11"

auto nums = ctl::range(0, 10);    // [0, 1, 2, ..., 9]
auto evens = ctl::filter(nums, [](int n) { return n % 2 == 0; });
```

### 5. Runtime Fallback

```cpp
ctl::runtime::visit(person, "name", [](const auto& val) {
    std::cout << val << "\n";
});

auto* age = ctl::runtime::try_get<int>(person, "age");
```

### 6. JSON Serialization (opt-in)

Requires `nlohmann/json`. Include `<ctl/json.hpp>` separately — it is **not** part of `<ctl/ctl.hpp>`.

```cpp
#include <ctl/json.hpp>

Person alice = { "Alice", 30, { "Seoul", 06100 } };

// Struct → JSON (nested structs, vectors, maps, optionals all handled)
nlohmann::json j = ctl::to_json(alice);
// {"name":"Alice","age":30,"address":{"city":"Seoul","zip_code":3136}}

// JSON → Struct
auto restored = ctl::from_json<Person>(j);
// restored.address.city == "Seoul"
```

### 7. Hashable Structs (opt-in)

Make any described struct usable as a key in `std::unordered_map` / `std::unordered_set`:

```cpp
struct Point { int x; int y; };
CTL_DESCRIBE_STRUCT(Point, (), (x, y))
CTL_MAKE_HASHABLE(Point)

std::unordered_map<Point, std::string> labels;
labels[{0, 0}] = "origin";

auto grouped = ctl::group_by(items, [](const auto& i) { return i.pos; });
```

Aggregation functions (`group_by`, `key_by`, `count_by`, etc.) automatically use `std::unordered_map` when the key type is hashable.

## Documentation

Full guides are available in [`docs/`](docs/):

- [Overview](docs/0-overview.md) — What ctl is, what it isn't
- [Installation](docs/1-install.md) — Pixi, CMake, FetchContent
- [Basic Usage](docs/2-basic-usage.md) — Getting started
- [Limitations](docs/3-limitations.md) — C++ constraints and workarounds

### Function Guides

- [Collections Concept](docs/guides/0-collections.md) — How ctl treats ranges
- [Access & Update](docs/guides/1-access-update.md) — `get`, `set`, `with`, `update`, `has`, `keys`, `values`
- [Struct Ops](docs/guides/2-struct-ops.md) — `is_equal`, `clone`, `pick`, `omit`, `merge`, `defaults`, …
- [Transform & Filter](docs/guides/3-transform-filter.md) — `map`, `filter`, `reject`, `flat_map`, `compact`, `partition`
- [Search & Check](docs/guides/4-search-check.md) — `find`, `every`, `some`, `includes`, `for_each`
- [Aggregation](docs/guides/5-aggregation.md) — `reduce`, `group_by`, `key_by`, `count_by`, `min_by`, `max_by`, `mean`
- [Ordering & Slicing](docs/guides/6-ordering-slicing.md) — `sort_by`, `order_by`, `take`, `drop`, `chunk`, `head`, `last`
- [Set Operations](docs/guides/7-set-ops.md) — `intersection`, `difference`, `set_union`, `xor_with`, `uniq`
- [Reshape](docs/guides/8-reshape.md) — `zip`, `unzip`, `flatten`, `flatten_deep`, `zip_object`
- [Map Operations](docs/guides/9-map-ops.md) — `pick_by`, `omit_by`, `map_keys`, `invert`
- [Utility](docs/guides/10-utility.md) — `pipe`, `compose`, `cond`, `times`, `range`, `identity`
- [Runtime](docs/guides/11-runtime.md) — `visit`, `try_get`, `try_update`
- [JSON Serialization](docs/guides/12-json.md) — `to_json`, `from_json` (opt-in, requires nlohmann/json)

## Installation

### Pixi

`ctl` is designed for the [pixi](https://pixi.sh) package manager. Add it to your project:

```bash
pixi add ctl
```

### CMake

As a header-only library, you can include it via `FetchContent` or by adding the subdirectory:

```cmake
find_package(ctl REQUIRED)
target_link_libraries(your_target PRIVATE ctl::ctl)
```

## Developer Commands

If you are contributing to `ctl`, use the following commands:

- `pixi run configure`: Initialize CMake build directory.
- `pixi run build`: Compile the library and examples.
- `pixi run test`: Run the test suite.
- `pixi run example`: Build and run the example programs.

## Requirements

- **C++23**: Requires a modern compiler (GCC 13+, Clang 16+, MSVC 19.36+).
- **CMake**: 3.25 or higher.
- **Boost**: Used internally for preprocessor utilities.
- **nlohmann/json** *(optional)*: Required only if you include `<ctl/json.hpp>`.

## License

MIT
