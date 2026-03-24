#include <ctl/ctl.hpp>

#include "../fixtures/person.hpp"

int main() {
#ifdef CTL_NEGATIVE_TEST
    Person person{"Alice", 30, {"Boston", 12345}};
    
    [[maybe_unused]] auto& invalid = ctl::get<999>(person);
#endif
    return 0;
}
