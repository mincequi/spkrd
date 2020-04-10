
#include <catch2/catch.hpp>

#include "passive_component.hpp"

TEST_CASE("Part type interface")
{
    REQUIRE(PART_TYPE_CAPACITOR == 1);
    REQUIRE(PART_TYPE_INDUCTOR == 2);
    REQUIRE(PART_TYPE_RESISTOR == 3);
}

TEST_CASE("Passive component")
{
    auto capacitor = passive_component(PART_TYPE_CAPACITOR, 0.5, "m");

    REQUIRE(capacitor.get_value() == 0.5);
    REQUIRE(capacitor.get_unit() == "m");

    capacitor.set_value(1.5);
    capacitor.set_unit("meg");

    REQUIRE(capacitor.get_value() == 1.5);
    REQUIRE(capacitor.get_unit() == "meg");
}
