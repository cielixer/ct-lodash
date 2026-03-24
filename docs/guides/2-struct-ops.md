# Struct Operations

Operations on described structs: comparison, cloning, conversion, and cross-field manipulation.

## Quick Reference

| Function | Description | Returns |
|---|---|---|
| `is_equal(a, b)` | Deep field-wise equality | `bool` |
| `clone(obj)` | Deep copy | `T` |
| `to_tuple(obj)` | Struct → tuple | `std::tuple<...>` |
| `from_tuple<T>(tup)` | Tuple → struct | `T` |
| `map_values(obj, fn)` | Transform all fields | `std::tuple<...>` |
| `to_pairs(obj)` | Struct → `vector<pair<string_view, any>>` | vector |
| `find_key(obj, pred)` | Name of first field matching pred | `optional<string_view>` |
| `matches(pattern)` | Create equality predicate | Lambda |
| `pick<Target>(src)` | Copy matching fields into Target | `Target` |
| `omit<Target>(src)` | Same as pick (caller defines kept fields) | `Target` |
| `merge(base, ...)` | Deep merge, rightmost wins | `T` |
| `defaults(base, ...)` | Deep merge, leftmost wins | `T` |

## Setup

```cpp
struct Address { std::string city; int zip; };
struct Person  { std::string name; int age; Address address; };
CTL_DESCRIBE_STRUCT(Address, (city, zip))
CTL_DESCRIBE_STRUCT(Person, (name, age, address))
```

## is_equal / clone

```cpp
Person a = {"Alice", 25, {"Seoul", 100}};
Person b = ctl::clone(a);
ctl::is_equal(a, b);  // true — deep recursive comparison
```

## to_tuple / from_tuple

```cpp
auto tup = ctl::to_tuple(a);  // tuple{"Alice", 25, Address{"Seoul", 100}}
auto p = ctl::from_tuple<Person>(tup);
```

## map_values

Transforms every field. Callback receives `(field_name, field_value)`. Returns a tuple (fields may have different types after transformation).

```cpp
auto result = ctl::map_values(a, [](std::string_view name, const auto& val) {
    return name;  // returns tuple of string_views
});
```

## to_pairs

```cpp
auto pairs = ctl::to_pairs(a);
// vector of {("name", any("Alice")), ("age", any(25)), ("address", any(...))}
```

## find_key

```cpp
auto key = ctl::find_key(a, [](const auto& v) -> bool {
    if constexpr (std::is_same_v<std::remove_cvref_t<decltype(v)>, int>) {
        return v == 25;
    }
    return false;
});
// key == "age"
```

## matches

Creates a predicate that checks full-struct equality.

```cpp
Person pattern = {"Alice", 25, {"Seoul", 100}};
auto pred = ctl::matches(pattern);
pred(a);  // true
```

## pick / omit

C++ cannot create new struct types at runtime. You must define the target struct yourself.

```cpp
// pick: copy only the fields in SmallPerson
struct SmallPerson { std::string name; int age; };
CTL_DESCRIBE_STRUCT(SmallPerson, (name, age))

auto small = ctl::pick<SmallPerson>(a);  // small.name, small.age copied

// omit: identical mechanism — define a struct WITHOUT the omitted fields
auto also_small = ctl::omit<SmallPerson>(a);
```

> See [Limitations](../3-limitations.md) for why this is necessary.

## merge / defaults

Both operate on same-type structs. Nested described fields are merged recursively.

```cpp
Person base     = {"Alice", 25, {"Seoul", 100}};
Person override = {"Alice", 30, {"Tokyo", 200}};

auto merged = ctl::merge(base, override);
// merged.age == 30, merged.address.city == "Tokyo" (rightmost wins)

auto defaulted = ctl::defaults(override, base);
// defaulted.age == 30 (leftmost wins, base only fills nested gaps)
```

## Next

- [Access & Update](1-access-update.md) — get, set, with, update
- [Limitations](../3-limitations.md) — pick/omit constraints explained
