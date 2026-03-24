#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <string>

#include "fixtures/person.hpp"

TEST_CASE("set<field<Ptr>> mutates top-level member in-place", "[update][set][field]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  using name_field = ctl::field<&Person::name>;
  using age_field = ctl::field<&Person::age>;

  auto& result = ctl::set<name_field>(person, std::string("Bob"));

  REQUIRE(&result == &person);
  REQUIRE(person.name == "Bob");

  ctl::set<age_field>(person, 25);
  REQUIRE(person.age == 25);
}

TEST_CASE("set<path<Ptr1, Ptr2>> mutates nested member in-place", "[update][set][path]") {
  Person person{"Charlie", 35, {"Chicago", 67890}};

  using city_path = ctl::path<&Person::address, &Address::city>;
  using zip_path = ctl::path<&Person::address, &Address::zip_code>;

  auto& result = ctl::set<city_path>(person, std::string("Denver"));

  REQUIRE(&result == &person);
  REQUIRE(person.address.city == "Denver");

  ctl::set<zip_path>(person, 80202);
  REQUIRE(person.address.zip_code == 80202);
}

TEST_CASE("with<field<Ptr>> returns modified copy, leaves original unchanged",
          "[update][with][field]") {
  Person original{"Diana", 40, {"Dallas", 75201}};

  using name_field = ctl::field<&Person::name>;

  auto modified = ctl::with<name_field>(original, std::string("Eve"));

  REQUIRE(original.name == "Diana");
  REQUIRE(modified.name == "Eve");
  REQUIRE(original.age == modified.age);
  REQUIRE(original.address.city == modified.address.city);
}

TEST_CASE("with<path<Ptr1, Ptr2>> returns modified copy with nested change",
          "[update][with][path]") {
  Person original{"Frank", 50, {"Phoenix", 85001}};

  using city_path = ctl::path<&Person::address, &Address::city>;

  auto modified = ctl::with<city_path>(original, std::string("Portland"));

  REQUIRE(original.address.city == "Phoenix");
  REQUIRE(modified.address.city == "Portland");
  REQUIRE(original.name == modified.name);
  REQUIRE(original.age == modified.age);
  REQUIRE(original.address.zip_code == modified.address.zip_code);
}

TEST_CASE("with<field<Ptr>> works with const source objects", "[update][with][const]") {
  const Person original{"Grace", 45, {"Seattle", 98101}};

  using age_field = ctl::field<&Person::age>;

  auto modified = ctl::with<age_field>(original, 46);

  REQUIRE(original.age == 45);
  REQUIRE(modified.age == 46);
}

TEST_CASE("update<field<Ptr>> applies function to top-level member", "[update][update][field]") {
  Person person{"Hannah", 32, {"Austin", 73301}};

  using name_field = ctl::field<&Person::name>;
  using age_field = ctl::field<&Person::age>;

  auto& result = ctl::update<name_field>(person, [](std::string& name) { name += " Smith"; });

  REQUIRE(&result == &person);
  REQUIRE(person.name == "Hannah Smith");

  ctl::update<age_field>(person, [](int& age) { age += 1; });
  REQUIRE(person.age == 33);
}

TEST_CASE("update<path<Ptr1, Ptr2>> applies function to nested member", "[update][update][path]") {
  Person person{"Ian", 29, {"Miami", 33101}};

  using city_path = ctl::path<&Person::address, &Address::city>;
  using zip_path = ctl::path<&Person::address, &Address::zip_code>;

  auto& result = ctl::update<city_path>(person, [](std::string& city) { city = "Tampa"; });

  REQUIRE(&result == &person);
  REQUIRE(person.address.city == "Tampa");

  ctl::update<zip_path>(person, [](int& zip) { zip += 500; });
  REQUIRE(person.address.zip_code == 33601);
}

TEST_CASE("set/with/update preserve object type", "[update][type_preservation]") {
  Person person{"Jane", 27, {"Portland", 97201}};

  using name_field = ctl::field<&Person::name>;

  auto& set_result = ctl::set<name_field>(person, std::string("Janet"));
  static_assert(std::is_same_v<decltype(set_result), Person&>);

  auto with_result = ctl::with<name_field>(person, std::string("Joan"));
  static_assert(std::is_same_v<decltype(with_result), Person>);

  auto& update_result = ctl::update<name_field>(person, [](std::string& name) { name = "Julia"; });
  static_assert(std::is_same_v<decltype(update_result), Person&>);
}
