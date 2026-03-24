#include <map>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>

// ============================================================================
// flat_map
// ============================================================================

TEST_CASE("flat_map flattens mapped vectors", "[collection][flat_map]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::flat_map(vec, [](int x) { return std::vector<int>{x, x * 10}; });

  REQUIRE(result == std::vector<int>{1, 10, 2, 20, 3, 30});
}

TEST_CASE("flat_map on empty returns empty", "[collection][flat_map]") {
  std::vector<int> vec;
  auto result = ctl::flat_map(vec, [](int x) { return std::vector<int>{x}; });

  REQUIRE(result.empty());
}

// ============================================================================
// reject
// ============================================================================

TEST_CASE("reject on vector keeps non-matching values", "[collection][reject]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto result = ctl::reject(vec, [](int x) { return x % 2 == 0; });

  REQUIRE(result == std::vector<int>{1, 3, 5});
}

TEST_CASE("reject on map supports key/value predicate", "[collection][reject]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 3}};
  auto result = ctl::reject(m, [](const std::string& key, int) { return key == "b"; });

  REQUIRE(result.size() == 2);
  REQUIRE(result[0].first == "a");
  REQUIRE(result[1].first == "c");
}

// ============================================================================
// take_while
// ============================================================================

TEST_CASE("take_while takes prefix while true", "[collection][take_while]") {
  std::vector<int> vec{2, 4, 6, 7, 8};
  auto result = ctl::take_while(vec, [](int x) { return x % 2 == 0; });

  REQUIRE(result == std::vector<int>{2, 4, 6});
}

TEST_CASE("take_while returns empty when first fails", "[collection][take_while]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::take_while(vec, [](int x) { return x % 2 == 0; });

  REQUIRE(result.empty());
}

// ============================================================================
// drop_while
// ============================================================================

TEST_CASE("drop_while skips prefix while true", "[collection][drop_while]") {
  std::vector<int> vec{1, 3, 5, 6, 7};
  auto result = ctl::drop_while(vec, [](int x) { return x % 2 == 1; });

  REQUIRE(result == std::vector<int>{6, 7});
}

TEST_CASE("drop_while returns empty when all dropped", "[collection][drop_while]") {
  std::vector<int> vec{1, 3, 5};
  auto result = ctl::drop_while(vec, [](int x) { return x % 2 == 1; });

  REQUIRE(result.empty());
}

// ============================================================================
// find_last
// ============================================================================

TEST_CASE("find_last returns final matching element", "[collection][find_last]") {
  std::vector<int> vec{1, 2, 3, 2, 1};
  auto result = ctl::find_last(vec, [](int x) { return x == 2; });

  REQUIRE(result.has_value());
  REQUIRE(*result == 2);
}

