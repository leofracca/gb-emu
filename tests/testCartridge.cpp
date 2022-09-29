#include "../third_party/catch2/catch.hpp"
#include "../include/cartridge.h"

namespace gameboyTest
{
    using namespace gameboy;

    const std::string TEST_ROM = "../tests/roms/cpu_instrs.gb";

    const uint8_t TEST_ROM_TITLE[] = {0x43, 0x50, 0x55, 0x5f, 0x49, 0x4e, 0x53, 0x54, 0x52, 0x53};

    TEST_CASE("Cartridge", "[cartridge]")
    {
        Cartridge cartridge;

        cartridge.loadROM(TEST_ROM);

        int j = 0;
        for (int i = 0x0134; i < 0x0142; i++)
        {
            REQUIRE(cartridge.read(i) == TEST_ROM_TITLE[j++]);
        }
    }
} // namespace gameboyTest
