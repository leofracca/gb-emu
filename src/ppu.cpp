/*
 * See pages 22 to 27 and pages 51 to 60 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/Rendering.html
 * See http://www.codeslinger.co.uk/pages/projects/gameboy/lcd.html
 * See http://www.codeslinger.co.uk/pages/projects/gameboy/graphics.html
 */

#include "ppu.h" // PPU

#include <any> // std::any

namespace gameboy
{
    PPU::PPU(Memory &memory)
        : m_memory(memory)
    {
        m_lcdc = &m_memory[ppu_registers::LCDC_REG_ADDRESS];
        m_stat = &m_memory[ppu_registers::STAT_REG_ADDRESS];
        m_scy = &m_memory[ppu_registers::SCY_REG_ADDRESS];
        m_scx = &m_memory[ppu_registers::SCX_REG_ADDRESS];
        m_ly = &m_memory[ppu_registers::LY_REG_ADDRESS];
        m_wx = &m_memory[ppu_registers::WX_REG_ADDRESS];
        m_wy = &m_memory[ppu_registers::WY_REG_ADDRESS];
    }

    void PPU::cycle(uint8_t cycles)
    {
        uint8_t interruptFlag = m_memory.read(interrupt_registers::INTERRUPT_FLAG_ADDRESS);

        m_cycles += cycles;

        // Check if the LCD is enabled
        if (!(*m_lcdc & 0x80))
        {
            m_mode = Mode::HBLANK;
            return;
        }

        // Cycle through the different modes
        switch (m_mode)
        {
            case Mode::HBLANK:
                if (m_cycles >= 204)
                {
                    m_cycles -= 204;

                    // Increment the scanline counter
                    *m_ly += 1;
                    m_mode = Mode::OAM;

                    // Check if the LY register is equal to the LYC register
                    setCoincidenceFlag();

                    // If LY >= 144, set mode to VBLANK to refresh the screen
                    if (*m_ly == screen_size::SCREEN_HEIGHT)
                    {
                        m_mode = Mode::VBLANK;
                        m_renderingEnabled = true;

                        // Set the interrupt flag for VBLANK
                        interruptFlag |= VBLANK_INTERRUPT_FLAG_VALUE;
                        m_memory.write(interrupt_registers::INTERRUPT_FLAG_ADDRESS, interruptFlag);

                        // If the bit of the VBLANK of the stat register is set (bit 4), set the interrupt flag
                        if (*m_stat & 0x10)
                        {
                            // Set the interrupt flag for LCD Status
                            interruptFlag |= LCD_STATUS_INTERRUPT_FLAG_VALUE;
                            m_memory.write(interrupt_registers::INTERRUPT_FLAG_ADDRESS, interruptFlag);
                        }
                    }
                    // If LY < 144, set mode to OAM to render the next scanline
                    else
                    {
                        // If the bit of the OAM of the stat register is set (bit 5), set the interrupt flag
                        if (*m_stat & 0x20)
                        {
                            // Set the interrupt flag for LCD Status
                            interruptFlag |= LCD_STATUS_INTERRUPT_FLAG_VALUE;
                            m_memory.write(interrupt_registers::INTERRUPT_FLAG_ADDRESS, interruptFlag);
                        }
                        m_mode = Mode::OAM;
                    }
                    // Update the stat register
                    *m_stat = (*m_stat & 0xFC) | static_cast<uint8_t>(m_mode);
                }
                break;
            case Mode::VBLANK:
                if (m_cycles >= 456)
                {
                    m_cycles -= 456;

                    // Increment the scanline counter
                    *m_ly += 1;
                    // Check if the LY register is equal to the LYC register
                    setCoincidenceFlag();

                    // End of VBLANK
                    if (*m_ly == 153)
                    {
                        // Reset the scanline counter
                        *m_ly = 0;
                        m_mode = Mode::OAM;

                        // Update the stat register
                        *m_stat = (*m_stat & 0xFC) | static_cast<uint8_t>(m_mode);

                        // If the bit of the OAM of the stat register is set (bit 5), set the interrupt flag
                        if (*m_stat & 0x20)
                        {
                            // Set the interrupt flag for LCD Status
                            interruptFlag |= LCD_STATUS_INTERRUPT_FLAG_VALUE;
                            m_memory.write(interrupt_registers::INTERRUPT_FLAG_ADDRESS, interruptFlag);
                        }
                    }
                }
                break;
            case Mode::OAM:
                if (m_cycles >= 80)
                {
                    m_cycles -= 80;
                    m_mode = Mode::VRAM;
                    // Update the stat register
                    *m_stat = (*m_stat & 0xFC) | static_cast<uint8_t>(m_mode);
                }
                break;
            case Mode::VRAM:
                if (m_cycles >= 172)
                {
                    m_cycles -= 172;
                    m_mode = Mode::HBLANK;

                    // Render the scanline
                    draw();

                    // Update the stat register
                    *m_stat = (*m_stat & 0xFC) | static_cast<uint8_t>(m_mode);

                    // If the bit of the HBLANK of the stat register is set (bit 3), set the interrupt flag
                    if (*m_stat & 0x08)
                    {
                        // Set the interrupt flag for LCD Status
                        interruptFlag |= LCD_STATUS_INTERRUPT_FLAG_VALUE;
                        m_memory.write(interrupt_registers::INTERRUPT_FLAG_ADDRESS, interruptFlag);
                    }
                }
                break;
        }
    }