TEST_CASE("find_last returns nullopt when no match", "[collection][find_last]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::find_last(vec, [](int x) { return x > 10; });

  REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// find_last_index
// ============================================================================

TEST_CASE("find_last_index returns last match index", "[collection][find_last_index]") {
  std::vector<int> vec{5, 7, 5, 9};
  auto result = ctl::find_last_index(vec, [](int x) { return x == 5; });

  REQUIRE(result.has_value());
  REQUIRE(*result == 2);
}

TEST_CASE("find_last_index returns nullopt for no match", "[collection][find_last_index]") {
  std::vector<int> vec{5, 7, 9};
  auto result = ctl::find_last_index(vec, [](int x) { return x == 1; });

  REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// without
// ============================================================================

TEST_CASE("without removes all excluded values", "[collection][without]") {
  std::vector<int> vec{1, 2, 3, 2, 4, 2};
  auto result = ctl::without(vec, {2, 4});

  REQUIRE(result == std::vector<int>{1, 3});
}

TEST_CASE("without with empty excludes keeps all", "[collection][without]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::without(vec, {});

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

// ============================================================================
// partition
// ============================================================================

TEST_CASE("partition splits matching and non-matching", "[collection][partition]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto result = ctl::partition(vec, [](int x) { return x % 2 == 0; });

  REQUIRE(result.first == std::vector<int>{2, 4});
  REQUIRE(result.second == std::vector<int>{1, 3, 5});
}

TEST_CASE("partition on empty returns two empty vectors", "[collection][partition]") {
  std::vector<int> vec;
  auto result = ctl::partition(vec, [](int) { return true; });

  REQUIRE(result.first.empty());
  REQUIRE(result.second.empty());
}

// ============================================================================
// uniq
// ============================================================================

TEST_CASE("uniq removes consecutive duplicates", "[collection][uniq]") {
  std::vector<int> vec{1, 1, 2, 2, 2, 3, 1, 1};
  auto result = ctl::uniq(vec);

  REQUIRE(result == std::vector<int>{1, 2, 3, 1});
}

TEST_CASE("uniq on empty returns empty", "[collection][uniq]") {
  std::vector<int> vec;
  auto result = ctl::uniq(vec);

  REQUIRE(result.empty());
}

// ============================================================================
// uniq_by
// ============================================================================

TEST_CASE("uniq_by removes consecutive duplicates by key", "[collection][uniq_by]") {
  std::vector<int> vec{1, 3, 2, 4, 5, 7};
  auto result = ctl::uniq_by(vec, [](int x) { return x % 2; });

  REQUIRE(result == std::vector<int>{1, 2, 5});
}

TEST_CASE("uniq_by keeps first of equal-key runs", "[collection][uniq_by]") {
  std::vector<std::string> vec{"a", "aa", "b", "bb", "bbb", "c"};
  auto result = ctl::uniq_by(vec, [](const std::string& s) { return s[0]; });

  REQUIRE(result == std::vector<std::string>{"a", "b", "c"});
}

// ============================================================================
// flatten
// ============================================================================

TEST_CASE("flatten flattens one level", "[collection][flatten]") {
  std::vector<std::vector<int>> vec{{1, 2}, {3}, {}, {4, 5}};
  auto result = ctl::flatten(vec);

  REQUIRE(result == std::vector<int>{1, 2, 3, 4, 5});
}

TEST_CASE("flatten on empty outer returns empty", "[collection][flatten]") {
  std::vector<std::vector<int>> vec;
  auto result = ctl::flatten(vec);

  REQUIRE(result.empty());
}

// ============================================================================
// group_by
// ============================================================================

TEST_CASE("group_by on vector with int key", "[collection][group_by]") {
  std::vector<int> vec{1, 2, 3, 4, 5, 6};
  auto result = ctl::group_by(vec, [](int x) { return x % 2; });

  REQUIRE(result.size() == 2);
  REQUIRE(result[0] == std::vector<int>{2, 4, 6});
  REQUIRE(result[1] == std::vector<int>{1, 3, 5});
}

TEST_CASE("group_by on vector with string key", "[collection][group_by]") {
  std::vector<std::string> vec{"one", "two", "three", "four", "five"};
  auto result = ctl::group_by(vec, [](const std::string& s) { return s.size(); });

  REQUIRE(result[3] == std::vector<std::string>{"one", "two"});
  REQUIRE(result[5] == std::vector<std::string>{"three"});
  REQUIRE(result[4] == std::vector<std::string>{"four", "five"});
}

TEST_CASE("group_by on empty container", "[collection][group_by]") {
  std::vector<int> vec;
  auto result = ctl::group_by(vec, [](int x) { return x; });

  REQUIRE(result.empty());
}

TEST_CASE("group_by on map with pair range", "[collection][group_by]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 1}, {"d", 2}};
  auto result = ctl::group_by(m, [](const std::string& /*k*/, int v) { return v; });

  REQUIRE(result.size() == 2);
  REQUIRE(result[1].size() == 2);
  REQUIRE(result[2].size() == 2);
}

// ============================================================================
// key_by
// ============================================================================

TEST_CASE("key_by indexes vector elements by key", "[collection][key_by]") {
  std::vector<std::string> vec{"apple", "banana", "cherry", "avocado"};
  auto result = ctl::key_by(vec, [](const std::string& s) { return s[0]; });

  REQUIRE(result.size() == 3);
  REQUIRE(result['a'] == "avocado");
  REQUIRE(result['b'] == "banana");
  REQUIRE(result['c'] == "cherry");
}

TEST_CASE("key_by last wins on duplicate keys", "[collection][key_by]") {
  std::vector<int> vec{1, 2, 3, 4, 5, 6};
  auto result = ctl::key_by(vec, [](int x) { return x % 3; });

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 6);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 5);
}

TEST_CASE("key_by on empty container", "[collection][key_by]") {
  std::vector<int> vec;
  auto result = ctl::key_by(vec, [](int x) { return x; });

  REQUIRE(result.empty());
}

TEST_CASE("key_by on map with pair range", "[collection][key_by]") {
  std::map<std::string, int> m{{"alice", 30}, {"bob", 25}, {"carol", 30}};
  auto result = ctl::key_by(m, [](const std::string& /*k*/, int v) { return v; });

  REQUIRE(result.size() == 2);
  REQUIRE(result[25].first == "bob");
  REQUIRE(result[30].first == "carol");
}

// ============================================================================
// intersection
// ============================================================================

TEST_CASE("intersection of two vectors", "[collection][intersection]") {
  std::vector<int> a{1, 2, 3, 4};
  std::vector<int> b{2, 4, 5, 6};
  auto result = ctl::intersection(a, b);

  REQUIRE(result == std::vector<int>{2, 4});
}

TEST_CASE("intersection of three vectors", "[collection][intersection]") {
  std::vector<int> a{1, 2, 3, 4, 5};
  std::vector<int> b{2, 3, 4, 6};
  std::vector<int> c{3, 4, 7};
  auto result = ctl::intersection(a, b, c);

  REQUIRE(result == std::vector<int>{3, 4});
}

TEST_CASE("intersection with no common elements", "[collection][intersection]") {
  std::vector<int> a{1, 2, 3};
  std::vector<int> b{4, 5, 6};
  auto result = ctl::intersection(a, b);

  REQUIRE(result.empty());
}

TEST_CASE("intersection preserves first range order", "[collection][intersection]") {
  std::vector<int> a{4, 3, 2, 1};
  std::vector<int> b{1, 2, 3, 4};
  auto result = ctl::intersection(a, b);

  REQUIRE(result == std::vector<int>{4, 3, 2, 1});
}

TEST_CASE("intersection deduplicates results", "[collection][intersection]") {
  std::vector<int> a{1, 2, 2, 3, 3};
  std::vector<int> b{2, 3, 3, 4};
  auto result = ctl::intersection(a, b);

  REQUIRE(result == std::vector<int>{2, 3});
}

TEST_CASE("intersection with empty container", "[collection][intersection]") {
  std::vector<int> a{1, 2, 3};
  std::vector<int> b;
  auto result = ctl::intersection(a, b);

  REQUIRE(result.empty());
}

// ============================================================================
// difference
// ============================================================================

TEST_CASE("difference of two vectors", "[collection][difference]") {
  std::vector<int> a{1, 2, 3, 4};
  std::vector<int> b{2, 4};
  auto result = ctl::difference(a, b);

  REQUIRE(result == std::vector<int>{1, 3});
}

TEST_CASE("difference with multiple exclusion ranges", "[collection][difference]") {
  std::vector<int> a{1, 2, 3, 4, 5};
  std::vector<int> b{2, 4};
  std::vector<int> c{3};
  auto result = ctl::difference(a, b, c);

  REQUIRE(result == std::vector<int>{1, 5});
}

TEST_CASE("difference preserves duplicates in first range", "[collection][difference]") {
  std::vector<int> a{1, 1, 2, 3, 3};
  std::vector<int> b{2};
  auto result = ctl::difference(a, b);

  REQUIRE(result == std::vector<int>{1, 1, 3, 3});
}

TEST_CASE("difference with no exclusions returns copy", "[collection][difference]") {
  std::vector<int> a{1, 2, 3};
  std::vector<int> b;
  auto result = ctl::difference(a, b);

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

TEST_CASE("difference with all excluded returns empty", "[collection][difference]") {
  std::vector<int> a{1, 2, 3};
  std::vector<int> b{1, 2, 3, 4, 5};
  auto result = ctl::difference(a, b);

  REQUIRE(result.empty());
}

// ============================================================================
// set_union
// ============================================================================

TEST_CASE("set_union combines two ranges without duplicates", "[collection][set_union]") {
  std::vector<int> a{1, 2, 3};
  std::vector<int> b{3, 4, 5};
  auto result = ctl::set_union(a, b);

  REQUIRE(result == std::vector<int>{1, 2, 3, 4, 5});
}

TEST_CASE("set_union preserves first-seen order", "[collection][set_union]") {
  std::vector<int> a{3, 1};
  std::vector<int> b{2, 1, 3};
  auto result = ctl::set_union(a, b);

  REQUIRE(result == std::vector<int>{3, 1, 2});
}

TEST_CASE("set_union deduplicates within a single range", "[collection][set_union]") {
  std::vector<int> a{1, 1, 2, 2, 3};
  std::vector<int> b{2, 3, 4};
  auto result = ctl::set_union(a, b);

  REQUIRE(result == std::vector<int>{1, 2, 3, 4});
}

TEST_CASE("set_union with multiple ranges", "[collection][set_union]") {
  std::vector<int> a{1, 2};
  std::vector<int> b{2, 3};
  std::vector<int> c{3, 4};
  auto result = ctl::set_union(a, b, c);

  REQUIRE(result == std::vector<int>{1, 2, 3, 4});
}

TEST_CASE("set_union with empty ranges", "[collection][set_union]") {
  std::vector<int> a;
  std::vector<int> b{1, 2};
  auto result = ctl::set_union(a, b);

  REQUIRE(result == std::vector<int>{1, 2});
}

TEST_CASE("set_union with strings", "[collection][set_union]") {
  std::vector<std::string> a{"hello", "world"};
  std::vector<std::string> b{"world", "foo"};
  auto result = ctl::set_union(a, b);

  REQUIRE(result == std::vector<std::string>{"hello", "world", "foo"});
}

// ============================================================================
// order_by
// ============================================================================

struct Employee {
  std::string name;
  int age;
  int salary;
};

TEST_CASE("order_by single key ascending", "[collection][order_by]") {
  std::vector<Employee> employees{
      {"Charlie", 30, 50000},
      {"Alice", 25, 60000},
      {"Bob", 35, 55000},
  };
  auto result = ctl::order_by(employees, ctl::asc([](const Employee& e) { return e.age; }));

  REQUIRE(result[0].name == "Alice");
  REQUIRE(result[1].name == "Charlie");
  REQUIRE(result[2].name == "Bob");
}

TEST_CASE("order_by single key descending", "[collection][order_by]") {
  std::vector<Employee> employees{
      {"Charlie", 30, 50000},
      {"Alice", 25, 60000},
      {"Bob", 35, 55000},
  };
  auto result = ctl::order_by(employees, ctl::desc([](const Employee& e) { return e.age; }));

  REQUIRE(result[0].name == "Bob");
  REQUIRE(result[1].name == "Charlie");
  REQUIRE(result[2].name == "Alice");
}

TEST_CASE("order_by multi-key: asc then desc", "[collection][order_by]") {
  std::vector<Employee> employees{
      {"Alice", 30, 60000},
      {"Bob", 30, 55000},
      {"Charlie", 25, 70000},
      {"Dave", 25, 50000},
  };
  auto result = ctl::order_by(
      employees,
      ctl::asc([](const Employee& e) { return e.age; }),
      ctl::desc([](const Employee& e) { return e.salary; }));

  REQUIRE(result[0].name == "Charlie");
  REQUIRE(result[1].name == "Dave");
  REQUIRE(result[2].name == "Alice");
  REQUIRE(result[3].name == "Bob");
}

TEST_CASE("order_by with strings", "[collection][order_by]") {
  std::vector<std::string> words{"banana", "apple", "cherry"};
  auto result = ctl::order_by(words, ctl::asc([](const std::string& s) { return s; }));

  REQUIRE(result == std::vector<std::string>{"apple", "banana", "cherry"});
}

TEST_CASE("order_by descending strings", "[collection][order_by]") {
  std::vector<std::string> words{"banana", "apple", "cherry"};
  auto result = ctl::order_by(words, ctl::desc([](const std::string& s) { return s; }));

  REQUIRE(result == std::vector<std::string>{"cherry", "banana", "apple"});
}

TEST_CASE("order_by empty container", "[collection][order_by]") {
  std::vector<int> empty;
  auto result = ctl::order_by(empty, ctl::asc([](int x) { return x; }));

  REQUIRE(result.empty());
}

TEST_CASE("order_by stable tiebreaker with second key", "[collection][order_by]") {
  std::vector<Employee> employees{
      {"Dave", 30, 50000},
      {"Alice", 30, 50000},
      {"Bob", 30, 70000},
  };
  auto result = ctl::order_by(
      employees,
      ctl::desc([](const Employee& e) { return e.salary; }),
      ctl::asc([](const Employee& e) { return e.name; }));

  REQUIRE(result[0].name == "Bob");
  REQUIRE(result[1].name == "Alice");
  REQUIRE(result[2].name == "Dave");
}

// ============================================================================
// transform
// ============================================================================

TEST_CASE("transform accumulates by reference", "[collection][transform]") {
  std::vector<int> vec{1, 2, 3, 4};
  auto result = ctl::transform(vec, std::vector<int>{}, [](auto& acc, int x) {
    acc.push_back(x * 2);
  });

  REQUIRE(result == std::vector<int>{2, 4, 6, 8});
}

TEST_CASE("transform early exit on false", "[collection][transform]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto result = ctl::transform(vec, std::vector<int>{}, [](auto& acc, int x) -> bool {
    if (x > 3) return false;
    acc.push_back(x);
    return true;
  });

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

TEST_CASE("transform with map accumulator", "[collection][transform]") {
  std::vector<std::string> words{"hello", "world", "hi"};
  auto result =
      ctl::transform(words, std::map<std::size_t, std::vector<std::string>>{},
                      [](auto& acc, const std::string& w) { acc[w.size()].push_back(w); });

  REQUIRE(result[5] == std::vector<std::string>{"hello", "world"});
  REQUIRE(result[2] == std::vector<std::string>{"hi"});
}

TEST_CASE("transform on pair range", "[collection][transform]") {
  std::map<std::string, int> scores{{"alice", 90}, {"bob", 80}};
  auto result = ctl::transform(scores, 0, [](int& acc, const std::string&, int v) { acc += v; });

  REQUIRE(result == 170);
}

TEST_CASE("transform on empty container", "[collection][transform]") {
  std::vector<int> empty;
  auto result = ctl::transform(empty, 0, [](int& acc, int x) { acc += x; });

  REQUIRE(result == 0);
}
