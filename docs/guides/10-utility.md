# Utility

Functional utilities for composition, iteration helpers, and control flow.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `identity` | `identity(val)` | `val` unchanged |
| `constant(v)` | Returns fn always yielding `v` | Lambda |
| `tap(obj, fn)` | Call fn for side effects, return obj | `T&` |
| `negate(fn)` | Invert predicate | Lambda |
| `noop` | Does nothing | `void` |
| `times(n, fn)` | Call fn(i) n times | `vector<R>` |
| `range(end)` | `[0, end)` | `vector<int>` |
| `range(start, end)` | `[start, end)` | `vector<int>` |
| `range(start, end, step)` | Stepped range | `vector<int>` |
| `pipe(f, g, ...)` | Left-to-right composition | Lambda |
| `compose(f, g, ...)` | Right-to-left composition | Lambda |
| `cond(pairs...)` | First matching predicate wins | Lambda → `optional<R>` |
| `thru(val, fn)` | Pass val through fn | `fn(val)` |

## identity / constant / noop

```cpp
auto x = ctl::identity(42);  // 42

auto always5 = ctl::constant(5);
always5();       // 5
always5(1,2,3);  // 5

ctl::noop();     // does nothing
ctl::noop(1, "hello", 3.14);  // still nothing
```

## tap

Side-effect hook that returns the original object.

```cpp
std::vector<int> nums = {3, 1, 2};
auto sorted = ctl::sort_by(
    ctl::tap(nums, [](auto& v) { std::cout << "size: " << v.size() << "\n"; }),
    ctl::identity
);
```

## negate

Inverts a predicate.

```cpp
auto is_even = [](int x) { return x % 2 == 0; };
auto is_odd = ctl::negate(is_even);
is_odd(3);  // true
```

## times

```cpp
auto squares = ctl::times(5, [](std::size_t i) { return i * i; });
// {0, 1, 4, 9, 16}
```

## range

```cpp
auto r1 = ctl::range(5);        // {0, 1, 2, 3, 4}
auto r2 = ctl::range(2, 5);     // {2, 3, 4}
auto r3 = ctl::range(0, 10, 3); // {0, 3, 6, 9}
auto r4 = ctl::range(5, 0);     // {5, 4, 3, 2, 1}
```

## pipe / compose

```cpp
auto transform = ctl::pipe(
    [](int x) { return x * 2; },
    [](int x) { return x + 1; },
    [](int x) { return std::to_string(x); }
);
transform(5);  // "11"

auto same = ctl::compose(
    [](int x) { return std::to_string(x); },
    [](int x) { return x + 1; },
    [](int x) { return x * 2; }
);
same(5);  // "11"
```

## cond

Evaluates `(predicate, transform)` pairs in order. Returns `std::optional<R>` — `std::nullopt` if no predicate matched.

```cpp
auto classify = ctl::cond(
    std::pair{[](int x) { return x < 0; },  [](int) { return std::string("negative"); }},
    std::pair{[](int x) { return x == 0; }, [](int) { return std::string("zero"); }},
    std::pair{[](int) { return true; },     [](int) { return std::string("positive"); }}
);
classify(-1);  // optional("negative")
classify(0);   // optional("zero")
classify(42);  // optional("positive")
```

## thru

Pass a value through a function and return the result.

```cpp
auto result = ctl::thru(42, [](int x) { return x * 2; });
// 84
```

## Next

- [Runtime Access](11-runtime.md)
- [Collections](0-collections.md)
