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

int main() {
  Person alice = {.name = "Alice", .age = 30, .address = {.city = "London", .zip_code = 12345}};

  std::cout << "Visiting fields of Alice:\n";
  ctl::runtime::visit(alice, "name", [](const auto& value) {
    if constexpr (requires { std::cout << value; }) {
      std::cout << " - Found field: " << value << "\n";
    } else {
      std::cout << " - Found field (complex type)\n";
    }
  });

  auto* age = ctl::runtime::try_get<int>(alice, "age");
  if (age) {
    std::cout << "Alice's age is " << *age << "\n";
  }
  assert(age != nullptr && *age == 30);

  bool updated = ctl::runtime::try_update<int>(alice, "age", 31);
  if (updated) {
    std::cout << "Updated Alice's age to " << alice.age << "\n";
  }
  assert(updated && alice.age == 31);

  assert(ctl::runtime::try_get<int>(alice, "unknown_field") == nullptr);
  assert(ctl::runtime::try_get<int>(alice, "name") == nullptr);

  std::cout << "Runtime escape hatch successful!\n";
  return 0;
}
