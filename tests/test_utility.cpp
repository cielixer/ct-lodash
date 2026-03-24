#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <string>
#include <vector>

TEST_CASE("identity returns value unchanged", "[utility][identity]") {
  REQUIRE(ctl::identity(42) == 42);
  REQUIRE(ctl::identity(std::string{"hello"}) == "hello");
}

TEST_CASE("identity works with map", "[utility][identity]") {
  std::vector<int> vec{1, 2, 3};
  auto result = ctl::map(vec, ctl::identity);

  REQUIRE(result == vec);
}

TEST_CASE("constant returns captured value regardless of input", "[utility][constant]") {
  auto always_42 = ctl::constant(42);

  REQUIRE(always_42() == 42);
  REQUIRE(always_42(1, 2, 3) == 42);
  REQUIRE(always_42("anything") == 42);
}

TEST_CASE("constant works with string", "[utility][constant]") {
  auto always_hello = ctl::constant(std::string{"hello"});

  REQUIRE(always_hello() == "hello");
  REQUIRE(always_hello(999) == "hello");
}

TEST_CASE("tap calls function and returns object", "[utility][tap]") {
  int x = 10;
  bool called = false;

  int& ref = ctl::tap(x, [&](int val) {
    called = true;
    REQUIRE(val == 10);
  });

  REQUIRE(called);
  REQUIRE(&ref == &x);
}

TEST_CASE("tap on const object", "[utility][tap]") {
  const int x = 42;
  bool called = false;

  const int& ref = ctl::tap(x, [&](int val) {
    called = true;
    REQUIRE(val == 42);
  });

  REQUIRE(called);
  REQUIRE(&ref == &x);
}

TEST_CASE("tap allows mutation via reference", "[utility][tap]") {
  std::vector<int> vec{1, 2, 3};

  auto& ref = ctl::tap(vec, [](std::vector<int>& v) { v.push_back(4); });

  REQUIRE(vec.size() == 4);
  REQUIRE(vec.back() == 4);
  REQUIRE(&ref == &vec);
}
