// Runtime smoke test using Catch2 v3
// Verifies basic test harness works

#include <catch2/catch_test_macros.hpp>

TEST_CASE("Smoke test: test harness is operational", "[smoke]") {
  // Basic assertion to verify Catch2 integration
  REQUIRE(true);

  // Simple arithmetic to ensure compiler optimization doesn't eliminate test
  int x = 1 + 1;
  REQUIRE(x == 2);
}

TEST_CASE("Smoke test: C++23 features available", "[smoke][c++23]") {
  // Verify constexpr works at compile time
  constexpr int compile_time_value = 42;
  REQUIRE(compile_time_value == 42);

  // Test that we can use auto in template parameter lists (C++20+)
  auto lambda = []<typename T>(T value) { return value; };
  REQUIRE(lambda(123) == 123);
}
