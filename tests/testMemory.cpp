#include "catch.hpp"
#include "memory.h"
#include "cartridge.h"

namespace gameboyTest
{
    using namespace gameboy;

    const std::string TEST_ROM = "test_roms/cpu_instrs.gb";
    
    TEST_CASE("Memory init", "[memory]")
    {
        Cartridge cartridge{};
        Memory memory(cartridge);

        REQUIRE(memory[0xFF00] == 0xCF);
        REQUIRE(memory[0xFF01] == 0x00);
        REQUIRE(memory[0xFF02] == 0x7E);
        REQUIRE(memory[0xFF04] == 0x00);
        REQUIRE(memory[0xFF05] == 0x00);
        REQUIRE(memory[0xFF06] == 0x00);
        REQUIRE(memory[0xFF07] == 0x00);
        REQUIRE(memory[0xFF0F] == 0xE1);
        REQUIRE(memory[0xFF10] == 0x80);
        REQUIRE(memory[0xFF11] == 0xBF);
        REQUIRE(memory[0xFF12] == 0xF3);
        REQUIRE(memory[0xFF14] == 0xBF);
        REQUIRE(memory[0xFF16] == 0x3F);
        REQUIRE(memory[0xFF17] == 0x00);
        REQUIRE(memory[0xFF19] == 0xBF);
        REQUIRE(memory[0xFF1A] == 0x7F);
        REQUIRE(memory[0xFF1B] == 0xFF);
        REQUIRE(memory[0xFF1C] == 0x9F);
        REQUIRE(memory[0xFF1E] == 0xBF);
        REQUIRE(memory[0xFF20] == 0xFF);
        REQUIRE(memory[0xFF21] == 0x00);
        REQUIRE(memory[0xFF22] == 0x00);
        REQUIRE(memory[0xFF23] == 0xBF);
        REQUIRE(memory[0xFF24] == 0x77);
        REQUIRE(memory[0xFF25] == 0xF3);
        REQUIRE(memory[0xFF26] == 0xF1);
        REQUIRE(memory[0xFF40] == 0x91);
        REQUIRE(memory[0xFF42] == 0x00);
        REQUIRE(memory[0xFF43] == 0x00);
        REQUIRE(memory[0xFF45] == 0x00);
        REQUIRE(memory[0xFF47] == 0xFC);
        REQUIRE(memory[0xFF48] == 0xFF);
        REQUIRE(memory[0xFF49] == 0xFF);
        REQUIRE(memory[0xFF4A] == 0x00);
        REQUIRE(memory[0xFF4B] == 0x00);
        REQUIRE(memory[0xFFFF] == 0x00);

        // Check the joypad
        REQUIRE(memory.getJoypadState() == 0xFF);
    }

    TEST_CASE("Joypad state", "[memory]")
    {
        Cartridge cartridge{};
        Memory memory(cartridge);

        memory.setJoypadState(0x0F);
        REQUIRE(memory.getJoypadState() == 0x0F);

        memory.setJoypadState(0xF0);
        REQUIRE(memory.getJoypadState() == 0xF0);

        memory[0xFF00] = 0x10;
        memory.setJoypadState(0x0E);
        REQUIRE(memory.read(0xFF00) == 0x1E);

        memory[0xFF00] = 0x20;
        memory.setJoypadState(0xEF);
        REQUIRE(memory.read(0xFF00) == 0x2E);
    }

    TEST_CASE("Unusable memory", "[memory]")
    {
        Cartridge cartridge{};
        Memory memory(cartridge);

        for (uint16_t i = 0xE000; i < 0xFE00; i++)
        {
            memory[i] = 0x00;
            memory.write(i, 0x01);
            REQUIRE(memory[i] == 0x00);
            REQUIRE(memory.read(i) == 0x01);
            REQUIRE(memory.read(i - 0x2000) == 0x01);
        }

        for (uint16_t i = 0xFEA0; i < 0xFF00; i++)
        {
            memory[i] = 0x00;
            memory.write(i, 0x01);
            REQUIRE(memory[i] == 0x00);
        }
    }
} // namespace gameboyTest
