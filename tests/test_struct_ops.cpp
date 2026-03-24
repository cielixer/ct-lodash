#include <string>
#include <tuple>

#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>

#include "fixtures/person.hpp"

TEST_CASE("is_equal returns true for identical structs", "[struct_ops][is_equal]") {
  Person a{"Alice", 30, {"Boston", 12345}};
  Person b{"Alice", 30, {"Boston", 12345}};

  REQUIRE(ctl::is_equal(a, b));
}

TEST_CASE("is_equal returns false when fields differ", "[struct_ops][is_equal]") {
  Person a{"Alice", 30, {"Boston", 12345}};
  Person b{"Alice", 31, {"Boston", 12345}};

  REQUIRE_FALSE(ctl::is_equal(a, b));
}

TEST_CASE("is_equal detects nested field difference", "[struct_ops][is_equal]") {
  Person a{"Alice", 30, {"Boston", 12345}};
  Person b{"Alice", 30, {"Paris", 12345}};

  REQUIRE_FALSE(ctl::is_equal(a, b));
}

TEST_CASE("is_equal works on Address directly", "[struct_ops][is_equal]") {
  Address a{"Boston", 12345};
  Address b{"Boston", 12345};
  Address c{"London", 99999};

  REQUIRE(ctl::is_equal(a, b));
  REQUIRE_FALSE(ctl::is_equal(a, c));
}

TEST_CASE("clone produces independent copy", "[struct_ops][clone]") {
  Person original{"Alice", 30, {"Boston", 12345}};
  auto copy = ctl::clone(original);

  REQUIRE(copy.name == "Alice");
  REQUIRE(copy.age == 30);
  REQUIRE(copy.address.city == "Boston");

  copy.name = "Bob";
  REQUIRE(original.name == "Alice");
}

TEST_CASE("clone preserves all fields", "[struct_ops][clone]") {
  Address addr{"Tokyo", 54321};
  auto copy = ctl::clone(addr);

  REQUIRE(copy.city == "Tokyo");
  REQUIRE(copy.zip_code == 54321);
}

TEST_CASE("to_tuple converts struct to tuple", "[struct_ops][to_tuple]") {
  Person person{"Alice", 30, {"Boston", 12345}};
  auto tup = ctl::to_tuple(person);

  REQUIRE(std::get<0>(tup) == "Alice");
  REQUIRE(std::get<1>(tup) == 30);
  REQUIRE(std::get<2>(tup).city == "Boston");
}

TEST_CASE("to_tuple returns copies", "[struct_ops][to_tuple]") {
  Person person{"Alice", 30, {"Boston", 12345}};
  auto tup = ctl::to_tuple(person);

  std::get<0>(tup) = "Modified";
  REQUIRE(person.name == "Alice");
}

TEST_CASE("to_tuple on Address", "[struct_ops][to_tuple]") {
  Address addr{"London", 99999};
  auto tup = ctl::to_tuple(addr);

  REQUIRE(std::tuple_size_v<decltype(tup)> == 2);
  REQUIRE(std::get<0>(tup) == "London");
  REQUIRE(std::get<1>(tup) == 99999);
}

TEST_CASE("from_tuple constructs struct from tuple", "[struct_ops][from_tuple]") {
  auto tup = std::make_tuple(std::string{"Bob"}, 25, Address{"Paris", 75000});
  auto person = ctl::from_tuple<Person>(tup);

  REQUIRE(person.name == "Bob");
  REQUIRE(person.age == 25);
  REQUIRE(person.address.city == "Paris");
  REQUIRE(person.address.zip_code == 75000);
}

TEST_CASE("from_tuple on Address", "[struct_ops][from_tuple]") {
  auto tup = std::make_tuple(std::string{"Berlin"}, 10115);
  auto addr = ctl::from_tuple<Address>(tup);

  REQUIRE(addr.city == "Berlin");
  REQUIRE(addr.zip_code == 10115);
}

TEST_CASE("to_tuple and from_tuple roundtrip", "[struct_ops][roundtrip]") {
  Person original{"Alice", 30, {"Boston", 12345}};
  auto reconstructed = ctl::from_tuple<Person>(ctl::to_tuple(original));

  REQUIRE(ctl::is_equal(original, reconstructed));
}
