#include <array>
#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <string_view>
#include <type_traits>

#include "fixtures/person.hpp"

namespace {

struct Undescribed {
  int value;
};

}  // namespace

TEST_CASE("describe wrapper registers fixture", "[describe][smoke]") {
  static_assert(ctl::described_record<Person>);
  static_assert(!ctl::described_record<Undescribed>);

  REQUIRE(ctl::field_count_v<Person> == 3);
}

TEST_CASE("adapter enumerates member metadata", "[describe][smoke]") {
  Person person{"Ada", 37, {"Paris", 75000}};

  std::array<std::string_view, ctl::detail::reflection::member_count_v<Person>> names{};
  std::size_t index = 0;

  ctl::detail::reflection::for_each_member(
      person, [&](std::string_view name, auto&) { names[index++] = name; });

  REQUIRE(index == names.size());
  REQUIRE(names[0] == "name");
  REQUIRE(names[1] == "age");
  REQUIRE(names[2] == "address");
}
