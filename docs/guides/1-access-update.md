# Access & Update

Read and write struct fields using compile-time paths, string names, or indices.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `get<I>` | `get<0>(obj)` | Field value by index |
| `get<"name">` | `get<"age">(obj)` | Field value by name |
| `get<Path>` | `get<path<&P::addr, &A::city>>(obj)` | Nested field value |
| `has<"name">` | `has<"age">(obj)` | `bool` (compile-time) |
| `keys<T>` | `keys<Person>()` | `std::array<string_view, N>` |
| `values` | `values(obj)` | `std::tuple` of all field values |
| `for_each_field` | `for_each_field(obj, fn)` | `void` |
| `set<Path>` | `set<"age">(obj, 30)` | `T&` (mutates in-place) |
| `with<Path>` | `with<"age">(obj, 30)` | `T` (new copy) |
| `update<Path>` | `update<"age">(obj, fn)` | `T&` (mutates via fn) |

## Setup

```cpp
#include <ctl/ctl.hpp>

struct Address { std::string city; int zip; };
struct Person  { std::string name; int age; Address address; };
CTL_DESCRIBE_STRUCT(Address, (city, zip))
CTL_DESCRIBE_STRUCT(Person, (name, age, address))

Person p = {"Alice", 25, {"Seoul", 12345}};
```

## get

```cpp
// By index
auto name = ctl::get<0>(p);           // "Alice"

// By name
auto age = ctl::get<"age">(p);        // 25

// By path (nested)
using CityPath = ctl::path<&Person::address, &Address::city>;
auto city = ctl::get<CityPath>(p);    // "Seoul"

// Nested string access
auto zip = ctl::get<"address", "zip">(p);  // 12345
```

## has

Compile-time field existence check. Returns `constexpr bool`.

```cpp
static_assert(ctl::has<"name">(p));
static_assert(!ctl::has<"email">(p));

// Nested path check
static_assert(ctl::has<"address", "city">(p));
static_assert(!ctl::has<"address", "phone">(p));
```

## keys / values

```cpp
constexpr auto k = ctl::keys<Person>();
// {"name", "age", "address"}

auto v = ctl::values(p);
// std::tuple{"Alice", 25, Address{"Seoul", 12345}}
```

## for_each_field

Iterates all fields with `(name, value)`.

```cpp
ctl::for_each_field(p, [](std::string_view name, const auto& value) {
    std::cout << name << "\n";
});
```

## set (in-place mutation)

```cpp
ctl::set<"age">(p, 30);                          // p.age == 30

// Nested — pointer-to-member path or string literals
using CityPath = ctl::path<&Person::address, &Address::city>;
ctl::set<CityPath>(p, "Tokyo");                   // p.address.city == "Tokyo"
ctl::set<"address", "city">(p, std::string("Berlin"));  // same thing with strings
```

## with (immutable copy)

```cpp
auto p2 = ctl::with<"age">(p, 31);               // p unchanged, p2.age == 31
auto p3 = ctl::with<"address", "city">(p, std::string("Paris"));  // nested
```

## update (functional mutation)

```cpp
ctl::update<"age">(p, [](int& age) { age += 1; });
ctl::update<"address", "city">(p, [](auto& c) { c += "!"; });  // nested
```

## Next

- [Struct Operations](2-struct-ops.md) — is_equal, clone, pick, merge, ...
- [Runtime Access](11-runtime.md) — dynamic field access by string
