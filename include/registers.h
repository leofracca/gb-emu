/**
 * @file registers.h
 * @brief This file contains the declaration of the Registers class.
 *        It emulates the behavior of the registers of the Gameboy.
 */

/*
 * See pages 60 to 64 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/CPU_Registers_and_Flags.html
 */

#pragma once

#include <cstdint> // uint8_t, uint16_t

namespace gameboy
{
    /*
     * The following are the flags of the flag register F
     */
    constexpr uint8_t ZERO_FLAG = 0x80; // Z
    constexpr uint8_t SUBTRACT_FLAG = 0x40; // N
    constexpr uint8_t HALF_CARRY_FLAG = 0x20; // H
    constexpr uint8_t CARRY_FLAG = 0x10; // C

    /**
     * @brief Registers class used to manipulate the registers of the Gameboy
     */
    class Registers
    {
    public:
        /**
         * @brief Registers
         * @details The Game Boy has eight 8-bit registers A,B,C,D,E,F,H,L
         *          and two 16-bit registers SP & PC.
         *          Some instructions, however, allow you to use the
         *          registers A,B,C,D,E,H, & L as 16-bit registers by
         *          pairing them up in the following manner: AF,BC,DE, &
         *          HL.
         *          The following structure comes from this observation.
         *          In this way, one can use the registers alone or in
         *          pairs.
         */
        struct
        {
            union
            {
                struct
                {
                    uint8_t f;
                    uint8_t a;
                };
                uint16_t af;
            };
        };

        struct
        {
            union
            {
                struct
                {
                    uint8_t c;
                    uint8_t b;
                };
                uint16_t bc;
            };
        };

        struct
        {
            union
            {
                struct
                {
                    uint8_t e;
                    uint8_t d;
                };
                uint16_t de;
            };
        };

        struct
        {
            union
            {
                struct
                {
                    uint8_t l;
                    uint8_t h;
                };
                uint16_t hl;
            };
        };

        uint16_t sp; ///< Stack Pointer
        uint16_t pc; ///< Program Counter

    public:
        // Flag register getter and setter
        /**
         * @brief Set the value of a bit of the flag register
         * @details The Flag Register (F) consists of the following bits:
         *   7 6 5 4 3 2 1 0
         *   Z N H C 0 0 0 0
         *
         * Z - Zero Flag
         *   This bit is set when the result of a math operation
         *   is zero or two values match when using the CP
         *   instruction.
         *
         * N - Subtract Flag
         *   This bit is set if a subtraction was performed in the
         *   last math instruction.
         *
         * H - Half Carry Flag
         *   This bit is set if a carry occurred from the lower nibble
         *   to the upper nibble during a math operation.
         *
         * C - Carry Flag
         *   This bit is set if a carry occurred from the lower byte to
         *   the upper byte during a math operation or if the register
         *   A is the smaller value when executing the CP instruction.
         *
         * @param flag The flag to set
         * @param value The value to set the flag to
         *
         * @see RegisterFlag
         */
        void setFlag(uint8_t flag, bool value);

        /**
         * @brief Get the value of a bit of the flag register
         * @details The Flag Register (F) consists of the following bits:
         *   7 6 5 4 3 2 1 0
         *   Z N H C 0 0 0 0
         *
         * Z - Zero Flag
         *   This bit is set when the result of a math operation
         *   is zero or two values match when using the CP
         *   instruction.
         *
         * N - Subtract Flag
         *   This bit is set if a subtraction was performed in the
         *   last math instruction.
         *
         * H - Half Carry Flag
         *   This bit is set if a carry occurred from the lower nibble
         *   to the upper nibble during a math operation.
         *
         * C - Carry Flag
         *   This bit is set if a carry occurred from the lower byte to
         *   the upper byte during a math operation or if the register
         *   A is the smaller value when executing the CP instruction.
         *
         * @param flag The flag to get the value of
         * @return true If the flag is set (1)
         * @return false If the flag is not set (0)
         *
         * @see RegisterFlag
         */
        bool getFlag(uint8_t flag) const;
    };
} // namespace gameboy
