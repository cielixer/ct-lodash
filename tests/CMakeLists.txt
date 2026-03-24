# Test suite configuration for ctl library

# Find Catch2
find_package(Catch2 3 REQUIRED)

# Include our negative-compile harness
include(${CMAKE_SOURCE_DIR}/cmake/ExpectCompileFailure.cmake)

# Runtime tests using Catch2
add_executable(smoke_runtime smoke_runtime.cpp)
target_link_libraries(smoke_runtime PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(smoke_runtime PRIVATE cxx_std_23)

add_test(NAME smoke_runtime COMMAND smoke_runtime)
set_tests_properties(smoke_runtime PROPERTIES LABELS "runtime")

add_executable(describe_smoke test_describe.cpp)
target_link_libraries(describe_smoke PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(describe_smoke PRIVATE cxx_std_23)

add_test(NAME describe_smoke COMMAND describe_smoke)
set_tests_properties(describe_smoke PROPERTIES LABELS "runtime;describe")

add_executable(test_access test_access.cpp)
target_link_libraries(test_access PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_access PRIVATE cxx_std_23)

add_test(NAME access_iteration_suite COMMAND test_access)
set_tests_properties(access_iteration_suite PROPERTIES LABELS "runtime;access")

add_executable(test_update test_update.cpp)
target_link_libraries(test_update PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_update PRIVATE cxx_std_23)

add_test(NAME update_suite COMMAND test_update)
set_tests_properties(update_suite PROPERTIES LABELS "runtime;update")

add_executable(test_runtime test_runtime.cpp)
target_link_libraries(test_runtime PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_runtime PRIVATE cxx_std_23)

add_test(NAME runtime_suite COMMAND test_runtime)
set_tests_properties(runtime_suite PROPERTIES LABELS "runtime")

add_executable(test_runtime_failure test_runtime_failure.cpp)
target_link_libraries(test_runtime_failure PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_runtime_failure PRIVATE cxx_std_23)

add_test(NAME runtime_failure_suite COMMAND test_runtime_failure)
set_tests_properties(runtime_failure_suite PROPERTIES LABELS "runtime")

add_executable(test_edge_cases test_edge_cases.cpp)
target_link_libraries(test_edge_cases PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_edge_cases PRIVATE cxx_std_23)

add_test(NAME edge_case_suite COMMAND test_edge_cases)
set_tests_properties(edge_case_suite PROPERTIES LABELS "runtime;edge_case")

add_executable(test_string_access test_string_access.cpp)
target_link_libraries(test_string_access PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_string_access PRIVATE cxx_std_23)

add_test(NAME string_access_suite COMMAND test_string_access)
set_tests_properties(string_access_suite PROPERTIES LABELS "runtime;string_access")

add_executable(test_collection test_collection.cpp)
target_link_libraries(test_collection PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_collection PRIVATE cxx_std_23)

add_test(NAME collection_suite COMMAND test_collection)
set_tests_properties(collection_suite PROPERTIES LABELS "runtime;collection")

add_executable(test_collection_ext test_collection_ext.cpp)
target_link_libraries(test_collection_ext PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_collection_ext PRIVATE cxx_std_23)

add_test(NAME collection_ext_suite COMMAND test_collection_ext)
set_tests_properties(collection_ext_suite PROPERTIES LABELS "runtime;collection")

add_executable(test_struct_ops test_struct_ops.cpp)
target_link_libraries(test_struct_ops PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_struct_ops PRIVATE cxx_std_23)

add_test(NAME struct_ops_suite COMMAND test_struct_ops)
set_tests_properties(struct_ops_suite PROPERTIES LABELS "runtime;struct_ops")

add_executable(test_utility test_utility.cpp)
target_link_libraries(test_utility PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_utility PRIVATE cxx_std_23)

add_test(NAME utility_suite COMMAND test_utility)
set_tests_properties(utility_suite PROPERTIES LABELS "runtime;utility")

add_executable(test_collection_trivial test_collection_trivial.cpp)
target_link_libraries(test_collection_trivial PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_collection_trivial PRIVATE cxx_std_23)

add_test(NAME collection_trivial_suite COMMAND test_collection_trivial)
set_tests_properties(collection_trivial_suite PROPERTIES LABELS "runtime;collection")

add_executable(test_collection_low test_collection_low.cpp)
target_link_libraries(test_collection_low PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_collection_low PRIVATE cxx_std_23)

add_test(NAME collection_low_suite COMMAND test_collection_low)
set_tests_properties(collection_low_suite PROPERTIES LABELS "runtime;collection")

add_executable(test_collection_medium test_collection_medium.cpp)
target_link_libraries(test_collection_medium PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_collection_medium PRIVATE cxx_std_23)

add_test(NAME collection_medium_suite COMMAND test_collection_medium)
set_tests_properties(collection_medium_suite PROPERTIES LABELS "runtime;collection")

add_executable(test_struct_ops_ext test_struct_ops_ext.cpp)
target_link_libraries(test_struct_ops_ext PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_struct_ops_ext PRIVATE cxx_std_23)

add_test(NAME struct_ops_ext_suite COMMAND test_struct_ops_ext)
set_tests_properties(struct_ops_ext_suite PROPERTIES LABELS "runtime;struct_ops")

add_executable(test_utility_ext test_utility_ext.cpp)
target_link_libraries(test_utility_ext PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_utility_ext PRIVATE cxx_std_23)

add_test(NAME utility_ext_suite COMMAND test_utility_ext)
set_tests_properties(utility_ext_suite PROPERTIES LABELS "runtime;utility")

# JSON serialization tests (requires nlohmann_json)
find_package(nlohmann_json REQUIRED)

add_executable(test_json test_json.cpp)
target_link_libraries(test_json PRIVATE ctl::ctl nlohmann_json::nlohmann_json Catch2::Catch2WithMain)
target_compile_features(test_json PRIVATE cxx_std_23)

add_test(NAME json_suite COMMAND test_json)
set_tests_properties(json_suite PROPERTIES LABELS "runtime;json")

add_executable(test_hash test_hash.cpp)
target_link_libraries(test_hash PRIVATE ctl::ctl Catch2::Catch2WithMain)
target_compile_features(test_hash PRIVATE cxx_std_23)

add_test(NAME hash_suite COMMAND test_hash)
set_tests_properties(hash_suite PROPERTIES LABELS "runtime;hash")

# Compile-time tests (static_assert verification)
# These targets only need to compile successfully, not run
add_library(smoke_compile_obj OBJECT smoke_compile.cpp)
target_link_libraries(smoke_compile_obj PRIVATE ctl::ctl)
target_compile_features(smoke_compile_obj PRIVATE cxx_std_23)

add_library(path_compile_obj OBJECT test_path.cpp)
target_link_libraries(path_compile_obj PRIVATE ctl::ctl)
target_compile_features(path_compile_obj PRIVATE cxx_std_23)

add_library(access_compile_obj OBJECT test_access_compile.cpp)
target_link_libraries(access_compile_obj PRIVATE ctl::ctl)
target_compile_features(access_compile_obj PRIVATE cxx_std_23)

add_library(update_compile_obj OBJECT test_update_compile.cpp)
target_link_libraries(update_compile_obj PRIVATE ctl::ctl)
target_compile_features(update_compile_obj PRIVATE cxx_std_23)

add_library(constexpr_suite_obj OBJECT test_constexpr_suite.cpp)
target_link_libraries(constexpr_suite_obj PRIVATE ctl::ctl)
target_compile_features(constexpr_suite_obj PRIVATE cxx_std_23)

# Create a test that verifies the compile-time target built successfully
add_test(NAME smoke_compile COMMAND ${CMAKE_COMMAND} -E echo "Compile-time assertions passed")
set_tests_properties(smoke_compile PROPERTIES LABELS "compile")

add_test(NAME path_compile_suite COMMAND ${CMAKE_COMMAND} -E echo "Path compile-time assertions passed")
set_tests_properties(path_compile_suite PROPERTIES LABELS "compile;path")

add_test(NAME access_compile_suite COMMAND ${CMAKE_COMMAND} -E echo "Access compile-time assertions passed")
set_tests_properties(access_compile_suite PROPERTIES LABELS "compile;access")

add_test(NAME update_compile_suite COMMAND ${CMAKE_COMMAND} -E echo "Update compile-time assertions passed")
set_tests_properties(update_compile_suite PROPERTIES LABELS "compile;update")

add_test(NAME constexpr_suite COMMAND ${CMAKE_COMMAND} -E echo "Constexpr compile-time assertions passed")
set_tests_properties(constexpr_suite PROPERTIES LABELS "compile;constexpr")

# Negative compile tests (expected failures)
expect_compile_failure(
  TARGET negative_invalid_type
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/negative/invalid_type.cpp
  EXPECTED_DIAGNOSTIC "static assertion failed"
)

expect_compile_failure(
  TARGET undescribed_type_negative
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/negative/undescribed_type.cpp
  EXPECTED_DIAGNOSTIC "ctl::get<I>: T must satisfy ctl::described_record"
)

expect_compile_failure(
  TARGET invalid_path_chain_negative
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/negative/invalid_path.cpp
  EXPECTED_DIAGNOSTIC "ctl::get<Path>: invalid member pointer chain for object type"
)

expect_compile_failure(
  TARGET index_out_of_range_negative
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/negative/index_out_of_range.cpp
  EXPECTED_DIAGNOSTIC "index out of range"
)

expect_compile_failure(
  TARGET const_mutation_negative
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/negative/const_mutation.cpp
  EXPECTED_DIAGNOSTIC "cannot mutate const object"
)

expect_compile_failure(
  TARGET type_mismatch_negative
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/negative/type_mismatch.cpp
  EXPECTED_DIAGNOSTIC "value type not assignable to target member type"
)

expect_compile_failure(
  TARGET move_only_with_negative
  SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/negative/move_only_with.cpp
  EXPECTED_DIAGNOSTIC "ctl::with<Path>: object type must be copy constructible"
)

# Two-TU include test (ODR violation check)
add_executable(two_tu_include_smoke 
  two_tu_test_1.cpp
  two_tu_test_2.cpp
)
target_link_libraries(two_tu_include_smoke PRIVATE ctl::ctl)
target_compile_features(two_tu_include_smoke PRIVATE cxx_std_23)

add_test(NAME two_tu_include_smoke COMMAND two_tu_include_smoke)
set_tests_properties(two_tu_include_smoke PROPERTIES LABELS "smoke")

# Build-tree consumer test (uses CMAKE_PREFIX_PATH to find build-tree export)
add_test(
  NAME build_tree_consumer_suite
  COMMAND ${CMAKE_COMMAND}
          -S "${CMAKE_CURRENT_SOURCE_DIR}/consumer"
          -B "${CMAKE_CURRENT_BINARY_DIR}/consumer_build"
          -G Ninja
          -DCMAKE_PREFIX_PATH=${CMAKE_BINARY_DIR}
          -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
          -DCMAKE_MAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}
          --fresh
)
set_tests_properties(build_tree_consumer_suite PROPERTIES LABELS "consumer;build_tree")

add_test(
  NAME build_tree_consumer_build
  COMMAND ${CMAKE_COMMAND} --build "${CMAKE_CURRENT_BINARY_DIR}/consumer_build"
)
set_tests_properties(build_tree_consumer_build PROPERTIES 
  LABELS "consumer;build_tree"
  DEPENDS build_tree_consumer_suite
)

add_test(
  NAME build_tree_consumer_run
  COMMAND "${CMAKE_CURRENT_BINARY_DIR}/consumer_build/consumer_test"
)
set_tests_properties(build_tree_consumer_run PROPERTIES 
  LABELS "consumer;build_tree"
  DEPENDS build_tree_consumer_build
)
