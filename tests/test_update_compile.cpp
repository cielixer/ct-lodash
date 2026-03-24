#include <ctl/ctl.hpp>
#include <string>
#include <type_traits>

#include "fixtures/person.hpp"

static_assert(ctl::described_record<Person>);

using name_field = ctl::field<&Person::name>;
using age_field = ctl::field<&Person::age>;
using city_path = ctl::path<&Person::address, &Address::city>;

Person person_instance{"Test", 25, {"City", 12345}};

auto& set_result = ctl::set<name_field>(person_instance, std::string("New"));
static_assert(std::is_same_v<decltype(set_result), Person&>);

auto with_result = ctl::with<age_field>(person_instance, 30);
static_assert(std::is_same_v<decltype(with_result), Person>);

auto& update_result =
    ctl::update<city_path>(person_instance, [](std::string& city) { city = "Updated"; });
static_assert(std::is_same_v<decltype(update_result), Person&>);

auto with_nested_result = ctl::with<city_path>(person_instance, std::string("Nested"));
static_assert(std::is_same_v<decltype(with_nested_result), Person>);
