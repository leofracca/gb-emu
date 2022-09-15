/*
 * See pages 35 to 37 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/Joypad_Input.html
 */

#include "../include/input.h" // Input

namespace gameboy
{
    Input::Input(Memory *memory)
        : m_memory(memory)
    {}

    void Input::setButton(JoypadButton button, bool pressed)
    {
        // Remember that the joypad state is inverted (0 = pressed, 1 = not pressed)
        if (pressed)
            m_currentJoypadState &= ~(static_cast<uint8_t>(button));
        else
            m_currentJoypadState |= static_cast<uint8_t>(button);
    }

    void Input::sendInterrupt()
    {
        // Set the interrupt
        uint8_t interruptFlag = m_memory->read(INTERRUPT_FLAG_ADDRESS);
        interruptFlag |= 0x10;
        m_memory->write(INTERRUPT_FLAG_ADDRESS, interruptFlag);

        // Save the joypad state
        m_memory->setJoypadState(m_currentJoypadState);
    }
} // namespace gameboy
