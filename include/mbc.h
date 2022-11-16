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
        MBC(std::vector<uint8_t> rom, std::vector<uint8_t> ram);

        virtual ~MBC() = default;

        /**
         * @brief Read a byte from the cartridge
         * @details Read a byte from the cartridge at the specified address and return it
         *
         * @param address The address to read from
         * @return The byte read
         */
        [[nodiscard]] virtual uint8_t read(uint16_t address) const = 0;

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

    /**
     * @brief ROMOnly class used to emulate the behavior of a cartridge with no MBC
     */
    class ROMOnly : public MBC
    {
    public:
        /**
         * @brief Construct a new ROMOnly object
         * @details Initialize the ROM and RAM of the cartridge
         *
         * @param rom The ROM of the cartridge
         * @param ram The RAM of the cartridge (empty)
         * @see MBC::MBC
         */
        explicit ROMOnly(std::vector<uint8_t> rom, std::vector<uint8_t> ram);

        /**
         * @brief Read a byte from the cartridge
         * @details Read a byte from the cartridge at the specified address and return it
         *          The address must be < 0x8000 (i.e. the cartridge ROM)
         *
         * @param address The address to read from
         * @return The byte read
         */
        [[nodiscard]] uint8_t read(uint16_t address) const override;

        /**
         * @brief Do nothing
         * @details If the address is < 0x8000, do nothing
         *          If the address is >= 0x8000, throw an exception
         *
         * @param address The address to write to
         * @param value The value to write
         */
        void write(uint16_t address, uint8_t value) override;
    };

    /**
     * @brief MBC1 class used to emulate the behavior of a cartridge with an MBC1 chip
     */
    class MBC1 : public MBC
    {
    public:
        /**
         * @brief Construct a new MBC1 object
         * @details Initialize the ROM and RAM of the cartridge
         *
         * @param rom The ROM of the cartridge
         * @param ram The RAM of the cartridge
         * @see MBC::MBC
         */
        MBC1(std::vector<uint8_t> rom, std::vector<uint8_t> ram);

        /**
         * @brief Read a byte from the cartridge
         * @details Read a byte from the cartridge at the specified address and return it
         *          Simulate the behavior of the MBC1 chip (MBC2, MBC3 and MBC5 read in the same way)
         *
         * @param address The address to read from
         * @return The byte read
         * @see MBC1::readROMBank, MBC1::readRAMBank
         */
        [[nodiscard]] uint8_t read(uint16_t address) const final;

        /**
         * @brief Write a byte to the cartridge
         * @details Write a byte to the cartridge at the specified address
         *          Simulate the behavior of the MBC1 chip
         *
         * @param address The address to write to
         * @param value The value to write
         * @see MBC1::writeRAMBank
         */
        void write(uint16_t address, uint8_t value) override;

    protected:
        bool m_ramEnabled = false; ///< Whether the RAM is enabled or not
        uint8_t m_romBank = 1; ///< The ROM bank to read from
        uint8_t m_ramBank = 0; ///< The RAM bank to read/write from/to

        /**
         * @brief Read the address of the ROM bank specified by m_romBank
         *
         * @param address The address to read from
         * @return The byte read
         */
        [[nodiscard]] uint8_t readROMBank(uint16_t address) const;

        /**
         * @brief Read the address of the RAM bank specified by m_ramBank
         *
         * @param address The address to read from
         * @return The byte read
         */
        [[nodiscard]] uint8_t readRAMBank(uint16_t address)const;

        /**
         * @brief Write the address of the RAM bank specified by m_ramBank
         *
         * @param address The address to write to
         * @param value The value to write
         */
        void writeRAMBank(uint16_t address, uint8_t value);

    private:
        /**
         * @brief Select the memory mode to use
         * @details If the value is false, the memory mode is ROM mode
         *          If the value is true, the memory mode is RAM mode
         *          It is modified by writing to range 0x6000-0x7FFF
         *          It is used to select bits 5-6 of ROM bank or the RAM bank in the range 0x4000-0x5FFF
         */
        bool m_mode = false;
    };

    /**
     * @brief MBC2 class used to emulate the behavior of a cartridge with an MBC2 chip
     */
    class MBC2 : public MBC1
    {
    public:
        /**
         * @brief Construct a new MBC2 object
         * @details Initialize the ROM and RAM of the cartridge
         *
         * @param rom The ROM of the cartridge
         * @param ram The RAM of the cartridge
         * @see MBC::MBC1
         */
        MBC2(std::vector<uint8_t> rom, std::vector<uint8_t> ram);

        /**
         * @brief Write a byte to the cartridge
         * @details Write a byte to the cartridge at the specified address
         *          Simulate the behavior of the MBC2 chip
         *
         * @param address The address to write to
         * @param value The value to write
         * @see MBC1::writeRAMBank
         */
        void write(uint16_t address, uint8_t value) override;
    };

    /**
     * @brief MBC3 class used to emulate the behavior of a cartridge with an MBC3 chip
     */
    class MBC3 : public MBC1
    {
    public:
        /**
         * @brief Construct a new MBC3 object
         * @details Initialize the ROM and RAM of the cartridge
         *
         * @param rom The ROM of the cartridge
         * @param ram The RAM of the cartridge
         * @see MBC::MBC1
         */
        MBC3(std::vector<uint8_t> rom, std::vector<uint8_t> ram);

        /**
         * @brief Write a byte to the cartridge
         * @details Write a byte to the cartridge at the specified address
         *          Simulate the behavior of the MBC3 chip
         *
         * @param address The address to write to
         * @param value The value to write
         * @see MBC1::writeRAMBank
         */
        void write(uint16_t address, uint8_t value) override;
    };

    /**
     * @brief MBC5 class used to emulate the behavior of a cartridge with an MBC5 chip
     */
    class MBC5 : public MBC1
    {
    public:
        /**
         * @brief Construct a new MBC5 object
         * @details Initialize the ROM and RAM of the cartridge
         *
         * @param rom The ROM of the cartridge
         * @param ram The RAM of the cartridge
         * @see MBC::MBC1
         */
        MBC5(std::vector<uint8_t> rom, std::vector<uint8_t> ram);

        /**
         * @brief Write a byte to the cartridge
         * @details Write a byte to the cartridge at the specified address
         *          Simulate the behavior of the MBC5 chip
         *
         * @param address The address to write to
         * @param value The value to write
         * @see MBC1::writeRAMBank
         */
        void write(uint16_t address, uint8_t value) override;
    };
} // namespace gameboy
