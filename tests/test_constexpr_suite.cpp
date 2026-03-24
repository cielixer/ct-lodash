#include <ctl/ctl.hpp>
#include <type_traits>

struct ConstexprInner {
  int value;
};

struct ConstexprRecord {
  int id;
  ConstexprInner inner;
};

CTL_DESCRIBE_STRUCT(ConstexprInner, (), (value))
CTL_DESCRIBE_STRUCT(ConstexprRecord, (), (id, inner))

constexpr bool constexpr_access_update_roundtrip() {
  ConstexprRecord record{10, {20}};

  using id_field = ctl::field<&ConstexprRecord::id>;
  using inner_value_path = ctl::path<&ConstexprRecord::inner, &ConstexprInner::value>;

  static_assert(ctl::path_applicable<ConstexprRecord&, id_field>);
  static_assert(ctl::path_applicable<ConstexprRecord&, inner_value_path>);

  if (ctl::get<id_field>(record) != 10) {
    return false;
  }

  ctl::set<id_field>(record, 15);
  ctl::update<inner_value_path>(record, [](int& value) { value += 5; });

  const auto copied = ctl::with<inner_value_path>(record, 42);

  return ctl::get<id_field>(record) == 15 && ctl::get<inner_value_path>(record) == 25 &&
         ctl::get<inner_value_path>(copied) == 42;
}

static_assert(constexpr_access_update_roundtrip());

using id_field = ctl::field<&ConstexprRecord::id>;
using inner_value_path = ctl::path<&ConstexprRecord::inner, &ConstexprInner::value>;

static_assert(std::is_same_v<decltype(ctl::get<id_field>(std::declval<ConstexprRecord&>())), int&>);
static_assert(std::is_same_v<decltype(ctl::get<id_field>(std::declval<const ConstexprRecord&>())),
                             const int&>);
static_assert(
    std::is_same_v<decltype(ctl::get<inner_value_path>(std::declval<ConstexprRecord&>())), int&>);
static_assert(
    std::is_same_v<decltype(ctl::get<inner_value_path>(std::declval<const ConstexprRecord&>())),
                   const int&>);
