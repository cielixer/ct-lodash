# Collections

ctl collection functions accept three kinds of inputs. You don't need to know `std::ranges` or concepts — just use standard containers.

## Input Categories

| Category | What it means | Behavior |
|---|---|---|
| **Non-pair range** | Any iterable whose value type is **not** `std::pair` | Element-wise operations |
| **Pair-value range** | Any iterable whose value type **is** `std::pair<K,V>` | Key-value aware; callbacks receive `(key, value)` |
| **Described record** | Any struct registered with `CTL_DESCRIBE_STRUCT` | Struct operations (see [Struct Ops](2-struct-ops.md)) |

Internally, ctl checks `std::ranges::range<C>` — anything with `begin()`/`end()` qualifies. You never write this concept yourself.

## Non-pair range — full list

All of these work out of the box:

| Container | Header |
|---|---|
| `std::vector<T>` | `<vector>` |
| `std::array<T, N>` | `<array>` |
| `std::deque<T>` | `<deque>` |
| `std::list<T>` | `<list>` |
| `std::forward_list<T>` | `<forward_list>` |
| `std::set<T>` | `<set>` |
| `std::unordered_set<T>` | `<unordered_set>` |
| `std::multiset<T>` | `<set>` |
| `std::unordered_multiset<T>` | `<unordered_set>` |
| `std::span<T>` | `<span>` |
| `std::string` / `std::string_view` | `<string>` / `<string_view>` |
| `T[N]` (C-style array) | — |
| `std::initializer_list<T>` | `<initializer_list>` |

```cpp
std::vector<int> nums = {1, 2, 3};
auto doubled = ctl::map(nums, [](int x) { return x * 2; });
// {2, 4, 6}

std::array<std::string, 2> arr = {"hello", "world"};
auto lens = ctl::map(arr, [](const std::string& s) { return s.size(); });
// {5, 5}

std::set<int> s = {3, 1, 2};
auto sq = ctl::map(s, [](int x) { return x * x; });
// {9, 1, 4}

std::deque<int> dq = {10, 20, 30};
auto filtered = ctl::filter(dq, [](int x) { return x > 15; });
// {20, 30}
```

## Pair-value range — full list

Any iterable whose elements are `std::pair<K,V>`:

| Container | Header |
|---|---|
| `std::map<K, V>` | `<map>` |
| `std::unordered_map<K, V>` | `<unordered_map>` |
| `std::multimap<K, V>` | `<map>` |
| `std::unordered_multimap<K, V>` | `<unordered_map>` |
| `std::vector<std::pair<K, V>>` | `<vector>` |

Callbacks on pair ranges receive `(key, value)` as two arguments:

```cpp
std::map<std::string, int> scores = {{"alice", 90}, {"bob", 70}};

ctl::for_each(scores, [](const std::string& name, int score) {
    std::cout << name << ": " << score << "\n";
});

auto names = ctl::map(scores, [](const std::string& name, int) {
    return name;
});
// {"alice", "bob"}

std::unordered_map<int, std::string> lookup = {{1, "one"}, {2, "two"}};
auto vals = ctl::map(lookup, [](int, const std::string& v) { return v; });
// {"one", "two"} (order unspecified)
```

Single-argument callbacks also work — they receive `std::pair`:

```cpp
auto filtered = ctl::filter(scores, [](const auto& pair) {
    return pair.second > 80;
});
```

## Custom containers

Any type that satisfies `std::ranges::range` works with ctl — no registration needed. Your container just needs `begin()` and `end()` returning valid iterators.

```cpp
struct MyRing {
    std::array<int, 4> data = {10, 20, 30, 40};
    auto begin() { return data.begin(); }
    auto end()   { return data.end(); }
    auto begin() const { return data.begin(); }
    auto end()   const { return data.end(); }
};

MyRing ring;
auto doubled = ctl::map(ring, [](int x) { return x * 2; });
// {20, 40, 60, 80}
```

If your custom container holds `std::pair` elements, ctl will automatically treat it as a pair-value range.

## What does NOT work

| Type | Why |
|---|---|
| Raw pointers (`int*`) | No `begin()`/`end()` — not a range |
| `std::stack`, `std::queue`, `std::priority_queue` | Container adaptors — no iterators |
| `std::optional<T>` | Not a range |
| `std::variant<...>` | Not a range |

If you need to iterate a raw pointer + length, wrap it in `std::span`:

```cpp
int arr[] = {1, 2, 3, 4};
int* ptr = arr;

// ctl::map(ptr, ...);  // ✗ won't compile

auto sp = std::span(ptr, 4);
auto result = ctl::map(sp, [](int x) { return x + 1; });
// {2, 3, 4, 5}
```

## Return types

Most collection functions return `std::vector`. Aggregation functions (`group_by`, `key_by`, `count_by`, `zip_object`, etc.) return a map — `std::unordered_map` when the key type is hashable, `std::map` otherwise.

| Function | Returns |
|---|---|
| map, filter, reject, take, drop, ... | `std::vector<T>` |
| group_by | `unordered_map<K, vector<V>>` or `map<K, vector<V>>` |
| key_by | `unordered_map<K, V>` or `map<K, V>` |
| count_by | `unordered_map<K, size_t>` or `map<K, size_t>` |
| zip_object | `unordered_map<K, V>` or `map<K, V>` |
| pick_by, omit_by, map_keys, invert | `unordered_map<K, V>` or `map<K, V>` |
| find, head, last | `std::optional<T>` |
| every, some, includes | `bool` |
| size | `std::size_t` |
| reduce | Accumulator type |

> **auto_map**: ctl uses `std::unordered_map` when `std::hash<K>` is available (e.g., `int`, `std::string`), falling back to `std::map` for types without a hash. Use `auto` for the return type and this is transparent. To make your own structs hashable, see `CTL_MAKE_HASHABLE` in [Basic Usage](../2-basic-usage.md).

## Next

- [Transform & Filter](3-transform-filter.md)
- [Search & Check](4-search-check.md)
- [Aggregation](5-aggregation.md)
