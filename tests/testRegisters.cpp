#define CATCH_CONFIG_MAIN
#include "../third_party/catch.hpp"
#include "../src/registers.cpp"

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

        REQUIRE(registers.getFlag(ZERO_FLAG) == true);
        REQUIRE(registers.getFlag(SUBTRACT_FLAG) == false);
        REQUIRE(registers.getFlag(HALF_CARRY_FLAG) == true);
        REQUIRE(registers.getFlag(CARRY_FLAG) == true);
    }

    TEST_CASE("Registers get/set", "[registers]")
    {
        Registers registers;
        registers.setFlag(ZERO_FLAG, false);
        REQUIRE(registers.getFlag(ZERO_FLAG) == false);
        registers.setFlag(ZERO_FLAG, true);
        REQUIRE(registers.getFlag(ZERO_FLAG) == true);

        registers.setFlag(SUBTRACT_FLAG, false);
        REQUIRE(registers.getFlag(SUBTRACT_FLAG) == false);
        registers.setFlag(SUBTRACT_FLAG, true);
        REQUIRE(registers.getFlag(SUBTRACT_FLAG) == true);

        registers.setFlag(HALF_CARRY_FLAG, false);
        REQUIRE(registers.getFlag(HALF_CARRY_FLAG) == false);
        registers.setFlag(HALF_CARRY_FLAG, true);
        REQUIRE(registers.getFlag(HALF_CARRY_FLAG) == true);

        registers.setFlag(CARRY_FLAG, false);
        REQUIRE(registers.getFlag(CARRY_FLAG) == false);
        registers.setFlag(CARRY_FLAG, true);
        REQUIRE(registers.getFlag(CARRY_FLAG) == true);
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
