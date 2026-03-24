# JSON Serialization

Opt-in JSON roundtrip for described structs. Requires [nlohmann/json](https://github.com/nlohmann/json) (≥ 3.11).

`#include <ctl/json.hpp>` — this header is **not** included by `<ctl/ctl.hpp>`.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `to_json(obj)` | Described struct → JSON | `nlohmann::json` |
| `from_json<T>(j)` | JSON → described struct | `T` |

## Setup

```cpp
#include <ctl/json.hpp>

struct Address { std::string city; int zip_code; };
struct Person  { std::string name; int age; Address address; };
CTL_DESCRIBE_STRUCT(Address, (), (city, zip_code))
CTL_DESCRIBE_STRUCT(Person,  (), (name, age, address))
```

Your CMakeLists.txt must link `nlohmann_json::nlohmann_json`:

```cmake
find_package(nlohmann_json REQUIRED)
target_link_libraries(your_target PRIVATE nlohmann_json::nlohmann_json)
```

## to_json

Converts a described struct to `nlohmann::json`. Nested structs, vectors, maps, and optionals are handled recursively.

```cpp
Person p{"Alice", 30, {"Seoul", 12345}};
nlohmann::json j = ctl::to_json(p);

// {"name":"Alice","age":30,"address":{"city":"Seoul","zip_code":12345}}
std::string s = j.dump();
```

## from_json

Converts JSON back to a described struct.

```cpp
auto restored = ctl::from_json<Person>(j);
// restored.name == "Alice"
// restored.address.city == "Seoul"
```

**Missing keys** default-construct the field:

```cpp
nlohmann::json partial = {{"name", "Bob"}};
auto p = ctl::from_json<Person>(partial);
// p.age == 0, p.address == Address{}
```

**Extra keys** are silently ignored.

## Supported Field Types

Anything nlohmann/json can handle natively, plus:

| Type | JSON Representation |
|---|---|
| Described struct | Nested object |
| `std::optional<T>` | Value or `null` |
| `std::vector<T>`, sequential containers | Array |
| `std::map<K,V>`, associative containers | Object (keys stringified) |

These compose — `std::vector<Person>`, `std::map<std::string, std::vector<Person>>`, etc. all work.

```cpp
struct Team {
    std::string name;
    std::vector<Person> members;
    std::optional<std::string> motto;
};
CTL_DESCRIBE_STRUCT(Team, (), (name, members, motto))

Team t{"dev", {{"Alice", 30, {"Seoul", 100}}, {"Bob", 25, {"Tokyo", 200}}}, "ship it"};
auto j = ctl::to_json(t);
// j["members"][0]["address"]["city"] == "Seoul"
// j["motto"] == "ship it"

Team t2{"ops", {}, std::nullopt};
auto j2 = ctl::to_json(t2);
// j2["motto"] is null
```

## Limitations

- **Opt-in only** — `<ctl/json.hpp>` is separate from `<ctl/ctl.hpp>` to avoid forcing the nlohmann/json dependency.
- **No field renaming** — JSON keys always match the C++ field names from `CTL_DESCRIBE_STRUCT`.
- **No field ignoring** — all described fields are serialized/deserialized.
- **No custom per-field serializers** — if you need custom logic, convert to JSON manually and patch.

## Next

- [Runtime Access](11-runtime.md) — string-based field access without JSON
- [Access & Update](1-access-update.md) — compile-time field access
- [Overview](../0-overview.md)
