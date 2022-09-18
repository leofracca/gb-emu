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
        void sendInterrupt();

    private:
        Memory *m_memory; ///< The memory
    };
} // namespace gameboy
