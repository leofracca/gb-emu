/**
 * @file ppu.h
 * @brief This file contains the declaration of the PPU class.
 *        It emulates the behavior of the PPU (Pixel Processing Unit) of a Gameboy.
 *        It is used to display the screen of the Gameboy.
 */

/*
 * See pages 22 to 27 and pages 51 to 60 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/Rendering.html
 * See http://www.codeslinger.co.uk/pages/projects/gameboy/lcd.html
 * See http://www.codeslinger.co.uk/pages/projects/gameboy/graphics.html
 */

#pragma once

#include "memory.h" // Memory

#include <array> // std::array

namespace gameboy
{
    constexpr int SCREEN_WIDTH = 160; ///< The width of the screen in pixels
    constexpr int SCREEN_HEIGHT = 144; ///< The height of the screen in pixels

    constexpr uint16_t LCDC_REG_ADDRESS = 0xFF40; ///< The address of the LCD Control Register
    constexpr uint16_t STAT_REG_ADDRESS = 0xFF41; ///< The address of the LCD Status Register
    constexpr uint16_t SCY_REG_ADDRESS = 0xFF42; ///< The address of the Scroll Y Register (Background Y Position)
    constexpr uint16_t SCX_REG_ADDRESS = 0xFF43; ///< The address of the Scroll X Register (Background X Position)
    constexpr uint16_t LY_REG_ADDRESS = 0xFF44; ///< The address of the LCDY Register
    constexpr uint16_t LYC_REG_ADDRESS = 0xFF45; ///< The address of the LY Compare Register
    // constexpr uint16_t BGP_REG_ADDRESS = 0xFF47; ///< The address of the BG Palette Register
    // constexpr uint16_t OBP0_REG_ADDRESS = 0xFF48; ///< The address of the Object Palette 0 Register
    // constexpr uint16_t OBP1_REG_ADDRESS = 0xFF49; ///< The address of the Object Palette 1 Register

    constexpr uint16_t WY_REG_ADDRESS = 0xFF4A; ///< The address of the Window Y Position Register (Window Y Position)
    constexpr uint16_t WX_REG_ADDRESS = 0xFF4B; ///< The address of the Window X Position Register (Window X Position - 7)

    constexpr uint8_t VBLANK_INTERRUPT_FLAG = 0x01; ///< The bitmask of the VBLANK interrupt flag
    constexpr uint8_t LCD_STATUS_INTERRUPT_FLAG = 0x02; ///< The bitmask of the LCD Status interrupt flag

    /**
     * @brief LCD status register (STAT, 0xFF41, bit 0-1)
     */
    enum class Mode : uint8_t
    {
        HBLANK = 0, ///< The PPU is in the HBLANK mode
        VBLANK = 1, ///< The PPU is in the VBLANK mode
        OAM = 2, ///< The PPU is in the OAM mode
        VRAM = 3 ///< The PPU is in the VRAM mode
    };

    /**
     * @brief The PPU class emulates the behavior of the PPU (Pixel Processing Unit) of a Gameboy.
     */
    class PPU
    {
    public:
        /**
         * @brief Construct a new PPU object
         *
         * @param memory The memory
         */
        explicit PPU(Memory *memory);

        /**
         * @brief Update the PPU
         * @details Manipulate the PPU and set the interrupt flag if necessary.
         *
         * @param cycles The number of cycles to increment the PPU by
         */
        void cycle(uint64_t cycles);

        /**
         * @brief Get the frame buffer
         *
         * @return The frame buffer
         */
        Colour* getFrameBuffer();

        /**
         * @biref Return whether the rendering is enabled
         *
         * @return true if the rendering is enabled
         */
        bool isRenderingEnabled() const;

        /**
         * @brief Set/Reset the variable that indicates whether the rendering is enabled
         *
         * @param enabled True if the rendering is enabled, false otherwise
         */
        void setRenderingEnabled(bool enabled);

    private:
        Memory *m_memory; ///< The memory

        std::array<Colour, SCREEN_WIDTH * (SCREEN_HEIGHT + 9)> m_frameBuffer{}; ///< The frame buffer
        bool m_renderingEnabled = false; ///< Whether the PPU can render the screen

        uint64_t m_cycles = 0; ///< The number of cycles since the last frame
        Mode m_mode = Mode::HBLANK; ///< The current mode of the PPU

        // Registers
        uint8_t *lcdc; ///< The LCD Control Register
        uint8_t *stat; ///< The LCD Status Register
        uint8_t *scy; ///< The Scroll Y Register (Background Y Position)
        uint8_t *scx; ///< The Scroll X Register (Background X Position)
        uint8_t *ly; ///< The LY Register

        /**
         * @brief Check if LY and LYC are equal
         * @details Check if the LY register and the LYC register are equal.
         *          If they are, set the coincidence flag (bit 2) in the STAT register. Otherwise, reset it.
         */
        void setCoincidenceFlag();

        // The following functions come from https://github.com/Mika412/NoobBoy/blob/master/src/ppu.h
        /**
         * @brief Draw the lines on the screen
         *
         * @see drawBackground
         * @see drawWindow
         * @see drawSprites
         */
        void draw();

        /**
         * @brief Draw the background on the screen
         */
        void drawBackground(bool *rowPixels);

        /**
         * @brief Draw the window on the screen
         */
        void drawWindow();

        /**
         * @brief Draw the sprites on the screen
         */
        void drawSprites(bool *rowPixels);
    };
} // namespace gameboy
