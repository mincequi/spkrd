
#include <catch2/catch.hpp>

#include "passive_component.hpp"
#include "crossover_list.hpp"

std::string const path = GSPK_TEST_FILE_DIRECTORY;

using namespace spkrd;

TEST_CASE("Populate crossover list from xml")
{
    SECTION("Empty case") { REQUIRE(crossover_list().data().empty()); }
    SECTION("File not found exception")
    {
        REQUIRE_THROWS_AS(crossover_list(""), std::runtime_error);
    }
    SECTION("Crossover1.xml")
    {
        auto crossovers = crossover_list(path + "/crossover1.xml");
        REQUIRE(crossovers.data().size() == 8);

        {
            auto const& crossover = crossovers.data().at(0);

            REQUIRE(crossover.get_type() == 8);

            {
                auto const& network = crossover.networks().at(0);
                REQUIRE(network.get_type() == 1);
                REQUIRE(network.get_has_imp_corr() == false);
                REQUIRE(network.get_has_damp() == false);
                REQUIRE(network.get_has_res() == false);
                REQUIRE(network.get_adv_imp_model() == 0);
                REQUIRE(network.get_inv_pol() == false);
                REQUIRE(network.get_speaker() == "Scan-Speak 15W/8530");
                REQUIRE(network.get_lowpass_family() == 1);
                REQUIRE(network.get_highpass_family() == 1);

                REQUIRE(network.parts().at(0).get_value() == Approx(0.95525));
                REQUIRE(network.parts().at(0).get_unit() == "m");
                REQUIRE(network.parts().at(0).get_type() == 2);

                REQUIRE(network.parts().at(1).get_value() == Approx(16.3534));
                REQUIRE(network.parts().at(1).get_unit() == "u");
                REQUIRE(network.parts().at(1).get_type() == 1);

                REQUIRE(network.parts().at(2).get_value() == Approx(0.19116));
                REQUIRE(network.parts().at(2).get_unit() == "m");
                REQUIRE(network.parts().at(2).get_type() == 2);
            }
        }
    }
}

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
