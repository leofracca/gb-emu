/**
 * @file memory.h
 * @brief This file contains the declaration of the Memory class.
 *        It emulates the behavior of the memory of a Gameboy and the cartridge.
 */

/*
 * See pages 8 to 18 of the documentation (docs/GB.pdf)
 * See https://gbdev.io/pandocs/Memory_Map.html
 */

#pragma once

#include "cartridge.h" // Cartridge

namespace gameboy
{
    constexpr uint16_t JOYPAD_ADDRESS = 0xFF00; ///< The address of the register containing info about the joypad
    constexpr uint16_t INTERRUPT_FLAG_ADDRESS = 0xFF0F; ///< The address of the Interrupt Flag Register
    constexpr uint16_t INTERRUPT_ENABLE_ADDRESS = 0xFFFF; ///< The address of the Interrupt Enable Register

    struct Colour
    {
        uint8_t colours[4];
    };

    class Memory
    {
    public:
        uint8_t m_memory[0x10000]; ///< The memory of the Gameboy

        struct Tile {
            uint8_t  pixels[8][8] = {0};
        } tiles[384];

        Colour palette_BGP[4] = {
            { 255, 255, 255, 255},
            { 0, 0, 0, 255 },
            { 0, 0, 0, 255 },
            { 0, 0, 0, 255 },
        };

        struct Sprite {
            bool ready;
            int y;
            int x;
            uint8_t tile;
            Colour *colourPalette;
            struct {
                union {
                    struct {
                        uint8_t gbcPaletteNumber1 : 1;
                        uint8_t gbcPaletteNumber2 : 1;
                        uint8_t gbcPaletteNumber3 : 1;
                        uint8_t gbcVRAMBank: 1;
                        uint8_t paletteNumber : 1;
                        uint8_t xFlip : 1;
                        uint8_t yFlip : 1;
                        uint8_t renderPriority : 1;
                    } bits;
                    uint8_t value;
                };
            } options;
        } sprites[40] = {Sprite()};

        /**
         * @brief Construct a new Memory object
         * @details Initialize the cartridge
         *
         * @param cartridge The cartridge of the memory
         */
        Memory(Cartridge *cartridge);

        /**
         * @brief Read a byte from the memory
         * @details Read a byte from the memory at the specified address and return it
         *
         * @param address The address to read from
         * @return uint8_t The byte read
         */
        uint8_t read(uint16_t address);

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
         * @return uint16_t The word read
         */
        uint16_t readWord(uint16_t address);

        /**
         * @brief Write a word to the memory
         * @details Write a word to the memory at the specified address
         *
         * @param address The address to write to
         * @param value The value to write
         */
        void writeWord(uint16_t address, uint16_t value);

        /**
         * @brief Save the joypad state
         *
         * @param state The new joypad state
         */
        void setJoypadState(uint8_t state);

    private:
        Cartridge *m_cartridge; ///< The cartridge
        uint8_t m_joypadState = 0xFF; ///< A temporary variable used to store the joypad state when an interrupt is sent

        const Colour palette_colours[4] = {
            { 255, 255, 255, 255},
            { 192,192,192,255},
            { 96,96,96, 255 },
            { 0, 0, 0, 255 },
        };

        Colour palette_OBP0[4] = {
            { 0, 0, 0, 255 },
            { 0, 0, 0, 255 },
            { 0, 0, 0, 255 },
            { 0, 0, 0, 255 },
        };

        Colour palette_OBP1[4] = {
            { 0, 0, 0, 255 },
            { 0, 0, 0, 255 },
            { 0, 0, 0, 255 },
            { 0, 0, 0, 255 },
        };

        void UpdateTile(uint16_t address);
        void UpdateSprite(uint16_t address, uint8_t value);
        void UpdatePalette(Colour *palette, uint8_t value);
    };
}
