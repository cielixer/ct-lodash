# Installation

## Requirements
- C++23 compiler (GCC 13+, Clang 17+, MSVC 19.36+)
- CMake 3.25+
- Boost (headers only — used internally for reflection)

## With Pixi
Note: not yet published, use local path for now.
```bash
pixi add ctl
```

## With CMake FetchContent
Add this to your `CMakeLists.txt`:
```cmake
include(FetchContent)
FetchContent_Declare(
  ctl
  GIT_REPOSITORY https://github.com/your-org/ctl.git
  GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(ctl)

target_link_libraries(your_target PRIVATE ctl::ctl)
```

## Manual / Subdirectory
```cmake
add_subdirectory(path/to/ctl)
target_link_libraries(your_target PRIVATE ctl::ctl)
```

## Verify
```cpp
#include <ctl/ctl.hpp>
#include <iostream>

struct Point { float x, y; };
CTL_DESCRIBE_STRUCT(Point, (x, y))

int main() {
    Point p{1.0f, 2.0f};
    std::cout << ctl::get<0>(p) << std::endl; // Prints 1.0
    return 0;
}
```
