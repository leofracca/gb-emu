/**
 * @file cartridge.h
 * @brief This file contains the declaration of the Cartridge class.
 *        It emulates the behavior of a Gameboy cartridge.
 */

/*
 * See pages 8 to 16 of the documentation (docs/GB.pdf)
 */

#pragma once

#include "mbc.h" // MBC

#include <string> // std::string

namespace gameboy
{
    constexpr uint16_t CARTRIDGE_TYPE_ADDRESS = 0x0147; ///< The address of the cartridge type in the header

    /**
     * @brief Cartridge class used to store the cartridge informations
     */
    class Cartridge
    {
    public:
        std::string m_ROMFilename; ///< The filename of the ROM
        MBC *m_MBC; ///< The MBC of the cartridge

        uint8_t m_rom[127 * 16384]; ///< The rom of the cartridge (max size is 127 banks of 16KB = 2MB)
        uint8_t m_ram[127 * 256]; ///< The ram of the cartridge (max size is 127 banks of 256B = 32KB)

        /**
         * @brief Construct a new Cartridge object
         * @details Call the method loadROM to load the ROM
         *
         * @param filename The name of the ROM file
         *
         * @see loadROM
         */
        Cartridge(std::string filename);

        /**
         * @brief Load the ROM into the cartridge
         * @details Save the filename, load the ROM into the cartridge and check the cartridge type
         *
         * @see checkCartridge
         */
        void loadROM();

        /**
         * @brief Check the cartridge type
         * @details Check the cartridge type and set the MBC accordingly
         *
         * @see MBC
         */
        void checkCartridge();

        /**
         * @brief Read a byte from the cartridge
         * @details Read a byte from the cartridge at the specified address and return it
         *
         * @param address The address of the byte to read
         * @return The byte read
         */
        uint8_t read(uint16_t address);

        /**
         * @brief Write a byte to the cartridge
         * @details Write a byte to the cartridge at the given address
         *
         * @param address The address of the byte to write
         * @param value The value to write
         */
        void write(uint16_t address, uint8_t value);
    };
} // namespace gameboy
