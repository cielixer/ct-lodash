#include <type_traits>

#ifdef CTL_NEGATIVE_TEST
constexpr int invalid_constexpr_function() {
  static_assert(false, "This should always fail compilation");
  return 42;
}
#endif

int main() {
#ifdef CTL_NEGATIVE_TEST
  return invalid_constexpr_function();
#else
  return 0;
#endif
}
