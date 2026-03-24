#include <array>
#include <optional>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>

// ============================================================================
// head
// ============================================================================

TEST_CASE("head returns first element", "[collection][head]") {
  std::vector<int> vec{10, 20, 30};
  auto result = ctl::head(vec);

  REQUIRE(result.has_value());
  REQUIRE(*result == 10);
}

TEST_CASE("head returns nullopt for empty", "[collection][head]") {
  std::vector<int> vec;
  auto result = ctl::head(vec);

  REQUIRE_FALSE(result.has_value());
}

// ============================================================================
// last
// ============================================================================

TEST_CASE("last returns final element", "[collection][last]") {
  std::vector<int> vec{10, 20, 30};
  auto result = ctl::last(vec);

  REQUIRE(result.has_value());
  REQUIRE(*result == 30);
}

TEST_CASE("last works on std::array", "[collection][last]") {
  std::array<int, 4> arr{1, 2, 3, 4};
  auto result = ctl::last(arr);

  REQUIRE(result.has_value());
  REQUIRE(*result == 4);
}

// ============================================================================
// tail
// ============================================================================

TEST_CASE("tail drops first element", "[collection][tail]") {
  std::vector<int> vec{1, 2, 3, 4};
  auto result = ctl::tail(vec);

  REQUIRE(result == std::vector<int>{2, 3, 4});
}

TEST_CASE("tail of empty is empty", "[collection][tail]") {
  std::vector<int> vec;
  auto result = ctl::tail(vec);

  REQUIRE(result.empty());
}

// ============================================================================
// initial
// ============================================================================

TEST_CASE("initial drops last element", "[collection][initial]") {
  std::vector<int> vec{1, 2, 3, 4};
  auto result = ctl::initial(vec);

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

TEST_CASE("initial of single element is empty", "[collection][initial]") {
  std::vector<int> vec{42};
  auto result = ctl::initial(vec);

  REQUIRE(result.empty());
}

// ============================================================================
// size
// ============================================================================

TEST_CASE("size returns element count", "[collection][size]") {
  std::vector<int> vec{1, 2, 3, 4, 5};
  REQUIRE(ctl::size(vec) == 5);
}

TEST_CASE("size returns zero for empty", "[collection][size]") {
  std::vector<int> vec;
  REQUIRE(ctl::size(vec) == 0);
}

// ============================================================================
// reverse
// ============================================================================

TEST_CASE("reverse returns reversed order", "[collection][reverse]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::reverse(vec);

  REQUIRE(result == std::vector<int>{3, 2, 1});
}

TEST_CASE("reverse empty stays empty", "[collection][reverse]") {
  std::vector<int> vec;
  auto result = ctl::reverse(vec);

  REQUIRE(result.empty());
}

// ============================================================================
// includes
// ============================================================================

TEST_CASE("includes returns true when present", "[collection][includes]") {
  std::vector<int> vec{1, 3, 5};
  REQUIRE(ctl::includes(vec, 3));
}

TEST_CASE("includes returns false when absent", "[collection][includes]") {
  std::vector<int> vec{1, 3, 5};
  REQUIRE_FALSE(ctl::includes(vec, 2));
}

// ============================================================================
// compact
// ============================================================================

TEST_CASE("compact removes numeric zeros", "[collection][compact]") {
  std::vector<int> vec{0, 1, 0, 2, 3, 0};
  auto result = ctl::compact(vec);

  REQUIRE(result == std::vector<int>{1, 2, 3});
}

TEST_CASE("compact removes empty optionals", "[collection][compact]") {
  std::vector<std::optional<int>> vec{std::nullopt, 1, std::nullopt, 2};
  auto result = ctl::compact(vec);

  REQUIRE(result.size() == 2);
  REQUIRE(result[0].has_value());
  REQUIRE(*result[0] == 1);
  REQUIRE(result[1].has_value());
  REQUIRE(*result[1] == 2);
}
