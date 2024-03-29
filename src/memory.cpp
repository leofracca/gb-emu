/*
 * See pages 8 to 18 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/Memory_Map.html
 */

#include "memory.h" // Memory

#include <iostream> // std::cout

namespace gameboy
{
    Memory::Memory(Cartridge &cartridge)
        : m_cartridge(cartridge)
    {
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

    uint8_t Memory::read(uint16_t address) const
    {
        // The areas from 0000-7FFF and A000-BFFF address external hardware on the cartridge
        if (address < 0x8000 || (address >= 0xA000 && address < 0xC000))
            return m_cartridge.read(address);

        // Echo RAM
        else if (address >= 0xE000 && address < 0xFE00)
        {
            // Forward the read to the WRAM (0xC000-0xDDFF)
            logInvalidReadOperation(address, "Echo RAM");
            return m_memory[address - 0x2000];
        }

        // Unusable memory
        else if (address >= 0xFEA0 && address < 0xFF00)
            logInvalidReadOperation(address, "Unusable memory");

        // Joypad
        else if (address == JOYPAD_ADDRESS)
        {
            // Is the pressed button an action button or a direction button?
            uint8_t actionOrDirection = m_memory[JOYPAD_ADDRESS] & 0x30;

            return actionOrDirection == 0x20
                           ? (m_joypadState >> 4) | 0x20 // Direction
                           : (m_joypadState & 0x0F) | 0x10; // Action
        }

        return m_memory[address];
    }

    void Memory::write(uint16_t address, uint8_t value)
    {
        // The areas from 0000-7FFF and A000-BFFF address external hardware on the cartridge
        if (address < 0x8000 || (address >= 0xA000 && address < 0xC000))
            m_cartridge.write(address, value);
        else
        {
            // Echo RAM
            if (address >= 0xE000 && address < 0xFE00)
            {
                // Forward the write to the WRAM (0xC000-0xDDFF)
                logInvalidWriteOperation(address, value, "Echo RAM");
                m_memory[address - 0x2000] = value;
            }

            // Unusable memory
            else if (address >= 0xFEA0 && address < 0xFF00)
                logInvalidWriteOperation(address, value, "Unusable memory");

            // Valid address, write the value to the memory
            else
                m_memory[address] = value;

            // Check if the LCD has been disabled (bit 7 of LCDC register)
            if (address == 0xFF40)
            {
                if (!(value & 0x80))
                {
                    m_memory[0xFF41] &= 0x7C; // Reset STAT register
                    m_memory[0xFF44] = 0x00; // Reset LY register
                }
            }

            // DMA Transfer
            else if (address == 0xFF46)
            {
                // The written value specifies the transfer source address divided by $100
                uint16_t sourceAddress = (value << 8);
                for (uint16_t i = 0; i < 0xA0; i++)
                {
                    write(0xFE00 + i, read(sourceAddress + i));
                }
            }

            // Update colour palette
            else if (address == 0xFF47)
                UpdatePalette(m_paletteBGP, value); // BG and Window palette
            else if (address == 0xFF48)
                UpdatePalette(m_paletteOBP0, value); // Object palette 0
            else if (address == 0xFF49)
                UpdatePalette(m_paletteOBP1, value); // Object palette 1
        }
    }

    uint16_t Memory::readWord(uint16_t address) const
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

    uint8_t &Memory::operator[](uint16_t address)
    {
        return m_memory[address];
    }

    void Memory::logInvalidWriteOperation(uint16_t address, uint8_t value, const std::string &memorySection)
    {
        std::cout << std::hex << "\x1B[33m!!!\033[0m " << "Writing value 0x" << +value << " to address 0x" << address << " (" << memorySection << ")\n";
    }

    void Memory::logInvalidReadOperation(uint16_t address, const std::string &memorySection)
    {
        std::cout << std::hex << "\x1B[33m!!!\033[0m " << "Reading from address 0x" << address << " (" << memorySection << ")\n";
    }

    void Memory::UpdatePalette(Colour (&palette)[4], uint8_t value)
    {
        palette[0] = paletteColours[value & 0x3];
        palette[1] = paletteColours[(value >> 2) & 0x3];
        palette[2] = paletteColours[(value >> 4) & 0x3];
        palette[3] = paletteColours[(value >> 6) & 0x3];
    }
} // namespace gameboy
