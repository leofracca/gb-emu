/*
 * See pages 8 to 16 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/MBCs.html
 */

#include "mbc.h" // MBC

#include <stdexcept> // std::runtime_error

namespace gameboy
{
    MBC::MBC(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram)
    {
        m_rom = rom;
        m_ram = ram;
    }

    ROMOnly::ROMOnly(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram)
        : MBC(rom, ram) {}

    uint8_t ROMOnly::read(uint16_t address)
    {
        // Check if the address is in the ROM
        if (address < 0x8000)
            return m_rom[address];
        return 0;
    }

    void ROMOnly::write(uint16_t address, uint8_t value)
    {
        /* Nothing to do here */
        if (address < 0x8000)
            return;
        throw std::runtime_error("Cannot write value " + std::to_string(value) + " to address " + std::to_string(address));
    }

    MBC1::MBC1(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram)
        : MBC(rom, ram) {}

    uint8_t MBC1::read(uint16_t address)
    {
        if (address <= 0x3FFF) // https://gbdev.io/pandocs/MBC1.html#00003fff--rom-bank-x0-read-only
        {
            return m_rom[address];
        }
        else if (address <= 0x7FFF) // https://gbdev.io/pandocs/MBC1.html#40007fff--rom-bank-01-7f-read-only
        {
            return readROMBank(address);
        }
        else if (address >= 0xA000 && address <= 0xBFFF) // https://gbdev.io/pandocs/MBC1.html#a000bfff--ram-bank-0003-if-any
        {
            if (m_ramEnabled)
            {
                return readRAMBank(address);
            }
            return 0;
        }
        else
            return 0;
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
            // Reset bits 5-6 of the ROM bank
            m_romBank &= 0x9F;
            // Set bits 5-6 of the ROM bank according to the value (bits 0-1)
            m_romBank |= (value & 0x03) << 5;
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

    uint8_t MBC1::readROMBank(uint16_t address)
    {
        auto relativeAddress = address - 0x4000;
        auto bankAddress = m_romBank * 0x4000;
        return m_rom[bankAddress + relativeAddress];
    }

    uint8_t MBC1::readRAMBank(uint16_t address)
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

    MBC2::MBC2(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram)
        : MBC1(rom, ram) {}

    uint8_t MBC2::read(uint16_t address)
    {
        if (address <= 0x3FFF) // https://gbdev.io/pandocs/MBC2.html#00003fff--rom-bank-0-read-only
        {
            return m_rom[address];
        }
        else if (address <= 0x7FFF) // https://gbdev.io/pandocs/MBC2.html#40007fff--rom-bank-01-0f-read-only
        {
            return readROMBank(address);
        }
        else if (address >= 0xA000 && address <= 0xBFFF) // https://gbdev.io/pandocs/MBC2.html#a000a1ff--built-in-ram and https://gbdev.io/pandocs/MBC2.html#a200bfff--15-echoes-of-a000a1ff
        {
            if (m_ramEnabled)
            {
                return readRAMBank(address);
            }
            return 0;
        }
        else
            return 0;
    }

    void MBC2::write(uint16_t address, uint8_t value)
    {
        // See https://gbdev.io/pandocs/MBC2.html#mbc2
        // See pages 14-15 of the documentation (PanDocs/GB.pdf)
        if (address <= 0x1FFF)
        {
            // Enable RAM if bit 8 of the address is clear and the value is 0x0A
            if ((address & 0x0100) == 0)
                m_ramEnabled = (value & 0x0F) == 0x0A;
        }
        else if (address <= 0x3FFF)
        {
            // If bit 8 of the address is set, set the lower 4 bits of the ROM bank
            if ((address & 0x0100) != 0)
            {
                value &= 0x0F;
                if (value == 0)
                    value = 1;
                m_romBank = value;
            }
        }
        else if (address <= 0x5FFF)
        {
            // Reset bits 5-6 of the ROM bank
            m_romBank &= 0x9F;
            // Set bits 5-6 of the ROM bank according to the value (bits 0-1)
            m_romBank |= (value & 0x03) << 5;
        }
        else if (address >= 0xA000 && address <= 0xBFFF)
        {
            if (m_ramEnabled)
            {
                writeRAMBank(address, value);
            }
        }
    }

    MBC3::MBC3(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram)
        : MBC1(rom, ram) {}

    uint8_t MBC3::read(uint16_t address)
    {
        if (address <= 0x3FFF) // https://gbdev.io/pandocs/MBC3.html#0000-3fff---rom-bank-00-read-only
        {
            return m_rom[address];
        }
        else if (address <= 0x7FFF) // https://gbdev.io/pandocs/MBC3.html#4000-7fff---rom-bank-01-7f-read-only
        {
            return readROMBank(address);
        }
        else if (address >= 0xA000 && address <= 0xBFFF) // https://gbdev.io/pandocs/MBC3.html#a000-bfff---ram-bank-00-03-if-any-readwrite
        {
            if (m_ramEnabled)
            {
                if (m_ramBank <= 0x03)
                {
                    return readRAMBank(address);
                }
            }
            return 0;
        }
        else
            return 0;
    }

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

    MBC5::MBC5(const std::vector<uint8_t> &rom, const std::vector<uint8_t> &ram)
        : MBC1(rom, ram) {}

    uint8_t MBC5::read(uint16_t address)
    {
        if (address <= 0x3FFF) // https://gbdev.io/pandocs/MBC5.html#0000-3fff---rom-bank-00-read-only
        {
            return m_rom[address];
        }
        else if (address <= 0x7FFF) // https://gbdev.io/pandocs/MBC5.html#4000-7fff---rom-bank-00-1ff-read-only
        {
            return readROMBank(address);
        }
        else if (address >= 0xA000 && address <= 0xBFFF) // https://gbdev.io/pandocs/MBC5.html#a000-bfff---ram-bank-00-0f-if-any-readwrite
        {
            if (m_ramEnabled)
            {
                return readRAMBank(address);
            }
            return 0;
        }
        else
            return 0;
    }

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
