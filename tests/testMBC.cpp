#include "catch.hpp"
#include "mbc.h"

namespace gameboyTest
{
    using namespace gameboy;

    const std::string TEST_ROM = "test_roms/cpu_instrs.gb";
    std::vector<uint8_t> rom(65536, 0x00);
    std::vector<uint8_t> ram(65536, 0x00);

    TEST_CASE("MBCs", "[mbc]")
    {
        for (int i = 0; i < 0x10000; i++)
        {
            rom[i] = i;
            ram[i] = i;
        }

        SECTION("No MBC, ROM only")
        {
            ROMOnly romOnly(rom, ram);

            for (int i = 0; i < 0x8000; i++)
                REQUIRE(romOnly.read(i) == rom[i]);

            for (int i = 0x8000; i < 0x10000; i++)
                REQUIRE(romOnly.read(i) == 0x00);

            for (int i = 0; i < 0x8000; i++)
            {
                romOnly.write(i, 0x00);
                REQUIRE(romOnly.read(i) == rom[i]);
            }

            for (int i = 0x8000; i < 0x10000; i++)
                REQUIRE_THROWS_AS(romOnly.write(i, 0x00), std::runtime_error);
        }

        SECTION("MBC1")
        {
            MBC1 mbc1(rom, ram);

            for (int i = 0; i < 0x4000; i++)
                REQUIRE(mbc1.read(i) == rom[i]);

            int romBank;
            SECTION("ROM bank 1")
            {
                romBank = 1;
            }
            SECTION("ROM bank 2")
            {
                romBank = 2;
                mbc1.write(0x2000, 0x02);
            }

            for (int i = 0x4000; i < 0x8000; i++)
                REQUIRE(mbc1.read(i) == rom[romBank * 0x4000 + i - 0x4000]);

            for (int i = 0; i < 0x8000; i++)
            {
                mbc1.write(i, 0x00);
                REQUIRE(mbc1.read(i) == rom[i]);
            }

            for (int i = 0xA000; i < 0xC000; i++)
                REQUIRE(mbc1.read(i) == 0x00);
            mbc1.write(0x1FFF, 0x0A);
            for (int i = 0xA000; i < 0xC000; i++)
                REQUIRE(mbc1.read(i) == ram[i - 0xA000]);

            for (int i = 0xA000; i < 0xC000; i++)
            {
                mbc1.write(i, 0x00);
                REQUIRE(mbc1.read(i) == 0x00);
            }
        }

        SECTION("MBC2")
        {
            MBC2 mbc2(rom, ram);

            for (int i = 0; i < 0x4000; i++)
                REQUIRE(mbc2.read(i) == rom[i]);

            int romBank;
            SECTION("ROM bank 1")
            {
                romBank = 1;
            }
            SECTION("ROM bank 2")
            {
                romBank = 2;
                mbc2.write(0x2000, 0x02);
            }

            for (int i = 0x4000; i < 0x8000; i++)
                REQUIRE(mbc2.read(i) == rom[romBank * 0x4000 + i - 0x4000]);

            for (int i = 0; i < 0x8000; i++)
            {
                mbc2.write(i, 0x00);
                REQUIRE(mbc2.read(i) == rom[i]);
            }

            for (int i = 0xA000; i < 0xC000; i++)
                REQUIRE(mbc2.read(i) == 0x00);
            mbc2.write(0x0000, 0x0A);
            for (int i = 0xA000; i < 0xC000; i++)
                REQUIRE(mbc2.read(i) == ram[i - 0xA000]);

            for (int i = 0xA000; i < 0xC000; i++)
            {
                mbc2.write(i, 0x00);
                REQUIRE(mbc2.read(i) == 0x00);
            }
        }

        SECTION("MBC3")
        {
            MBC3 mbc3(rom, ram);

            for (int i = 0; i < 0x4000; i++)
                REQUIRE(mbc3.read(i) == rom[i]);

            int romBank;
            SECTION("ROM bank 1")
            {
                romBank = 1;
            }
            SECTION("ROM bank 2")
            {
                romBank = 2;
                mbc3.write(0x2000, 0x02);
            }

            for (int i = 0x4000; i < 0x8000; i++)
                REQUIRE(mbc3.read(i) == rom[romBank * 0x4000 + i - 0x4000]);

            for (int i = 0; i < 0x8000; i++)
            {
                mbc3.write(i, 0x00);
                REQUIRE(mbc3.read(i) == rom[i]);
            }

            for (int i = 0xA000; i < 0xC000; i++)
                REQUIRE(mbc3.read(i) == 0x00);
            mbc3.write(0x1FFF, 0x0A);
            for (int i = 0xA000; i < 0xC000; i++)
                REQUIRE(mbc3.read(i) == ram[i - 0xA000]);

            for (int i = 0xA000; i < 0xC000; i++)
            {
                mbc3.write(i, 0x00);
                REQUIRE(mbc3.read(i) == 0x00);
            }
        }

        SECTION("MBC5")
        {
            MBC5 mbc5(rom, ram);

            for (int i = 0; i < 0x4000; i++)
                REQUIRE(mbc5.read(i) == rom[i]);

            int romBank;
            SECTION("ROM bank 1")
            {
                romBank = 1;
            }
            SECTION("ROM bank 2")
            {
                romBank = 2;
                mbc5.write(0x2000, 0x02);
            }

            for (int i = 0x4000; i < 0x8000; i++)
                REQUIRE(mbc5.read(i) == rom[romBank * 0x4000 + i - 0x4000]);

            for (int i = 0; i < 0x8000; i++)
            {
                mbc5.write(i, 0x00);
                REQUIRE(mbc5.read(i) == rom[i]);
            }

            for (int i = 0xA000; i < 0xC000; i++)
                REQUIRE(mbc5.read(i) == 0x00);
            mbc5.write(0x1FFF, 0x0A);
            for (int i = 0xA000; i < 0xC000; i++)
                REQUIRE(mbc5.read(i) == ram[i - 0xA000]);

            for (int i = 0xA000; i < 0xC000; i++)
            {
                mbc5.write(i, 0x00);
                REQUIRE(mbc5.read(i) == 0x00);
            }
        }
    }
} // namespace gameboyTest
