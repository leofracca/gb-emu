/*
 * See pages 8 to 16 of the documentation (docs/GB.pdf)
 */

#include "cartridge.h" // Cartridge

#include <fstream> // ifstream

namespace gameboy
{
    Cartridge::~Cartridge()
    {
        delete m_MBC;
    }

    void Cartridge::loadROM(const std::string &filename)
    {
        m_ROMFilename = filename;

        // Open the file
        std::ifstream romFile(m_ROMFilename, std::ios::binary);
        if (!romFile.is_open())
            throw std::runtime_error("Could not open the file");

        // Save the game in the memory
        romFile.seekg(std::ios::beg);
        romFile.read(reinterpret_cast<char *>(m_rom), sizeof(m_rom));

        checkCartridge();
    }

    void Cartridge::checkCartridge()
    {
        // Get the cartridge type
        uint8_t cartridgeType = m_rom[CARTRIDGE_TYPE_ADDRESS];

        // Check the cartridge type and set the MBC accordingly
        switch (cartridgeType)
        {
            case 0x00:
            case 0x08:
            case 0x09:
                m_MBC = new ROMOnly(m_rom);
                break;
            case 0x01:
            case 0x02:
            case 0x03:
                m_MBC = new MBC1(m_rom, m_ram);
                break;
            case 0x05:
            case 0x06:
                m_MBC = new MBC2(m_rom, m_ram);
                break;
            case 0x0f:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
                m_MBC = new MBC3(m_rom, m_ram);
                break;
            case 0x19:
            case 0x1a:
            case 0x1b:
            case 0x1c:
            case 0x1d:
            case 0x1e:
                m_MBC = new MBC5(m_rom, m_ram);
                break;
            default:
                throw std::runtime_error("Invalid cartridge type");
        }
    }

    uint8_t Cartridge::read(uint16_t address)
    {
        return m_MBC->read(address);
    }

    void Cartridge::write(uint16_t address, uint8_t value)
    {
        m_MBC->write(address, value);
    }
} // namespace gameboy
