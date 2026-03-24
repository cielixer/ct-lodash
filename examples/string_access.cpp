#include <cassert>
#include <ctl/ctl.hpp>
#include <iostream>
#include <string>

struct Coord {
  double lat;
  double lng;
};

struct Office {
  std::string name;
  std::string city;
  Coord location;
};

struct Company {
  std::string name;
  int founded;
  Office hq;
};

CTL_DESCRIBE_STRUCT(Coord, (), (lat, lng))
CTL_DESCRIBE_STRUCT(Office, (), (name, city, location))
CTL_DESCRIBE_STRUCT(Company, (), (name, founded, hq))

inline std::ostream& operator<<(std::ostream& os, const Coord& c) {
  return os << "(" << c.lat << ", " << c.lng << ")";
}

inline std::ostream& operator<<(std::ostream& os, const Office& o) {
  return os << "{ " << o.name << ", " << o.city << ", " << o.location << " }";
}

int main() {
  Company acme{
      .name = "Acme Corp",
      .founded = 1999,
      .hq = {.name = "HQ", .city = "San Francisco", .location = {37.7749, -122.4194}},
  };

  std::cout << "=== ctl string-literal access ===\n\n";

  std::cout << "Company : " << ctl::get<"name">(acme) << "\n";
  std::cout << "Founded : " << ctl::get<"founded">(acme) << "\n";
  std::cout << "HQ city : " << ctl::get<"hq", "city">(acme) << "\n";
  std::cout << "HQ lat  : " << ctl::get<"hq", "location", "lat">(acme) << "\n\n";

  ctl::set<"hq", "city">(acme, "Seoul");
  std::cout << "[set]    HQ city -> " << acme.hq.city << "\n";
  assert(acme.hq.city == "Seoul");

  Company branch = ctl::with<"hq", "name">(acme, "Tokyo Office");
  std::cout << "[with]   branch HQ name -> " << branch.hq.name << "\n";
  assert(branch.hq.name == "Tokyo Office");
  assert(acme.hq.name == "HQ");

  ctl::update<"founded">(acme, [](int& y) { y = 2025; });
  std::cout << "[update] founded -> " << acme.founded << "\n";
  assert(acme.founded == 2025);

  ctl::update<"hq", "location", "lat">(acme, [](double& lat) { lat = 37.5665; });
  ctl::update<"hq", "location", "lng">(acme, [](double& lng) { lng = 126.9780; });
  std::cout << "[update] HQ coords -> (" << acme.hq.location.lat << ", " << acme.hq.location.lng
            << ")\n";

  std::cout << "\nAll fields:\n";
  ctl::for_each_field(acme, [](std::string_view name, const auto& value) {
    std::cout << "  " << name << ": " << value << "\n";
  });

  std::cout << "\n=== has / keys / values ===\n\n";

  std::cout << std::boolalpha;
  std::cout << "has<\"name\">    : " << ctl::has<"name">(acme) << "\n";
  std::cout << "has<\"email\">   : " << ctl::has<"email">(acme) << "\n";
  std::cout << "has<\"hq\",\"city\">: " << ctl::has<"hq", "city">(acme) << "\n\n";

  constexpr auto k = ctl::keys<Company>();
  std::cout << "keys<Company>: ";
  for (auto key : k) {
    std::cout << key << " ";
  }
  std::cout << "\n";
  assert(k.size() == 3);

  auto vals = ctl::values(acme);
  std::cout << "values(acme)[0] (name): " << std::get<0>(vals) << "\n";
  std::cout << "values(acme)[1] (founded): " << std::get<1>(vals) << "\n";
  assert(std::get<0>(vals) == "Acme Corp");

  std::cout << "\nString-literal access example successful!\n";
  return 0;
}
