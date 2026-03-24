#pragma once

#include <ctl/describe.hpp>

#include <array>
#include <memory>
#include <string>

struct EmptyRecord {};

struct Coordinates {
    double latitude;
    double longitude;
};

struct GeoAddress {
    std::string city;
    Coordinates coordinates;
};

struct GeoPerson {
    std::string name;
    GeoAddress address;
};

struct MoveOnlyRecord {
    std::unique_ptr<int> payload;
    int version;
};

enum class Mode {
    draft,
    published,
    archived,
};

struct ArrayEnumRecord {
    std::array<int, 5> values;
    Mode mode;
};

CTL_DESCRIBE_STRUCT(EmptyRecord, (), ())
CTL_DESCRIBE_STRUCT(Coordinates, (), (latitude, longitude))
CTL_DESCRIBE_STRUCT(GeoAddress, (), (city, coordinates))
CTL_DESCRIBE_STRUCT(GeoPerson, (), (name, address))
CTL_DESCRIBE_STRUCT(MoveOnlyRecord, (), (payload, version))
CTL_DESCRIBE_STRUCT(ArrayEnumRecord, (), (values, mode))
