#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>

// ============================================================================
// difference_by
// ============================================================================

TEST_CASE("difference_by excludes by iteratee key", "[collection][difference_by]") {
  std::vector<int> first{1, 2, 3, 4};
  std::vector<int> rest{3, 6};

  auto result = ctl::difference_by([](int x) { return x % 3; }, first, rest);

  REQUIRE(result == std::vector<int>{1, 2, 4});
}

TEST_CASE("difference_by on empty first returns empty", "[collection][difference_by]") {
  std::vector<int> first;
  std::vector<int> rest{1, 2, 3};

  auto result = ctl::difference_by([](int x) { return x % 3; }, first, rest);

  REQUIRE(result.empty());
}

// ============================================================================
// intersection_by
// ============================================================================

TEST_CASE("intersection_by keeps first-range values with matching keys", "[collection][intersection_by]") {
  std::vector<int> first{1, 2, 3};
  std::vector<int> second{4, 5, 6};

  auto result = ctl::intersection_by([](int x) { return x % 3; }, first, second);

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

TEST_CASE("intersection_by with no matching keys returns empty", "[collection][intersection_by]") {
  std::vector<int> first{1, 2, 3};
  std::vector<int> second{4, 5, 6};

  auto result = ctl::intersection_by([](int x) { return x + 10; }, first, second);

  REQUIRE(result.empty());
}

// ============================================================================
// union_by
// ============================================================================

TEST_CASE("union_by keeps first-seen element per key", "[collection][union_by]") {
  std::vector<int> first{1, 2, 3};
  std::vector<int> second{4, 5, 6};

  auto result = ctl::union_by([](int x) { return x % 3; }, first, second);

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

TEST_CASE("union_by combines multiple ranges by unique keys", "[collection][union_by]") {
  std::vector<int> first{10, 21};
  std::vector<int> second{32, 43};
  std::vector<int> third{54, 65};

  auto result = ctl::union_by([](int x) { return x % 10; }, first, second, third);

  REQUIRE(result == std::vector<int>{10, 21, 32, 43, 54, 65});
}

// ============================================================================
// xor_with
// ============================================================================

TEST_CASE("xor_with returns elements present in exactly one range", "[collection][xor_with]") {
  std::vector<int> first{1, 2, 3};
  std::vector<int> second{2, 3, 4};

  auto result = ctl::xor_with(first, second);

  REQUIRE(result == std::vector<int>{1, 4});
}

TEST_CASE("xor_with across three ranges drops values present in two", "[collection][xor_with]") {
  std::vector<int> first{1, 2};
  std::vector<int> second{2, 3};
  std::vector<int> third{3, 4};

  auto result = ctl::xor_with(first, second, third);

  REQUIRE(result == std::vector<int>{1, 4});
}

// ============================================================================
// xor_by
// ============================================================================

TEST_CASE("xor_by returns first element for keys present in one range", "[collection][xor_by]") {
  std::vector<int> first{11, 22};
  std::vector<int> second{12, 23};

  auto result = ctl::xor_by([](int x) { return x % 10; }, first, second);

  REQUIRE(result == std::vector<int>{11, 23});
}

TEST_CASE("xor_by with all keys shared returns empty", "[collection][xor_by]") {
  std::vector<int> first{11, 22};
  std::vector<int> second{31, 42};

  auto result = ctl::xor_by([](int x) { return x % 10; }, first, second);

  REQUIRE(result.empty());
}

// ============================================================================
// flatten_deep
// ============================================================================

TEST_CASE("flatten_deep recursively flattens three levels", "[collection][flatten_deep]") {
  std::vector<std::vector<std::vector<int>>> nested{{{1, 2}, {3}}, {{4}, {5, 6}}};

  auto result = ctl::flatten_deep(nested);

  REQUIRE(result == std::vector<int>{1, 2, 3, 4, 5, 6});
}

TEST_CASE("flatten_deep handles two-level input", "[collection][flatten_deep]") {
  std::vector<std::vector<int>> nested{{1, 2}, {}, {3}};

  auto result = ctl::flatten_deep(nested);

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

// ============================================================================
// flatten_depth
// ============================================================================

TEST_CASE("flatten_depth<0> returns outer-level copy", "[collection][flatten_depth]") {
  std::vector<std::vector<std::vector<int>>> nested{{{1, 2}}, {{3}}};

  auto result = ctl::flatten_depth<0>(nested);

  REQUIRE(result == nested);
}

TEST_CASE("flatten_depth<1> flattens one level", "[collection][flatten_depth]") {
  std::vector<std::vector<std::vector<int>>> nested{{{1, 2}, {3}}, {{4}}};

  auto result = ctl::flatten_depth<1>(nested);

  REQUIRE(result == std::vector<std::vector<int>>{{1, 2}, {3}, {4}});
}

TEST_CASE("flatten_depth<2> flattens two levels", "[collection][flatten_depth]") {
  std::vector<std::vector<std::vector<int>>> nested{{{1, 2}, {3}}, {{4, 5}}};

  auto result = ctl::flatten_depth<2>(nested);

  REQUIRE(result == std::vector<int>{1, 2, 3, 4, 5});
}

// ============================================================================
// unzip
// ============================================================================

TEST_CASE("unzip splits vector of pairs", "[collection][unzip]") {
  std::vector<std::pair<int, std::string>> pairs{{1, "a"}, {2, "b"}, {3, "c"}};

  auto result = ctl::unzip(pairs);

  REQUIRE(result.first == std::vector<int>{1, 2, 3});
  REQUIRE(result.second == std::vector<std::string>{"a", "b", "c"});
}

TEST_CASE("unzip supports std::map and empty input", "[collection][unzip]") {
  std::map<std::string, int> m{{"x", 10}, {"y", 20}};
  auto mapped = ctl::unzip(m);
  REQUIRE(mapped.first == std::vector<std::string>{"x", "y"});
  REQUIRE(mapped.second == std::vector<int>{10, 20});

  std::vector<std::pair<int, std::string>> empty;
  auto empty_result = ctl::unzip(empty);
  REQUIRE(empty_result.first.empty());
  REQUIRE(empty_result.second.empty());
}

// ============================================================================
// zip_with
// ============================================================================

TEST_CASE("zip_with zips and transforms paired elements", "[collection][zip_with]") {
  std::vector<int> a{1, 2, 3};
  std::vector<int> b{10, 20, 30};

  auto result = ctl::zip_with(a, b, [](int x, int y) { return x + y; });

  REQUIRE(result == std::vector<int>{11, 22, 33});
}

TEST_CASE("zip_with stops at shorter range", "[collection][zip_with]") {
  std::vector<int> a{1, 2, 3, 4};
  std::vector<int> b{10, 20};

  auto result = ctl::zip_with(a, b, [](int x, int y) { return x + y; });

  REQUIRE(result == std::vector<int>{11, 22});
}

// ============================================================================
// zip_object
// ============================================================================

TEST_CASE("zip_object builds map from keys and values", "[collection][zip_object]") {
  std::vector<std::string> keys{"a", "b", "c"};
  std::vector<int> values{1, 2, 3};

  auto result = ctl::zip_object(keys, values);

  REQUIRE(result == std::unordered_map<std::string, int>{{"a", 1}, {"b", 2}, {"c", 3}});
}

TEST_CASE("zip_object stops at shorter input", "[collection][zip_object]") {
  std::vector<std::string> keys{"a", "b", "c"};
  std::vector<int> values{1, 2};

  auto result = ctl::zip_object(keys, values);

  REQUIRE(result == std::unordered_map<std::string, int>{{"a", 1}, {"b", 2}});
}

// ============================================================================
// pick_by
// ============================================================================

TEST_CASE("pick_by filters map by value predicate", "[collection][pick_by]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 3}, {"c", 5}};

  auto result = ctl::pick_by(m, [](const std::string&, int value) { return value > 2; });

  REQUIRE(result == std::unordered_map<std::string, int>{{"b", 3}, {"c", 5}});
}

TEST_CASE("pick_by supports key/value predicate", "[collection][pick_by]") {
  std::map<std::string, int> m{{"alpha", 1}, {"beta", 2}, {"axis", 3}};

  auto result = ctl::pick_by(m, [](const std::string& key, int) { return key.starts_with('a'); });

  REQUIRE(result == std::unordered_map<std::string, int>{{"alpha", 1}, {"axis", 3}});
}

// ============================================================================
// omit_by
// ============================================================================

TEST_CASE("omit_by removes map entries matching predicate", "[collection][omit_by]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 3}, {"c", 5}};

  auto result = ctl::omit_by(m, [](const std::string&, int value) { return value > 2; });

  REQUIRE(result == std::unordered_map<std::string, int>{{"a", 1}});
}

TEST_CASE("omit_by supports key/value predicate", "[collection][omit_by]") {
  std::map<std::string, int> m{{"alpha", 1}, {"beta", 2}, {"axis", 3}};

  auto result = ctl::omit_by(m, [](const std::string& key, int) { return key.starts_with('a'); });

  REQUIRE(result == std::unordered_map<std::string, int>{{"beta", 2}});
}

// ============================================================================
// map_keys
// ============================================================================

TEST_CASE("map_keys transforms keys using key and value", "[collection][map_keys]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 2}};

  auto result = ctl::map_keys(m, [](const std::string& key, int value) {
    return std::string{"k_"} + key + std::to_string(value);
  });

  REQUIRE(result == std::unordered_map<std::string, int>{{"k_a1", 1}, {"k_b2", 2}});
}

TEST_CASE("map_keys on empty input returns empty map", "[collection][map_keys]") {
  std::map<std::string, int> m;

  auto result = ctl::map_keys(m, [](const std::string& key, int) { return std::string{"new_"} + key; });

  REQUIRE(result.empty());
}

// ============================================================================
// invert
// ============================================================================

TEST_CASE("invert swaps key and value types", "[collection][invert]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 2}};

  auto result = ctl::invert(m);

  REQUIRE(result == std::unordered_map<int, std::string>{{1, "a"}, {2, "b"}});
}

TEST_CASE("invert with duplicate values keeps last key", "[collection][invert]") {
  std::map<std::string, int> m{{"first", 1}, {"second", 1}};

  auto result = ctl::invert(m);

  REQUIRE(result == std::unordered_map<int, std::string>{{1, "second"}});
}

// ============================================================================
// min_by
// ============================================================================

TEST_CASE("min_by returns element with smallest iteratee key", "[collection][min_by]") {
  struct Person {
    std::string name;
    int age;

    bool operator==(const Person&) const = default;
  };

  std::vector<Person> people{{"Alice", 33}, {"Bob", 21}, {"Carol", 25}};

  auto result = ctl::min_by(people, [](const Person& p) { return p.age; });

  REQUIRE(result.has_value());
  REQUIRE(*result == Person{"Bob", 21});
}

TEST_CASE("min_by on empty returns nullopt", "[collection][min_by]") {
  std::vector<int> values;

  auto result = ctl::min_by(values, [](int x) { return x; });

  REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// max_by
// ============================================================================

TEST_CASE("max_by returns element with largest iteratee key", "[collection][max_by]") {
  struct Person {
    std::string name;
    int age;

    bool operator==(const Person&) const = default;
  };

  std::vector<Person> people{{"Alice", 33}, {"Bob", 21}, {"Carol", 25}};

  auto result = ctl::max_by(people, [](const Person& p) { return p.age; });

  REQUIRE(result.has_value());
  REQUIRE(*result == Person{"Alice", 33});
}

TEST_CASE("max_by on empty returns nullopt", "[collection][max_by]") {
  std::vector<int> values;

  auto result = ctl::max_by(values, [](int x) { return x; });

  REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// sum_by
// ============================================================================

TEST_CASE("sum_by accumulates iteratee outputs", "[collection][sum_by]") {
  std::vector<int> values{1, 2, 3};

  auto result = ctl::sum_by(values, [](int x) { return x * 2; });

  REQUIRE(result == 12);
}

TEST_CASE("sum_by on empty returns zero value", "[collection][sum_by]") {
  std::vector<int> values;

  auto result = ctl::sum_by(values, [](int x) { return x * 2; });

  REQUIRE(result == 0);
}

// ============================================================================
// mean
// ============================================================================

TEST_CASE("mean returns arithmetic average", "[collection][mean]") {
  std::vector<int> values{1, 2, 3, 4};

  auto result = ctl::mean(values);

  REQUIRE(result == Catch::Approx(2.5));
}

TEST_CASE("mean on empty returns 0.0", "[collection][mean]") {
  std::vector<int> values;

  auto result = ctl::mean(values);

  REQUIRE(result == Catch::Approx(0.0));
}

// ============================================================================
// mean_by
// ============================================================================

TEST_CASE("mean_by averages iteratee output", "[collection][mean_by]") {
  struct Person {
    std::string name;
    int age;
  };

  std::vector<Person> people{{"Alice", 20}, {"Bob", 30}, {"Carol", 40}};

  auto result = ctl::mean_by(people, [](const Person& p) { return p.age; });

  REQUIRE(result == Catch::Approx(30.0));
}

TEST_CASE("mean_by on empty returns 0.0", "[collection][mean_by]") {
  std::vector<int> values;

  auto result = ctl::mean_by(values, [](int x) { return x; });

  REQUIRE(result == Catch::Approx(0.0));
}
