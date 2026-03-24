cmake_minimum_required(VERSION 3.25)
project(ctl_consumer_test CXX)

find_package(ctl CONFIG REQUIRED)

add_executable(consumer_test consumer_test.cpp)
target_link_libraries(consumer_test PRIVATE ctl::ctl)
target_compile_features(consumer_test PRIVATE cxx_std_23)
