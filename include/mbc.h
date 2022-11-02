/**
 * @file mbc.h
 * @brief This file contains the declaration of the MBC class.
 *        It emulates the behavior of the MBC of a Gameboy cartridge.
 */

/*
 * See pages 8 to 16 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/MBCs.html
 */

#pragma once

#include <cstdint> // uint8_t, uint16_t
#include <string> // std::string
#include <vector> // std::vector

namespace gameboy
{
    /**
     * @brief MBC class used to store the type of MBC of the cartridge
     */
    class MBC
    {
    public:
        /**
         * @brief Construct a new MBC object
         * @details Initialize the ROM and RAM of the cartridge
         *
         * @param rom The ROM of the cartridge
         * @param ram The RAM of the cartridge
         */
        MBC(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram);

        virtual ~MBC() = default;

        /**
         * @brief Read a byte from the cartridge
         * @details Read a byte from the cartridge at the specified address and return it
         *
         * @param address The address to read from
         * @return uint8_t The byte read
         */
        virtual uint8_t read(uint16_t address) = 0;

        /**
         * @brief Write a byte to the cartridge
         * @details Write a byte to the cartridge at the specified address
         *
         * @param address The address to write to
         * @param value The value to write
         */
        virtual void write(uint16_t address, uint8_t value) = 0;

        /**
         * @brief Save the current content of the RAM to a file
         * @details If the game uses the RAM, save the current content of the RAM to a file.
         *          If the game doesn't use the RAM, do nothing
         * @param filename The name of the file to save the RAM to (i.e. the name of the ROM file with the extension .sav)
         */
        void saveRAMData(const std::string &filename);

    protected:
        std::vector<uint8_t> m_rom; ///< The ROM of the cartridge
        std::vector<uint8_t> m_ram; ///< The RAM of the cartridge
    };

    class ROMOnly : public MBC
    {
    public:
        explicit ROMOnly(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram);

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;
    };

    class MBC1 : public MBC
    {
    public:
        MBC1(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram);

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;

    protected:
        bool m_ramEnabled = false;
        uint8_t m_romBank = 1;
        uint8_t m_ramBank = 0;

        uint8_t readROMBank(uint16_t address);
        uint8_t readRAMBank(uint16_t address);
        void writeRAMBank(uint16_t address, uint8_t value);

    private:
        bool m_mode = false;
    };

    class MBC2 : public MBC1
    {
    public:
        MBC2(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram);

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;
    };

    class MBC3 : public MBC1
    {
    public:
        MBC3(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram);

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;
    };

    class MBC5 : public MBC1
    {
    public:
        MBC5(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram);

        uint8_t read(uint16_t address) override;
        void write(uint16_t address, uint8_t value) override;
    };
} // namespace gameboy
