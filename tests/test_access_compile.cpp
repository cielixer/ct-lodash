#include <concepts>
#include <ctl/ctl.hpp>
#include <string>
#include <type_traits>

#include "fixtures/person.hpp"

static_assert(ctl::field_count_v<Person> == 3);
static_assert(ctl::field_count_v<Address> == 2);

static_assert(std::same_as<decltype(ctl::get<0>(std::declval<Person&>())), std::string&>);
static_assert(std::same_as<decltype(ctl::get<1>(std::declval<Person&>())), int&>);
static_assert(std::same_as<decltype(ctl::get<2>(std::declval<Person&>())), Address&>);

static_assert(
    std::same_as<decltype(ctl::get<0>(std::declval<const Person&>())), const std::string&>);
static_assert(std::same_as<decltype(ctl::get<1>(std::declval<const Person&>())), const int&>);
static_assert(std::same_as<decltype(ctl::get<2>(std::declval<const Person&>())), const Address&>);

using name_field = ctl::field<&Person::name>;
using age_field = ctl::field<&Person::age>;
using address_field = ctl::field<&Person::address>;

static_assert(std::same_as<decltype(ctl::get<name_field>(std::declval<Person&>())), std::string&>);
static_assert(std::same_as<decltype(ctl::get<age_field>(std::declval<Person&>())), int&>);
static_assert(std::same_as<decltype(ctl::get<address_field>(std::declval<Person&>())), Address&>);

static_assert(std::same_as<decltype(ctl::get<name_field>(std::declval<const Person&>())),
                           const std::string&>);
static_assert(
    std::same_as<decltype(ctl::get<age_field>(std::declval<const Person&>())), const int&>);
static_assert(
    std::same_as<decltype(ctl::get<address_field>(std::declval<const Person&>())), const Address&>);

using city_path = ctl::path<&Person::address, &Address::city>;
using zip_path = ctl::path<&Person::address, &Address::zip_code>;

static_assert(std::same_as<decltype(ctl::get<city_path>(std::declval<Person&>())), std::string&>);
static_assert(std::same_as<decltype(ctl::get<zip_path>(std::declval<Person&>())), int&>);

static_assert(
    std::same_as<decltype(ctl::get<city_path>(std::declval<const Person&>())), const std::string&>);
static_assert(
    std::same_as<decltype(ctl::get<zip_path>(std::declval<const Person&>())), const int&>);
