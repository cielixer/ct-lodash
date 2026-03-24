#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <string>
#include <string_view>
#include <tuple>

#include "fixtures/person.hpp"

TEST_CASE("get<\"name\"> returns flat field by string", "[string_access][get]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  REQUIRE(ctl::get<"name">(person) == "Alice");
  REQUIRE(ctl::get<"age">(person) == 30);
  REQUIRE(ctl::get<"address">(person).city == "Boston");
}

TEST_CASE("get<\"address\", \"city\"> returns nested field by string chain",
          "[string_access][get]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  REQUIRE(ctl::get<"address", "city">(person) == "Boston");
  REQUIRE(ctl::get<"address", "zip_code">(person) == 12345);
}

TEST_CASE("get<\"name\"> returns mutable reference", "[string_access][get]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  ctl::get<"name">(person) = "Bob";
  REQUIRE(person.name == "Bob");

  ctl::get<"address", "city">(person) = "Paris";
  REQUIRE(person.address.city == "Paris");
}

TEST_CASE("get<\"name\"> preserves const on const object", "[string_access][get]") {
  const Person person{"Alice", 30, {"Boston", 12345}};

  REQUIRE(ctl::get<"name">(person) == "Alice");
  static_assert(std::is_const_v<std::remove_reference_t<decltype(ctl::get<"name">(person))>>);
  static_assert(
      std::is_const_v<
          std::remove_reference_t<decltype(ctl::get<"address", "city">(person))>>);
}

TEST_CASE("set<\"name\"> mutates flat field in-place", "[string_access][set]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  ctl::set<"name">(person, "Bob");
  REQUIRE(person.name == "Bob");
}

TEST_CASE("set<\"address\", \"city\"> mutates nested field in-place",
          "[string_access][set]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  ctl::set<"address", "city">(person, "Paris");
  REQUIRE(person.address.city == "Paris");
}

TEST_CASE("with<\"name\"> returns modified copy", "[string_access][with]") {
  Person alice{"Alice", 30, {"Boston", 12345}};

  Person bob = ctl::with<"name">(alice, "Bob");
  REQUIRE(bob.name == "Bob");
  REQUIRE(alice.name == "Alice");
}

TEST_CASE("with<\"address\", \"city\"> returns copy with nested field changed",
          "[string_access][with]") {
  Person alice{"Alice", 30, {"Boston", 12345}};

  Person moved = ctl::with<"address", "city">(alice, "Paris");
  REQUIRE(moved.address.city == "Paris");
  REQUIRE(alice.address.city == "Boston");
}

TEST_CASE("update<\"age\"> applies function to field", "[string_access][update]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  ctl::update<"age">(person, [](int& a) { a += 1; });
  REQUIRE(person.age == 31);
}

TEST_CASE("update<\"address\", \"zip_code\"> applies function to nested field",
          "[string_access][update]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  ctl::update<"address", "zip_code">(person, [](int& z) { z = 99999; });
  REQUIRE(person.address.zip_code == 99999);
}

TEST_CASE("set<\"name\"> returns reference to original object", "[string_access][set]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  Person& ref = ctl::set<"name">(person, "Bob");
  REQUIRE(&ref == &person);
}

TEST_CASE("update<\"name\"> returns reference to original object",
          "[string_access][update]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  Person& ref = ctl::update<"name">(person, [](std::string& n) { n = "Bob"; });
  REQUIRE(&ref == &person);
}

TEST_CASE("has<\"name\"> returns true for existing field", "[has]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  REQUIRE(ctl::has<"name">(person));
  REQUIRE(ctl::has<"age">(person));
  REQUIRE(ctl::has<"address">(person));
}

TEST_CASE("has<\"missing\"> returns false for non-existent field", "[has]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  REQUIRE_FALSE(ctl::has<"missing">(person));
  REQUIRE_FALSE(ctl::has<"email">(person));
}

TEST_CASE("has<\"address\", \"city\"> checks nested path", "[has]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  REQUIRE(ctl::has<"address", "city">(person));
  REQUIRE(ctl::has<"address", "zip_code">(person));
  REQUIRE_FALSE(ctl::has<"address", "country">(person));
}

TEST_CASE("has<Path> works with path types", "[has]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  using CityPath = ctl::path<&Person::address, &Address::city>;
  REQUIRE(ctl::has<CityPath>(person));
  REQUIRE(ctl::has<ctl::field<&Person::name>>(person));
}

TEST_CASE("has is constexpr", "[has]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  static_assert(ctl::has<"name">(person));
  static_assert(!ctl::has<"missing">(person));
  static_assert(ctl::has<"address", "city">(person));
}

TEST_CASE("keys<Person>() returns field names", "[keys]") {
  constexpr auto k = ctl::keys<Person>();

  REQUIRE(k.size() == 3);
  REQUIRE(k[0] == "name");
  REQUIRE(k[1] == "age");
  REQUIRE(k[2] == "address");
}

TEST_CASE("keys<Address>() returns field names", "[keys]") {
  constexpr auto k = ctl::keys<Address>();

  REQUIRE(k.size() == 2);
  REQUIRE(k[0] == "city");
  REQUIRE(k[1] == "zip_code");
}

TEST_CASE("keys is consteval", "[keys]") {
  static_assert(ctl::keys<Person>().size() == 3);
  static_assert(ctl::keys<Person>()[0] == "name");
}

TEST_CASE("values(obj) returns tuple of all field values", "[values]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  auto vals = ctl::values(person);

  REQUIRE(std::get<0>(vals) == "Alice");
  REQUIRE(std::get<1>(vals) == 30);
  REQUIRE(std::get<2>(vals).city == "Boston");
}

TEST_CASE("values(obj) returns copies", "[values]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  auto vals = ctl::values(person);
  std::get<0>(vals) = "Modified";
  REQUIRE(person.name == "Alice");
}

TEST_CASE("values on const object", "[values]") {
  const Person person{"Alice", 30, {"Boston", 12345}};

  auto vals = ctl::values(person);
  REQUIRE(std::get<0>(vals) == "Alice");
  REQUIRE(std::tuple_size_v<decltype(vals)> == 3);
}
