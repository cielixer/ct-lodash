#include <ctl/ctl.hpp>

#include "../fixtures/person.hpp"

using invalid_path_t = ctl::path<&Person::name, &Address::city>;

int main() {
#ifdef CTL_NEGATIVE_TEST
    Person person{"Alice", 30, {"Boston", 12345}};
    [[maybe_unused]] auto& invalid = ctl::get<invalid_path_t>(person);
#endif
    return 0;
}
