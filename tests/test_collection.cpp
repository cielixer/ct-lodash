#include <array>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>

// ============================================================================
// for_each — non-pair ranges
// ============================================================================

TEST_CASE("for_each on std::vector", "[collection][for_each]") {
  std::vector<int> vec{1, 2, 3, 4};
  int sum = 0;
  ctl::for_each(vec, [&](int x) { sum += x; });
  REQUIRE(sum == 10);
}

TEST_CASE("for_each on std::array", "[collection][for_each]") {
  std::array<int, 4> arr{10, 20, 30, 40};
  int sum = 0;
  ctl::for_each(arr, [&](int x) { sum += x; });
  REQUIRE(sum == 100);
}

TEST_CASE("for_each on C array", "[collection][for_each]") {
  int arr[] = {5, 10, 15};  // NOLINT(modernize-avoid-c-arrays)
  int sum = 0;
  ctl::for_each(arr, [&](int x) { sum += x; });
  REQUIRE(sum == 30);
}

// ============================================================================
// for_each — pair ranges
// ============================================================================

TEST_CASE("for_each on std::map with (key, value) callback", "[collection][for_each]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 2}};
  std::string keys;
  int sum = 0;
  ctl::for_each(m, [&](const std::string& k, int v) {
    keys += k;
    sum += v;
  });
  REQUIRE(keys == "ab");
  REQUIRE(sum == 3);
}

TEST_CASE("for_each on std::unordered_map with pair callback", "[collection][for_each]") {
  std::unordered_map<int, int> m{{1, 10}, {2, 20}};
  int sum = 0;
  ctl::for_each(m, [&](std::pair<int, int> p) { sum += p.second; });
  REQUIRE(sum == 30);
}

// ============================================================================
// map — non-pair ranges
// ============================================================================

TEST_CASE("map on std::vector", "[collection][map]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::map(vec, [](int x) { return x * 2; });

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 2);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 6);
}

TEST_CASE("map on std::array", "[collection][map]") {
  std::array<int, 3> arr{10, 20, 30};
  auto result = ctl::map(arr, [](int x) { return x + 1; });

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 11);
  REQUIRE(result[1] == 21);
  REQUIRE(result[2] == 31);
}

TEST_CASE("map on C array", "[collection][map]") {
  int arr[] = {1, 2, 3};  // NOLINT(modernize-avoid-c-arrays)
  auto result = ctl::map(arr, [](int x) { return x * x; });

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 9);
}

TEST_CASE("map transforms element types", "[collection][map]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::map(vec, [](int x) { return std::to_string(x); });

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == "1");
  REQUIRE(result[1] == "2");
  REQUIRE(result[2] == "3");
}

// ============================================================================
// map — pair ranges
// ============================================================================

TEST_CASE("map on std::map with (key, value) callback", "[collection][map]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 3}};
  auto result = ctl::map(m, [](const std::string& k, int v) { return k + "=" + std::to_string(v); });

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == "a=1");
  REQUIRE(result[1] == "b=2");
  REQUIRE(result[2] == "c=3");
}

TEST_CASE("map on std::unordered_map extracts values", "[collection][map]") {
  std::unordered_map<std::string, int> m{{"x", 42}};
  auto result = ctl::map(m, [](const std::string& /*k*/, int v) { return v * 2; });

  REQUIRE(result.size() == 1);
  REQUIRE(result[0] == 84);
}

// ============================================================================
// filter — non-pair ranges
// ============================================================================

TEST_CASE("filter on std::vector", "[collection][filter]") {
  std::vector<int> vec{1, 2, 3, 4, 5, 6};
  auto result = ctl::filter(vec, [](int x) { return x % 2 == 0; });

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 2);
  REQUIRE(result[1] == 4);
  REQUIRE(result[2] == 6);
}

TEST_CASE("filter on std::array", "[collection][filter]") {
  std::array<int, 5> arr{10, 15, 20, 25, 30};
  auto result = ctl::filter(arr, [](int x) { return x > 20; });

  REQUIRE(result.size() == 2);
  REQUIRE(result[0] == 25);
  REQUIRE(result[1] == 30);
}

TEST_CASE("filter on C array", "[collection][filter]") {
  int arr[] = {1, 2, 3, 4, 5};  // NOLINT(modernize-avoid-c-arrays)
  auto result = ctl::filter(arr, [](int x) { return x <= 3; });

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == 1);
  REQUIRE(result[1] == 2);
  REQUIRE(result[2] == 3);
}

