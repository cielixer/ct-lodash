#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <string>

#include "fixtures/person.hpp"

TEST_CASE("runtime::visit - failure cases", "[runtime]") {
  Person p{"Alice", 30, {"NYC", 10001}};

  SECTION("unknown field name returns false") {
    int call_count = 0;
    bool found = ctl::runtime::visit(p, "unknown_field", [&](auto&) { ++call_count; });

    REQUIRE_FALSE(found);
    REQUIRE(call_count == 0);
  }

  SECTION("visitor not called for unknown field") {
    std::string captured = "original";
    bool found = ctl::runtime::visit(p, "nonexistent", [&](auto&) { captured = "modified"; });

    REQUIRE_FALSE(found);
    REQUIRE(captured == "original");
  }
}

TEST_CASE("runtime::try_get - failure cases", "[runtime]") {
  Person p{"Bob", 25, {"LA", 90001}};

  SECTION("unknown field name returns nullptr") {
    auto* ptr = ctl::runtime::try_get<std::string>(p, "unknown_field");
    REQUIRE(ptr == nullptr);
  }

  SECTION("wrong type for field returns nullptr - int for string field") {
    auto* ptr = ctl::runtime::try_get<int>(p, "name");
    REQUIRE(ptr == nullptr);
  }

  SECTION("wrong type for field returns nullptr - string for int field") {
    auto* ptr = ctl::runtime::try_get<std::string>(p, "age");
    REQUIRE(ptr == nullptr);
  }

  SECTION("wrong type for nested struct returns nullptr") {
    auto* ptr = ctl::runtime::try_get<int>(p, "address");
    REQUIRE(ptr == nullptr);
  }

  SECTION("incompatible type returns nullptr") {
    auto* ptr = ctl::runtime::try_get<double>(p, "age");
    REQUIRE(ptr == nullptr);
  }
}

TEST_CASE("runtime::try_update - failure cases", "[runtime]") {
  Person p{"Charlie", 35, {"SF", 94102}};

  SECTION("unknown field name returns false and object unchanged") {
    Person original = p;
    bool updated = ctl::runtime::try_update<std::string>(p, "unknown_field", "NewValue");

    REQUIRE_FALSE(updated);
    REQUIRE(p.name == original.name);
    REQUIRE(p.age == original.age);
  }

  SECTION("wrong type for field returns false - int for string field") {
    std::string original_name = p.name;
    bool updated = ctl::runtime::try_update<int>(p, "name", 999);

    REQUIRE_FALSE(updated);
    REQUIRE(p.name == original_name);
  }

  SECTION("wrong type for field returns false - string for int field") {
    int original_age = p.age;
    bool updated = ctl::runtime::try_update<std::string>(p, "age", "not_a_number");

    REQUIRE_FALSE(updated);
    REQUIRE(p.age == original_age);
  }

  SECTION("incompatible type returns false") {
    int original_age = p.age;
    bool updated = ctl::runtime::try_update<double>(p, "age", 3.14);

    REQUIRE_FALSE(updated);
    REQUIRE(p.age == original_age);
  }

  SECTION("no exceptions thrown on failure") {
    bool r1 = false;
    bool r2 = false;
    REQUIRE_NOTHROW(r1 = ctl::runtime::try_update<std::string>(p, "nonexistent", "value"));
    REQUIRE_NOTHROW(r2 = ctl::runtime::try_update<int>(p, "name", 42));
    REQUIRE_FALSE(r1);
    REQUIRE_FALSE(r2);
  }
}
