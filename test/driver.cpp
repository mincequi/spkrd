
#include <catch2/catch.hpp>

#include "speaker_list.hpp"

std::string const path = GSPK_TEST_FILE_DIRECTORY;

TEST_CASE("Populate enclosure list from xml")
{
    SECTION("Empty case") { REQUIRE(speaker_list().data().empty()); }
    SECTION("File not found exception")
    {
        REQUIRE_THROWS_AS(speaker_list(""), std::runtime_error);
    }
    SECTION("Test xml parsing")
    {
        auto drivers = speaker_list(path + "/vifa.xml");
        REQUIRE(drivers.data().size() == 7);

        {
            auto const& driver = drivers.data().at(0);
            REQUIRE(driver.get_type() == 3);
            REQUIRE(driver.get_qts() == Approx(0.20999));
            REQUIRE(driver.get_vas() == Approx(14));
            REQUIRE(driver.get_fs() == Approx(39));
            REQUIRE(driver.get_rdc() == Approx(5.5));
            REQUIRE(driver.get_lvc() == Approx(0.69999));
            REQUIRE(driver.get_qms() == Approx(1.39999));
            REQUIRE(driver.get_qes() == Approx(0.23999));
            REQUIRE(driver.get_imp() == Approx(8));
            REQUIRE(driver.get_sens() == Approx(87));
            REQUIRE(driver.get_mmd() == Approx(0.00958));
            REQUIRE(driver.get_ad() == Approx(0.04889));
            REQUIRE(driver.get_bl() == Approx(7.29999));
            REQUIRE(driver.get_rms() == Approx(1.8));
            REQUIRE(driver.get_cms() == Approx(1.7));
        }
    }
}
