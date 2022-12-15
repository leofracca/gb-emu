/**
 * @file cartridge.h
 * @brief This file contains the declaration of the Cartridge class.
 *        It emulates the behavior of a Gameboy cartridge.
 */

/*
 * See pages 8 to 16 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/The_Cartridge_Header.html
 */

#pragma once

#include "mbc.h" // MBC

#include <string> // std::string
#include <vector> // std::vector

namespace gameboy
{
    namespace cartridge_info
    {
        constexpr uint16_t CARTRIDGE_TITLE_START_ADDRESS = 0x134; ///< The address of the first byte of the cartridge title in the header
        constexpr uint16_t CARTRIDGE_TITLE_END_ADDRESS = 0x142; ///< The address of the last byte of the cartridge title in the header
        constexpr uint16_t CARTRIDGE_NEW_LICENSEE_CODE_ADDRESS = 0x144; ///< The address of the first byte of the new licensee code in the header
        constexpr uint16_t CARTRIDGE_TYPE_ADDRESS = 0x0147; ///< The address of the cartridge type in the header
        constexpr uint16_t CARTRIDGE_ROM_SIZE_ADDRESS = 0x0148; ///< The address of the ROM size in the header
        constexpr uint16_t CARTRIDGE_RAM_SIZE_ADDRESS = 0x0149; ///< The address of the RAM size in the header
        constexpr uint16_t CARTRIDGE_OLD_LICENSEE_CODE_ADDRESS = 0x014B; ///< The address of the old licensee code in the header
    } // namespace cartridge_info

    /**
     * @brief Cartridge class used to store the cartridge informations
     */
    class Cartridge
    {
    public:
        /**
         * @brief Free the memory used by the MBC
         */
        ~Cartridge();

        /**
         * @brief Load the ROM into the cartridge
         * @details Save the filename, save the ROM, save the RAM, check the cartridge type and print cartridge informations
         *
         * @param filename The name of the ROM file
         * @return true if the ROM was loaded successfully, false otherwise
         * @see checkCartridge, printCartridgeInfo
         */
        bool loadROM(const std::string &filename);

        /**
         * @brief Read a byte from the cartridge
         * @details Read a byte from the cartridge at the specified address and return it
         *
         * @param address The address of the byte to read
         * @return The byte read
         */
        [[nodiscard]] uint8_t read(uint16_t address) const;

        /**
         * @brief Write a byte to the cartridge
         * @details Write a byte to the cartridge at the given address
         *
         * @param address The address of the byte to write
         * @param value The value to write
         */
        void write(uint16_t address, uint8_t value);

        /**
         * @brief Save the current content of the RAM to a file
         *
         * @see MBC::saveRAMData
         */
        void saveRAMData() const;

    private:
        std::string m_ROMFilename; ///< The filename of the ROM
        MBC *m_MBC; ///< The MBC of the cartridge

        std::string m_title; ///< The title of the cartridge (used for printing)
        std::string m_MBCAsString; ///< The MBC as a string (used for printing)
        std::string m_licenseeCode; ///< The licensee code of the cartridge (used for printing)
        std::string m_ROMSizeAsString; ///< The ROM size as a string (used for printing)
        std::string m_RAMSizeAsString; ///< The RAM size as a string (used for printing)

        std::vector<uint8_t> m_rom; ///< The ROM of the cartridge
        std::vector<uint8_t> m_ram; ///< The RAM of the cartridge

        /**
         * @brief Check the cartridge type
         * @details Check the cartridge type and set the MBC accordingly
         *
         * @see MBC
         */
        void checkCartridge();

        /**
         * @brief Print the cartridge informations
         * @details Print title, Licensee, MBC, ROM size, RAM size
         *
         * @see getTitle, getLicensee, getROMSize, getRAMSize
         */
        void printCartridgeInfo();

        /**
         * @brief Get the cartridge title
         * @details The title is contained in bytes 0x0134 to 0x0142
         *
         * @return The cartridge title
         */
        [[nodiscard]] std::string getTitle() const;

        /**
         * @brief Get the cartridge licensee
         * @details The licensee is contained in byte 0x014B (old licensee)
         *          If the licensee is 0x33, the licensee is contained in bytes 0x0144 to 0x0145 (new licensee)
         *
         * @return The cartridge licensee
         * @see getNewLicensee
         */
        [[nodiscard]] std::string getLicensee() const;

        /**
         * @brief Get the cartridge licensee
         * @details The licensee is contained in bytes 0x0144 to 0x0145 (as ASCII)
         *
         * @return The cartridge licensee
         * @see getLicensee
         */
        [[nodiscard]] std::string getNewLicensee() const;

        /**
         * @brief Get the cartridge ROM size
         * @details The ROM size is contained in byte 0x0148
         *
         * @return The cartridge ROM size
         */
        [[nodiscard]] std::string getROMSize() const;

        /**
         * @brief Get the cartridge RAM size
         * @details The RAM size is contained in byte 0x0149
         *          The returned value is a pair of integers and string
         *          The integer is used to initialize the RAM vector
         *          The string is used to print the RAM size
         *
         * @return The cartridge RAM size
         */
        [[nodiscard]] std::pair<int, std::string> getRAMSize() const;
    };
} // namespace gameboy
