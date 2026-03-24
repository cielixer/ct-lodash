#include <array>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <memory>
#include <string>
#include <string_view>
#include <type_traits>

#include "fixtures/edge_cases.hpp"

TEST_CASE("empty described struct supports count and zero-iteration", "[edge_case][empty]") {
  static_assert(ctl::field_count_v<EmptyRecord> == 0);
  static_assert(ctl::described_record<EmptyRecord>);

  using person_name_field = ctl::field<&GeoPerson::name>;
  static_assert(!ctl::path_applicable<EmptyRecord&, person_name_field>);

  EmptyRecord empty{};
  int visit_count = 0;
  ctl::for_each_field(empty, [&](std::string_view, auto&) { ++visit_count; });

  REQUIRE(visit_count == 0);
}

TEST_CASE("nested paths work through 3 levels", "[edge_case][nested]") {
  GeoPerson person{"Ada", {"London", {51.5074, -0.1278}}};

  using city_path = ctl::path<&GeoPerson::address, &GeoAddress::city>;
  using latitude_path =
      ctl::path<&GeoPerson::address, &GeoAddress::coordinates, &Coordinates::latitude>;

  REQUIRE(ctl::get<city_path>(person) == "London");
  REQUIRE(ctl::get<latitude_path>(person) == Catch::Approx(51.5074));

  ctl::set<city_path>(person, std::string("Paris"));
  ctl::update<latitude_path>(person, [](double& latitude) { latitude += 1.0; });

  REQUIRE(person.address.city == "Paris");
  REQUIRE(person.address.coordinates.latitude == Catch::Approx(52.5074));
}

TEST_CASE("move-only members support access and in-place mutation", "[edge_case][move_only]") {
  MoveOnlyRecord record{std::make_unique<int>(7), 1};

  using payload_field = ctl::field<&MoveOnlyRecord::payload>;
  using version_field = ctl::field<&MoveOnlyRecord::version>;

  REQUIRE(*ctl::get<payload_field>(record) == 7);
  REQUIRE(ctl::get<version_field>(record) == 1);

  ctl::update<payload_field>(
      record, [](std::unique_ptr<int>& payload) { payload = std::make_unique<int>(11); });
  ctl::set<version_field>(record, 2);

  REQUIRE(*record.payload == 11);
  REQUIRE(record.version == 2);
}

TEST_CASE("std::array and enum fields work across access/update/runtime",
          "[edge_case][array][enum]") {
  ArrayEnumRecord record{{1, 2, 3, 4, 5}, Mode::draft};

  using values_field = ctl::field<&ArrayEnumRecord::values>;
  using mode_field = ctl::field<&ArrayEnumRecord::mode>;

  REQUIRE(ctl::get<values_field>(record)[0] == 1);
  REQUIRE(ctl::get<mode_field>(record) == Mode::draft);

  ctl::update<values_field>(record, [](std::array<int, 5>& values) { values[4] = 99; });
  ctl::set<mode_field>(record, Mode::published);

  REQUIRE(record.values[4] == 99);
  REQUIRE(record.mode == Mode::published);

  auto* mode_ptr = ctl::runtime::try_get<Mode>(record, "mode");
  REQUIRE(mode_ptr != nullptr);
  REQUIRE(*mode_ptr == Mode::published);

  const bool updated = ctl::runtime::try_update<Mode>(record, "mode", Mode::archived);
  REQUIRE(updated);
  REQUIRE(record.mode == Mode::archived);
}

TEST_CASE("const and mutable access behavior", "[edge_case][const]") {
  ArrayEnumRecord mutable_record{{5, 4, 3, 2, 1}, Mode::draft};
  const ArrayEnumRecord const_record{{9, 8, 7, 6, 5}, Mode::published};

  using values_field = ctl::field<&ArrayEnumRecord::values>;
  using mode_field = ctl::field<&ArrayEnumRecord::mode>;

  static_assert(
      std::is_same_v<decltype(ctl::get<values_field>(mutable_record)), std::array<int, 5>&>);
  static_assert(
      std::is_same_v<decltype(ctl::get<values_field>(const_record)), const std::array<int, 5>&>);
  static_assert(
      std::is_same_v<decltype(ctl::runtime::try_get<Mode>(mutable_record, "mode")), Mode*>);
  static_assert(
      std::is_same_v<decltype(ctl::runtime::try_get<Mode>(const_record, "mode")), const Mode*>);

  REQUIRE(ctl::get<mode_field>(mutable_record) == Mode::draft);
  REQUIRE(ctl::get<mode_field>(const_record) == Mode::published);
}
