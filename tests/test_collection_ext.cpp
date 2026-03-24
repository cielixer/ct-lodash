#include <array>
#include <cstddef>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>

// ============================================================================
// find
// ============================================================================

TEST_CASE("find on vector returns first match", "[collection][find]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto result = ctl::find(vec, [](int x) { return x > 3; });

  REQUIRE(result.has_value());
  REQUIRE(*result == 4);
}

TEST_CASE("find on vector returns nullopt when no match", "[collection][find]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::find(vec, [](int x) { return x > 100; });

  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("find on std::array", "[collection][find]") {
  std::array<int, 4> arr{10, 20, 30, 40};
  auto result = ctl::find(arr, [](int x) { return x == 30; });

  REQUIRE(result.has_value());
  REQUIRE(*result == 30);
}

TEST_CASE("find on std::map with (key, value)", "[collection][find]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 5}, {"c", 3}};
  auto result = ctl::find(m, [](const std::string& /*k*/, int v) { return v > 4; });

  REQUIRE(result.has_value());
  REQUIRE(result->first == "b");
  REQUIRE(result->second == 5);
}

// ============================================================================
// find_index
// ============================================================================

TEST_CASE("find_index on vector returns index of first match", "[collection][find_index]") {
  std::vector<int> vec{10, 20, 30, 40};
  auto result = ctl::find_index(vec, [](int x) { return x >= 30; });

  REQUIRE(result.has_value());
  REQUIRE(*result == 2);
}

TEST_CASE("find_index returns nullopt when no match", "[collection][find_index]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::find_index(vec, [](int x) { return x > 100; });

  REQUIRE_FALSE(result.has_value());
}

TEST_CASE("find_index on std::array", "[collection][find_index]") {
  std::array<std::string, 3> arr{"apple", "banana", "cherry"};
  auto result = ctl::find_index(arr, [](const std::string& s) { return s == "banana"; });

  REQUIRE(result.has_value());
  REQUIRE(*result == 1);
}

// ============================================================================
// every
// ============================================================================

TEST_CASE("every returns true when all match", "[collection][every]") {
  std::vector<int> vec{2, 4, 6, 8};
  REQUIRE(ctl::every(vec, [](int x) { return x % 2 == 0; }));
}

TEST_CASE("every returns false when one fails", "[collection][every]") {
  std::vector<int> vec{2, 4, 5, 8};
  REQUIRE_FALSE(ctl::every(vec, [](int x) { return x % 2 == 0; }));
}

TEST_CASE("every on empty container returns true", "[collection][every]") {
  std::vector<int> vec;
  REQUIRE(ctl::every(vec, [](int) { return false; }));
}

TEST_CASE("every on map with (key, value)", "[collection][every]") {
  std::map<std::string, int> m{{"a", 2}, {"b", 4}};
  REQUIRE(ctl::every(m, [](const std::string& /*k*/, int v) { return v % 2 == 0; }));
}

// ============================================================================
// some
// ============================================================================

TEST_CASE("some returns true when one matches", "[collection][some]") {
  std::vector<int> vec{1, 3, 5, 6};
  REQUIRE(ctl::some(vec, [](int x) { return x % 2 == 0; }));
}

TEST_CASE("some returns false when none match", "[collection][some]") {
  std::vector<int> vec{1, 3, 5, 7};
  REQUIRE_FALSE(ctl::some(vec, [](int x) { return x % 2 == 0; }));
}

TEST_CASE("some on empty container returns false", "[collection][some]") {
  std::vector<int> vec;
  REQUIRE_FALSE(ctl::some(vec, [](int) { return true; }));
}

TEST_CASE("some on map with (key, value)", "[collection][some]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 5}};
  REQUIRE(ctl::some(m, [](const std::string& /*k*/, int v) { return v > 4; }));
}

// ============================================================================
// count_by
// ============================================================================

TEST_CASE("count_by groups and counts elements", "[collection][count_by]") {
  std::vector<int> vec{1, 2, 3, 4, 5, 6};
  auto result = ctl::count_by(vec, [](int x) { return x % 2 == 0 ? "even" : "odd"; });

  REQUIRE(result.size() == 2);
  REQUIRE(result["even"] == 3);
  REQUIRE(result["odd"] == 3);
}

TEST_CASE("count_by with string length", "[collection][count_by]") {
  std::vector<std::string> vec{"a", "bb", "c", "ddd", "ee"};
  auto result = ctl::count_by(vec, [](const std::string& s) { return s.size(); });

  REQUIRE(result[1] == 2);
  REQUIRE(result[2] == 2);
  REQUIRE(result[3] == 1);
}

TEST_CASE("count_by on map", "[collection][count_by]") {
  std::map<std::string, int> m{{"a", 1}, {"b", 2}, {"c", 1}};
  auto result = ctl::count_by(m, [](const std::string& /*k*/, int v) { return v; });

  REQUIRE(result[1] == 2);
  REQUIRE(result[2] == 1);
}

// ============================================================================
// sort_by
// ============================================================================

TEST_CASE("sort_by sorts by key function", "[collection][sort_by]") {
  std::vector<std::string> vec{"banana", "apple", "cherry"};
  auto result = ctl::sort_by(vec, [](const std::string& s) { return s; });

  REQUIRE(result[0] == "apple");
  REQUIRE(result[1] == "banana");
  REQUIRE(result[2] == "cherry");
}

TEST_CASE("sort_by with custom key", "[collection][sort_by]") {
  std::vector<std::string> vec{"bb", "a", "ccc"};
  auto result = ctl::sort_by(vec, [](const std::string& s) { return s.size(); });

  REQUIRE(result[0] == "a");
  REQUIRE(result[1] == "bb");
  REQUIRE(result[2] == "ccc");
}

TEST_CASE("sort_by does not modify original", "[collection][sort_by]") {
  std::vector<int> vec{3, 1, 2};
  auto result = ctl::sort_by(vec, [](int x) { return x; });

  REQUIRE(vec[0] == 3);
  REQUIRE(result[0] == 1);
}

// ============================================================================
// zip
// ============================================================================

TEST_CASE("zip combines two vectors", "[collection][zip]") {
  std::vector<int> a{1, 2, 3};
  std::vector<std::string> b{"a", "b", "c"};
  auto result = ctl::zip(a, b);

  REQUIRE(result.size() == 3);
  REQUIRE(result[0].first == 1);
  REQUIRE(result[0].second == "a");
  REQUIRE(result[2].first == 3);
  REQUIRE(result[2].second == "c");
}

TEST_CASE("zip truncates to shorter range", "[collection][zip]") {
  std::vector<int> a{1, 2, 3, 4, 5};
  std::vector<int> b{10, 20};
  auto result = ctl::zip(a, b);

  REQUIRE(result.size() == 2);
  REQUIRE(result[0].first == 1);
  REQUIRE(result[0].second == 10);
}

TEST_CASE("zip with array and vector", "[collection][zip]") {
  std::array<int, 3> a{1, 2, 3};
  std::vector<std::string> b{"x", "y", "z"};
  auto result = ctl::zip(a, b);

  REQUIRE(result.size() == 3);
  REQUIRE(result[1].first == 2);
  REQUIRE(result[1].second == "y");
}

// ============================================================================
// chunk
// ============================================================================

TEST_CASE("chunk splits vector into groups", "[collection][chunk]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto result = ctl::chunk(vec, 2);

  REQUIRE(result.size() == 3);
  REQUIRE(result[0] == std::vector<int>{1, 2});
  REQUIRE(result[1] == std::vector<int>{3, 4});
  REQUIRE(result[2] == std::vector<int>{5});
}

TEST_CASE("chunk with exact division", "[collection][chunk]") {
  std::vector<int> vec{1, 2, 3, 4};
  auto result = ctl::chunk(vec, 2);

  REQUIRE(result.size() == 2);
  REQUIRE(result[0] == std::vector<int>{1, 2});
  REQUIRE(result[1] == std::vector<int>{3, 4});
}

TEST_CASE("chunk with size 0 returns empty", "[collection][chunk]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::chunk(vec, 0);

  REQUIRE(result.empty());
}

TEST_CASE("chunk with size larger than container", "[collection][chunk]") {
  std::vector<int> vec{1, 2};
  auto result = ctl::chunk(vec, 10);

  REQUIRE(result.size() == 1);
  REQUIRE(result[0] == std::vector<int>{1, 2});
}

// ============================================================================
// take
// ============================================================================

TEST_CASE("take returns first n elements", "[collection][take]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto result = ctl::take(vec, 3);

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

TEST_CASE("take more than size returns all", "[collection][take]") {
  std::vector<int> vec{1, 2};
  auto result = ctl::take(vec, 10);

  REQUIRE(result == std::vector<int>{1, 2});
}

TEST_CASE("take 0 returns empty", "[collection][take]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::take(vec, 0);

  REQUIRE(result.empty());
}

TEST_CASE("take on array", "[collection][take]") {
  std::array<int, 4> arr{10, 20, 30, 40};
  auto result = ctl::take(arr, 2);

  REQUIRE(result == std::vector<int>{10, 20});
}

// ============================================================================
// drop
// ============================================================================

TEST_CASE("drop skips first n elements", "[collection][drop]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  auto result = ctl::drop(vec, 2);

  REQUIRE(result == std::vector<int>{3, 4, 5});
}

TEST_CASE("drop more than size returns empty", "[collection][drop]") {
  std::vector<int> vec{1, 2};
  auto result = ctl::drop(vec, 10);

  REQUIRE(result.empty());
}

TEST_CASE("drop 0 returns all", "[collection][drop]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::drop(vec, 0);

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

TEST_CASE("drop on array", "[collection][drop]") {
  std::array<int, 4> arr{10, 20, 30, 40};
  auto result = ctl::drop(arr, 3);

  REQUIRE(result == std::vector<int>{40});
}
