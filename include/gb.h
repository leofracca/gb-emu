/**
 * @file gb.h
 * @brief This file contains the declaration of the GB class.
 *        It includes all the other classes and is responsible for the emulator to run.
 */

#pragma once

#include "cartridge.h" // Cartridge
#include "cpu.h" // CPU
#include "input.h" // Input
#include "memory.h" // Memory
#include "platform.h" // Platform
#include "ppu.h" // PPU
#include "timer.h" // Timer

namespace gameboy
{
    /**
     * @brief The GB class is responsible for the emulator to run.
     */
    class GB
    {
    public:
        /**
         * @brief Create and initialize the emulator
         *
         * @param scale The scale of the window
         */
        explicit GB(int scale);

        /**
         * @brief Run the emulator
         * @details Do the steps of the CPU, PPU, Timer and Input
         *
         * @param filename The name of the ROM file
         * @return 1 if there are no file with name filename or the CPU encountered an error (unexpected opcode), 0 otherwise
         */
        int run(const std::string &filename);

    private:
        Platform m_platform; ///< The platform

        static constexpr int FPS = 60; ///< The number of frames per second
        static constexpr int FRAMERATE = 1000 / FPS; ///< The number of milliseconds per frame

        /**
         * @brief Update the screen and handle the inputs
         * @details If the PPU is rendering, update the screen and handle the inputs
         *
         * @param lastCycleTime The last time the screen was updated
         * @param ppu The PPU
         * @param input The Input
         * @return True if the user wants to quit, false otherwise
         */
        bool updatePlatform(uint32_t &lastCycleTime, PPU &ppu, Input &input);

        /**
         * @brief Save the current content of the RAM to a file
         *
         * @param cartridge The cartridge
         * @see Cartridge::saveRAMData, MBC::saveRAMData
         */
        static void saveRAMData(const Cartridge &cartridge);
    };
} // namespace gameboy
