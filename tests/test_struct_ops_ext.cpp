#include <any>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>

#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>

#include "fixtures/person.hpp"

TEST_CASE("map_values transforms all fields", "[struct_ops][map_values]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  auto mapped = ctl::map_values(person, [](std::string_view, const auto& value) {
    using value_t = std::remove_cvref_t<decltype(value)>;
    if constexpr (std::is_same_v<value_t, std::string>) {
      return value;
    } else if constexpr (std::is_same_v<value_t, int>) {
      return std::to_string(value);
    } else {
      return value.city + ":" + std::to_string(value.zip_code);
    }
  });

  REQUIRE(std::get<0>(mapped) == "Alice");
  REQUIRE(std::get<1>(mapped) == "30");
  REQUIRE(std::get<2>(mapped) == "Boston:12345");
}

TEST_CASE("to_pairs exports field names and any values", "[struct_ops][to_pairs]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  auto pairs = ctl::to_pairs(person);

  REQUIRE(pairs.size() == 3);
  REQUIRE(pairs[0].first == "name");
  REQUIRE(pairs[1].first == "age");
  REQUIRE(pairs[2].first == "address");
  REQUIRE(std::any_cast<int>(pairs[1].second) == 30);
}

TEST_CASE("find_key returns first matching field", "[struct_ops][find_key]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  auto key = ctl::find_key(person, [](const auto& value) {
    using value_t = std::remove_cvref_t<decltype(value)>;
    if constexpr (std::is_integral_v<value_t>) {
      return value > 20;
    }
    return false;
  });

  REQUIRE(key.has_value());
  REQUIRE(*key == "age");
}

TEST_CASE("find_key returns nullopt when no values match", "[struct_ops][find_key]") {
  Person person{"Alice", 30, {"Boston", 12345}};

  auto key = ctl::find_key(person, [](const auto& value) {
    using value_t = std::remove_cvref_t<decltype(value)>;
    if constexpr (std::is_integral_v<value_t>) {
      return value < 0;
    }
    return false;
  });

  REQUIRE_FALSE(key.has_value());
  REQUIRE(key == std::nullopt);
}

TEST_CASE("matches builds predicate from pattern struct", "[struct_ops][matches]") {
  auto matcher = ctl::matches(Person{"Alice", 30, {"Boston", 12345}});

  REQUIRE(matcher(Person{"Alice", 30, {"Boston", 12345}}));
  REQUIRE_FALSE(matcher(Person{"Alice", 31, {"Boston", 12345}}));
}

// ============================================================================
// pick
// ============================================================================

TEST_CASE("pick extracts single field by name", "[struct_ops][pick]") {
  Person person{"Alice", 30, {"Boston", 12345}};
  auto result = ctl::pick<PersonName>(person);

  REQUIRE(result.name == "Alice");
}

TEST_CASE("pick extracts multiple fields", "[struct_ops][pick]") {
  Person person{"Alice", 30, {"Boston", 12345}};
  auto result = ctl::pick<PersonNoAddress>(person);

  REQUIRE(result.name == "Alice");
  REQUIRE(result.age == 30);
}

// ============================================================================
// omit
// ============================================================================

TEST_CASE("omit excludes fields via target type", "[struct_ops][omit]") {
  Person person{"Alice", 30, {"Boston", 12345}};
  auto result = ctl::omit<PersonNoAddress>(person);

  REQUIRE(result.name == "Alice");
  REQUIRE(result.age == 30);
}

// ============================================================================
// merge
// ============================================================================

TEST_CASE("merge overwrites flat fields from right", "[struct_ops][merge]") {
  Person base{"Alice", 30, {"Boston", 12345}};
  Person override_src{"Bob", 25, {"LA", 90001}};
  auto result = ctl::merge(base, override_src);

  REQUIRE(result.name == "Bob");
  REQUIRE(result.age == 25);
  REQUIRE(result.address.city == "LA");
}

TEST_CASE("merge deep-merges nested described structs", "[struct_ops][merge]") {
  Person base{"Alice", 30, {"Boston", 12345}};
  Person patch = base;
  patch.address.city = "NYC";
  auto result = ctl::merge(base, patch);

  REQUIRE(result.name == "Alice");
  REQUIRE(result.address.city == "NYC");
  REQUIRE(result.address.zip_code == 12345);
}

TEST_CASE("merge with multiple sources, rightmost wins", "[struct_ops][merge]") {
  Person a{"Alice", 30, {"Boston", 12345}};
  Person b = a;
  b.name = "Bob";
  Person c = b;
  c.age = 99;
  auto result = ctl::merge(a, b, c);

  REQUIRE(result.name == "Bob");
  REQUIRE(result.age == 99);
}

// ============================================================================
// defaults
// ============================================================================

TEST_CASE("defaults keeps base values for flat structs", "[struct_ops][defaults]") {
  Person base{"Alice", 30, {"Boston", 12345}};
  Person fallback{"Bob", 25, {"LA", 90001}};
  auto result = ctl::defaults(base, fallback);

  REQUIRE(result.name == "Alice");
  REQUIRE(result.age == 30);
  REQUIRE(result.address.city == "Boston");
}

TEST_CASE("defaults recurses into nested described structs", "[struct_ops][defaults]") {
  Person base{"Alice", 30, {"Boston", 12345}};
  Person fallback{"Bob", 25, {"LA", 90001}};
  auto result = ctl::defaults(base, fallback);

  REQUIRE(result.address.city == "Boston");
  REQUIRE(result.address.zip_code == 12345);
}
