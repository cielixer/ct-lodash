#include <ctl/ctl.hpp>
#include <iostream>

#include "../tests/fixtures/person.hpp"

int main() {
  Person person{
      .name = "Junsup", .age = 30, .address = Address{.city = "Seoul", .zip_code = 12345}};

  std::cout << "[Index-based access]\n";
  std::cout << "field_count<Person> = " << ctl::field_count_v<Person> << '\n';

  auto& f0 = ctl::get_by_index<0>(person);
  auto& f1 = ctl::get_by_index<1>(person);
  auto& f2 = ctl::get_by_index<2>(person);

  std::cout << "index 0 = " << f0 << '\n';
  std::cout << "index 1 = " << f1 << '\n';
  std::cout << "index 2 = " << f2 << '\n';

  ctl::get_by_index<1>(person) = 31;
  std::cout << "updated age by index = " << person.age << "\n\n";

  std::cout << "[Name-based access]\n";

  if (auto* p = ctl::get_ptr<std::string>(person, "name")) {
    std::cout << "name = " << *p << '\n';
    *p = "Alice";
  }

  if (auto* p = ctl::get_ptr<int>(person, "age")) {
    std::cout << "age = " << *p << '\n';
    *p = 42;
  }

  if (auto* p = ctl::get_ptr<Address>(person, "address")) {
    std::cout << "address = " << *p << '\n';
    p->city = "Busan";
  }

  std::cout << "\n[visit_member_by_name examples]\n";
  ctl::visit_member_by_name(
      person, "name", [](auto& member) { std::cout << "visited 'name' -> " << member << '\n'; });

  ctl::visit_member_by_name(person, "age", [](auto& member) {
    using T = std::remove_cvref_t<decltype(member)>;
    if constexpr (std::is_same_v<T, int>) {
      member += 10;
    }
  });

  std::cout << "\n[Wrong queries]\n";
  if (auto* p = ctl::get_ptr<double>(person, "age")) {
    std::cout << *p << '\n';
  } else {
    std::cout << "get_ptr<double>(person, \"age\") -> null\n";
  }

  if (!ctl::visit_member_by_name(person, "unknown_field", [](auto&) {})) {
    std::cout << "unknown_field not found\n";
  }
}
