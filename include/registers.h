/**
 * @file registers.h
 * @brief This file contains the declaration of the Registers class.
 *        It emulates the behavior of the registers of the Gameboy.
 */

/*
 * See pages 17-18 and 60 to 64 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/CPU_Registers_and_Flags.html
 * See https://gbdev.io/pandocs/Power_Up_Sequence.html
 */

#pragma once

#include <cstdint> // uint8_t, uint16_t

namespace gameboy
{
    namespace flags
    {
        /*
         * The following are the flags of the flag register F
         */
        constexpr uint8_t ZERO_FLAG = 0x80; ///< The zero flag (Z)
        constexpr uint8_t SUBTRACT_FLAG = 0x40; ///< The subtract flag (N)
        constexpr uint8_t HALF_CARRY_FLAG = 0x20; ///< The half carry flag (H)
        constexpr uint8_t CARRY_FLAG = 0x10; ///< The carry flag (C)
    } // namespace flags

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
         *          registers A,B,C,D,E,F,H, & L as 16-bit registers by
         *          pairing them up in the following manner: AF,BC,DE, &
         *          HL.
         */
        uint8_t a; ///< A register
        uint8_t f; ///< F register
        uint8_t b; ///< B register
        uint8_t c; ///< C register
        uint8_t d; ///< D register
        uint8_t e; ///< E register
        uint8_t h; ///< H register
        uint8_t l; ///< L register

        uint16_t sp; ///< Stack Pointer
        uint16_t pc; ///< Program Counter

        /**
         * @brief Registers constructor
         * @details Initialize the registers with the values they must have at the start
         */
        Registers();

        // Getter and setter for pairs of registers (2 bytes instead of 1)
        /**
         * @brief Get the pair AF
         * @return The value of the pair AF
         */
        [[nodiscard]] uint16_t getAF() const;

        /**
         * @brief Get the pair BC
         * @return The value of the pair BC
         */
        [[nodiscard]] uint16_t getBC() const;

        /**
         * @brief Get the pair DE
         * @return The value of the pair DE
         */
        [[nodiscard]] uint16_t getDE() const;

        /**
         * @brief Get the pair HL
         * @return The value of the pair HL
         */
        [[nodiscard]] uint16_t getHL() const;

        /**
         * @brief Set the pair AF
         * @param value The value to set the pair AF to
         */
        void setAF(uint16_t value);

        /**
         * @brief Set the pair BC
         * @param value The value to set the pair BC to
         */
        void setBC(uint16_t value);

        /**
         * @brief Set the pair DE
         * @param value The value to set the pair DE to
         */
        void setDE(uint16_t value);

        /**
         * @brief Set the pair HL
         * @param value The value to set the pair HL to
         */
        void setHL(uint16_t value);

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
        [[nodiscard]] bool getFlag(uint8_t flag) const;

    private:
        /**
         * @brief Get the value of a pair of registers
         *
         * @param high The high register of the pair
         * @param low The low register of the pair
         * @return The value of the pair
         */
        static uint16_t getRegisterPair(uint8_t high, uint8_t low);

        /**
         * @brief Set the value of a pair of registers
         *
         * @param high The high register of the pair
         * @param low The low register of the pair
         * @param value The value to set the pair to
         */
        static void setRegisterPair(uint8_t &high, uint8_t &low, uint16_t value);
    };
} // namespace gameboy
