# Runtime Access

Escape hatch for when field names are only known at runtime.

All functions live in `ctl::runtime::`. They use string-based lookup internally and return `bool` or pointer to indicate success.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `visit(obj, name, fn)` | Call fn with field value | `bool` (found?) |
| `try_get<T>(obj, name)` | Get typed pointer to field | `T*` or `nullptr` |
| `try_update<T>(obj, name, val)` | Set field if name & type match | `bool` (updated?) |

## Setup

```cpp
#include <ctl/ctl.hpp>

struct Person { std::string name; int age; };
CTL_DESCRIBE_STRUCT(Person, (name, age))

Person p = {"Alice", 25};
```

## visit

Apply a visitor to a field by name. Returns `true` if the field was found.

```cpp
ctl::runtime::visit(p, "name", [](const auto& val) {
    std::cout << val << "\n";  // "Alice"
});

bool found = ctl::runtime::visit(p, "email", [](const auto&) {});
// false — "email" doesn't exist
```

## try_get

Returns a typed pointer to the field, or `nullptr` if the name doesn't exist or the type doesn't match.

```cpp
auto* age = ctl::runtime::try_get<int>(p, "age");
if (age) {
    std::cout << *age << "\n";  // 25
}

auto* wrong = ctl::runtime::try_get<std::string>(p, "age");
// nullptr — type mismatch (age is int, not string)

auto* missing = ctl::runtime::try_get<int>(p, "email");
// nullptr — field doesn't exist
```

Respects const: `try_get` on `const Person&` returns `const int*`.

## try_update

Update a field by name if both name and type match. Only works on mutable objects.

```cpp
bool ok = ctl::runtime::try_update<int>(p, "age", 30);
// ok == true, p.age == 30

bool fail = ctl::runtime::try_update<int>(p, "email", 99);
// false — no such field

const Person& cp = p;
// ctl::runtime::try_update<int>(cp, "age", 30);  // compile error: const object
```

## When to Use

- Config systems where field names come from external input
- Serialization/deserialization helpers
- Debug/inspector tools

For compile-time access, prefer [get/set/with](1-access-update.md) — they're type-safe with zero runtime overhead.

## Next

- [Access & Update](1-access-update.md) — compile-time counterpart
- [Overview](../0-overview.md)
