/*
 * See pages 8 to 16 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/MBCs.html
 */

#include "mbc.h" // MBC

#include <fstream> // std::ofstream
#include <iostream> // std::cout
#include <iterator> // std::ostreambuf_iterator
#include <utility> // std::move

namespace gameboy
{
    MBC::MBC(std::vector<uint8_t> rom, std::vector<uint8_t> ram)
        : m_rom(std::move(rom)), m_ram(std::move(ram))
    {}

    void MBC::saveRAMData(const std::string &filename) const
    {
        // If the game has no RAM, we don't need to save it
        if (m_ram.empty())
            return;

        std::ofstream ramFile(filename);
        std::copy(m_ram.begin(), m_ram.end(), std::ostreambuf_iterator<char>(ramFile));
        ramFile.close();
    }

    ROMOnly::ROMOnly(std::vector<uint8_t> rom, std::vector<uint8_t> ram)
        : MBC(std::move(rom), std::move(ram)) {}

    uint8_t ROMOnly::read(uint16_t address) const
    {
        // Check if the address is in the ROM
        if (address < 0x8000)
            return m_rom[address];
        return 0;
    }

    void ROMOnly::write(uint16_t address, uint8_t value)
    {
        /* Nothing to do here */
        std::cout << std::hex << "\x1B[33m!!!\033[0m " << "Cannot write value 0x" << +value << " to address 0x" << address << " (ROM Only)\n";
    }

    MBC1::MBC1(std::vector<uint8_t> rom, std::vector<uint8_t> ram)
        : MBC(std::move(rom), std::move(ram)) {}

    uint8_t MBC1::read(uint16_t address) const
    {
        if (address <= 0x3FFF) // https://gbdev.io/pandocs/MBC1.html#00003fff--rom-bank-x0-read-only
        {
            return m_rom[address];
        }
        else if (address <= 0x7FFF) // https://gbdev.io/pandocs/MBC1.html#40007fff--rom-bank-01-7f-read-only
        {
            return readROMBank(address);
        }
        else // https://gbdev.io/pandocs/MBC1.html#a000bfff--ram-bank-0003-if-any
        {
            if (m_ramEnabled)
            {
                return readRAMBank(address);
            }
            return 0xFF;
        }
    }

    void MBC1::write(uint16_t address, uint8_t value)
    {
        if (address <= 0x1FFF) // https://gbdev.io/pandocs/MBC1.html#00001fff--ram-enable-write-only
        {
            m_ramEnabled = (value & 0x0F) == 0x0A;
        }
        else if (address <= 0x3FFF) // https://gbdev.io/pandocs/MBC1.html#20003fff--rom-bank-number-write-only
        {
            value &= 0x1F;
            if (value == 0)
                value = 1;
            m_romBank = (m_romBank & 0x60) | value;
        }
        else if (address <= 0x5FFF) // https://gbdev.io/pandocs/MBC1.html#40005fff--ram-bank-number--or--upper-bits-of-rom-bank-number-write-only
        {
            value &= 0x03;

            if (m_mode)
                // RAM bank number
                m_ramBank = value;
            else
            {
                // Upper bits of ROM bank number

                // Reset bits 5-6 of the ROM bank
                m_romBank &= 0x9F;
                // Set bits 5-6 of the ROM bank according to the value (bits 0-1)
                m_romBank |= value << 5;
            }
        }
        else if (address <= 0x7FFF) // https://gbdev.io/pandocs/MBC1.html#60007fff--banking-mode-select-write-only
        {
            m_mode = value & 0x01;
        }
        else if (address >= 0xA000 && address <= 0xBFFF) // https://gbdev.io/pandocs/MBC1.html#a000bfff--ram-bank-0003-if-any
        {
            if (m_ramEnabled)
            {
                writeRAMBank(address, value);
            }
        }
    }

