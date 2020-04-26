
#include <catch2/catch.hpp>

#include "enclosure_list.hpp"
#include "enclosure_model.hpp"

std::string const path = GSPK_TEST_FILE_DIRECTORY;

using namespace gspk;

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
TEST_CASE("8 inch woofer - sealed enclosure model")
{
    // 8 inch woofer and T/S parameters
    auto const Qts = 0.45;
    auto const Qes = 0.53;
    auto const Qms = 3.0;
    auto const Fs = 31.5;
    auto const excursion_maximum = 3.5;
    auto const Sd = 2.15E-2;
    auto const Vd = 7.525E-5;
    auto const Vas = 84.1;

    SECTION("Minimal transient Qtc=0.5")
    {
        auto const Qtc = 0.5;
        REQUIRE(sealed::volume(Qtc, Vas, Qts) == Approx(358.5315));
        REQUIRE(sealed::resonance_frequency(Qtc, Fs, Qts) == Approx(35.0));
        REQUIRE(sealed::minus_3dB_rolloff(Qtc, sealed::resonance_frequency(Qtc, Fs, Qts))
                == Approx(54.382));
    }
    SECTION("0.7")
    {
        auto const Qtc = 0.7;
        REQUIRE(sealed::volume(Qtc, Vas, Qts) == Approx(59.2356521739));
        REQUIRE(sealed::resonance_frequency(Qtc, Fs, Qts) == Approx(49.0));
        REQUIRE(sealed::minus_3dB_rolloff(Qtc, sealed::resonance_frequency(Qtc, Fs, Qts))
                == Approx(49.5025246624));
    }
    SECTION("0.8")
    {
        auto const Qtc = 0.8;
        REQUIRE(sealed::volume(Qtc, Vas, Qts) == Approx(38.9262857143));
        REQUIRE(sealed::resonance_frequency(Qtc, Fs, Qts) == Approx(56.0));
        REQUIRE(sealed::minus_3dB_rolloff(Qtc, sealed::resonance_frequency(Qtc, Fs, Qts))
                == Approx(50.2409644953));
    }
    SECTION("0.9")
    {
        auto const Qtc = 0.9;
        REQUIRE(sealed::volume(Qtc, Vas, Qts) == Approx(0.99 * 28.3168));
        REQUIRE(sealed::resonance_frequency(Qtc, Fs, Qts) == Approx(63.0));
        REQUIRE(sealed::minus_3dB_rolloff(Qtc, sealed::resonance_frequency(Qtc, Fs, Qts))
                == Approx(52.2565187989));
    }
    SECTION("1.0")
    {
        auto const Qtc = 1.0;
        REQUIRE(sealed::volume(Qtc, Vas, Qts) == Approx(21.3545454545));
        REQUIRE(sealed::resonance_frequency(Qtc, Fs, Qts) == Approx(70.0));
        REQUIRE(sealed::minus_3dB_rolloff(Qtc, sealed::resonance_frequency(Qtc, Fs, Qts))
                == Approx(55.030596443));
    }
}
