#include "../third_party/catch.hpp"
#include "../include/mbc.h"

namespace gameboyTest
{
    using namespace gameboy;

    const std::string TEST_ROM = "../tests/roms/cpu_instrs.gb";
    uint8_t rom[127 * 16384];
    uint8_t ram[127 * 256];

    TEST_CASE("No MBC", "[mbc]")
    {
        ROMOnly romOnly(rom);

        for (int i = 0; i < 0x8000; i++)
        {
            romOnly.write(i, i);
            REQUIRE(romOnly.read(i) == 0x00);
        }
    }

    TEST_CASE("MBC1", "[mbc]")
    {
        MBC1 mbc1(rom, ram);

        for (int i = 0; i < 0x8000; i++)
        {
            mbc1.write(i, i);
        }
        for (int i = 0xA000; i < 0xC000; i++)
        {
            mbc1.write(i, i);
        }

        for (int i = 0; i < 0x4000; i++)
        {
            REQUIRE(mbc1.read(i) == 0x00);
        }
    }

    TEST_CASE("MBC2", "[mbc]")
    {
        MBC2 mbc2(rom, ram);

        for (int i = 0; i < 0x8000; i++)
        {
            mbc2.write(i, i);
        }
        for (int i = 0xA000; i < 0xC000; i++)
        {
            mbc2.write(i, i);
        }

        for (int i = 0; i < 0x4000; i++)
        {
            REQUIRE(mbc2.read(i) == 0x00);
        }
    }

    TEST_CASE("MBC3", "[mbc]")
    {
        MBC3 mbc3(rom, ram);

        for (int i = 0; i < 0x8000; i++)
        {
            mbc3.write(i, i);
        }
        for (int i = 0xA000; i < 0xC000; i++)
        {
            mbc3.write(i, i);
        }

        for (int i = 0; i < 0x4000; i++)
        {
            REQUIRE(mbc3.read(i) == 0x00);
        }
    }

    TEST_CASE("MBC5", "[mbc]")
    {
        MBC5 mbc5(rom, ram);

        for (int i = 0; i < 0x8000; i++)
        {
            mbc5.write(i, i);
        }
        for (int i = 0xA000; i < 0xC000; i++)
        {
            mbc5.write(i, i);
        }

        for (int i = 0; i < 0x4000; i++)
        {
            REQUIRE(mbc5.read(i) == 0x00);
        }
    }
} // namespace gameboyTest
