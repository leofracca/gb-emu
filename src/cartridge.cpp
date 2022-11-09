/*
 * See pages 8 to 16 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/The_Cartridge_Header.html
 */

#include "cartridge.h" // Cartridge

#include <fstream> // std::ifstream
#include <iostream> // std::cout, std::endl
#include <iterator> // std::istreambuf_iterator
#include <utility> // std::pair

namespace gameboy
{
    Cartridge::~Cartridge()
    {
        delete m_MBC;
    }

    void Cartridge::loadROM(const std::string &filename)
    {
        // Save the filename without the extension
        m_ROMFilename = filename.substr(0, filename.find_last_of('.'));

        // Open the file
        std::ifstream romFile(filename, std::ios::binary);
        if (!romFile.is_open())
            throw std::runtime_error("Could not open the file");

        // Initialize ROM
        m_rom = std::vector<uint8_t>(std::istreambuf_iterator<char>(romFile), {});
        romFile.close();

        // Check if there is a save file to initialize the RAM
        // If there is no save file, the RAM will be initialized to 0
        std::ifstream ramFile(m_ROMFilename + ".sav", std::ios::binary);
        if (!ramFile.is_open())
            m_ram = std::vector<uint8_t>(getRAMSize().first, 0x00);
        else
            m_ram = std::vector<uint8_t>(std::istreambuf_iterator<char>(ramFile), {});
        ramFile.close();

        checkCartridge();
        printCartridgeInfo();
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
                m_MBC = new ROMOnly(m_rom, m_ram);
                m_MBCAsString = "No MBC (ROM Only)";
                break;
            case 0x01:
            case 0x02:
            case 0x03:
                m_MBC = new MBC1(m_rom, m_ram);
                m_MBCAsString = "MBC1";
                break;
            case 0x05:
            case 0x06:
                m_MBC = new MBC2(m_rom, m_ram);
                m_MBCAsString = "MBC2";
                break;
            case 0x0F:
            case 0x10:
            case 0x11:
            case 0x12:
            case 0x13:
                m_MBC = new MBC3(m_rom, m_ram);
                m_MBCAsString = "MBC3";
                break;
            case 0x19:
            case 0x1A:
            case 0x1B:
            case 0x1C:
            case 0x1D:
            case 0x1E:
                m_MBC = new MBC5(m_rom, m_ram);
                m_MBCAsString = "MBC5";
                break;
            default:
                throw std::runtime_error("Invalid cartridge type");
        }
    }

    uint8_t Cartridge::read(uint16_t address) const
    {
        return m_MBC->read(address);
    }

    void Cartridge::write(uint16_t address, uint8_t value)
    {
        m_MBC->write(address, value);
    }

    void Cartridge::saveRAMData()
    {
        m_MBC->saveRAMData(m_ROMFilename + ".sav");
    }

    void Cartridge::printCartridgeInfo()
    {
        std::cout << "--------------- Cartridge info ----------------" << std::endl;
        std::cout << "Title: " << getTitle() << std::endl;
        std::cout << "Cartridge type: " << m_MBCAsString << std::endl;
        std::cout << "Licensee: " << getLicensee() << std::endl;
        std::cout << "ROM size: " << getROMSize() << std::endl;
        std::cout << "RAM size: " << getRAMSize().second << std::endl;
        std::cout << "-----------------------------------------------" << std::endl;
    }

    std::string Cartridge::getTitle() const
    {
        std::string title;
        for (int i = CARTRIDGE_TITLE_START_ADDRESS; i < CARTRIDGE_TITLE_END_ADDRESS + 1; i++)
            title += static_cast<char>(m_rom[i]);
        return title;
    }

    std::string Cartridge::getLicensee() const
    {
        // Old licensee code (0x014B)
        switch (m_rom[CARTRIDGE_OLD_LICENSEE_CODE_ADDRESS])
        {
            case 0x00: return "None";
            case 0x01: return "Nintendo";
            case 0x08: return "Capcom";
            case 0x09: return "Hot B";
            case 0x0A: return "Jaleco";
            case 0x0B: return "Coconuts Japan";
            case 0x0C: return "Elite Systems";
            case 0x13: return "EA (Electronic Arts)";
            case 0x18: return "Hudsonsoft";
            case 0x19: return "ITC Entertainment";
            case 0x1A: return "Yanoman";
            case 0x1D: return "Japan Clary";
            case 0x1F: return "Virgin Interactive";
            case 0x24: return "PCM Complete";
            case 0x25: return "San-X";
            case 0x28: return "Kotobuki Systems";
            case 0x29: return "Seta";
            case 0x30: return "Infogrames";
            case 0x31: return "Nintendo";
            case 0x32: return "Bandai";
            case 0x33: return getNewLicensee(); // New licensee code (0x0144-0x0145)
            case 0x34: return "Konami";
            case 0x35: return "HectorSoft";
            case 0x38: return "Capcom";
            case 0x39: return "Banpresto";
            case 0x3C: return ".Entertainment i";
            case 0x3E: return "Gremlin";
            case 0x41: return "Ubisoft";
            case 0x42: return "Atlus";
            case 0x44: return "Malibu";
            case 0x46: return "Angel";
            case 0x47: return "Spectrum Holoby";
            case 0x49: return "Irem";
            case 0x4A: return "Virgin Interactive";
            case 0x4D: return "Malibu";
            case 0x4F: return "U.S. Gold";
            case 0x50: return "Absolute";
            case 0x51: return "Acclaim";
            case 0x52: return "Activision";
            case 0x53: return "American Sammy";
            case 0x54: return "GameTek";
            case 0x55: return "Park Place";
            case 0x56: return "LJN";
            case 0x57: return "Matchbox";
            case 0x59: return "Milton Bradley";
            case 0x5A: return "Mindscape";
            case 0x5B: return "Romstar";
            case 0x5C: return "Naxat Soft";
            case 0x5D: return "Tradewest";
            case 0x60: return "Titus";
            case 0x61: return "Virgin Interactive";
            case 0x67: return "Ocean";
            case 0x69: return "EA (Electronic Arts)";
            case 0x6E: return "Elite Systems";
            case 0x6F: return "Electro Brain";
            case 0x70: return "Infogrames";
            case 0x71: return "Interplay";
            case 0x72: return "Broderbund";
            case 0x73: return "Sculptered Soft";
            case 0x75: return "The Sales Curve";
            case 0x78: return "t.hq";
            case 0x79: return "Accolade";
            case 0x7A: return "Triffix Entertainment";
            case 0x7C: return "Microprose";
            case 0x7F: return "Kemco";
            case 0x80: return "Misawa Entertainment";
            case 0x83: return "Lozc";
            case 0x86: return "Tokuma Shoten Intermedia";
            case 0x8B: return "Bullet-Proof Software";
            case 0x8C: return "Vic Tokai";
            case 0x8E: return "Ape";
            case 0x8F: return "I'Max";
            case 0x91: return "Chunsoft Co.";
            case 0x92: return "Video System";
            case 0x93: return "Tsubaraya Productions Co.";
            case 0x95: return "Varie Corporation";
            case 0x96: return "Yonezawa/S'Pal";
            case 0x97: return "Kaneko";
            case 0x99: return "Arc";
            case 0x9A: return "Nihon Bussan";
            case 0x9B: return "Tecmo";
            case 0x9C: return "Imagineer";
            case 0x9D: return "Banpresto";
            case 0x9F: return "Nova";
            case 0xA1: return "Hori Electric";
            case 0xA2: return "Bandai";
            case 0xA4: return "Konami";
            case 0xA6: return "Kawada";
            case 0xA7: return "Takara";
            case 0xA9: return "Technos Japan";
            case 0xAA: return "Broderbund";
            case 0xAC: return "Toei Animation";
            case 0xAD: return "Toho";
            case 0xAF: return "Namco";
            case 0xB0: return "acclaim";
            case 0xB1: return "ASCII or Nexsoft";
            case 0xB2: return "Bandai";
            case 0xB4: return "Square Enix";
            case 0xB6: return "HAL Laboratory";
            case 0xB7: return "SNK";
            case 0xB9: return "Pony Canyon";
            case 0xBA: return "Culture Brain";
            case 0xBB: return "Sunsoft";
            case 0xBD: return "Sony Imagesoft";
            case 0xBF: return "Sammy";
            case 0xC0: return "Taito";
            case 0xC2: return "Kemco";
            case 0xC3: return "Squaresoft";
            case 0xC4: return "Tokuma Shoten Intermedia";
            case 0xC5: return "Data East";
            case 0xC6: return "Tonkinhouse";
            case 0xC8: return "Koei";
            case 0xC9: return "UFL";
            case 0xCA: return "Ultra";
            case 0xCB: return "Vap";
            case 0xCC: return "Use Corporation";
            case 0xCD: return "Meldac";
            case 0xCE: return ".Pony Canyon";
            case 0xCF: return "Angel";
            case 0xD0: return "Taito";
            case 0xD1: return "Sofel";
            case 0xD2: return "Quest";
            case 0xD3: return "Sigma Enterprises";
            case 0xD4: return "ASK Kodansha Co.";
            case 0xD6: return "Naxat Soft";
            case 0xD7: return "Copya System";
            case 0xD9: return "Banpresto";
            case 0xDA: return "Tomy";
            case 0xDB: return "LJN";
            case 0xDD: return "NCS";
            case 0xDE: return "Human";
            case 0xDF: return "Altron";
            case 0xE0: return "Jaleco";
            case 0xE1: return "Towa Chiki";
            case 0xE2: return "Yutaka";
            case 0xE3: return "Varie";
            case 0xE5: return "Epoch";
            case 0xE7: return "Athena";
            case 0xE8: return "Asmik ACE Entertainment";
            case 0xE9: return "Natsume";
            case 0xEA: return "King Records";
            case 0xEB: return "Atlus";
            case 0xEC: return "Epic/Sony Records";
            case 0xEE: return "IGS";
            case 0xF0: return "A Wave";
            case 0xF3: return "Extreme Entertainment";
            case 0xFF: return "LJN";
            default: return "Unknown";
        }
    }

    std::string Cartridge::getNewLicensee() const
    {
        // Read addresses 0x0144 and 0x0145 as ascii
        std::string newLicenseeCode;
        newLicenseeCode += static_cast<char>(m_rom[CARTRIDGE_NEW_LICENSEE_CODE_ADDRESS]);
        newLicenseeCode += static_cast<char>(m_rom[CARTRIDGE_NEW_LICENSEE_CODE_ADDRESS + 1]);

        switch (std::stoi(newLicenseeCode))
        {
            case 0x00: return "None";
            case 0x01: return "Nintendo R&D1";
            case 0x08: return "Capcom";
            case 0x13: return "Electronic Arts";
            case 0x18: return "Hudson Soft";
            case 0x19: return "b-ai";
            case 0x20: return "kss";
            case 0x22: return "pow";
            case 0x24: return "PCM Complete";
            case 0x25: return "san-x";
            case 0x28: return "Kemco Japan";
            case 0x29: return "seta";
            case 0x30: return "Viacom";
            case 0x31: return "Nintendo";
            case 0x32: return "Bandai";
            case 0x33: return "Ocean/Acclaim";
            case 0x34: return "Konami";
            case 0x35: return "Hector";
            case 0x37: return "Taito";
            case 0x38: return "Hudson";
            case 0x39: return "Banpresto";
            case 0x41: return "Ubi Soft";
            case 0x42: return "Atlus";
            case 0x44: return "Malibu";
            case 0x46: return "angel";
            case 0x47: return "Bullet-Proof";
            case 0x49: return "irem";
            case 0x50: return "Absolute";
            case 0x51: return "Acclaim";
            case 0x52: return "Activision";
            case 0x53: return "American Sammy";
            case 0x54: return "Konami";
            case 0x55: return "Hi tech entertainment";
            case 0x56: return "LJN";
            case 0x57: return "Matchbox";
            case 0x58: return "Mattel";
            case 0x59: return "Milton Bradley";
            case 0x60: return "Titus";
            case 0x61: return "Virgin";
            case 0x64: return "LucasArts";
            case 0x67: return "Ocean";
            case 0x69: return "Electronic Arts";
            case 0x70: return "Infogrames";
            case 0x71: return "Interplay";
            case 0x72: return "Broderbund";
            case 0x73: return "sculptured";
            case 0x75: return "sci";
            case 0x78: return "THQ";
            case 0x79: return "Accolade";
            case 0x80: return "misawa";
            case 0x83: return "lozc";
            case 0x86: return "Tokuma Shoten Intermedia";
            case 0x87: return "Tsukuda Original";
            case 0x91: return "Chunsoft";
            case 0x92: return "Video system";
            case 0x93: return "Ocean/Acclaim";
            case 0x95: return "Varie";
            case 0x96: return "Yonezawa/s'pal";
            case 0x97: return "Kaneko";
            case 0x99: return "Pack in soft";
            case 0xA4: return "Konami (Yu-Gi-Oh!)";
            default: return "Unknown";
        }
    }

    std::string Cartridge::getROMSize() const
    {
        switch (m_rom[CARTRIDGE_ROM_SIZE_ADDRESS])
        {
            case 0x00: return "32KByte (2 banks, no ROM banking)";
            case 0x01: return "64KByte (4 banks)";
            case 0x02: return "128KByte (8 banks)";
            case 0x03: return "256KByte (16 banks)";
            case 0x04: return "512KByte (32 banks)";
            case 0x05: return "1MByte (64 banks)";
            case 0x06: return "2MByte (128 banks)";
            case 0x07: return "4MByte (256 banks)";
            case 0x08: return "8MByte (512 banks)";
            default: return "Unknown";
        }
    }

    std::pair<int, std::string> Cartridge::getRAMSize() const
    {
        switch (m_rom[CARTRIDGE_RAM_SIZE_ADDRESS])
        {
            case 0x00: return std::make_pair(0, "No RAM");
            case 0x02: return std::make_pair(8192, "8 KByte (1 bank)");
            case 0x03: return std::make_pair(32768, "32 KByte (4 banks of 8KByte each)");
            case 0x04: return std::make_pair(131072, "128 KByte (16 banks of 8KByte each)");
            case 0x05: return std::make_pair(65536, "64 KByte (8 banks of 8KByte each)");
            default: return std::make_pair(0, "Unknown");
        }
    }
} // namespace gameboy
