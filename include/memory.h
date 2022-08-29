/**
 * @file memory.h
 * @brief This file contains the declaration of the Memory class.
 *        It emulates the behavior of the memory of a Gameboy and the cartridge.
 */

/*
 * See pages 8 to 18 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/Memory_Map.html
 */

#pragma once

#include "cartridge.h" // Cartridge

namespace gameboy
{
    class Memory
    {
    public:
        Cartridge *m_cartridge; ///< The cartridge

        uint8_t m_memory[0x10000]; ///< The memory of the Gameboy

        /**
         * @brief Construct a new Memory object
         * @details Initialize the cartridge
         *
         * @param cartridge The cartridge of the memory
         */
        Memory(Cartridge *cartridge);

        /**
         * @brief Read a byte from the memory
         * @details Read a byte from the memory at the specified address and return it
         *
         * @param address The address to read from
         * @return uint8_t The byte read
         */
        uint8_t read(uint16_t address);

        /**
         * @brief Write a byte to the memory
         * @details Write a byte to the memory at the specified address
         *
         * @param address The address to write to
         * @param value The value to write
         */
        void write(uint16_t address, uint8_t value);

        /**
         * @brief Read a word from the memory
         * @details Read a word from the memory at the specified address and return it
         *
         * @param address The address to read from
         * @return uint16_t The word read
         */
        uint16_t readWord(uint16_t address);

        /**
         * @brief Write a word to the memory
         * @details Write a word to the memory at the specified address
         *
         * @param address The address to write to
         * @param value The value to write
         */
        void writeWord(uint16_t address, uint16_t value);
    };
}
