#include "catch.hpp"
#include "input.h"
#include "memory.h"
#include "cartridge.h"

namespace gameboyTest
{
    using namespace gameboy;

    TEST_CASE("Input setButton", "[input]")
    {
        Cartridge cartridge{};
        Memory memory(cartridge);
        Input input(memory);
        input.setButton(JoypadButton::BUTTON_A, true);
        REQUIRE(memory.getJoypadState() == 0xFE);
        input.setButton(JoypadButton::BUTTON_A, false);
        REQUIRE(memory.getJoypadState() == 0xFF);
        input.setButton(JoypadButton::BUTTON_B, true);
        REQUIRE(memory.getJoypadState() == 0xFD);
        input.setButton(JoypadButton::BUTTON_B, false);
        REQUIRE(memory.getJoypadState() == 0xFF);
        input.setButton(JoypadButton::BUTTON_SELECT, true);
        REQUIRE(memory.getJoypadState() == 0xFB);
        input.setButton(JoypadButton::BUTTON_SELECT, false);
        REQUIRE(memory.getJoypadState() == 0xFF);
        input.setButton(JoypadButton::BUTTON_START, true);
        REQUIRE(memory.getJoypadState() == 0xF7);
        input.setButton(JoypadButton::BUTTON_START, false);
        REQUIRE(memory.getJoypadState() == 0xFF);
        input.setButton(JoypadButton::DIRECTION_UP, true);
        REQUIRE(memory.getJoypadState() == 0xBF);
        input.setButton(JoypadButton::DIRECTION_UP, false);
        REQUIRE(memory.getJoypadState() == 0xFF);
        input.setButton(JoypadButton::DIRECTION_DOWN, true);
        REQUIRE(memory.getJoypadState() == 0x7F);
        input.setButton(JoypadButton::DIRECTION_DOWN, false);
        REQUIRE(memory.getJoypadState() == 0xFF);
        input.setButton(JoypadButton::DIRECTION_LEFT, true);
        REQUIRE(memory.getJoypadState() == 0xDF);
        input.setButton(JoypadButton::DIRECTION_LEFT, false);
        REQUIRE(memory.getJoypadState() == 0xFF);
        input.setButton(JoypadButton::DIRECTION_RIGHT, true);
        REQUIRE(memory.getJoypadState() == 0xEF);
        input.setButton(JoypadButton::DIRECTION_RIGHT, false);
        REQUIRE(memory.getJoypadState() == 0xFF);
    }

    TEST_CASE("Input sendInterrupt", "[input]")
    {
        Cartridge cartridge{};
        Memory memory(cartridge);
        Input input(memory);
        input.sendInterrupt();
        REQUIRE(memory.read(interrupt_registers::INTERRUPT_FLAG_ADDRESS) == (0xE1 | 0x10));
    }
} // namespace gameboyTest
