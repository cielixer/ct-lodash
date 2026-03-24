# Basic Usage

This page covers the core patterns of ctl. It is not an exhaustive API reference.

## 1. Register Your Structs
Use `CTL_DESCRIBE_STRUCT` to enable reflection for your types.
```cpp
#include <ctl/ctl.hpp>
#include <string>

struct Address {
    std::string city;
};
CTL_DESCRIBE_STRUCT(Address, (city))

struct Person {
    std::string name;
    int age;
    Address address;
};
CTL_DESCRIBE_STRUCT(Person, (name, age, address))
```

## 2. Access Fields
Access fields using compile-time constants.
```cpp
Person p = {"Alice", 25, {"New York"}};

// By index
auto name = ctl::get<0>(p);

// By name
auto age = ctl::get<"age">(p);

// By path (nested access)
auto city = ctl::get<ctl::path<&Person::address, &Address::city>>(p);

// Same thing with string literals
auto city2 = ctl::get<"address", "city">(p);

// Check existence
static_assert(ctl::has<"name">(p));
static_assert(ctl::has<"address", "city">(p));
```

## 3. Update Fields
Modify structs with in-place or immutable operations.
```cpp
// In-place update
ctl::set<"age">(p, 26);
ctl::set<"address", "city">(p, std::string("Tokyo"));

// Immutable copy with change
auto p2 = ctl::with<"age">(p, 31);
auto p3 = ctl::with<"address", "city">(p, std::string("Berlin"));

// Functional transformation
ctl::update<"age">(p, [](int age) { return age + 1; });
ctl::update<"address", "city">(p, [](auto& c) { c += "!"; });
```

## 4. Collections
Functional operations on `std::vector`, `std::map`, and arrays.
```cpp
std::vector<Person> users = {/*...*/};

auto active_names = ctl::pipe(
    ctl::filter([](const auto& p) { return p.age > 20; }),
    ctl::map([](const auto& p) { return p.name; })
)(users);
```

## 5. Compose
Compose multiple functions into a single pipeline.
```cpp
auto transform = ctl::pipe(
    [](int x) { return x * 2; },
    [](int x) { return std::to_string(x); }
);

auto result = transform(10); // "20"
```

## 6. Hashable Structs (opt-in)

Make described structs usable as keys in `std::unordered_map` / `std::unordered_set`:

```cpp
struct Point { int x; int y; };
CTL_DESCRIBE_STRUCT(Point, (), (x, y))
CTL_MAKE_HASHABLE(Point)

std::unordered_set<Point> points;
points.insert({1, 2});

auto grouped = ctl::group_by(items, [](const auto& i) { return i.pos; });
```

`CTL_MAKE_HASHABLE` generates `std::hash<T>` and `operator==` by combining hashes of all fields. All fields must individually be hashable.

## 7. What's Next
See the [Guides](0-overview.md#guides) for comprehensive coverage of specific operations.
