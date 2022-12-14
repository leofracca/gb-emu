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

    TEST_CASE("Registers get/set flags", "[registers]")
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

        REQUIRE(registers.getAF() == 0x01B0);
        REQUIRE(registers.a == 0x01);
        REQUIRE(registers.f == 0xB0);
        for (auto i = 0; i < 0x10000; i++)
        {
            registers.setAF(i);
            REQUIRE(registers.getAF() == i);
            REQUIRE(registers.a == (i >> 8));
            REQUIRE(registers.f == (i & 0xFF));
        }

        REQUIRE(registers.getBC() == 0x0013);
        REQUIRE(registers.b == 0x00);
        REQUIRE(registers.c == 0x13);
        for (auto i = 0; i < 0x10000; i++)
        {
            registers.setBC(i);
            REQUIRE(registers.getBC() == i);
            REQUIRE(registers.b == (i >> 8));
            REQUIRE(registers.c == (i & 0xFF));
        }

        REQUIRE(registers.getDE() == 0x00D8);
        REQUIRE(registers.d == 0x00);
        REQUIRE(registers.e == 0xD8);
        for (auto i = 0; i < 0x10000; i++)
        {
            registers.setDE(i);
            REQUIRE(registers.getDE() == i);
            REQUIRE(registers.d == (i >> 8));
            REQUIRE(registers.e == (i & 0xFF));
        }

        REQUIRE(registers.getHL() == 0x014D);
        REQUIRE(registers.h == 0x01);
        REQUIRE(registers.l == 0x4D);
        for (auto i = 0; i < 0x10000; i++)
        {
            registers.setHL(i);
            REQUIRE(registers.getHL() == i);
            REQUIRE(registers.h == (i >> 8));
            REQUIRE(registers.l == (i & 0xFF));
        }
    }
} // namespace gameboyTest