    uint8_t MBC1::readROMBank(uint16_t address) const
    {
        auto relativeAddress = address - 0x4000;
        auto bankAddress = m_romBank * 0x4000;
        return m_rom[bankAddress + relativeAddress];
    }

    uint8_t MBC1::readRAMBank(uint16_t address) const
    {
        auto relativeAddress = address - 0xA000;
        auto bankAddress = m_ramBank * 0x2000;
        return m_ram[bankAddress + relativeAddress];
    }

    void MBC1::writeRAMBank(uint16_t address, uint8_t value)
    {
        auto relativeAddress = address - 0xA000;
        auto bankAddress = m_ramBank * 0x2000;
        m_ram[bankAddress + relativeAddress] = value;
    }

    MBC2::MBC2(std::vector<uint8_t> rom, std::vector<uint8_t> ram)
        : MBC1(std::move(rom), std::move(ram)) {}

    void MBC2::write(uint16_t address, uint8_t value)
    {
        // See https://gbdev.io/pandocs/MBC2.html#mbc2
        // See pages 14-15 of the documentation (PanDocs/GB.pdf)
        if (address <= 0x3FFF)
        {
            // Enable RAM if bit 8 of the address is clear and the value contained in the bits 0-3 is 0x0A
            if ((address & 0x0100) == 0)
                m_ramEnabled = (value & 0x0F) == 0x0A;
            // Set the lower 4 bits of the ROM bank if the bit 8 of the address is set
            else
            {
                value &= 0x0F;
                if (value == 0)
                    value = 1;
                m_romBank = value;
            }
        }
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            if (m_ramEnabled)
            {
                writeRAMBank(address, value);
            }
        }
    }

    MBC3::MBC3(std::vector<uint8_t> rom, std::vector<uint8_t> ram)
        : MBC1(std::move(rom), std::move(ram)) {}

    void MBC3::write(uint16_t address, uint8_t value)
    {
        if (address <= 0x1FFF) // https://gbdev.io/pandocs/MBC3.html#0000-1fff---ram-and-timer-enable-write-only
        {
            m_ramEnabled = (value & 0x0F) == 0x0A;
        }
        else if (address <= 0x3FFF) // https://gbdev.io/pandocs/MBC3.html#2000-3fff---rom-bank-number-write-only
        {
            value &= 0x7F;
            if (value == 0)
                value = 1;
            m_romBank = value;
        }
        else if (address <= 0x5FFF) // https://gbdev.io/pandocs/MBC3.html#4000-5fff---ram-bank-number---or---rtc-register-select-write-only
        {
            if (value <= 0x03)
            {
                m_ramBank = value;
            }
        }
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            if (m_ramEnabled)
            {
                writeRAMBank(address, value);
            }
        }
    }

    MBC5::MBC5(std::vector<uint8_t> rom, std::vector<uint8_t> ram)
        : MBC1(std::move(rom), std::move(ram)) {}

    void MBC5::write(uint16_t address, uint8_t value)
    {
        if (address <= 0x1FFF) // https://gbdev.io/pandocs/MBC5.html#0000-1fff---ram-enable-write-only
        {
            m_ramEnabled = (value & 0x0F) == 0x0A;
        }
        else if (address <= 0x2FFF) // https://gbdev.io/pandocs/MBC5.html#2000-2fff---8-least-significant-bits-of-rom-bank-number-write-only
        {
            m_romBank &= 0x100;
            m_romBank |= value;
        }
        else if (address <= 0x3FFF) // https://gbdev.io/pandocs/MBC5.html#3000-3fff---9th-bit-of-rom-bank-number-write-only
        {
            m_romBank &= 0xFF;
            m_romBank |= (value & 0x01) << 8;
        }
        else if (address <= 0x5FFF) // https://gbdev.io/pandocs/MBC5.html#4000-5fff---ram-bank-number-write-only
        {
            m_ramBank = value & 0x0F;
        }
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            if (m_ramEnabled)
            {
                writeRAMBank(address, value);
            }
        }
    }
} // namespace gameboy
