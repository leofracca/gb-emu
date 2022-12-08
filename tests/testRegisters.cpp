#include "catch.hpp"
#include "registers.h"

namespace gameboyTest
{
    using namespace gameboy;

    TEST_CASE("Registers init", "[registers]")
    {
        Registers registers;
        REQUIRE(registers.a == 0x01);
        REQUIRE(registers.f == 0xB0);
        REQUIRE(registers.b == 0x00);
        REQUIRE(registers.c == 0x13);
        REQUIRE(registers.d == 0x00);
        REQUIRE(registers.e == 0xD8);
        REQUIRE(registers.h == 0x01);
        REQUIRE(registers.l == 0x4D);
        REQUIRE(registers.sp == 0xFFFE);
        REQUIRE(registers.pc == 0x0100);

        REQUIRE(registers.getFlag(flags::ZERO_FLAG) == true);
        REQUIRE(registers.getFlag(flags::SUBTRACT_FLAG) == false);
        REQUIRE(registers.getFlag(flags::HALF_CARRY_FLAG) == true);
        REQUIRE(registers.getFlag(flags::CARRY_FLAG) == true);
    }

    TEST_CASE("Registers get/set", "[registers]")
    {
        Registers registers;
        registers.setFlag(flags::ZERO_FLAG, false);
        REQUIRE(registers.getFlag(flags::ZERO_FLAG) == false);
        registers.setFlag(flags::ZERO_FLAG, true);
        REQUIRE(registers.getFlag(flags::ZERO_FLAG) == true);

        registers.setFlag(flags::SUBTRACT_FLAG, false);
        REQUIRE(registers.getFlag(flags::SUBTRACT_FLAG) == false);
        registers.setFlag(flags::SUBTRACT_FLAG, true);
        REQUIRE(registers.getFlag(flags::SUBTRACT_FLAG) == true);

        registers.setFlag(flags::HALF_CARRY_FLAG, false);
        REQUIRE(registers.getFlag(flags::HALF_CARRY_FLAG) == false);
        registers.setFlag(flags::HALF_CARRY_FLAG, true);
        REQUIRE(registers.getFlag(flags::HALF_CARRY_FLAG) == true);

        registers.setFlag(flags::CARRY_FLAG, false);
        REQUIRE(registers.getFlag(flags::CARRY_FLAG) == false);
        registers.setFlag(flags::CARRY_FLAG, true);
        REQUIRE(registers.getFlag(flags::CARRY_FLAG) == true);
    }

    TEST_CASE("Registers get/set pairs", "[registers]")
    {
        Registers registers;
        registers.setAF(0x01B0);
        REQUIRE(registers.getAF() == 0x01B0);
        REQUIRE(registers.a == 0x01);
        REQUIRE(registers.f == 0xB0);

        registers.setBC(0x0013);
        REQUIRE(registers.getBC() == 0x0013);
        REQUIRE(registers.b == 0x00);
        REQUIRE(registers.c == 0x13);

        registers.setDE(0x00D8);
        REQUIRE(registers.getDE() == 0x00D8);
        REQUIRE(registers.d == 0x00);
        REQUIRE(registers.e == 0xD8);

        registers.setHL(0x014D);
        REQUIRE(registers.getHL() == 0x014D);
        REQUIRE(registers.h == 0x01);
        REQUIRE(registers.l == 0x4D);
    }
} // namespace gameboyTest
