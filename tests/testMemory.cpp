#include "catch.hpp"
#include "memory.h"

namespace gameboyTest
{
    using namespace gameboy;

    const std::string TEST_ROM = "test_roms/cpu_instrs.gb";

    constexpr uint8_t TEST_ROM_TITLE[] = {0x43, 0x50, 0x55, 0x5F, 0x49, 0x4E, 0x53, 0x54, 0x52, 0x53}; //  = CPU_INSTRS

    TEST_CASE("Memory init", "[memory]")
    {
        Memory memory(TEST_ROM);

        REQUIRE(memory.m_memory[0xFF00] == 0xCF);
        REQUIRE(memory.m_memory[0xFF01] == 0x00);
        REQUIRE(memory.m_memory[0xFF02] == 0x7E);
        REQUIRE(memory.m_memory[0xFF04] == 0x00);
        REQUIRE(memory.m_memory[0xFF05] == 0x00);
        REQUIRE(memory.m_memory[0xFF06] == 0x00);
        REQUIRE(memory.m_memory[0xFF07] == 0x00);
        REQUIRE(memory.m_memory[0xFF0F] == 0xE1);
        REQUIRE(memory.m_memory[0xFF10] == 0x80);
        REQUIRE(memory.m_memory[0xFF11] == 0xBF);
        REQUIRE(memory.m_memory[0xFF12] == 0xF3);
        REQUIRE(memory.m_memory[0xFF14] == 0xBF);
        REQUIRE(memory.m_memory[0xFF16] == 0x3F);
        REQUIRE(memory.m_memory[0xFF17] == 0x00);
        REQUIRE(memory.m_memory[0xFF19] == 0xBF);
        REQUIRE(memory.m_memory[0xFF1A] == 0x7F);
        REQUIRE(memory.m_memory[0xFF1B] == 0xFF);
        REQUIRE(memory.m_memory[0xFF1C] == 0x9F);
        REQUIRE(memory.m_memory[0xFF1E] == 0xBF);
        REQUIRE(memory.m_memory[0xFF20] == 0xFF);
        REQUIRE(memory.m_memory[0xFF21] == 0x00);
        REQUIRE(memory.m_memory[0xFF22] == 0x00);
        REQUIRE(memory.m_memory[0xFF23] == 0xBF);
        REQUIRE(memory.m_memory[0xFF24] == 0x77);
        REQUIRE(memory.m_memory[0xFF25] == 0xF3);
        REQUIRE(memory.m_memory[0xFF26] == 0xF1);
        REQUIRE(memory.m_memory[0xFF40] == 0x91);
        REQUIRE(memory.m_memory[0xFF42] == 0x00);
        REQUIRE(memory.m_memory[0xFF43] == 0x00);
        REQUIRE(memory.m_memory[0xFF45] == 0x00);
        REQUIRE(memory.m_memory[0xFF47] == 0xFC);
        REQUIRE(memory.m_memory[0xFF48] == 0xFF);
        REQUIRE(memory.m_memory[0xFF49] == 0xFF);
        REQUIRE(memory.m_memory[0xFF4A] == 0x00);
        REQUIRE(memory.m_memory[0xFF4B] == 0x00);
        REQUIRE(memory.m_memory[0xFFFF] == 0x00);

        // Check that the ROM title is correct
        std::string title;
        for (int i = 0x0134; i < 0x0143; i++)
        {
            if (memory.read(i) == 0x00)
                break;
            title += static_cast<char>(memory.read(i));
        }

        REQUIRE(title == "CPU_INSTRS");

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