TEST_CASE("filter returns empty when nothing matches", "[collection][filter]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::filter(vec, [](int x) { return x > 100; });

  REQUIRE(result.empty());
}

// ============================================================================
// filter — pair ranges
// ============================================================================

TEST_CASE("filter on std::map with (key, value) predicate", "[collection][filter]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 5}, {"c", 3}};
  auto result = ctl::filter(m, [](const std::string& /*k*/, int v) { return v > 2; });

  REQUIRE(result.size() == 2);
  REQUIRE(result[0].first == "b");
  REQUIRE(result[0].second == 5);
  REQUIRE(result[1].first == "c");
  REQUIRE(result[1].second == 3);
}

TEST_CASE("filter on std::map with pair predicate", "[collection][filter]") {
  std::map<int, std::string> m{{1, "one"}, {2, "two"}, {3, "three"}};
  auto result =
      ctl::filter(m, [](std::pair<int, std::string> p) { return p.first >= 2; });

  REQUIRE(result.size() == 2);
  REQUIRE(result[0].first == 2);
  REQUIRE(result[1].first == 3);
}

// ============================================================================
// reduce — non-pair ranges
// ============================================================================

TEST_CASE("reduce on std::vector (sum)", "[collection][reduce]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto result = ctl::reduce(vec, 0, [](int acc, int x) { return acc + x; });

  REQUIRE(result == 15);
}

TEST_CASE("reduce on std::array (product)", "[collection][reduce]") {
  std::array<int, 4> arr{1, 2, 3, 4};
  auto result = ctl::reduce(arr, 1, [](int acc, int x) { return acc * x; });

  REQUIRE(result == 24);
}

TEST_CASE("reduce on C array", "[collection][reduce]") {
  int arr[] = {10, 20, 30};  // NOLINT(modernize-avoid-c-arrays)
  auto result = ctl::reduce(arr, 0, [](int acc, int x) { return acc + x; });

  REQUIRE(result == 60);
}

TEST_CASE("reduce with type conversion (string concatenation)", "[collection][reduce]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::reduce(vec, std::string{}, [](std::string acc, int x) {
    return acc + std::to_string(x);
  });

  REQUIRE(result == "123");
}

TEST_CASE("reduce on empty container returns init", "[collection][reduce]") {
  std::vector<int> vec;
  auto result = ctl::reduce(vec, 42, [](int acc, int x) { return acc + x; });

  REQUIRE(result == 42);
}

// ============================================================================
// reduce — pair ranges
// ============================================================================

TEST_CASE("reduce on std::map with (acc, key, value) callback", "[collection][reduce]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 3}};
  auto result = ctl::reduce(m, 0, [](int acc, const std::string& /*k*/, int v) {
    return acc + v;
  });

  REQUIRE(result == 6);
}

TEST_CASE("reduce on std::map with (acc, pair) callback", "[collection][reduce]") {
  std::map<std::string, int> m{{"x", 10}, {"y", 20}};
  auto result = ctl::reduce(m, std::string{},
                            [](std::string acc, std::pair<std::string, int> p) {
                              return acc + p.first + std::to_string(p.second);
                            });

  REQUIRE(result == "x10y20");
}

// ============================================================================
// Composability — chaining collection functions
// ============================================================================

TEST_CASE("map then filter (pipeline)", "[collection][compose]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto doubled = ctl::map(vec, [](int x) { return x * 2; });
  auto big = ctl::filter(doubled, [](int x) { return x > 6; });

  REQUIRE(big.size() == 2);
  REQUIRE(big[0] == 8);
  REQUIRE(big[1] == 10);
}

TEST_CASE("filter then reduce (pipeline)", "[collection][compose]") {
  std::vector<int> vec{1, 2, 3, 4, 5, 6};
  auto evens = ctl::filter(vec, [](int x) { return x % 2 == 0; });
  auto sum = ctl::reduce(evens, 0, [](int acc, int x) { return acc + x; });

  REQUIRE(sum == 12);
}

TEST_CASE("map then reduce (pipeline)", "[collection][compose]") {
  std::vector<std::string> vec{"hello", "world", "foo"};
  auto lengths = ctl::map(vec, [](const std::string& s) { return static_cast<int>(s.size()); });
  auto total = ctl::reduce(lengths, 0, [](int acc, int x) { return acc + x; });

  REQUIRE(total == 13);
}
