/*
 * See pages 8 to 18 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/Memory_Map.html
 */

#include "memory.h" // Memory

namespace gameboy
{
    Memory::Memory(const std::string &romPath)
    {
        m_cartridge.loadROM(romPath);
    }

    uint8_t Memory::read(uint16_t address)
    {
        // Joypad
        if (address == JOYPAD_ADDRESS)
        {
            uint8_t actionOrDirection = m_memory[JOYPAD_ADDRESS] & 0x30;

            if (actionOrDirection == 0x20) // Direction
                return ((m_joypadState >> 4) & 0xF) | 0x20;
            else if (actionOrDirection == 0x10) // Action
                return (m_joypadState & 0xF) | 0x10;
        }

        // The areas from 0000-7FFF and A000-BFFF address external hardware on the cartridge
        if (address < 0x8000 || (address >= 0xA000 && address < 0xC000))
            return m_cartridge.read(address);
        else
            return m_memory[address];
    }

    void Memory::write(uint16_t address, uint8_t value)
    {
        if (address == 0xFF40)
        {
            m_memory[address] =  value;
            if (!(value & (1 << 7)))
            {
                m_memory[0xFF44] = 0x00;
                m_memory[0xFF41] &= 0x7C;
            }
        }

        // DMA Transfer
        if (address == 0xFF46)
            for (uint16_t i = 0; i < 160; i++)
                write(0xFE00 + i, read((value << 8) + i));

        // Update colour palette
        if (address == 0xFF47) UpdatePalette(palette_BGP, value); // BG and Window palette
        if (address == 0xFF48) UpdatePalette(palette_OBP0, value); // Object palette 0
        if (address == 0xFF49) UpdatePalette(palette_OBP1, value); // Object palette 1

        // The areas from 0000-7FFF and A000-BFFF address external hardware on the cartridge
        if (address < 0x8000 || (address >= 0xA000 && address < 0xC000))
            m_cartridge.write(address, value);
        else
            m_memory[address] = value;

        if (address >= 0x8000 && address < 0x9800)
            UpdateTile(address);

        if (address >= 0xFE00 && address <= 0xFE9F)
            UpdateSprite(address, value);
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

    uint8_t Memory::getJoypadState()
    {
        return m_joypadState;
    }

    void Memory::setJoypadState(uint8_t state)
    {
        m_joypadState = state;
    }

    // The following functions come from https://github.com/Mika412/NoobBoy/blob/master/src/memory.cpp
    void Memory::UpdateTile(uint16_t laddress) {
        uint16_t address = laddress & 0xFFFE;

        uint16_t tile = (address >> 4) & 511;
        uint16_t y = (address >> 1) & 7;

        uint8_t bitIndex;
        for (uint8_t x = 0; x < 8; x++)
        {
            bitIndex = 1 << (7 - x);

            tiles[tile].pixels[y][x] = ((m_memory[address] & bitIndex) ? 1 : 0) + ((m_memory[address + 1] & bitIndex) ? 2 : 0);
        }
    }

    void Memory::UpdateSprite(uint16_t address, uint8_t value)
    {
        uint16_t relativeAddress = address - 0xFE00;
        Sprite *sprite = &sprites[relativeAddress >> 2];
        sprite->ready = false;
        switch(relativeAddress & 3)
        {
            case 0:
                sprite->y = value - 16;
                break;
            case 1:
                sprite->x = value - 8;
                break;
            case 2:
                sprite->tile = value;
                break;
            case 3:
                sprite->options.value = value;
                sprite->colourPalette = (sprite->options.bits.paletteNumber) ? palette_OBP1 : palette_OBP0;
                sprite->ready = true;
                break;
        }
    }

    void Memory::UpdatePalette(Colour *palette, uint8_t value)
    {
        palette[0] = palette_colours[value & 0x3];
        palette[1] = palette_colours[(value >> 2) & 0x3];
        palette[2] = palette_colours[(value >> 4) & 0x3];
        palette[3] = palette_colours[(value >> 6) & 0x3];
    }
} // namespace gameboy
