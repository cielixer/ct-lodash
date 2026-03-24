#include <cassert>
#include <ctl/ctl.hpp>
#include <iostream>
#include <string>

struct Address {
  std::string city;
  int zip_code;
};

struct Person {
  std::string name;
  int age;
  Address address;
};

CTL_DESCRIBE_STRUCT(Address, (), (city, zip_code))
CTL_DESCRIBE_STRUCT(Person, (), (name, age, address))

inline std::ostream& operator<<(std::ostream& os, const Address& a) {
  return os << "{ city: " << a.city << ", zip_code: " << a.zip_code << " }";
}

int main() {
  Person alice = {.name = "Alice", .age = 30, .address = {.city = "London", .zip_code = 12345}};

  using CityPath = ctl::path<&Person::address, &Address::city>;
  using NamePath = ctl::field<&Person::name>;

  std::string city = ctl::get<CityPath>(alice);
  std::cout << "Original city: " << city << "\n";
  assert(city == "London");

  ctl::set<CityPath>(alice, "Paris");
  std::cout << "Updated city (in-place): " << alice.address.city << "\n";
  assert(alice.address.city == "Paris");

  Person bob = ctl::with<NamePath>(alice, "Bob");
  std::cout << "New person name: " << bob.name << "\n";
  assert(bob.name == "Bob");
  assert(alice.name == "Alice");

  ctl::update<ctl::field<&Person::age>>(bob, [](int& age) { age += 1; });
  std::cout << "Older Bob age: " << bob.age << "\n";
  assert(bob.age == 31);

  std::cout << "\n--- String-literal access (same operations, ergonomic syntax) ---\n";

  std::string city2 = ctl::get<"address", "city">(alice);
  std::cout << "String get city: " << city2 << "\n";
  assert(city2 == "Paris");

  ctl::set<"address", "city">(alice, "Tokyo");
  std::cout << "String set city: " << alice.address.city << "\n";
  assert(alice.address.city == "Tokyo");

  Person charlie = ctl::with<"name">(alice, "Charlie");
  std::cout << "String with name: " << charlie.name << "\n";
  assert(charlie.name == "Charlie");
  assert(alice.name == "Alice");

  ctl::update<"age">(charlie, [](int& a) { a += 10; });
  std::cout << "String update age: " << charlie.age << "\n";
  assert(charlie.age == 40);

  std::cout << "\nFields of Alice:\n";
  ctl::for_each_field(alice, [](std::string_view name, const auto& value) {
    std::cout << " - " << name << ": " << value << "\n";
  });

  std::cout << "Compile-time quickstart successful!\n";
  return 0;
}
