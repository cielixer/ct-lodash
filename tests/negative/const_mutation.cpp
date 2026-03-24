#include <ctl/ctl.hpp>

#include <string>

#include "../fixtures/person.hpp"

int main() {
#ifdef CTL_NEGATIVE_TEST
    const Person person{"Alice", 30, {"Boston", 12345}};

    using name_field = ctl::field<&Person::name>;

    ctl::set<name_field>(person, std::string("Bob"));
#endif

    return 0;
}
