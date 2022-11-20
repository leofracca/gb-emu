/**
 * @file memory.h
 * @brief This file contains the declaration of the Memory class.
 *        It emulates the behavior of the memory of a Gameboy and the cartridge.
 */

/*
 * See pages 8 to 18 of the documentation (PanDocs/GB.pdf)
 * See https://gbdev.io/pandocs/Memory_Map.html
 */

#pragma once

#include "cartridge.h" // Cartridge

#include <array> // std::array

namespace gameboy
{
    constexpr uint16_t DMA_REG_ADDRESS = 0xFF46; ///< The address of the DMA register
    constexpr uint16_t BGP_REG_ADDRESS = 0xFF47; ///< The address of the BG Palette Register
    constexpr uint16_t OBP0_REG_ADDRESS = 0xFF48; ///< The address of the Object Palette 0 Register
    constexpr uint16_t OBP1_REG_ADDRESS = 0xFF49; ///< The address of the Object Palette 1 Register

    constexpr uint16_t JOYPAD_ADDRESS = 0xFF00; ///< The address of the register containing info about the joypad
    constexpr uint16_t INTERRUPT_FLAG_ADDRESS = 0xFF0F; ///< The address of the Interrupt Flag Register
    constexpr uint16_t INTERRUPT_ENABLE_ADDRESS = 0xFFFF; ///< The address of the Interrupt Enable Register

    /**
     * @brief Contains the values of the RGB colors (and the transparency)
     */
    struct Colour
    {
        uint8_t colours[4]; ///< 0 - R, 1 - G, 2 - B, 3 - Transparency
    };

    constexpr Colour paletteColours[4] = {
            {255, 255, 255, 255},
            {192, 192, 192, 255},
            {96, 96, 96, 255},
            {0, 0, 0, 255},
    }; ///< The 4 colours of the palette (white, light grey, dark grey, black)

    /**
     * @brief Memory class used to store the memory of the Gameboy
     */
    class Memory
    {
    public:
        /**
         * @brief A tile is a 8x8 pixel image. Each tile is 16 bytes long.
         */
        struct Tile
        {
            uint8_t pixels[8][8] = {{0}}; ///< The pixels of the tile (8x8)
        } m_tiles[384]; ///< There are 384 tiles

        /**
         * @brief BG & Window Palette Data
         * @details This selects the shade of grays to use for the background (BG) & window pixels.
         *          Since each pixel uses 2 bits, the corresponding shade will be selected from here.
         */
        Colour m_paletteBGP[4] = {
                {255, 255, 255, 255},
                {0, 0, 0, 255},
                {0, 0, 0, 255},
                {0, 0, 0, 255},
        };

        /**
         * @brief All the information about the sprites
         */
        struct Sprite
        {
            Colour *colourPalette; ///< The colour palette to use for the sprite
            struct
            {
                union
                {
                    struct
                    {
                        uint8_t gbcPaletteNumber1 : 1;
                        uint8_t gbcPaletteNumber2 : 1;
                        uint8_t gbcPaletteNumber3 : 1;
                        uint8_t gbcVRAMBank : 1;
                        uint8_t paletteNumber : 1;
                        uint8_t xFlip : 1;
                        uint8_t yFlip : 1;
                        uint8_t renderPriority : 1;
                    } bits;
                    uint8_t value;
                } flags;
            } options; ///< The options of the sprite
            int y; ///< The y position of the sprite
            int x; ///< The x position of the sprite
            uint8_t tile; ///< The tile number of the sprite
            bool ready; ///< True if the sprite is ready to be rendered
        } m_sprites[40] = {}; ///< The sprites

        /**
         * @brief Construct a new Memory object
         * @details Initialize the cartridge
         *
         * @param romPath The path to the ROM file
         */
        explicit Memory(const std::string &romPath);

        /**
         * @brief Read a byte from the memory
         * @details Read a byte from the memory at the specified address and return it
         *
         * @param address The address to read from
         * @return The byte read
         */
        [[nodiscard]] uint8_t read(uint16_t address) const;

        /**
         * @brief Write a byte to the memory
         * @details Write a byte to the memory at the specified address
         *
         * @param address The address to write to
         * @param value The value to write
         */
        void write(uint16_t address, uint8_t value);

        /**
         * @brief Read a word from the memory
         * @details Read a word from the memory at the specified address and return it
         *
         * @param address The address to read from
         * @return The word read
         */
        [[nodiscard]] uint16_t readWord(uint16_t address) const;

        /**
         * @brief Write a word to the memory
         * @details Write a word to the memory at the specified address
         *
         * @param address The address to write to
         * @param value The value to write
         */
        void writeWord(uint16_t address, uint16_t value);

        /**
         * @brief Get the joypad state
         *
         * @return The joypad state
         */
        [[nodiscard]] uint8_t getJoypadState() const;

        /**
         * @brief Save the joypad state
         *
         * @param state The new joypad state
         */
        void setJoypadState(uint8_t state);

        /**
         * @brief Get the cartridge
         *
         * @return The cartridge
         */
        [[nodiscard]] Cartridge &getCartridge();

        /**
         * @brief Array subscript operator
         * @details Access the field m_memory at the specified index
         *
         * @param address The address to access
         * @return A reference to the value at the specified address
         * @see m_memory
         */
        [[nodiscard]] uint8_t &operator[](uint16_t address);

    private:
        std::array<uint8_t, 0x10000> m_memory{}; ///< The memory of the Game Boy
        Cartridge m_cartridge; ///< The cartridge

        /**
         * @brief The current state of the joypad
         * @details The state is inverted (0 = pressed, 1 = not pressed),
         *          bits 0 to 3 are used to store the state of the buttons,
         *          bits 4 to 7 are used to store the state of the directions.
         *          Based on which bit is set (technically, which bit is reset), the joypad will send an interrupt.
         *
         *          This variable does not store the state like the gameboy does/expects.
         *          To get the joypad state like the Gameboy does (and the games expect), we need to call the read function.
         *
         * @see read
         */
        uint8_t m_joypadState = 0xFF; ///< A temporary variable used to store the joypad state when an interrupt is sent

        /**
         * @brief Object Palette 0 Data
         * @details This selects the colour palette for sprite palette 0.
         */
        Colour m_paletteOBP0[4] = {
                {0, 0, 0, 255},
                {0, 0, 0, 255},
                {0, 0, 0, 255},
                {0, 0, 0, 255},
        };

        /**
         * @brief Object Palette 1 Data
         * @details This selects the colour palette for sprite palette 1.
         */
        Colour m_paletteOBP1[4] = {
                {0, 0, 0, 255},
                {0, 0, 0, 255},
                {0, 0, 0, 255},
                {0, 0, 0, 255},
        };

        /**
         * @brief Update the tile at the specified address
         *
         * @param address The address of the tile
         */
        void UpdateTile(uint16_t address);

        /**
         * @brief Update the sprite at the specified address
         *
         * @param address The address of the sprite
         * @param value The value to write
         */
        void UpdateSprite(uint16_t address, uint8_t value);

        /**
         * @brief Update the palette
         * @details Update the palette with the new value
         *
         * @param address The address of the palette (BGP, OBP0 or OBP1)
         * @param value The new value of the palette (2 bits per colour)
         */
        static void UpdatePalette(Colour (&palette)[4], uint8_t value);
    };
} // namespace gameboy
