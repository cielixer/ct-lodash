#include <ctl/ctl.hpp>

struct RawType {
    int id;
};

int main() {
#ifdef CTL_NEGATIVE_TEST
    RawType raw{42};
    [[maybe_unused]] auto& id = ctl::get<0>(raw);
#endif
    return 0;
}
