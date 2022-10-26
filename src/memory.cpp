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

        // Initialize memory registers
        m_memory[0xFF00] = 0xCF; // P1
        m_memory[0xFF01] = 0x00; // SB
        m_memory[0xFF02] = 0x7E; // SC
        m_memory[0xFF04] = 0x00; // DIV
        m_memory[0xFF05] = 0x00; // TIMA
        m_memory[0xFF06] = 0x00; // TMA
        m_memory[0xFF07] = 0x00; // TAC
        m_memory[0xFF0F] = 0xE1; // IF
        m_memory[0xFF10] = 0x80; // NR10
        m_memory[0xFF11] = 0xBF; // NR11
        m_memory[0xFF12] = 0xF3; // NR12
        m_memory[0xFF14] = 0xBF; // NR14
        m_memory[0xFF16] = 0x3F; // NR21
        m_memory[0xFF17] = 0x00; // NR22
        m_memory[0xFF19] = 0xBF; // NR24
        m_memory[0xFF1A] = 0x7F; // NR30
        m_memory[0xFF1B] = 0xFF; // NR31
        m_memory[0xFF1C] = 0x9F; // NR32
        m_memory[0xFF1E] = 0xBF; // NR33
        m_memory[0xFF20] = 0xFF; // NR41
        m_memory[0xFF21] = 0x00; // NR42
        m_memory[0xFF22] = 0x00; // NR43
        m_memory[0xFF23] = 0xBF; // NR30
        m_memory[0xFF24] = 0x77; // NR50
        m_memory[0xFF25] = 0xF3; // NR51
        m_memory[0xFF26] = 0xF1; // NR52
        m_memory[0xFF40] = 0x91; // LCDC
        m_memory[0xFF42] = 0x00; // SCY
        m_memory[0xFF43] = 0x00; // SCX
        m_memory[0xFF45] = 0x00; // LYC
        m_memory[0xFF47] = 0xFC; // BGP
        m_memory[0xFF48] = 0xFF; // OBP0
        m_memory[0xFF49] = 0xFF; // OBP1
        m_memory[0xFF4A] = 0x00; // WY
        m_memory[0xFF4B] = 0x00; // WX
        m_memory[0xFFFF] = 0x00; // IE
    }

    uint8_t Memory::read(uint16_t address)
    {
        // The areas from 0000-7FFF and A000-BFFF address external hardware on the cartridge
        if (address < 0x8000 || (address >= 0xA000 && address < 0xC000))
            return m_cartridge.read(address);

        // Joypad
        if (address == JOYPAD_ADDRESS)
        {
            // Is the pressed button an action button or a direction button?
            uint8_t actionOrDirection = m_memory[JOYPAD_ADDRESS] & 0x30;

            return actionOrDirection == 0x20
                           ? (m_joypadState >> 4) | 0x20 // Direction
                           : m_joypadState | 0x10; // Action
        }

        return m_memory[address];
    }

    void Memory::write(uint16_t address, uint8_t value)
    {
        // The areas from 0000-7FFF and A000-BFFF address external hardware on the cartridge
        if (address < 0x8000 || (address >= 0xA000 && address < 0xC000))
            m_cartridge.write(address, value);
        else
            m_memory[address] = value;

        // VRAM
        if (address >= 0x8000 && address < 0x9800)
            UpdateTile(address);

        // OAM
        else if (address >= 0xFE00 && address < 0xFEA0)
            UpdateSprite(address, value);

        // Check if the LCD has been disabled (bit 7 of LCDC register)
        else if (address == 0xFF40)
        {
            if (!(value & (1 << 7)))
            {
                m_memory[0xFF41] &= 0x7C; // Reset STAT register
                m_memory[0xFF44] = 0x00; // Reset LY register
            }
        }

        // DMA Transfer
        else if (address == DMA_REG_ADDRESS)
            for (uint16_t i = 0; i < 160; i++)
                write(0xFE00 + i, read((value << 8) + i));

        // Update colour palette
        else if (address == BGP_REG_ADDRESS)
            UpdatePalette(palette_BGP, value); // BG and Window palette
        else if (address == OBP0_REG_ADDRESS)
            UpdatePalette(palette_OBP0, value); // Object palette 0
        else if (address == OBP1_REG_ADDRESS)
            UpdatePalette(palette_OBP1, value); // Object palette 1
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

    uint8_t Memory::getJoypadState() const
    {
        return m_joypadState;
    }

    void Memory::setJoypadState(uint8_t state)
    {
        m_joypadState = state;
    }

    // The following functions come from https://github.com/Mika412/NoobBoy/blob/master/src/memory.cpp
    void Memory::UpdateTile(uint16_t laddress)
    {
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
        switch (relativeAddress & 3)
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
                sprite->options.flags.value = value;
                sprite->colourPalette = (sprite->options.flags.bits.paletteNumber) ? palette_OBP1 : palette_OBP0;
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
