/**
 * @file timer.h
 * @brief This file contains the declaration of the Timer class.
 *        It emulates the behavior of the system timer of a Gameboy.
 */

/*
 * See pages 30 to 40 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/Timer_and_Divider_Registers.html
 */

#pragma once

#include "memory.h" // Memory

#include <cstdint> // uint8_t, uint16_t, uint32_t, uint64_t

namespace gameboy
{
    constexpr uint16_t DIV_REG_ADDRESS = 0xFF04; ///< The address of the Divider Register
    constexpr uint16_t TIMA_REG_ADDRESS = 0xFF05; ///< The address of the Timer Counter Register
    constexpr uint16_t TMA_REG_ADDRESS = 0xFF06; ///< The address of the Timer Modulo Register
    constexpr uint16_t TAC_REG_ADDRESS = 0xFF07; ///< The address of the Timer Control Register

    constexpr uint8_t TIMER_OVERFLOW_INTERRUPT_FLAG_VALUE = 0x04; ///< The bitmask of the Timer Interrupt Flag

    /**
     * @brief The Timer class emulates the behavior of the system timer of a Gameboy.
     */
    class Timer
    {
    public:
        /**
         * @brief Construct a new Timer object
         * @details Initialize all registers to 0
         *
         * @param memory The memory
         */
        explicit Timer(Memory *memory);

        /**
         * @brief Increment and manipulate the timer registers
         * @details Manipulate the timer registers and set the interrupt flag if necessary.
         *          At the end save the new value of the registers in memory.
         *
         * @param cycles The number of cycles to increment the timer by
         */
        void cycle(int cycles);

    private:
        Memory *m_memory; ///< The memory

        /**
         * @brief This register is incremented 16384 times a second.
         *        Writing any value sets it to $00.
         */
        uint8_t m_div = 0; ///< Divider register (0xFF04)

        /**
         * @brief This register is incremented at the rate (clock
         *        frequency) specified by the TAC register ($FF07).
         *        When the value overflows (exceeds $FF),
         *        it is reset to the value specified in TMA ($FF06)
         *        and an interrupt is requested.
         */
        uint8_t m_tima = 0; ///< Timer counter register (0xFF05)

        /**
         * @brief This register contains the value to
         *        load into the TIMA when the TIMA overflows.
         */
        uint8_t m_tma = 0; ///< Timer modulo register (0xFF06)

        /**
         * @brief This register controls the timer.
         *        Bit 2 - Timer Stop
         *               0: Stop Timer
         *               1: Start Timer
         *        Bits 1+0 - Input Clock Select
         *               00: CPU Clock / 1024 = 4.096 KHz   (~4.194 KHz SGB)
         *               01: CPU Clock / 16   = 262.144 Khz (~268.4 KHz SGB)
         *               10: CPU Clock / 64   = 65.536 KHz  (~67.11 KHz SGB)
         *               11: CPU Clock / 256  = 16.384 KHz  (~16.78 KHz SGB)
         */
        uint8_t m_tac = 0; ///< Timer control register (0xFF07)

        int m_divCycles = 0; ///< Divider register cycles
        int m_timaCycles = 0; ///< Timer counter cycles

        /**
         * @brief Get the current value of the timer registers
         * @details Save the current value of the registers in the member variables of this class
         */
        void getRegisters();

        /**
         * @brief Set the value of the timer registers
         * @details Set the value of the timer registers in the memory
         */
        void setRegisters();
    };
} // namespace gameboy