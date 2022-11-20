/*
 * See pages 22 to 27 and pages 51 to 60 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/Rendering.html
 * See http://www.codeslinger.co.uk/pages/projects/gameboy/lcd.html
 * See http://www.codeslinger.co.uk/pages/projects/gameboy/graphics.html
 */

#include "ppu.h" // PPU

namespace gameboy
{
    PPU::PPU(Memory *memory)
        : m_memory(memory)
    {
        m_lcdc = &(*m_memory)[LCDC_REG_ADDRESS];
        m_stat = &(*m_memory)[STAT_REG_ADDRESS];
        m_scy = &(*m_memory)[SCY_REG_ADDRESS];
        m_scx = &(*m_memory)[SCX_REG_ADDRESS];
        m_ly = &(*m_memory)[LY_REG_ADDRESS];
    }

    void PPU::cycle(int cycles)
    {
        uint8_t interruptFlag = m_memory->read(INTERRUPT_FLAG_ADDRESS);

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
                    if (*m_ly == SCREEN_HEIGHT)
                    {
                        m_mode = Mode::VBLANK;
                        m_renderingEnabled = true;

                        // Set the interrupt flag for VBLANK
                        interruptFlag |= VBLANK_INTERRUPT_FLAG_VALUE;
                        m_memory->write(INTERRUPT_FLAG_ADDRESS, interruptFlag);

                        // If the bit of the VBLANK of the stat register is set (bit 4), set the interrupt flag
                        if (*m_stat & 0x10)
                        {
                            // Set the interrupt flag for LCD Status
                            interruptFlag |= LCD_STATUS_INTERRUPT_FLAG_VALUE;
                            m_memory->write(INTERRUPT_FLAG_ADDRESS, interruptFlag);
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
                            m_memory->write(INTERRUPT_FLAG_ADDRESS, interruptFlag);
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
                            m_memory->write(INTERRUPT_FLAG_ADDRESS, interruptFlag);
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
                        m_memory->write(INTERRUPT_FLAG_ADDRESS, interruptFlag);
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
        uint8_t lyc = m_memory->read(LYC_REG_ADDRESS);

        if (*m_ly == lyc)
            // Set the coincidence flag (bit 2 of the STAT register)
            *m_stat |= 0x04;
        else
            // Reset the coincidence flag (bit 2 of the STAT register)
            *m_stat &= 0xFB;

        if (*m_ly == lyc && (*m_stat & 0x40))
        {
            // If the bit of the coincidence of the stat register is set (bit 6), set the interrupt flag
            uint8_t interruptFlag = m_memory->read(INTERRUPT_FLAG_ADDRESS);
            interruptFlag |= LCD_STATUS_INTERRUPT_FLAG_VALUE;
            m_memory->write(INTERRUPT_FLAG_ADDRESS, interruptFlag);
        }
    }

    // The following functions come from https://github.com/Mika412/NoobBoy/blob/master/src/ppu.h
    void PPU::draw()
    {
        bool rowPixels[160] = {false};
        drawBackground(rowPixels);
        drawWindow();
        drawSprites(rowPixels);
    }

    void PPU::drawBackground(bool *rowPixels)
    {
        uint16_t address = 0x9800;

        // Background tile map address
        if (*m_lcdc & 0x08)
            address += 0x400;

        // Background tile data address
        address += ((*m_scy + *m_ly) / 8 * 32) % (32 * 32);

        uint16_t start_row_address = address;
        uint16_t end_row_address = address + 32;
        address += (*m_scx >> 3);

        int x = *m_scx & 7;
        int y = (*m_ly + *m_scy) & 7;
        int pixelOffset = *m_ly * 160;

        int pixel = 0;
        for (int i = 0; i < 21; i++)
        {
            uint16_t tile_address = address + i;
            if (tile_address >= end_row_address)
                tile_address = (start_row_address + tile_address % end_row_address);

            int tile = m_memory->read(tile_address);
            if (!(*m_lcdc & 0x10) && tile < 128)
                tile += 256;

            for (; x < 8; x++)
            {
                if (pixel >= 160) break;

                int colour = m_memory->m_tiles[tile].pixels[y][x];
                m_frameBuffer[pixelOffset++] = m_memory->m_paletteBGP[colour];
                if (colour > 0)
                    rowPixels[pixel] = true;
                pixel++;
            }
            x = 0;
        }
    }

    void PPU::drawWindow()
    {
        if (!(*m_lcdc & 0x20))
            return;

        if (m_memory->read(WY_REG_ADDRESS) > *m_ly)
            return;

        uint16_t address = 0x9800;
        if (*m_lcdc & 0x40)
            address += 0x400;

        address += ((*m_ly - m_memory->read(WY_REG_ADDRESS)) / 8) * 32;
        int y = (*m_ly - m_memory->read(WY_REG_ADDRESS)) & 7;
        int x = 0;

        unsigned pixelOffset = *m_ly * 160;
        pixelOffset += m_memory->read(WX_REG_ADDRESS) - 7;
        for (uint16_t tile_address = address; tile_address < address + 20; tile_address++)
        {
            int tile = m_memory->read(tile_address);

            if (!(*m_lcdc & 0x10) && tile < 128)
                tile += 256;

            for (; x < 8; x++)
            {
                if (pixelOffset > sizeof(m_frameBuffer)) continue;
                int colour = m_memory->m_tiles[tile].pixels[y][x];
                m_frameBuffer[pixelOffset++] = m_memory->m_paletteBGP[colour];
            }
            x = 0;
        }
    }

    void PPU::drawSprites(const bool *rowPixels)
    {
        int sprite_height = *m_lcdc & 0x04 ? 16 : 8;

        bool visible_sprites[40];
        int sprite_row_count = 0;

        for (int i = 39; i >= 0; i--)
        {
            auto sprite = m_memory->m_sprites[i];

            if (!sprite.ready)
            {
                visible_sprites[i] = false;
                continue;
            }

            if ((sprite.y > *m_ly) || ((sprite.y + sprite_height) <= *m_ly))
            {
                visible_sprites[i] = false;
                continue;
            }

            visible_sprites[i] = sprite_row_count++ <= 10;
        }

        for (int i = 39; i >= 0; i--)
        {
            if (!visible_sprites[i])
                continue;

            auto sprite = m_memory->m_sprites[i];

            if ((sprite.x < -7) || (sprite.x >= 160))
                continue;

            // Flip vertically
            int pixel_y = *m_ly - sprite.y;
            pixel_y = sprite.options.flags.bits.yFlip ? (7 + 8 * (*m_lcdc & 0x04)) - pixel_y : pixel_y;

            for (int x = 0; x < 8; x++)
            {
                int tile_num = sprite.tile & ((*m_lcdc & 0x04) ? 0xFE : 0xFF);
                int colour;

                int x_temp = sprite.x + x;
                if (x_temp < 0 || x_temp >= 160)
                    continue;

                int pixelOffset = *this->m_ly * 160 + x_temp;

                // Flip horizontally
                uint8_t pixel_x = sprite.options.flags.bits.xFlip ? 7 - x : x;

                if ((*m_lcdc & 0x04) && (pixel_y >= 8))
                    colour = m_memory->m_tiles[tile_num + 1].pixels[pixel_y - 8][pixel_x];
                else
                    colour = m_memory->m_tiles[tile_num].pixels[pixel_y][pixel_x];

                // Black is transparent
                if (!colour)
                    continue;

                if (!rowPixels[x_temp] || !sprite.options.flags.bits.renderPriority)
                    m_frameBuffer[pixelOffset] = sprite.colourPalette[colour];
            }
        }
    }
} // namespace gameboy
