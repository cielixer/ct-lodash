# Limitations & Lodash Comparison

## constexpr Note
ctl functions are marked `constexpr`, but compile-time evaluation requires `constexpr` inputs. When used with `std::string` or `std::vector` at runtime, the same API executes at runtime.

## What ctl Cannot Do

### Dynamic Struct Creation
Lodash's `pick({a:1,b:2,c:3}, ['a','b'])` returns a new object shape. C++ cannot create new struct types at runtime. ctl's `pick<Target>(source)` requires a pre-defined target struct.

This applies to: `pick`, `omit`, `merge`, and `defaults` when used across different types.

**Workaround**: Define the target struct yourself. ctl will copy matching fields by name.

**Future**: C++26 reflection (P2996) with `define_aggregate` could solve this, but compiler support is not yet available.

### Functions Not Provided

| Category | Functions | Reason |
|---|---|---|
| JS built-ins | concat, slice, indexOf, lastIndexOf, join, nth, reduceRight, forEachRight | Direct equivalents in C++ std or trivial with iterators |
| Type checks | isString, isNumber, isArray, isObject, isNull, isUndefined, isEmpty, ... | C++ type system handles this at compile time |
| Comparison | eq, gt, gte, lt, lte | C++ operators |
| String ops | camelCase, kebabCase, trim, pad, ... | Not struct-related; use a string library |
| FP utils | curry, partial, once, clamp | Direct std:: equivalents (bind_front, call_once, std::clamp) |
| Timer-based | debounce, throttle | Requires runtime timer infrastructure |
| Chainable | chain, tap, value | Use functional `pipe` instead |
| Dynamic | pick/omit (cross-type), merge (cross-type) | See "Dynamic Struct Creation" above |

### Behavioral Differences from Lodash

| Function | Lodash Behavior | ctl Behavior | Why |
|---|---|---|---|
| pick/omit | Returns new plain object | Requires user-defined target struct | No dynamic struct creation in C++ |
| merge/defaults | Works across different shapes | Same-type only (deep recursive) | Type safety and shape stability |
| uniq/uniq_by | Global uniqueness | Consecutive duplicates only | O(n) without hash; use sort_by + uniq for global |
| flattenDeep | Handles mixed [1, [2, [3]]] | Homogeneous nesting only | C++ type system is homogeneous |
| flatten_depth | Runtime depth | Compile-time depth | Return type depends on depth |
| *_by | fn is last argument | fn is FIRST argument | C++ variadic pack must be last |
| pick_by/omit_by | Works on objects | Map (pair_value_range) only | Struct field filtering needs known result type |
