#include <catch2/catch_test_macros.hpp>
#include <ctl/ctl.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

struct Point {
  int x;
  int y;
};
CTL_DESCRIBE_STRUCT(Point, (), (x, y))
CTL_MAKE_HASHABLE(Point)

struct Color {
  std::string name;
  int r;
  int g;
  int b;
};
CTL_DESCRIBE_STRUCT(Color, (), (name, r, g, b))
CTL_MAKE_HASHABLE(Color)

TEST_CASE("CTL_MAKE_HASHABLE enables unordered_set usage", "[hash]") {
  std::unordered_set<Point> points;
  points.insert({1, 2});
  points.insert({3, 4});
  points.insert({1, 2});

  REQUIRE(points.size() == 2);
}

TEST_CASE("CTL_MAKE_HASHABLE enables unordered_map key usage", "[hash]") {
  std::unordered_map<Point, std::string> labels;
  labels[{0, 0}] = "origin";
  labels[{1, 0}] = "right";

  REQUIRE(labels[{0, 0}] == "origin");
  REQUIRE(labels[{1, 0}] == "right");
}

TEST_CASE("equal Points produce same hash", "[hash]") {
  Point a{10, 20};
  Point b{10, 20};

  REQUIRE(std::hash<Point>{}(a) == std::hash<Point>{}(b));
}

TEST_CASE("different Points produce different hash", "[hash]") {
  Point a{1, 2};
  Point b{2, 1};

  REQUIRE(std::hash<Point>{}(a) != std::hash<Point>{}(b));
}

TEST_CASE("struct with string field is hashable", "[hash]") {
  std::unordered_set<Color> colors;
  colors.insert({"red", 255, 0, 0});
  colors.insert({"green", 0, 255, 0});
  colors.insert({"red", 255, 0, 0});

  REQUIRE(colors.size() == 2);
}

TEST_CASE("hashable struct works with ctl::group_by as key", "[hash]") {
  struct Item {
    std::string label;
    Point pos;
  };

  std::vector<Item> items = {
      {"a", {1, 2}},
      {"b", {3, 4}},
      {"c", {1, 2}},
  };

  auto grouped = ctl::group_by(items, [](const Item& i) { return i.pos; });

  REQUIRE(grouped[{1, 2}].size() == 2);
  REQUIRE(grouped[{3, 4}].size() == 1);
}

TEST_CASE("hashable struct works with ctl::key_by", "[hash]") {
  struct Item {
    std::string label;
    Point pos;
  };

  std::vector<Item> items = {
      {"a", {1, 2}},
      {"b", {3, 4}},
  };

  auto indexed = ctl::key_by(items, [](const Item& i) { return i.pos; });

  REQUIRE(indexed[{1, 2}].label == "a");
  REQUIRE(indexed[{3, 4}].label == "b");
}

TEST_CASE("hashable struct works with ctl::count_by", "[hash]") {
  struct Item {
    std::string label;
    Point pos;
  };

  std::vector<Item> items = {
      {"a", {1, 2}},
      {"b", {3, 4}},
      {"c", {1, 2}},
  };

  auto counts = ctl::count_by(items, [](const Item& i) { return i.pos; });

  REQUIRE(counts[{1, 2}] == 2);
  REQUIRE(counts[{3, 4}] == 1);
}
