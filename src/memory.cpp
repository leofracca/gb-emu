/*
 * See pages 8 to 18 of the documentation (docs/GB.pdf)
 */

#include "../include/memory.h" // Memory

namespace gameboy
{
    Memory::Memory(Cartridge *cartridge)
        : m_cartridge(cartridge)
    {}

    uint8_t Memory::read(uint16_t address)
    {
        // The areas from 0000-7FFF and A000-BFFF address external hardware on the cartridge
        if (address < 0x8000 || (address >= 0xA000 && address < 0xC000))
            return m_cartridge->read(address);
        else
            return m_memory[address];
    }

    void Memory::write(uint16_t address, uint8_t value)
    {
        // The areas from 0000-7FFF and A000-BFFF address external hardware on the cartridge
        if (address < 0x8000 || (address >= 0xA000 && address < 0xC000))
            m_cartridge->write(address, value);
        else
            m_memory[address] = value;
    }

    uint16_t Memory::readWord(uint16_t address)
    {
        uint8_t lo = read(address);
        uint8_t hi = read(address + 1);

        return lo | (hi << 8);
    }

    void Memory::writeWord(uint16_t address, uint16_t value)
    {
        uint8_t lo = value & 0xFF;
        uint8_t hi = (value >> 8) & 0xFF;
        write(address, lo);
        write(address + 1, hi);
    }
} // namespace gameboy
