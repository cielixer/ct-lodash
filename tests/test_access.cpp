#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <string>

#include "fixtures/person.hpp"

TEST_CASE("field_count_v returns correct count", "[access][field_count]") {
  REQUIRE(ctl::field_count_v<Person> == 3);
  REQUIRE(ctl::field_count_v<Address> == 2);
}

TEST_CASE("get<I> returns correct field by index on mutable object", "[access][get][positional]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  REQUIRE(ctl::get<0>(person) == "Alice");
  REQUIRE(ctl::get<1>(person) == 30);
  REQUIRE(ctl::get<2>(person).city == "Boston");

  ctl::get<0>(person) = "Bob";
  REQUIRE(person.name == "Bob");

  ctl::get<1>(person) = 25;
  REQUIRE(person.age == 25);
}

TEST_CASE("get<I> returns const reference on const object", "[access][get][positional]") {
  const Person person{"Charlie", 40, {"Chicago", 67890}};

  REQUIRE(ctl::get<0>(person) == "Charlie");
  REQUIRE(ctl::get<1>(person) == 40);
  REQUIRE(ctl::get<2>(person).city == "Chicago");

  static_assert(std::is_const_v<std::remove_reference_t<decltype(ctl::get<0>(person))>>);
  static_assert(std::is_const_v<std::remove_reference_t<decltype(ctl::get<1>(person))>>);
}

TEST_CASE("get<field<Ptr>> returns correct member by compile-time path", "[access][get][path]") {
  Person person{"Diana", 35, {"Denver", 80202}};

  using name_field = ctl::field<&Person::name>;
  using age_field = ctl::field<&Person::age>;
  using address_field = ctl::field<&Person::address>;

  REQUIRE(ctl::get<name_field>(person) == "Diana");
  REQUIRE(ctl::get<age_field>(person) == 35);
  REQUIRE(ctl::get<address_field>(person).city == "Denver");

  ctl::get<name_field>(person) = "Eve";
  REQUIRE(person.name == "Eve");

  ctl::get<age_field>(person) = 28;
  REQUIRE(person.age == 28);
}

TEST_CASE("get<path<Ptr1, Ptr2>> returns nested member", "[access][get][path]") {
  Person person{"Frank", 50, {"Phoenix", 85001}};

  using city_path = ctl::path<&Person::address, &Address::city>;
  using zip_path = ctl::path<&Person::address, &Address::zip_code>;

  REQUIRE(ctl::get<city_path>(person) == "Phoenix");
  REQUIRE(ctl::get<zip_path>(person) == 85001);

  ctl::get<city_path>(person) = "Portland";
  REQUIRE(person.address.city == "Portland");

  ctl::get<zip_path>(person) = 97201;
  REQUIRE(person.address.zip_code == 97201);
}

TEST_CASE("get<path> preserves const qualification", "[access][get][path]") {
  const Person person{"Grace", 45, {"Seattle", 98101}};

  using city_path = ctl::path<&Person::address, &Address::city>;

  REQUIRE(ctl::get<city_path>(person) == "Seattle");

  static_assert(std::is_const_v<std::remove_reference_t<decltype(ctl::get<city_path>(person))>>);
}

TEST_CASE("for_each_field iterates all described members", "[access][for_each_field]") {
  Person person{"Hannah", 32, {"Austin", 73301}};

  int field_count = 0;
  ctl::for_each_field(person, [&](std::string_view name, auto& value) {
    using value_type = std::remove_cvref_t<decltype(value)>;

    ++field_count;

    if (name == "name") {
      if constexpr (std::is_same_v<value_type, std::string>) {
        REQUIRE(value == "Hannah");
      }
    } else if (name == "age") {
      if constexpr (std::is_same_v<value_type, int>) {
        REQUIRE(value == 32);
      }
    } else if (name == "address") {
      if constexpr (std::is_same_v<value_type, Address>) {
        REQUIRE(value.city == "Austin");
      }
    }
  });

  REQUIRE(field_count == 3);
}

TEST_CASE("for_each_field allows mutation", "[access][for_each_field]") {
  Person person{"Ian", 29, {"Miami", 33101}};

  ctl::for_each_field(person, [](std::string_view, auto& value) {
    using value_type = std::remove_cvref_t<decltype(value)>;

    if constexpr (std::is_same_v<value_type, std::string>) {
      value = "Modified";
    } else if constexpr (std::is_same_v<value_type, int>) {
      value = 99;
    } else if constexpr (std::is_same_v<value_type, Address>) {
      value.city = "Modified";
    }
  });

  REQUIRE(person.name == "Modified");
  REQUIRE(person.age == 99);
  REQUIRE(person.address.city == "Modified");
}

TEST_CASE("for_each_field preserves const on const object", "[access][for_each_field]") {
  const Person person{"Jane", 27, {"Tampa", 33601}};

  ctl::for_each_field(person, [](std::string_view, auto& value) {
    static_assert(std::is_const_v<std::remove_reference_t<decltype(value)>>);
  });
}
