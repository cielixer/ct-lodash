#include <cstddef>
#include <optional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>

TEST_CASE("negate inverts predicate", "[utility][negate]") {
  auto is_even = [](int x) { return x % 2 == 0; };
  auto is_odd = ctl::negate(is_even);

  REQUIRE(is_odd(3));
  REQUIRE_FALSE(is_odd(4));
}

TEST_CASE("noop accepts arbitrary arguments", "[utility][noop]") {
  STATIC_REQUIRE(std::is_void_v<decltype(ctl::noop())>);
  STATIC_REQUIRE(std::is_void_v<decltype(ctl::noop(1, "x", 3.14))>);

  ctl::noop();
  ctl::noop(1, "x", 3.14);
  SUCCEED();
}

TEST_CASE("times generates values from indices", "[utility][times]") {
  auto values = ctl::times(5, [](std::size_t i) { return static_cast<int>(i * 2); });

  REQUIRE(values == std::vector<int>{0, 2, 4, 6, 8});
}

TEST_CASE("range supports all overloads", "[utility][range]") {
  REQUIRE(ctl::range(5) == std::vector<int>{0, 1, 2, 3, 4});
  REQUIRE(ctl::range(2, 5) == std::vector<int>{2, 3, 4});
  REQUIRE(ctl::range(0, 10, 3) == std::vector<int>{0, 3, 6, 9});
  REQUIRE(ctl::range(5, 0, -1) == std::vector<int>{5, 4, 3, 2, 1});
}

TEST_CASE("pipe composes left to right", "[utility][pipe]") {
  auto fn = ctl::pipe([](int x) { return x + 1; }, [](int x) { return x * 2; });

  REQUIRE(fn(3) == 8);
}

TEST_CASE("compose composes right to left", "[utility][compose]") {
  auto fn = ctl::compose([](int x) { return x + 1; }, [](int x) { return x * 2; });

  REQUIRE(fn(3) == 7);
}

TEST_CASE("cond returns first matching transform", "[utility][cond]") {
  auto fn = ctl::cond(
      std::pair{[](int x) { return x < 0; }, [](int) { return std::string{"negative"}; }},
      std::pair{[](int x) { return x > 0; }, [](int) { return std::string{"positive"}; }});

  auto positive = fn(5);
  REQUIRE(positive.has_value());
  REQUIRE(*positive == "positive");
  REQUIRE(fn(0) == std::nullopt);
}

TEST_CASE("thru returns transformed value", "[utility][thru]") {
  REQUIRE(ctl::thru(5, [](int x) { return x * 2; }) == 10);
}
