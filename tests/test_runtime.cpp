#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <string>

#include "fixtures/person.hpp"

TEST_CASE("runtime::visit - success cases", "[runtime]") {
  Person p{"Alice", 30, {"NYC", 10001}};

  SECTION("visit existing string field") {
    std::string captured;
    bool found = ctl::runtime::visit(p, "name", [&](auto& field) {
      if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field)>, std::string>) {
        captured = field;
      }
    });

    REQUIRE(found);
    REQUIRE(captured == "Alice");
  }

  SECTION("visit existing int field") {
    int captured = 0;
    bool found = ctl::runtime::visit(p, "age", [&](auto& field) {
      if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field)>, int>) {
        captured = field;
      }
    });

    REQUIRE(found);
    REQUIRE(captured == 30);
  }

  SECTION("visit existing nested struct field") {
    std::string captured_city;
    bool found = ctl::runtime::visit(p, "address", [&](auto& field) {
      if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field)>, Address>) {
        captured_city = field.city;
      }
    });

    REQUIRE(found);
    REQUIRE(captured_city == "NYC");
  }

  SECTION("visitor can mutate field") {
    bool found = ctl::runtime::visit(p, "age", [&](auto& field) {
      if constexpr (std::is_same_v<std::remove_cvref_t<decltype(field)>, int>) {
        field = 31;
      }
    });

    REQUIRE(found);
    REQUIRE(p.age == 31);
  }
}

TEST_CASE("runtime::try_get - success cases", "[runtime]") {
  Person p{"Bob", 25, {"LA", 90001}};

  SECTION("get existing string field with correct type") {
    auto* name_ptr = ctl::runtime::try_get<std::string>(p, "name");

    REQUIRE(name_ptr != nullptr);
    REQUIRE(*name_ptr == "Bob");
  }

  SECTION("get existing int field with correct type") {
    auto* age_ptr = ctl::runtime::try_get<int>(p, "age");

    REQUIRE(age_ptr != nullptr);
    REQUIRE(*age_ptr == 25);
  }

  SECTION("get existing nested struct with correct type") {
    auto* addr_ptr = ctl::runtime::try_get<Address>(p, "address");

    REQUIRE(addr_ptr != nullptr);
    REQUIRE(addr_ptr->city == "LA");
    REQUIRE(addr_ptr->zip_code == 90001);
  }

  SECTION("const object returns const pointer") {
    const Person& cp = p;
    auto* name_ptr = ctl::runtime::try_get<std::string>(cp, "name");

    REQUIRE(name_ptr != nullptr);
    REQUIRE(*name_ptr == "Bob");

    static_assert(std::is_same_v<decltype(name_ptr), const std::string*>);
  }

  SECTION("pointer can be used to mutate field") {
    auto* age_ptr = ctl::runtime::try_get<int>(p, "age");
    REQUIRE(age_ptr != nullptr);

    *age_ptr = 26;
    REQUIRE(p.age == 26);
  }
}

TEST_CASE("runtime::try_update - success cases", "[runtime]") {
  Person p{"Charlie", 35, {"SF", 94102}};

  SECTION("update existing string field with correct type") {
    bool updated = ctl::runtime::try_update<std::string>(p, "name", "Charles");

    REQUIRE(updated);
    REQUIRE(p.name == "Charles");
  }

  SECTION("update existing int field with correct type") {
    bool updated = ctl::runtime::try_update<int>(p, "age", 36);

    REQUIRE(updated);
    REQUIRE(p.age == 36);
  }

  SECTION("update existing nested struct with correct type") {
    Address new_addr{"Seattle", 98101};
    bool updated = ctl::runtime::try_update<Address>(p, "address", new_addr);

    REQUIRE(updated);
    REQUIRE(p.address.city == "Seattle");
    REQUIRE(p.address.zip_code == 98101);
  }

  SECTION("can update with convertible type") {
    bool updated = ctl::runtime::try_update<std::string>(p, "name", std::string("Dave"));

    REQUIRE(updated);
    REQUIRE(p.name == "Dave");
  }
}
