/*
 * See pages 8 to 16 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/MBCs.html
 */

#include "../include/mbc.h" // MBC

// Almost all the code of this file comes from https://github.com/Mika412/NoobBoy/blob/master/src/mbc.cpp
// TODO: document how each function works and refactor the code

namespace gameboy
{
    MBC::MBC(uint8_t *rom, uint8_t *ram)
    {
        m_rom = rom;
        m_ram = ram;
    }

    ROMOnly::ROMOnly(uint8_t *rom) : MBC(rom, nullptr) {}

    uint8_t ROMOnly::read(uint16_t address){
        if (address < 0x8000)
            return m_rom[address];

        return 0;
    }

    void ROMOnly::write(uint16_t address, uint8_t value)
    {
        /* Nothing to do here */
        m_rom[address] = value;
    }

    MBC1::MBC1(uint8_t *rom, uint8_t *ram) : MBC(rom, ram) {}

    uint8_t MBC1::read(uint16_t address){
        if (address < 0x4000)
            return m_rom[address];
        else if (address < 0x8000)
        {
            if (m_mode)
                return m_rom[(m_romBank & 0x1f) * 0x4000 + address - 0x4000];

            return m_rom[(m_romBank & 0x7f) * 0x4000 + address - 0x4000];
        }
        else if (address >= 0xA000 && address < 0xC000)
        {
            if (m_ramEnabled)
                return m_ram[m_ramBank * m_mode * 0x2000 + address - 0xA000];
        }
        return 0;
    }

    void MBC1::write(uint16_t address, uint8_t value){
        if (address < 0x2000)
            m_ramEnabled = (value & 0x0f) == 0x0a;
        else if (address < 0x4000)
        {
            value = value & 0x1f;
            if (value == 0x00)
                value = 0x1;

            m_romBank = (m_romBank & 0x60) | value;
        }
        else if (address < 0x6000)
            m_romBank = (m_romBank & 0x9f) | ((value & 0x03) << 5);
        else if (address < 0x8000)
        {
            if (value > 0x01)
                return;

            m_mode = value;
        }
        else if (address >= 0xA000 && address < 0xC000)
        {
            if (m_ramEnabled)
                m_ram[m_ramBank * m_mode * 0x2000 + address - 0xA000] = value;
        }
    }

    MBC2::MBC2(uint8_t *rom, uint8_t *ram) : MBC1(rom, ram) {}

    uint8_t MBC2::read(uint16_t address){
        if (address < 0x4000)
            return m_rom[address];
        else if (address < 0x8000)
            return m_rom[m_romBank * 0x4000 + address - 0x4000];
        else if (address >= 0xA000 && address < 0xC000)
        {
            if (m_ramEnabled)
                return m_ram[m_ramBank * 0x2000 + address - 0xA000];
        }

        return 0;
    }

    void MBC2::write(uint16_t address, uint8_t value){
        if (address < 0x2000)
        {
            if ((address & 0x0100) == 0)
                m_ramEnabled = value == 0x0a;
        }
        else if (address < 0x4000)
        {
            if ((address & 0x0100) != 0)
                m_romBank = value;
        }
        else if (address >= 0xA000 && address < 0xC000)
        {
            if (m_ramEnabled)
                m_ram[m_ramBank * 0x2000 + address - 0xA000] = value;
        }
    }

    MBC3::MBC3(uint8_t *rom, uint8_t *ram) : MBC1(rom, ram) {}
    
    uint8_t MBC3::read(uint16_t address)
    {
        if (address < 0x4000)
            return m_rom[address];
        else if (address < 0x8000)
            return m_rom[m_romBank * 0x4000 + address - 0x4000];
        else if (address >= 0xA000 && address < 0xC000)
        {
            if (m_ramEnabled)
            {
                if (m_ramBank <= 0x03)
                    return m_ram[m_ramBank * 0x2000 + address - 0xA000];
            }
        }

        return 0;
    }

    void MBC3::write(uint16_t address, uint8_t value){
        if (address < 0x2000)
            m_ramEnabled = (value & 0x0f) == 0x0a;
        else if (address < 0x4000)
        {
            m_romBank = value & 0x7f;
            if (m_romBank == 0x00)
                m_romBank = 0x01;
        }
        else if (address < 0x6000)
            m_ramBank = value & 0x0f;
        else if (address >= 0xA000 && address < 0xC000)
        {
            if (m_ramEnabled) {
                if (m_ramBank <= 0x03)
                    m_ram[m_ramBank * 0x2000 + address - 0xA000] = value;
            }
        }
    }

    MBC5::MBC5(uint8_t *rom, uint8_t *ram) : MBC1(rom, ram) {}
    
    uint8_t MBC5::read(uint16_t address)
    {
        if (address < 0x4000)
            return m_rom[address];
        else if (address < 0x8000)
            return m_rom[m_romBank * 0x4000 + address - 0x4000];
        else if (address >= 0xA000 && address < 0xC000)
        {
            if (m_ramEnabled)
                return m_ram[m_ramBank * 0x2000 + address - 0xA000];
        }

        return 0;
    }

    void MBC5::write(uint16_t address, uint8_t value){
        if (address < 0x2000)
            m_ramEnabled = (value & 0x0f) == 0x0a;
        else if (address < 0x3000)
            m_romBank = (m_romBank & 0x100) | value;
        else if (address < 0x4000)
            m_romBank = (m_romBank & 0x0ff) | ((value & 0x01) << 8);
        else if (address < 0x6000)
            m_ramBank = value & 0x0f;
        else if (address >= 0xA000 && address < 0xC000)
        {
            if (m_ramEnabled)
                m_ram[m_ramBank * 0x2000 + address - 0xA000] = value;
        }
    }
} // namespace gameboy
