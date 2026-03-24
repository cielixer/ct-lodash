#include <type_traits>

namespace ctl_test {

static_assert(true, "Compile-time test harness operational");

static_assert(std::is_integral<int>::value, "int should be integral type");
static_assert(!std::is_integral<float>::value, "float should not be integral type");

constexpr int add(int a, int b) { return a + b; }

static_assert(add(2, 2) == 4, "constexpr addition should work at compile time");

}  // namespace ctl_test
