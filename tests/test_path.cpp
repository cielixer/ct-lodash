#include <concepts>
#include <ctl/ctl.hpp>
#include <string>

#include "fixtures/person.hpp"

using name_field_t = ctl::field<&Person::name>;
using city_path_t = ctl::path<&Person::address, &Address::city>;
using invalid_chain_t = ctl::path<&Person::name, &Address::city>;

static_assert(ctl::path_applicable<Person&, name_field_t>);
static_assert(ctl::path_applicable<const Person&, name_field_t>);
static_assert(ctl::path_applicable<Person&&, name_field_t>);

static_assert(ctl::path_applicable<Person&, city_path_t>);
static_assert(ctl::path_applicable<const Person&, city_path_t>);
static_assert(ctl::path_applicable<Person&&, city_path_t>);

static_assert(!ctl::path_applicable<Person&, invalid_chain_t>);

static_assert(std::same_as<ctl::path_value_t<Person&, name_field_t>, std::string&>);
static_assert(std::same_as<ctl::path_value_t<const Person&, name_field_t>, const std::string&>);
static_assert(std::same_as<ctl::path_value_t<Person&&, name_field_t>, std::string&&>);

static_assert(std::same_as<ctl::path_value_t<Person&, city_path_t>, std::string&>);
static_assert(std::same_as<ctl::path_value_t<const Person&, city_path_t>, const std::string&>);
static_assert(std::same_as<ctl::path_value_t<Person&&, city_path_t>, std::string&&>);
