# Reshape

Restructure data: zip, unzip, flatten nested containers.

## Quick Reference

| Function | Signature | Returns |
|---|---|---|
| `zip(a, b)` | Pair up elements | `vector<pair<A,B>>` |
| `zip_with(a, b, fn)` | Zip and transform | `vector<R>` |
| `zip_object(keys, vals)` | Keys + values → map | `map<K,V>` |
| `unzip(c)` | Split pairs into two vectors | `pair<vector<A>, vector<B>>` |
| `flatten(c)` | Flatten one level | `vector<T>` |
| `flatten_deep(c)` | Recursively flatten all levels | `vector<Leaf>` |
| `flatten_depth<N>(c)` | Flatten N levels (compile-time) | `vector<T>` |

## zip / zip_with

Stops at the shorter input.

```cpp
std::vector<std::string> names = {"Alice", "Bob"};
std::vector<int> ages = {25, 30};

auto pairs = ctl::zip(names, ages);
// {{"Alice", 25}, {"Bob", 30}}

auto msgs = ctl::zip_with(names, ages, [](const std::string& n, int a) {
    return n + "=" + std::to_string(a);
});
// {"Alice=25", "Bob=30"}
```

## zip_object

Pairs key and value ranges into `std::map`.

```cpp
std::vector<std::string> keys = {"a", "b", "c"};
std::vector<int> vals = {1, 2, 3};
auto m = ctl::zip_object(keys, vals);
// {"a": 1, "b": 2, "c": 3}
```

## unzip

Inverse of `zip`. Works on any pair-value range.

```cpp
std::vector<std::pair<std::string, int>> zipped = {{"Alice", 25}, {"Bob", 30}};
auto [names, ages] = ctl::unzip(zipped);
// names: {"Alice", "Bob"}, ages: {25, 30}
```

## flatten

Flattens one level of nesting.

```cpp
std::vector<std::vector<int>> nested = {{1, 2}, {3, 4}, {5}};
auto flat = ctl::flatten(nested);  // {1, 2, 3, 4, 5}
```

## flatten_deep

Recursively flattens all nesting levels to leaf values. Only works with **homogeneous** nesting (e.g., `vector<vector<vector<int>>>`).

```cpp
std::vector<std::vector<std::vector<int>>> deep = {{{1, 2}, {3}}, {{4}}};
auto flat = ctl::flatten_deep(deep);  // {1, 2, 3, 4}
```

> C++ containers are homogeneous — mixed nesting like JS `[1, [2, [3]]]` is not possible.

## flatten_depth

Flatten exactly N levels. Depth is a compile-time template parameter (return type depends on it).

```cpp
std::vector<std::vector<std::vector<int>>> deep = {{{1, 2}, {3}}, {{4}}};
auto one   = ctl::flatten_depth<1>(deep);  // vector<vector<int>>: {{1,2}, {3}, {4}}
auto two   = ctl::flatten_depth<2>(deep);  // vector<int>: {1, 2, 3, 4}
```

## Next

- [Map Operations](9-map-ops.md)
- [Set Operations](7-set-ops.md)
