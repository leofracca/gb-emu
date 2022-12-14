/*
 * See pages 35 to 37 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/Joypad_Input.html
 */

#include "input.h" // Input

namespace gameboy
{
    Input::Input(Memory &memory)
        : m_memory(memory)
    {}

    void Input::setButton(JoypadButton button, bool pressed)
    {
        uint8_t currentJoypadState = m_memory.getJoypadState();

        // Remember that the joypad state is inverted (0 = pressed, 1 = not pressed)
        if (pressed)
            currentJoypadState &= ~(static_cast<uint8_t>(button));
        else
            currentJoypadState |= static_cast<uint8_t>(button);

        // Save the joypad state
        m_memory.setJoypadState(currentJoypadState);
    }

    void Input::sendInterrupt()
    {
        // Set the interrupt
        uint8_t interruptFlag = m_memory.read(interrupt_registers::INTERRUPT_FLAG_ADDRESS);
        interruptFlag |= JOYPAD_INTERRUPT_FLAG_VALUE;
        m_memory.write(interrupt_registers::INTERRUPT_FLAG_ADDRESS, interruptFlag);
    }
} // namespace gameboy
