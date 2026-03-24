# Set Operations

Intersection, difference, union, symmetric difference, and uniqueness.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `intersection(a, b, ...)` | Elements in all ranges | `vector<T>` |
| `difference(a, b, ...)` | Elements in first but not others | `vector<T>` |
| `set_union(a, b, ...)` | Unique elements from all | `vector<T>` |
| `xor_with(a, b, ...)` | Present in exactly one range | `vector<T>` |
| `intersection_by(fn, a, b, ...)` | Intersection by key | `vector<T>` |
| `difference_by(fn, a, b, ...)` | Difference by key | `vector<T>` |
| `union_by(fn, a, b, ...)` | Union by key | `vector<T>` |
| `xor_by(fn, a, b, ...)` | Symmetric diff by key | `vector<T>` |
| `uniq(c)` | Remove consecutive duplicates | `vector<T>` |
| `uniq_by(c, fn)` | Remove consecutive dups by key | `vector<T>` |

> **`_by` variants**: The iteratee function is the **first** argument (C++ variadic packs must be last).

> **`union`** is a C++ keyword → named `set_union`. **`xor`** is a reserved token → named `xor_with`.

## Examples

```cpp
std::vector<int> a = {1, 2, 3, 4};
std::vector<int> b = {3, 4, 5, 6};
std::vector<int> c = {4, 5, 6, 7};
```

## intersection / difference / set_union

```cpp
auto inter = ctl::intersection(a, b);     // {3, 4}
auto inter3 = ctl::intersection(a, b, c); // {4}

auto diff = ctl::difference(a, b);        // {1, 2}

auto uni = ctl::set_union(a, b);          // {1, 2, 3, 4, 5, 6}
```

## xor_with

Elements present in exactly one input range.

```cpp
auto sym = ctl::xor_with(a, b);  // {1, 2, 5, 6}
```

## *_by variants

Compare by iteratee output instead of direct equality.

```cpp
struct Item { int id; std::string name; };
std::vector<Item> x = {{1, "a"}, {2, "b"}};
std::vector<Item> y = {{2, "B"}, {3, "c"}};
auto id_fn = [](const Item& i) { return i.id; };

auto inter = ctl::intersection_by(id_fn, x, y);  // {Item{2, "b"}}
auto diff  = ctl::difference_by(id_fn, x, y);    // {Item{1, "a"}}
auto uni   = ctl::union_by(id_fn, x, y);         // {Item{1,"a"}, Item{2,"b"}, Item{3,"c"}}
auto sym   = ctl::xor_by(id_fn, x, y);           // {Item{1,"a"}, Item{3,"c"}}
```

## uniq / uniq_by

Removes **consecutive** duplicates (not global). Sort first for global uniqueness.

```cpp
std::vector<int> nums = {1, 1, 2, 3, 3, 2};
auto u = ctl::uniq(nums);  // {1, 2, 3, 2}

// For global uniqueness:
auto sorted = ctl::sort_by(nums, ctl::identity);
auto global = ctl::uniq(sorted);  // {1, 2, 3}
```

```cpp
struct Person { std::string name; int age; };
std::vector<Person> people = {{"A", 25}, {"B", 25}, {"C", 30}};
auto unique_ages = ctl::uniq_by(people, [](const Person& p) { return p.age; });
// {Person{"A", 25}, Person{"C", 30}}
```

## Next

- [Reshape](8-reshape.md)
- [Collections](0-collections.md)
