#include <ctl/ctl.hpp>
#include <iostream>
#include <string>
#include <cassert>

struct Point {
    int x;
    int y;
};

struct Shape {
    std::string name;
    Point center;
};

CTL_DESCRIBE_STRUCT(Point, (), (x, y))
CTL_DESCRIBE_STRUCT(Shape, (), (name, center))

int main() {
    // Test ctl::get
    Shape s = {.name = "Circle", .center = {.x = 10, .y = 20}};
    
    auto name = ctl::get<ctl::field<&Shape::name>>(s);
    assert(name == "Circle");
    
    auto x_val = ctl::get<ctl::path<&Shape::center, &Point::x>>(s);
    assert(x_val == 10);
    
    // Test ctl::set
    ctl::set<ctl::field<&Shape::name>>(s, "Square");
    assert(s.name == "Square");
    
    ctl::set<ctl::path<&Shape::center, &Point::y>>(s, 30);
    assert(s.center.y == 30);
    
    // Test ctl::for_each_field
    bool found_name = false;
    ctl::for_each_field(s, [&](std::string_view field_name, const auto& value) {
        if (field_name == "name") {
            found_name = true;
        }
    });
    assert(found_name);
    
    std::cout << "Consumer test successful! ctl package works correctly.\n";
    return 0;
}