    Colour *PPU::getFrameBuffer()
    {
        return m_frameBuffer.data();
    }

    bool PPU::isRenderingEnabled() const
    {
        return m_renderingEnabled;
    }

    void PPU::setRenderingEnabled(bool enabled)
    {
        m_renderingEnabled = enabled;
    }

    void PPU::setCoincidenceFlag()
    {
        uint8_t lyc = m_memory.read(ppu_registers::LYC_REG_ADDRESS);

        if (*m_ly == lyc)
        {
            // Set the coincidence flag (bit 2 of the STAT register)
            *m_stat |= 0x04;

            if (*m_stat & 0x40)
            {
                // If the bit of the coincidence of the stat register is set (bit 6), set the interrupt flag
                uint8_t interruptFlag = m_memory.read(interrupt_registers::INTERRUPT_FLAG_ADDRESS);
                interruptFlag |= LCD_STATUS_INTERRUPT_FLAG_VALUE;
                m_memory.write(interrupt_registers::INTERRUPT_FLAG_ADDRESS, interruptFlag);
            }
        }
        else
            // Reset the coincidence flag (bit 2 of the STAT register)
            *m_stat &= 0xFB;
    }

    void PPU::draw()
    {
        // Render only if the LCD is enabled (bit 7 of the LCDC register)
        if (*m_lcdc & 0x80)
        {
            renderBackground();
            renderWindow();
            renderSprites();
        }
    }

    void PPU::renderBackground()
    {
        // Tile map area
        uint16_t tileMapOffset = (*m_lcdc & 0x08) ? 0x9C00 : 0x9800;
        // Tile data area
        uint16_t tileDataOffset = (*m_lcdc & 0x10) ? 0x8000 : 0x8800;
        bool unsignedTileNumbers = (tileDataOffset == 0x8000);

        // Get the y coordinate of the tile
        uint8_t y = *m_ly + *m_scy;
        // Get the row of the pixel of the tile the scanline is on
        uint16_t tileRow = (y / 8) * 32;

        // Draw the scanline
        auto bufferOffset = *m_ly * screen_size::SCREEN_WIDTH;
        for (uint8_t pixel = 0; pixel < screen_size::SCREEN_WIDTH; pixel++)
        {
            uint8_t x = pixel + *m_scx;

            uint16_t tileColumn = x / 8;
            // Get the tile id number
            std::any tileNumber = m_memory.read(tileMapOffset + tileRow + tileColumn);
            if (!unsignedTileNumbers)
                tileNumber = static_cast<int8_t>(std::any_cast<uint8_t>(tileNumber));

            // Get the current tile address
            uint16_t tileAddress = tileDataOffset;
            if (unsignedTileNumbers)
                tileAddress += (std::any_cast<uint8_t>(tileNumber) * 16);
            else
                tileAddress += ((std::any_cast<int8_t>(tileNumber) + 128) * 16);

            uint8_t line = y % 8;
            line *= 2; // Each line takes 2 bytes
            uint8_t data1 = m_memory.read(tileAddress + line);
            uint8_t data2 = m_memory.read(tileAddress + line + 1);

            uint8_t colourBit = -((x % 8) - 7);
            uint8_t colourId = ((data2 >> colourBit) & 1) << 1 | ((data1 >> colourBit) & 1);
            m_frameBuffer[bufferOffset + pixel] = m_memory.m_paletteBGP[colourId];
        }
    }

