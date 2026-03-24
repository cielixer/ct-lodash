#include <ctl/json.hpp>

#include <catch2/catch_test_macros.hpp>
#include <map>
#include <optional>
#include <string>
#include <vector>

#include "fixtures/person.hpp"

struct WithOptional {
  std::string name;
  std::optional<int> score;
};
CTL_DESCRIBE_STRUCT(WithOptional, (), (name, score))

struct WithVector {
  std::string label;
  std::vector<int> values;
};
CTL_DESCRIBE_STRUCT(WithVector, (), (label, values))

struct WithMap {
  std::string id;
  std::map<std::string, int> tags;
};
CTL_DESCRIBE_STRUCT(WithMap, (), (id, tags))

struct WithNestedVec {
  std::string team;
  std::vector<Person> members;
};
CTL_DESCRIBE_STRUCT(WithNestedVec, (), (team, members))

TEST_CASE("to_json: flat struct") {
  PersonNoAddress p{"Alice", 30};
  auto j = ctl::to_json(p);

  REQUIRE(j["name"] == "Alice");
  REQUIRE(j["age"] == 30);
}

TEST_CASE("to_json: nested struct") {
  Person p{"Bob", 25, {"Seoul", 12345}};
  auto j = ctl::to_json(p);

  REQUIRE(j["name"] == "Bob");
  REQUIRE(j["age"] == 25);
  REQUIRE(j["address"]["city"] == "Seoul");
  REQUIRE(j["address"]["zip_code"] == 12345);
}

TEST_CASE("from_json: flat struct roundtrip") {
  PersonNoAddress original{"Charlie", 40};
  auto j = ctl::to_json(original);
  auto restored = ctl::from_json<PersonNoAddress>(j);

  REQUIRE(restored.name == "Charlie");
  REQUIRE(restored.age == 40);
}

TEST_CASE("from_json: nested struct roundtrip") {
  Person original{"Diana", 35, {"Tokyo", 54321}};
  auto j = ctl::to_json(original);
  auto restored = ctl::from_json<Person>(j);

  REQUIRE(restored.name == "Diana");
  REQUIRE(restored.age == 35);
  REQUIRE(restored.address.city == "Tokyo");
  REQUIRE(restored.address.zip_code == 54321);
}

TEST_CASE("to_json/from_json: optional with value") {
  WithOptional original{"Eve", 100};
  auto j = ctl::to_json(original);

  REQUIRE(j["score"] == 100);

  auto restored = ctl::from_json<WithOptional>(j);
  REQUIRE(restored.name == "Eve");
  REQUIRE(restored.score.has_value());
  REQUIRE(*restored.score == 100);
}

TEST_CASE("to_json/from_json: optional nullopt") {
  WithOptional original{"Frank", std::nullopt};
  auto j = ctl::to_json(original);

  REQUIRE(j["score"].is_null());

  auto restored = ctl::from_json<WithOptional>(j);
  REQUIRE(restored.name == "Frank");
  REQUIRE_FALSE(restored.score.has_value());
}

TEST_CASE("to_json/from_json: vector field") {
  WithVector original{"nums", {1, 2, 3, 4, 5}};
  auto j = ctl::to_json(original);

  REQUIRE(j["values"] == nlohmann::json({1, 2, 3, 4, 5}));

  auto restored = ctl::from_json<WithVector>(j);
  REQUIRE(restored.label == "nums");
  REQUIRE(restored.values == std::vector{1, 2, 3, 4, 5});
}

TEST_CASE("to_json/from_json: map field") {
  WithMap original{"item1", {{"a", 1}, {"b", 2}}};
  auto j = ctl::to_json(original);

  REQUIRE(j["tags"]["a"] == 1);
  REQUIRE(j["tags"]["b"] == 2);

  auto restored = ctl::from_json<WithMap>(j);
  REQUIRE(restored.id == "item1");
  REQUIRE(restored.tags.at("a") == 1);
  REQUIRE(restored.tags.at("b") == 2);
}

TEST_CASE("to_json/from_json: vector of nested structs") {
  WithNestedVec original{
      "dev",
      {{"Alice", 30, {"Seoul", 100}}, {"Bob", 25, {"Tokyo", 200}}}};
  auto j = ctl::to_json(original);

  REQUIRE(j["team"] == "dev");
  REQUIRE(j["members"].size() == 2);
  REQUIRE(j["members"][0]["name"] == "Alice");
  REQUIRE(j["members"][1]["address"]["city"] == "Tokyo");

  auto restored = ctl::from_json<WithNestedVec>(j);
  REQUIRE(restored.team == "dev");
  REQUIRE(restored.members.size() == 2);
  REQUIRE(restored.members[0].name == "Alice");
  REQUIRE(restored.members[1].address.city == "Tokyo");
}

TEST_CASE("from_json: missing key uses default value") {
  nlohmann::json j = {{"name", "Grace"}};
  auto result = ctl::from_json<PersonNoAddress>(j);

  REQUIRE(result.name == "Grace");
  REQUIRE(result.age == 0);
}

TEST_CASE("from_json: extra keys are ignored") {
  nlohmann::json j = {{"name", "Heidi"}, {"age", 28}, {"email", "h@x.com"}};
  auto result = ctl::from_json<PersonNoAddress>(j);

  REQUIRE(result.name == "Heidi");
  REQUIRE(result.age == 28);
}

TEST_CASE("to_json: produces valid JSON string") {
  Person p{"Ivan", 22, {"Berlin", 10115}};
  auto j = ctl::to_json(p);
  std::string s = j.dump();

  REQUIRE_FALSE(s.empty());
  auto reparsed = nlohmann::json::parse(s);
  REQUIRE(reparsed == j);
}
