#include "catch.hpp"
#include "cartridge.h"

namespace gameboyTest
{
    using namespace gameboy;

    const std::string TEST_ROM = "test_roms/cpu_instrs.gb";

    constexpr uint8_t TEST_ROM_TITLE[] = {0x43, 0x50, 0x55, 0x5F, 0x49, 0x4E, 0x53, 0x54, 0x52, 0x53}; //  = CPU_INSTRS

    TEST_CASE("Cartridge", "[cartridge]")
    {
        Cartridge cartridge;

        cartridge.loadROM(TEST_ROM);

        // Check that the ROM title is correct
        std::string title;
        for (int i = 0x0134; i < 0x0143; i++)
        {
            if (cartridge.read(i) == 0x00)
                break;
            title += static_cast<char>(cartridge.read(i));
        }

        REQUIRE(title == "CPU_INSTRS");
    }
} // namespace gameboyTest
