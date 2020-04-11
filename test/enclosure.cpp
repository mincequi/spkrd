
#include <catch2/catch.hpp>

#include "enclosure_list.hpp"

std::string const path = GSPK_TEST_FILE_DIRECTORY;

TEST_CASE("Populate enclosure list from xml")
{
    SECTION("Empty case") { REQUIRE(enclosure_list().data().empty()); }
    SECTION("File not found exception")
    {
        REQUIRE_THROWS_AS(enclosure_list(""), std::runtime_error);
    }
    SECTION("Test xml parsing")
    {
        std::puts(GSPK_TEST_FILE_DIRECTORY);
        auto enclosures = enclosure_list(path + "/enclosure.xml");
        REQUIRE(enclosures.data().size() == 4);

        {
            auto const& enclosure = enclosures.data().at(0);
            REQUIRE(enclosure.get_id_string() == "Box: Wed Sep 18 14:59:49 2002");
            REQUIRE(enclosure.get_type() == 2);
            REQUIRE(enclosure.get_vb1() == Approx(23));
            REQUIRE(enclosure.get_fb1() == Approx(33));
            REQUIRE(enclosure.get_vb2() == Approx(0));
            REQUIRE(enclosure.get_fb2() == Approx(0));
        }
    }
}