    void PPU::renderWindow()
    {
        // Check if window is enabled
        if (!(*m_lcdc & 0x20) || *m_wy > *m_ly)
            return;

        // Tile map area
        uint16_t tileMapOffset = (*m_lcdc & 0x40) ? 0x9C00 : 0x9800;
        // Tile data area
        uint16_t tileDataOffset = (*m_lcdc & 0x10) ? 0x8000 : 0x8800;
        bool unsignedTileNumbers = (tileDataOffset == 0x8000);

        // Get the y coordinate of the tile
        uint8_t y = *m_ly - *m_wy;
        // Get the row of the pixel of the tile the scanline is on
        uint16_t tileRow = tileMapOffset + (y / 8) * 32;

        // Draw the scanline
        auto bufferOffset = *m_ly * screen_size::SCREEN_WIDTH;
        for (uint8_t pixel = 0; pixel < screen_size::SCREEN_WIDTH; pixel++)
        {
            if (pixel < *m_wx - 7)
                continue;

            uint8_t x = pixel - (*m_wx - 7);

            uint16_t tileColumn = x / 8;
            // Get the tile id number
            std::any tileNumber = m_memory.read(tileRow + tileColumn);
            if (!unsignedTileNumbers)
                tileNumber = static_cast<int8_t>(std::any_cast<uint8_t>(tileNumber));

            // Get the current tile address
            uint16_t tileAddress = tileDataOffset;
            if (unsignedTileNumbers)
                tileAddress += (std::any_cast<uint8_t>(tileNumber) * 16);
            else
                tileAddress += ((std::any_cast<int8_t>(tileNumber) + 128) * 16);

            uint8_t line = y % 8;
            line *= 2; // Each line takes 2 bytes
            uint8_t data1 = m_memory.read(tileAddress + line);
            uint8_t data2 = m_memory.read(tileAddress + line + 1);

            uint8_t colourBit = -((x % 8) - 7);
            uint8_t colourId = ((data2 >> colourBit) & 1) << 1 | ((data1 >> colourBit) & 1);
            m_frameBuffer[bufferOffset + pixel] = m_memory.m_paletteBGP[colourId];
        }
    }

    void PPU::renderSprites()
    {
        // Check if sprites are enabled
        if (!(*m_lcdc & 0x02))
            return;

        for (int sprite = 0; sprite < 40; sprite++)
        {
            uint8_t index = sprite * 4; // Each sprite takes 4 bytes
            // Get the sprite attributes
            auto y = static_cast<int16_t>(m_memory.read(ppu_registers::OAM_ADDRESS + index) - 16);
            auto x = static_cast<int16_t>(m_memory.read(ppu_registers::OAM_ADDRESS + index + 1) - 8);
            uint8_t tileIndex = m_memory.read(ppu_registers::OAM_ADDRESS + index + 2);
            uint8_t flags = m_memory.read(ppu_registers::OAM_ADDRESS + index + 3);

            // Sprite size: 8x8 or 8x16
            uint8_t height = (*m_lcdc & 0x04) ? 16 : 8;

            // Check if the sprite is on the current scanline
            if (*m_ly >= y && *m_ly < y + height)
            {
                // Get the sprite line
                uint8_t line = *m_ly - y;
                // Check if the sprite is y-flipped
                if (flags & 0x40)
                    line = -(line - (height - 1));
                line *= 2; // Each line takes 2 bytes

                // Get the sprite tile address
                uint16_t tileAddress = 0x8000 + (tileIndex * 16) + line;

                uint8_t data1 = m_memory.read(tileAddress);
                uint8_t data2 = m_memory.read(tileAddress + 1);

                // Draw the sprite
                for (int pixel = 7; pixel >= 0; pixel--)
                {
                    uint8_t colourBit = pixel;
                    // Check if the sprite is x-flipped
                    if (flags & 0x20)
                        colourBit = -((pixel - 7));
                    uint8_t colourId = ((data2 >> colourBit) & 1) << 1 | ((data1 >> colourBit) & 1);

                    // Check if the colour is not transparent
                    if (colourId != 0)
                    {
                        int pixell = x + (7 - pixel);
                        if ((pixell < 0) || (pixell > 159)) continue;

                        // Check if the sprite is behind the background
                        if (flags & 0x80)
                        {
                            // Check if the background pixel is not white
                            if (m_frameBuffer[*m_ly * screen_size::SCREEN_WIDTH + pixell][0] != 255 ||
                                m_frameBuffer[*m_ly * screen_size::SCREEN_WIDTH + pixell][1] != 255 ||
                                m_frameBuffer[*m_ly * screen_size::SCREEN_WIDTH + pixell][2] != 255 ||
                                m_frameBuffer[*m_ly * screen_size::SCREEN_WIDTH + pixell][3] != 255)
                            {
                                continue;
                            }
                        }

                        // Set the correct OBJ palette (bit 4 of the attributes of the sprite)
                        if (flags & 0x10)
                            m_frameBuffer[*m_ly * screen_size::SCREEN_WIDTH + pixell] = m_memory.m_paletteOBP1[colourId];
                        else
                            m_frameBuffer[*m_ly * screen_size::SCREEN_WIDTH +pixell] = m_memory.m_paletteOBP0[colourId];
                    }
                }
            }
        }
    }
} // namespace gameboy
