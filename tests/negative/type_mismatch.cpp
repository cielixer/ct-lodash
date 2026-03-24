#include <ctl/ctl.hpp>

#include <string>

#include "../fixtures/person.hpp"

int main() {
#ifdef CTL_NEGATIVE_TEST
    Person person{"Alice", 30, {"Boston", 12345}};

    using age_field = ctl::field<&Person::age>;

    ctl::set<age_field>(person, std::string("invalid"));
#endif

    return 0;
}
