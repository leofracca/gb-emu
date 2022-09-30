#include "catch.hpp"
#include "memory.h"

namespace gameboyTest
{
    using namespace gameboy;

    const std::string TEST_ROM = "cpu_instrs.gb";

    const uint8_t TEST_ROM_TITLE[] = {0x43, 0x50, 0x55, 0x5f, 0x49, 0x4e, 0x53, 0x54, 0x52, 0x53};

    TEST_CASE("Memory init", "[memory]")
    {
        Memory memory(TEST_ROM);

        // Check if the ROM is loaded correctly
        int j = 0;
        for (int i = 0x0134; i < 0x0142; i++)
        {
            REQUIRE(memory.read(i) == TEST_ROM_TITLE[j++]);
        }

        // Check the joypad
        REQUIRE(memory.getJoypadState() == 0xFF);
    }

    TEST_CASE("Joypad state", "[memory]")
    {
        Memory memory(TEST_ROM);

        memory.setJoypadState(0x0F);
        REQUIRE(memory.getJoypadState() == 0x0F);

        memory.setJoypadState(0xF0);
        REQUIRE(memory.getJoypadState() == 0xF0);
    }
} // namespace gameboyTest
