#pragma once

#include <ctl/describe.hpp>
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

struct PersonName {
    std::string name;
};

struct PersonNoAddress {
    std::string name;
    int age;
};

CTL_DESCRIBE_STRUCT(PersonName, (), (name))
CTL_DESCRIBE_STRUCT(PersonNoAddress, (), (name, age))

inline std::ostream& operator<<(std::ostream& os, const Address& a) {
    return os << "{ city: " << a.city << ", zip_code: " << a.zip_code << " }";
}
