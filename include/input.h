/**
 * @file input.h
 * @brief This file contains the declaration of the Input class.
 *        It emulates the behavior of a Gameboy controller/joypad.
 */

/*
 * See pages 35 to 37 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/Joypad_Input.html
 */

#pragma once

#include "memory.h" // Memory

namespace gameboy
{
    /**
     * @brief The different buttons of the joypad
     */
    enum class JoypadButton
    {
        BUTTON_A = 0x01, ///< The A button
        BUTTON_B = 0x02, ///< The B button
        BUTTON_SELECT = 0x04, ///< The SELECT button
        BUTTON_START = 0x08, ///< The START button
        DIRECTION_RIGHT = 0x10, ///< The RIGHT button
        DIRECTION_LEFT = 0x20, ///< The LEFT button
        DIRECTION_UP = 0x40, ///< The UP button
        DIRECTION_DOWN = 0x80 ///< The DOWN button
    };

    class Input
    {
    public:
        /**
         * @brief Construct a new Input object
         *
         * @param memory The memory
         */
        Input(Memory *memory);

        /**
         * @brief Set/Reset the bit of the joypad state
         *
         * @param button The button to set/reset
         * @param pressed True if the button is pressed, false otherwise
         */
        void setButton(JoypadButton button, bool pressed);

        /**
         * @brief Send an interrupt.
         *
         * @param memory The memory
         */
        void sendInterrupt(Memory *memory);

    private:
        /**
         * @brief The current state of the joypad
         * @details The state is inverted (0 = pressed, 1 = not pressed),
         *          bits 0 to 3 are used to store the state of the buttons,
         *          bits 4 to 7 are used to store the state of the directions.
         *          Based on which bit is set (technically, which bit is reset), the joypad will send an interrupt.
         *
         *          This variable does not store the state like the gameboy does/expects.
         *          To get the joypad state like the Gameboy does (and the games expect), we need to call the read function of the memory.
         *          This is done by the Memory class.
         */
        uint8_t m_currentJoypadState = 0; ///< The joypad state

        Memory *m_memory; ///< The memory
    };
} // namespace gameboy
