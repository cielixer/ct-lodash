#include <ctl/ctl.hpp>

#include <memory>

#include "../fixtures/edge_cases.hpp"

int main() {
#ifdef CTL_NEGATIVE_TEST
    MoveOnlyRecord record{std::make_unique<int>(5), 1};

    using payload_field = ctl::field<&MoveOnlyRecord::payload>;

    [[maybe_unused]] auto copy = ctl::with<payload_field>(record, std::make_unique<int>(9));
#endif

    return 0;
}
